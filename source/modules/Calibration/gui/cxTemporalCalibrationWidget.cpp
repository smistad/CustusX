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
#include <cxTemporalCalibrationWidget.h>


#include <QtWidgets>

#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "cxTrackingService.h"
#include "cxLabeledComboBoxWidget.h"
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include "cxDoubleWidgets.h"
#include "cxTypeConversions.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolProperty.h"
#include "cxDoublePropertyTemporalCalibration.h"
#include "cxUtilHelpers.h"
#include "cxVolumeHelpers.h"
#include "vtkImageCorrelation.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"

typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;
typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;


namespace cx
{

typedef unsigned char uchar;


TemporalCalibrationWidget::TemporalCalibrationWidget(VisServicesPtr services, AcquisitionServicePtr acquisitionService, QWidget* parent) :
	BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibration"),
	mInfoLabel(new QLabel(""))
{
	mServices = services;

  mAlgorithm.reset(new TemporalCalibration);
  connect(mServices->getPatientService().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

//  connect(mAcquisition.get(), SIGNAL(ready(bool,QString)), mRecordSessionWidget, SLOT(setReady(bool,QString)));
  connect(acquisitionService.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(selectData(QString)));
//  mAcquisition->checkIfReadySlot();

  mRecordSessionWidget = new RecordSessionWidget(acquisitionService, this, "temporal_calib");

//  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), mAcquisition.get(), SLOT(saveSession(QString)));
//  connect(mRecordSessionWidget, SIGNAL(started(QString)), mAcquisition.get(), SLOT(startRecord(QString)));
//  connect(mRecordSessionWidget, SIGNAL(stopped(bool)), mAcquisition.get(), SLOT(stopRecord(bool)));
  mRecordSessionWidget->setDescriptionVisibility(false);

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  // add recording widgets
  QLabel* acqLabel = new QLabel("<b>Acquisition</b>");
  topLayout->addWidget(acqLabel);
  acqLabel->setToolTip(this->defaultWhatsThis());
  topLayout->addWidget(mInfoLabel);
  topLayout->addWidget(mRecordSessionWidget);
  topLayout->addWidget(new LabeledComboBoxWidget(this, StringPropertyActiveProbeConfiguration::New(mServices->getToolManager())));
  topLayout->addWidget(new SpinBoxGroupWidget(this, DoublePropertyTimeCalibration::New()));

  topLayout->addWidget(this->createHorizontalLine());

  // add calibration widgets
  QLabel* calLabel = new QLabel("<b>Calibration</b>");
  topLayout->addWidget(calLabel);
  calLabel->setToolTip(this->defaultWhatsThis());

  mFileSelectWidget = new FileSelectWidget(this);
  connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));
  mFileSelectWidget->setNameFilter(QStringList() << "*.fts");
  topLayout->addWidget(mFileSelectWidget);

  mVerbose = new QCheckBox("Save data to temporal_calib.txt");
  topLayout->addWidget(mVerbose);

	QPushButton* calibrateButton = new QPushButton("Calibrate");
	calibrateButton->setToolTip("Calculate the temporal shift for the selected acqusition."
															"The shift is not applied in any way."
															"<p><b>NB:</b>"
															" The calculation takes a few seconds, and in this time the program will be unresponsive</p>");

  connect(calibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  topLayout->addWidget(calibrateButton);

  mResult = new QLineEdit;
  mResult->setReadOnly(true);
  topLayout->addWidget(mResult);

  topLayout->addStretch();

  this->patientChangedSlot();
}

TemporalCalibrationWidget::~TemporalCalibrationWidget()
{}

QString TemporalCalibrationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Temporal Calibration.</h3>"
			"<p><i>Calibrate the time shift between the tracking system and the video acquisition source.</i></br>"
			"<p>Part 1, Acqusition: Move the probe in a sinusoidal pattern up and down in a water tank or similar."
			"The <i>first</i> image should be a typical image, as it is used to correlate against all the others."
			"<p>Part 2, Calibration: Press calibrate to calculate the temporal shift for the selected acquisition."
			"The shift is not applied in any way. Refer to the log folder for the calibration curves."
			"<p><b>NB:</b> Previous temporal calibration is applied to probe frames during acqusition.</p>"
			"</html>";
}

void TemporalCalibrationWidget::showEvent(QShowEvent* event)
{
  mFileSelectWidget->refresh();
}

void TemporalCalibrationWidget::patientChangedSlot()
{
//  std::cout << "TemporalCalibrationWidget::patientChangedSlot() "  << std::endl;
  QString filename = mServices->getPatientService()->getActivePatientFolder() + "/US_Acq/";
  mFileSelectWidget->setPath(filename);
//  this->selectData(filename);
}

void TemporalCalibrationWidget::selectData(QString filename)
{
//  std::cout << "TemporalCalibrationWidget::selectData " << filename << std::endl;
  mAlgorithm->selectData(filename);
  mFileSelectWidget->setFilename(filename);
  mResult->setText("");
}

/**
 *
 */
void TemporalCalibrationWidget::calibrateSlot()
{
  if (mVerbose->isChecked())
	mAlgorithm->setDebugFolder(mServices->getPatientService()->getActivePatientFolder()+"/Logs/");
  else
    mAlgorithm->setDebugFolder("");

  bool success = true;
  double shift = mAlgorithm->calibrate(&success);
  if (success)
  {
	  reportSuccess(QString("Completed temporal calibration, found shift %1 ms").arg(shift,0,'f',0));
	  mResult->setText(QString("Shift = %1 ms").arg(shift, 0, 'f', 0));
  }
  else
  {
	  reportError(QString("Temporal calibration failed"));
	  mResult->setText(QString("failed"));
  }
}


}//namespace cx


