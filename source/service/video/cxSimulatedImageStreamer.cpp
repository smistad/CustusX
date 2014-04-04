#include "cxSimulatedImageStreamer.h"

#include <boost/math/special_functions/round.hpp>

#include "vtkPNGWriter.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkImageMask.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxSliceProxy.h"
#include "cxProbeSector.h"
#include "cxProbeData.h"
//#include "cxToolManager.h"
#include "cxTransform3D.h"
#include "cxVolumeHelpers.h"
//#include "cxToolManager.h"

#include "cxSlicedImageProxy.h"
#include "cxSliceProxy.h"
#include "vtkImageChangeInformation.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"

namespace cx
{

SimulatedImageStreamer::SimulatedImageStreamer() :
	mTimer(new CyclicActionLogger())
{
	this->setSendInterval(40);
}

SimulatedImageStreamer::~SimulatedImageStreamer()
{}


void SimulatedImageStreamer::initialize(ImagePtr image, ToolPtr tool, DataServicePtr dataManager)
{
	if(!image || !tool || !dataManager)
	{
		this->setInitialized(false);
		return;
	}
	mDataManager = dataManager;
	this->createSendTimer();

	this->setSourceImage(image);
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot()));
	connect(mTool->getProbe().get(), SIGNAL(activeConfigChanged()), this, SLOT(resetMask()));

//	this->generateMaskSlot();

	this->setInitialized(true);
}

bool SimulatedImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		reportError("SimulatedImageStreamer: Failed to start streaming: Not initialized.");
		return false;
	}
	mSender = sender;
	mSendTimer->start(this->getSendInterval());
	return true;
}

void SimulatedImageStreamer::stopStreaming()
{
	if(mSendTimer)
		mSendTimer->stop();
}

QString SimulatedImageStreamer::getType()
{
	return "SimulatedImageStreamer";
}

void SimulatedImageStreamer::streamSlot()
{
	PackagePtr package(new Package());
	package->mImage = this->getSlice();
	mSender->send(package);
}

ImagePtr SimulatedImageStreamer::getSlice()
{
	if(!mTool || !mSourceImage)
		return ImagePtr();

	if (!mCachedImageToSend)
	{
		mCachedImageToSend = this->calculateSlice(mSourceImage);
	}

	return mCachedImageToSend;
}

vtkImageDataPtr SimulatedImageStreamer::getMask()
{
	if (!mCachedMask)
	{
		ProbeSectorPtr sector = mTool->getProbe()->getSector();
		mCachedMask = sector->getMask();
	}
	return mCachedMask;
}

void SimulatedImageStreamer::resetMask()
{
	mCachedMask = NULL;
	mCachedImageToSend.reset();
}

void SimulatedImageStreamer::sliceSlot()
{
	mCachedImageToSend.reset();
}

void SimulatedImageStreamer::setSourceToImageSlot(QString imageUid)
{
	ImagePtr image = mDataManager->getImage(imageUid);
	this->setSourceImage(image);
}

void SimulatedImageStreamer::setSourceImage(ImagePtr image)
{
	mSourceImage = image;
	connect(mSourceImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(sliceSlot()));
	this->sliceSlot();
}

ImagePtr SimulatedImageStreamer::calculateSlice(ImagePtr source)
{
	mTimer->begin();
	vtkImageDataPtr framegrabbedSlice = this->frameGrab(source);
	mTimer->time("Grab");
	vtkImageDataPtr maskedFramedgrabbedSlice = this->maskSlice(framegrabbedSlice);
	mTimer->time("Mask");
	vtkImageDataPtr simulatedSlice = this->simulateUS(maskedFramedgrabbedSlice);
	mTimer->time("Simulate");
	ImagePtr slice = this->convertToSscImage(simulatedSlice, source);
	mTimer->time("Convert");

	return slice;
}

vtkImageDataPtr SimulatedImageStreamer::frameGrab(ImagePtr source)
{
	SlicedImageProxyPtr imageSlicer(new SlicedImageProxy);
	imageSlicer->setImage(source);

	SimpleSliceProxyPtr slicer(new SimpleSliceProxy);
	Transform3D vMr = this->getTransform_vMr();
	slicer->set_sMr(vMr);
	imageSlicer->setSliceProxy(slicer);

	ProbeDefinition probedata = mTool->getProbe()->getProbeData();
	Eigen::Array3i outDim(probedata.getSize().width(), probedata.getSize().height(), 1);
	imageSlicer->setOutputFormat(Vector3D(0,0,0), outDim, probedata.getSpacing());

	imageSlicer->update();
	imageSlicer->getOutputPort()->Update();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageSlicer->getOutput());
	return retval;
}

vtkImageDataPtr SimulatedImageStreamer::maskSlice(vtkImageDataPtr unmaskedSlice)
{
	vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
	maskFilter->SetMaskInputData(this->getMask());
	maskFilter->SetImageInputData(unmaskedSlice);
	maskFilter->SetMaskedOutputValue(0.0);
	maskFilter->Update();

	vtkImageDataPtr maskedSlice = maskFilter->GetOutput();
	return maskedSlice;
}

vtkImageDataPtr SimulatedImageStreamer::simulateUS(vtkImageDataPtr maskedFramedgrabbedSlice)
{
	vtkImageDataPtr simulatedSlice;
	QString simulationType = settings()->value("USsimulation/type", "Original data").toString();
	if(simulationType == "CT to US")
		simulatedSlice = simulateUSFromCTSlice(maskedFramedgrabbedSlice);
	else if(simulationType == "MR to US")
		simulatedSlice = simulateUSFromMRSlice(maskedFramedgrabbedSlice);
	else if(simulationType == "Original data")
	{
//		cx::reporter()->sendDebug("SimulatedImageStreamer::simulateUS(): Original data selected");
		simulatedSlice = maskedFramedgrabbedSlice;
	}
	else
	{
		cx::reporter()->sendWarning("SimulatedImageStreamer::simulateUS(): Unknown simulation: " + simulationType);
		simulatedSlice = maskedFramedgrabbedSlice;
	}

	return simulatedSlice;
}

//TODO: implement, and put in a separate class
vtkImageDataPtr SimulatedImageStreamer::simulateUSFromCTSlice(vtkImageDataPtr maskedFramedgrabbedSlice)
{
//	cx::reporter()->sendDebug("SimulatedImageStreamer: CT to US");
	vtkImageDataPtr simulatedSlice;
	simulatedSlice = maskedFramedgrabbedSlice;
	return simulatedSlice;
}

//TODO: implement, and put in a separate class
vtkImageDataPtr SimulatedImageStreamer::simulateUSFromMRSlice(vtkImageDataPtr maskedFramedgrabbedSlice)
{
//	cx::reporter()->sendDebug("SimulatedImageStreamer MR to US");
	vtkImageDataPtr simulatedSlice;
	simulatedSlice = maskedFramedgrabbedSlice;
	return simulatedSlice;
}

ImagePtr SimulatedImageStreamer::convertToSscImage(vtkImageDataPtr slice, ImagePtr volume)
{
	ImagePtr retval = ImagePtr(new Image("Simulated US", slice, "Simulated US"));
	return retval;
}

Transform3D SimulatedImageStreamer::getTransform_vMr()
{
	ProbeDefinition probedata = mTool->getProbe()->getProbeData();
	ProbeSector probesector;
	probesector.setData(probedata);

	Transform3D uMt = probesector.get_tMu().inv();
	Transform3D vMu = probesector.get_uMv().inv();
	Transform3D vMt = vMu * uMt;

	Transform3D tMpr = mTool->get_prMt().inv();
	Transform3D prMr = mDataManager->get_rMpr().inv();

	Transform3D vMr = vMt * tMpr * prMr;
	return vMr;
}

int SimulatedImageStreamer::getAverageTimePerSimulatedFrame()
{
	cx::reporter()->sendDebug("Grab frame: " + qstring_cast(mTimer->getTime(QString("Grab"))));
	cx::reporter()->sendDebug("Mask frame: " + qstring_cast(mTimer->getTime(QString("Mask"))));
	cx::reporter()->sendDebug("Run simulation: " + qstring_cast(mTimer->getTime(QString("Simulate"))));
	cx::reporter()->sendDebug("Convert frame to Image: " + qstring_cast(mTimer->getTime(QString("Convert"))));
	return mTimer->getTotalLoggedTime();
}

} /* namespace cx */
