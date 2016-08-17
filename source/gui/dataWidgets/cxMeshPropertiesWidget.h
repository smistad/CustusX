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
#ifndef CXMESHPROPERTIESWIDGET_H
#define CXMESHPROPERTIESWIDGET_H

#include "cxGuiExport.h"

#include <vector>
//#include <QtWidgets>

#include "cxMesh.h"
#include "cxDataInterface.h"
#include "cxBaseWidget.h"


namespace cx
{
typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;
class ReplacableContentWidget;

/**
 * \class MeshPropertiesWidget
 *
 * \brief Widget for displaying glyps information about meshes.
 *
 * \ingroup cx_gui
 *
 * split from class MeshInfoWidget
 */
class cxGui_EXPORT MeshPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
	MeshPropertiesWidget(SelectDataStringPropertyBasePtr meshSelector,
				   PatientModelServicePtr patientModelService, ViewServicePtr viewService,
				   QWidget* parent);
  virtual ~MeshPropertiesWidget();

	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }

protected:
  void setupUI();
  virtual void prePaintEvent();

private:
  void updateFrontend();

protected slots:
  void meshSelectedSlot();

private:
  MeshPtr mMesh;
  SelectDataStringPropertyBasePtr mMeshSelector;

  ReplacableContentWidget* mPropertiesWidget;

  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

  MeshPropertiesWidget();
  void clearUI();
};

}//end namespace cx

#endif // CXMESHPROPERTIESWIDGET_H
