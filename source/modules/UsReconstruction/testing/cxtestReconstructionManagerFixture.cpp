// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxtestReconstructionManagerFixture.h"

#include "cxDataManager.h"
#include "catch.hpp"
#include "cxPNNReconstructAlgorithm.h"
#include <QApplication>
#include "cxDoubleDataAdapterXml.h"
#include "cxTimedAlgorithm.h"

#include "cxBoolDataAdapterXml.h"

#include "cxDataLocations.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructionExecuter.h"
#include "cxLogicManager.h"

namespace cxtest
{

ReconstructionManagerTestFixture::ReconstructionManagerTestFixture()
{
	mVerbose = false;
	cx::LogicManager::initialize();
}

ReconstructionManagerTestFixture::~ReconstructionManagerTestFixture()
{
	cx::LogicManager::shutdown();
}

void ReconstructionManagerTestFixture::setPNN_InterpolationSteps(int value)
{
	cx::ReconstructionManagerPtr manager = this->getManager();
	// set an algorithm-specific parameter
	QDomElement algo = manager->getSettings().getElement("algorithms", "PNN");
	boost::shared_ptr<cx::PNNReconstructAlgorithm> algorithm;
	algorithm = boost::dynamic_pointer_cast<cx::PNNReconstructAlgorithm>(manager->createAlgorithm());
	REQUIRE(algorithm);// Check if we got the PNN algorithm

	algorithm->getInterpolationStepsOption(algo)->setValue(value);
}

void ReconstructionManagerTestFixture::reconstruct()
{
	mOutput.clear();
	cx::ReconstructionManagerPtr reconstructer = this->getManager();
	cx::ReconstructionExecuterPtr executor(new cx::ReconstructionExecuter);
	bool validInputData = true; //TODO should be checked in some way???
	cx::ReconstructPreprocessorPtr preprocessor = executor->createPreprocessor(reconstructer->createCoreParameters(), reconstructer->getSelectedFileData());
	bool createBModeWhenAngio = reconstructer->getParams()->mCreateBModeWhenAngio->getValue();
	std::vector<cx::ReconstructCorePtr> cores = executor->createCores(reconstructer->createAlgorithm(), reconstructer->createCoreParameters(), createBModeWhenAngio);
//	preprocessor->initializeCores(cores);


	//......
	std::vector<bool> angio;
	for (unsigned i=0; i<cores.size(); ++i)
		angio.push_back(cores[i]->getInputParams().mAngio);

	std::vector<cx::ProcessedUSInputDataPtr> processedInput = preprocessor->createProcessedInput(angio);
	REQUIRE(cores.size() == processedInput.size());

	for (unsigned i=0; i<cores.size(); ++i)
	{
		cores[i]->initialize(processedInput[i], preprocessor->getOutputVolumeParams());
	}
	//......
	for (unsigned i=0; i<cores.size(); ++i)
	{
		cores[i]->reconstruct();
		mOutput.push_back(cores[i]->getOutput());
	}

}

void ReconstructionManagerTestFixture::threadedReconstruct()
{
	mOutput.clear();
	cx::ReconstructionManagerPtr manager = this->getManager();

	// start threaded reconstruction
	std::vector<cx::ReconstructCorePtr> cores = manager->startReconstruction();
	std::set<cx::TimedAlgorithmPtr> threads = manager->getThreadedReconstruction();
	REQUIRE(threads.size()==1);
	cx::TimedAlgorithmPtr thread = *threads.begin();
	QObject::connect(thread.get(), SIGNAL(finished()), qApp, SLOT(quit()));
	qApp->exec();


	// validate output
	REQUIRE(thread->isFinished());
	REQUIRE(!thread->isRunning());

	for (unsigned i=0; i<cores.size(); ++i)
	{
		mOutput.push_back(cores[i]->getOutput());
	}
}

std::vector<cx::ImagePtr> ReconstructionManagerTestFixture::getOutput()
{
	return mOutput;
}

SyntheticVolumeComparerPtr ReconstructionManagerTestFixture::getComparerForOutput(SyntheticReconstructInputPtr input, int index)
{
	SyntheticVolumeComparerPtr comparer(new SyntheticVolumeComparer());
	comparer->setVerbose(this->getVerbose());
	comparer->setPhantom(input->getPhantom());
	comparer->setTestImage(this->getOutput()[index]);
	return comparer;
}

cx::ReconstructionManagerPtr ReconstructionManagerTestFixture::getManager()
{
	if (!mManager)
	{
		cx::XmlOptionFile settings;
		cx::ReconstructionManagerPtr reconstructer(new cx::ReconstructionManagerImpl(settings,""));

		reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
		reconstructer->setOutputRelativePath("Images");

		mManager = reconstructer;
	}
	return mManager;
}

} // namespace cxtest


