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
#include "cxToolManualCalibrationWidget.h"

#include <QLabel>
#include <QGroupBox>
#include "cxActiveToolWidget.h"
#include "cxTrackingService.h"
#include "cxHelperWidgets.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"

namespace cx
{

ToolManualCalibrationWidget::ToolManualCalibrationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ToolManualCalibrationWidget", "Tool Manual Calibrate"),
	mServices(services)
{
  //layout
  QVBoxLayout* mToptopLayout = new QVBoxLayout(this);
  //toptopLayout->setMargin(0);

  this->setToolTip("Edit tool calibration matrix sMt");
  mTool = StringPropertySelectTool::New(mServices->tracking());
  mToptopLayout->addWidget(sscCreateDataWidget(this, mTool));

  mToptopLayout->addWidget(new QLabel("<font color=red>Caution: sMt is changed directly by this control.</font>"));
  mGroup = new QGroupBox(this);
  mGroup->setTitle("Calibration matrix sMt");
  mToptopLayout->addWidget(mGroup);
  QVBoxLayout* groupLayout = new QVBoxLayout;
  mGroup->setLayout(groupLayout);
  groupLayout->setMargin(0);
  mMatrixWidget = new Transform3DWidget(mGroup);
  groupLayout->addWidget(mMatrixWidget);
  connect(mMatrixWidget, SIGNAL(changed()), this, SLOT(matrixWidgetChanged()));
  connect(mTool.get(), SIGNAL(changed()), this, SLOT(toolCalibrationChanged()));

  this->toolCalibrationChanged();
  mMatrixWidget->setEditable(true);

  mToptopLayout->addStretch();

  connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &ToolManualCalibrationWidget::toolCalibrationChanged);
}

void ToolManualCalibrationWidget::toolCalibrationChanged()
{
	ToolPtr tool = mTool->getTool();
  if (!tool)
    return;

  mMatrixWidget->blockSignals(true);
  mMatrixWidget->setMatrix(tool->getCalibration_sMt());
  mMatrixWidget->blockSignals(false);
}

void ToolManualCalibrationWidget::matrixWidgetChanged()
{
	ToolPtr tool = mTool->getTool();
	if (!tool)
      return;

  Transform3D M = mMatrixWidget->getMatrix();
  tool->setCalibration_sMt(M);
}



}
