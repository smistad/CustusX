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

#ifndef CXPREFERENCETAB_H_
#define CXPREFERENCETAB_H_

#include "cxGuiExport.h"

#include "cxDoubleProperty.h"
#include "cxStringProperty.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QGridLayout;
class QVBoxLayout;

namespace cx
{

class cxGui_EXPORT PreferenceTab : public QWidget
{
  Q_OBJECT
public:
	PreferenceTab(QWidget *parent = 0);

  virtual void init() = 0;

public slots:
  virtual void saveParametersSlot() = 0;

signals:
  void savedParameters();

protected:
  QVBoxLayout* mTopLayout;
};

/**
 * \class PerformanceTab
 *
 * \brief Configure performance tab in preferences dialog
 *
 * \date Mar 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT PerformanceTab : public PreferenceTab
{
    Q_OBJECT

public:
  PerformanceTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();

protected:
  QSpinBox* mRenderingIntervalSpinBox;
  QLabel* mRenderingRateLabel;
  QCheckBox* mSmartRenderCheckBox;
  QCheckBox* mGPU2DRenderCheckBox;
  QCheckBox* mOptimizedViewsCheckBox;
  QCheckBox* mGPU3DDepthPeelingCheckBox;
  QCheckBox* mShadingCheckBox;
  QGridLayout* mMainLayout;
  DoublePropertyPtr mMaxRenderSize;
  DoublePropertyPtr mStillUpdateRate;
  StringPropertyPtr m3DVisualizer;

private slots:
  void renderingIntervalSlot(int interval);
  static QStringList getAvailableVisualizers();
  static std::map<QString, QString> getAvailableVisualizerDisplayNames();
};

} /* namespace cx */
#endif /* CXPREFERENCETAB_H_ */
