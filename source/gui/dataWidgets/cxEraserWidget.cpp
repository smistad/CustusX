/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include <cxEraserWidget.h>

#include <QTimer>
#include <QCheckBox>

#include "vtkSphereWidget.h"
#include "vtkSplineWidget.h"
#include "vtkSplineWidget2.h"
#include "vtkSplineRepresentation.h"
#include "vtkRenderWindow.h"
#include <vtkSphere.h>
#include <vtkClipPolyData.h>
#include <vtkImageData.h>

#include "cxMesh.h"
#include "cxStringDataAdapter.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDefinitionStrings.h"
#include "cxDataManager.h"
#include "cxUtilHelpers.h"
#include "cxReporter.h"
#include "cxRegistrationTransform.h"
#include "cxImageAlgorithms.h"
#include "cxDoubleWidgets.h"
#include "cxImage.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxInteractiveCropper.h"
#include "cxViewManager.h"
#include "cxViewWrapper.h"
#include "cxViewGroup.h"
#include "cxVolumeHelpers.h"
#include "cxPatientModelService.h"

namespace cx
{

EraserWidget::EraserWidget(QWidget* parent) :
	BaseWidget(parent, "EraserWidget", "Eraser"),
	mPreviousCenter(0,0,0),
	mPreviousRadius(0)
{

	QVBoxLayout* layout = new QVBoxLayout(this);

	mContinousEraseTimer = new QTimer(this);
	connect(mContinousEraseTimer, SIGNAL(timeout()), this, SLOT(continousRemoveSlot())); // this signal will be executed in the thread of THIS, i.e. the main thread.

//	this->setToolTip(this->defaultWhatsThis());

//	layout->addWidget(new QLabel(QString("<font size=4 color=red><b>%1</b></font><br>Erase parts of active volume using a sphere.").arg("Experimental Widget!!")));
//	layout->addStretch();

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	layout->addLayout(buttonLayout);
	QHBoxLayout* buttonLayout2 = new QHBoxLayout;
	layout->addLayout(buttonLayout2);

	mShowEraserCheckBox = new QCheckBox("Show");
	mShowEraserCheckBox->setToolTip("Show eraser sphere in the views.");
	connect(mShowEraserCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleShowEraser(bool)));
	buttonLayout->addWidget(mShowEraserCheckBox);

	mContinousEraseCheckBox = new QCheckBox("Continous");
	mContinousEraseCheckBox->setToolTip("Erase continously using the sphere. (might be slow)");
	connect(mContinousEraseCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleContinous(bool)));
	buttonLayout2->addWidget(mContinousEraseCheckBox);

	mDuplicateAction = this->createAction(this, QIcon(), "Duplicate", "Duplicate active volume - do this before erasing!",
		SLOT(duplicateSlot()), buttonLayout);

	mSaveAction = this->createAction(this, QIcon(), "Save", "Save modified image to disk",
		SLOT(saveSlot()), buttonLayout);

	mRemoveAction = this->createAction(this, QIcon(), "Erase", "Erase everything inside sphere",
		SLOT(removeSlot()), buttonLayout2);


	double sphereRadius = 10;
	mSphereSizeAdapter = DoubleDataAdapterXml::initialize("SphereSize", "Sphere Size", "Radius of Eraser Sphere", sphereRadius, DoubleRange(1,200,1), 0, QDomNode());
	connect(mSphereSizeAdapter.get(), SIGNAL(changed()), this, SLOT(sphereSizeChangedSlot()));
	mSphereSize = new SpinBoxAndSliderGroupWidget(this, mSphereSizeAdapter);
	layout->addWidget(mSphereSize);

	layout->addStretch();

	this->enableButtons();
}

void EraserWidget::enableButtons()
{
	bool e = mShowEraserCheckBox->isChecked();

	mContinousEraseCheckBox->setEnabled(e);
//	mDuplicateAction->setEnabled(e);
//	mSaveAction->setEnabled(e);
	mRemoveAction->setEnabled(e);
	mSphereSize->setEnabled(e);
}

EraserWidget::~EraserWidget()
{
}

QString EraserWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Functionality for erasing parts of volumes/meshes.</h3>"
		"<p>"
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

void EraserWidget::toggleContinous(bool on)
{
	if (on)
	{
		mContinousEraseTimer->start(300);
	}
	else
	{
		mContinousEraseTimer->stop();
	}
}

void EraserWidget::continousRemoveSlot()
{
	Transform3D rMd = viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->get_rMd();
	Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();

	// optimization: dont remove if idle
	if (similar(mPreviousCenter, c) && similar(mPreviousRadius, r))
		return;

	this->removeSlot();
}

void EraserWidget::duplicateSlot()
{
	ImagePtr original = dataManager()->getActiveImage();
//	QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

	ImagePtr duplicate = duplicateImage(dataService(), original);
	patientService()->insertData(duplicate);
//	dataManager()->loadData(duplicate);
//	dataManager()->saveImage(duplicate, outputBasePath);
	dataManager()->setActiveImage(duplicate);

	// replace viz of original with duplicate
	std::vector<ViewGroupPtr> viewGroups = viewManager()->getViewGroups();
	for (unsigned i = 0; i < viewGroups.size(); ++i)
	{
		if (viewGroups[i]->getData()->removeData(original))
			viewGroups[i]->getData()->addData(duplicate);
	}
}

void EraserWidget::sphereSizeChangedSlot()
{
	if (mSphere)
	{
		mSphere->SetRadius(mSphereSizeAdapter->getValue());
		mSphere->Update();
	}
}

/**The image data themselves are not saved during normal file save.
 * This slot saves the file data to mhd.
 *
 */
void EraserWidget::saveSlot()
{
	patientService()->insertData(dataManager()->getActiveImage());
//	ImagePtr image = dataManager()->getActiveImage();
//	QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

//	dataManager()->saveImage(image, outputBasePath);
}


template <class TYPE>
void EraserWidget::eraseVolume(TYPE* volumePointer, TYPE replaceVal)
{
	ImagePtr image = dataManager()->getActiveImage();
	vtkImageDataPtr img = image->getBaseVtkImageData();

	Eigen::Array3i dim(img->GetDimensions());
	Vector3D spacing(img->GetSpacing());

	Transform3D rMd = viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->get_rMd();
	Vector3D c(mSphere->GetCenter());
	c = rMd.coord(c);
	double r = mSphere->GetRadius();
	mPreviousCenter = c;
	mPreviousRadius = r;

	DoubleBoundingBox3D bb_r(c[0]-r, c[0]+r, c[1]-r, c[1]+r, c[2]-r, c[2]+r);

	Transform3D dMr = image->get_rMd().inv();
	Transform3D rawMd = createTransformScale(spacing).inv();
	Transform3D rawMr = rawMd * dMr;
	Vector3D c_d = dMr.coord(c);
	double r_d = dMr.vector(r * Vector3D::UnitX()).length();
	c = rawMr.coord(c);
	r = rawMr.vector(r * Vector3D::UnitX()).length();
	DoubleBoundingBox3D bb0_raw = transform(rawMr, bb_r);
	IntBoundingBox3D bb1_raw(0, dim[0], 0, dim[1], 0, dim[2]);

//	std::cout << "     sphere: " << bb0_raw << std::endl;
//	std::cout << "        raw: " << bb1_raw << std::endl;

	for (int i=0; i<3; ++i)
	{
		bb1_raw[2*i] = std::max<double>(bb1_raw[2*i], bb0_raw[2*i]);
		bb1_raw[2*i+1] = std::min<double>(bb1_raw[2*i+1], bb0_raw[2*i+1]);
	}

	for (int x = bb1_raw[0]; x < bb1_raw[1]; ++x)
		for (int y = bb1_raw[2]; y < bb1_raw[3]; ++y)
			for (int z = bb1_raw[4]; z < bb1_raw[5]; ++z)
			{
				int index = x + y * dim[0] + z * dim[0] * dim[1];
				if ((Vector3D(x*spacing[0], y*spacing[1], z*spacing[2]) - c_d).length() < r_d)
					volumePointer[index] = replaceVal;
			}
}

//#define VTK_VOID            0
//#define VTK_BIT             1
//#define VTK_CHAR            2
//#define VTK_SIGNED_CHAR    15
//#define VTK_UNSIGNED_CHAR   3
//#define VTK_SHORT           4
//#define VTK_UNSIGNED_SHORT  5
//#define VTK_INT             6
//#define VTK_UNSIGNED_INT    7
//#define VTK_LONG            8
//#define VTK_UNSIGNED_LONG   9
//#define VTK_FLOAT          10
//#define VTK_DOUBLE         11
//#define VTK_ID_TYPE        12

void EraserWidget::removeSlot()
{
	if (!mSphere)
		return;

//	vtkPolyDataPtr poly = vtkPolyDataPtr::New();
//	mEraserSphere->GetPolyData(poly);

#if 0
	// experimental clipping of mesh - has no effect...
	std::map<QString,MeshPtr> meshes = dataManager()->getMeshes();
	if (!meshes.empty())
	{
		MeshPtr mesh = meshes.begin()->second;

		Vector3D c(mEraserSphere->GetCenter());
		double r = mEraserSphere->GetRadius();
		Transform3D dMr = mesh->get_rMd().inv();
		Vector3D c_d = dMr.coord(c);
		double r_d = dMr.vector(r * Vector3D::UnitX()).length();
		vtkSphere* sphere = vtkSphere::New();
		sphere->SetRadius(r_d);
		sphere->SetCenter(c_d.data());


//		mEraserSphere->GetSphere(sphere);
		vtkClipPolyData* clipper = vtkClipPolyData::New();
		clipper->SetInput(mesh->getVtkPolyData());
		clipper->SetClipFunction(sphere);
		clipper->Update();
		mesh->setVtkPolyData(clipper->GetOutput());
		return;
	}
#endif

	ImagePtr image = dataManager()->getActiveImage();
	vtkImageDataPtr img = image->getBaseVtkImageData();

	if (img->GetScalarType()==VTK_CHAR)
		this->eraseVolume(static_cast<char*> (img->GetScalarPointer()), VTK_CHAR_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_CHAR)
		this->eraseVolume(static_cast<unsigned char*> (img->GetScalarPointer()), VTK_UNSIGNED_CHAR_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_SHORT)
		this->eraseVolume(static_cast<unsigned short*> (img->GetScalarPointer()), VTK_UNSIGNED_SHORT_MIN);
	if (img->GetScalarType()==VTK_SHORT)
		this->eraseVolume(static_cast<short*> (img->GetScalarPointer()), VTK_SHORT_MIN);
	if (img->GetScalarType()==VTK_UNSIGNED_INT)
		this->eraseVolume(static_cast<unsigned int*> (img->GetScalarPointer()), VTK_UNSIGNED_INT_MIN);
	if (img->GetScalarType()==VTK_INT)
		this->eraseVolume(static_cast<int*> (img->GetScalarPointer()), VTK_INT_MIN);

	ImageLUT2DPtr tf2D = image->getLookupTable2D();
	ImageTF3DPtr tf3D = image->getTransferFunctions3D();

//	img->Modified();
	setDeepModified(img);
	image->setVtkImageData(img);

	// keep existing transfer functions
	image->setLookupTable2D(tf2D);
	image->setTransferFunctions3D(tf3D);
}

void EraserWidget::toggleShowEraser(bool on)
{
	if (on)
	{
		std::vector<ViewGroupPtr> viewGroups = viewManager()->getViewGroups();
		mSphere = vtkSphereSourcePtr::New();

		mSphere->SetRadius(40);
		mSphere->SetThetaResolution(16);
		mSphere->SetPhiResolution(12);
		mSphere->LatLongTessellationOn(); // more natural wireframe view

		double a = mSphereSizeAdapter->getValue();
		mSphere->SetRadius(a);
		mSphere->Update();
		MeshPtr glyph = viewGroups.front()->getData()->getOptions().mPickerGlyph;
		glyph->setVtkPolyData(mSphere->GetOutput());
		glyph->setColor(QColor(255, 204, 0)); // same as tool
		glyph->setIsWireframe(true);

		// set same glyph in all groups
		for (unsigned i=0; i<viewGroups.size(); ++i)
		{
			ViewGroupData::Options options = viewGroups[i]->getData()->getOptions();
			options.mPickerGlyph = glyph;
			viewGroups[i]->getData()->setOptions(options);
		}
	}
	else
	{
		viewManager()->getViewGroups().front()->getData()->getOptions().mPickerGlyph->setVtkPolyData(NULL);
		mContinousEraseCheckBox->setChecked(false);
	}

	this->enableButtons();
}

}
