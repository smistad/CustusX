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

#include "cxUr5PluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxUr5GUIExtenderService.h"
#include "cxRegisteredService.h"
#include "trackingSystemRobot/cxRobotTrackingSystemService.h"

namespace cx
{

Ur5PluginActivator::Ur5PluginActivator()
{
    std::cout << "Created Ur5PluginActivator" << std::endl;
}

Ur5PluginActivator::~Ur5PluginActivator()
{}

void Ur5PluginActivator::start(ctkPluginContext* context)
{   
    mUr5Robot = Ur5RobotPtr(new Ur5Robot);

    Ur5GUIExtenderService* gui = new Ur5GUIExtenderService(context,mUr5Robot);
    RobotTrackingSystemService* tracking = new RobotTrackingSystemService(mUr5Robot);

    mRegistrationGui = RegisteredService::create<Ur5GUIExtenderService>(context,gui, GUIExtenderService_iid);
    mRegistrationTracking = RegisteredService::create<RobotTrackingSystemService>(context,tracking,TrackingSystemService_iid);
}

void Ur5PluginActivator::stop(ctkPluginContext* context)
{
    mRegistrationGui.reset();
    mRegistrationTracking.reset();

	Q_UNUSED(context);
}

} // namespace cx



