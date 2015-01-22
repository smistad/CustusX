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

#include "cxApplicationsParser.h"

#include <iostream>
#include <QApplication>
#include <QByteArray>
#include <QDir>
#include "cxEnumConverter.h"
#include "cxXmlOptionItem.h"

#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxWorkflowStateMachine.h"
#include "cxDataLocations.h"
#include "cxConfig.h"
#include "cxVLCRecorder.h"
#include "cxStateServiceBackend.h"

#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxVideoServiceProxy.h"
#include "cxProfile.h"

namespace cx
{

ApplicationsParser::ApplicationsParser()
{
//	mXmlFile = profile()->getXmlSettings().descend("applications");

	QString fullState =
			"AAAA/wAAAAD9AAAAAgAAAAAAAAGEAAADlvwCAAAACvsAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA/AAAAEEAAAI7AAACAwEAAB36AAAAAAEAAAAV+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABGQD////7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAP4A////+wAAADoAUABvAGkAbgB0AFMAYQBtAHAAbABpAG4AZwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD7AAAAOgBDAGEAbQBlAHIAYQBDAG8AbgB0AHIAbwBsAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAAAuAEkARwBUAEwAaQBuAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAUEA////+wAAADgAVQBTAEEAYwBxAHUAcwBpAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAANQA////+wAAAEIAVAByAGEAYwBrAGUAZABDAGUAbgB0AGUAcgBsAGkAbgBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAADDAP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAMEA////+wAAADIARgByAGEAbQBlAFQAcgBlAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGYA////+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAGEAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAQIA////+wAAAEQAQwBhAGwAaQBiAHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAZMA////+wAAAEgAVgBpAHMAdQBhAGwAaQB6AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAFMAP////sAAABGAFMAZQBnAG0AZQBuAHQAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAPEA////+wAAAEYAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABYgD////7AAAAPgBJAG0AYQBnAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAA8AD////7AAAAFABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAAGBAAABSgD////7AAAALABNAGUAdAByAGkAYwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAZgD////7AAAALABFAHIAYQBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAA2wD////7AAAAMABUAHIAYQBjAGsAUABhAGQAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAHQA////+wAAADYAVwBpAHIAZQBQAGgAYQBuAHQAbwBtAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAADyAP////wAAAKCAAABVQAAAL4BAAAd+gAAAAEBAAAAAvsAAAAuAEMAbwBuAHMAbwBsAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAFQA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAAAAAFvAAAAiAD////8AAADKAAAAMsAAAAAAP////oAAAAAAQAAAAH7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAAAAAAAAPsAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAAAAAAAAAD7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPwAAAJdAAABQAAAAAAA////+gAAAAABAAAAAfsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAAAAAAAAPsAAAA2AFQAbwBvAGwATQBhAG4AYQBnAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAiAD///8AAAADAAAAAAAAAAD8AQAAAAH7AAAAMABQAGwAYQB5AGIAYQBjAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAVMA////AAAF9gAAA5YAAAAEAAAABAAAAAgAAAAI/AAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAACAAAACQAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAQAAAOX/////AAAAAAAAAAAAAAAiAE4AYQB2AGkAZwBhAHQAaQBvAG4AVABvAG8AbABCAGEAcgEAAAGC/////wAAAAAAAAAAAAAAJgBDAGEAbQBlAHIAYQAzAEQAVgBpAGUAdwBUAG8AbwBsAEIAYQByAQAAAfv/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgEAAAME/////wAAAAAAAAAAAAAAIgBTAGMAcgBlAGUAbgBzAGgAbwB0AFQAbwBvAGwAQgBhAHIBAAADWf////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAA4r/////AAAAAAAAAAAAAAAsAEkAbgB0AGUAcgBhAGMAdABvAHIAUwB0AHkAbABlAFQAbwBvAGwAQgBhAHIAAAAD3/////8AAAAAAAAAAAAAABYASABlAGwAcABUAG8AbwBsAEIAYQByAQAAA98AAAF4AAAAAAAAAAA=";

	//Lab application
	this->addDefaultDesktops(
				"PatientDataUid",
				"LAYOUT_3D_ACS",
				"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AGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGQA////+wAAAD4AUABsAHUAZwBpAG4ARgByAGEAbQBlAHcAbwByAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGQA////+wAAADQAVQBTAFIAZQBjAG8AbgBzAHQAcgB1AGMAdABpAG8AbgBEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAZAD////7AAAAOABSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAZAD////7AAAAKABIAGUAbABwAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAABkAP///wAAAAMAAAAAAAAAAPwBAAAAAfsAAAAwAFAAbABhAHkAYgBhAGMAawBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABVwD///8AAAXtAAAEBQAAAAQAAAAEAAAACAAAAAj8AAAABAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAIAAAALAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIBAAABHf////8AAAAAAAAAAAAAACIATgBhAHYAaQBnAGEAdABpAG8AbgBUAG8AbwBsAEIAYQByAQAAAd7/////AAAAAAAAAAAAAAAmAEMAYQBtAGUAcgBhADMARABWAGkAZQB3AFQAbwBvAGwAQgBhAHIBAAACcf////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAQAAA7z/////AAAAAAAAAAAAAAAiAFMAYwByAGUAZQBuAHMAaABvAHQAVABvAG8AbABCAGEAcgEAAAQh/////wAAAAAAAAAAAAAAHABEAGUAcwBrAHQAbwBwAFQAbwBvAGwAQgBhAHIBAAAEWP////8AAAAAAAAAAAAAACwASQBuAHQAZQByAGEAYwB0AG8AcgBTAHQAeQBsAGUAVABvAG8AbABCAGEAcgAAAAPf/////wAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAAEvQAAAXgAAAAAAAAAAAAAABwAUwBhAG0AcABsAGUAcgBUAG8AbwBsAEIAYQByAAAABCX/////AAAAAAAAAAAAAAAiAFQAbwBvAGwATwBmAGYAcwBlAHQAVABvAG8AbABCAGEAcgAAAAUo/////wAAAAAAAAAA");
	this->addDefaultDesktops(
				"NavigationUid",
				"LAYOUT_3D_ACS",
				"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");
	this->addDefaultDesktops(
				"RegistrationUid",
				"LAYOUT_3D_ACS",
				"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");
	this->addDefaultDesktops(
				"PreOpPlanningUid",
				"LAYOUT_3D_ACS",
				"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");
	this->addDefaultDesktops(
				"IntraOpImagingUid",
				"LAYOUT_US_Acquisition",
				"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");
	this->addDefaultDesktops(
				"PostOpControllUid",
				"LAYOUT_3D_ACS",
				"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");
	this->addDefaultDesktops("DEFAULT", "LAYOUT_3D_ACS", fullState);
}

void ApplicationsParser::addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate)
{
	mWorkflowDefaultDesktops[workflowStateUid] = Desktop(layoutUid,
														 QByteArray::fromBase64(mainwindowstate.toLatin1()));
}

Desktop ApplicationsParser::getDefaultDesktop(QString workflowName)
{
	//TODO use applicationName!!!
	if (!mWorkflowDefaultDesktops.count(workflowName))
		return mWorkflowDefaultDesktops["DEFAULT"];
	return mWorkflowDefaultDesktops[workflowName];
}

Desktop ApplicationsParser::getDesktop(QString workflowName)
{
	Desktop retval;
	XmlOptionFile file = this->getSettings();
	QDomElement workflowElement = file.descend(workflowName).getElement();
	QDomElement desktopElement;
	if (workflowElement.namedItem("custom").isNull())
	{
		return this->getDefaultDesktop(workflowName);
	}
	else
	{
		desktopElement = workflowElement.namedItem("custom").toElement();
	}
	retval.mMainWindowState = QByteArray::fromBase64(desktopElement.attribute("mainwindowstate").toLatin1());
	retval.mLayoutUid = desktopElement.attribute("layoutuid");
	retval.mSecondaryLayoutUid = desktopElement.attribute("secondarylayoutuid");

	return retval;
}

void ApplicationsParser::setDesktop(QString workflowName, Desktop desktop)
{
	XmlOptionFile file = this->getSettings();
	QDomElement desktopElement = file.descend(workflowName).descend("custom").getElement();
//	QDomElement desktopElement =
//			mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).descend("custom").getElement();
	desktopElement.setAttribute("mainwindowstate", QString(desktop.mMainWindowState.toBase64()));
	desktopElement.setAttribute("layoutuid", desktop.mLayoutUid);
	desktopElement.setAttribute("secondarylayoutuid", desktop.mSecondaryLayoutUid);
	file.save();
}

XmlOptionFile ApplicationsParser::getSettings()
{
	XmlOptionFile retval = ProfileManager::getInstance()->activeProfile()->getXmlSettings();
	retval = retval.descend("workflows");
	return retval;
}

void ApplicationsParser::resetDesktop(QString workflowName)
{
	XmlOptionFile file = this->getSettings();

	QDomElement workflowElement = file.descend(workflowName).getElement();
	workflowElement.removeChild(workflowElement.namedItem("custom"));
	file.save();
}


} //namespace cx
