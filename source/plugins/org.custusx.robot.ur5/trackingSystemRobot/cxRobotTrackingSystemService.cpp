#include "cxRobotTrackingSystemService.h"
#include <iostream>

#include "cxRobotTool.h"

namespace cx
{
std::vector<ToolPtr> toVector(std::map<QString, RobotToolPtr> map)
{
    std::vector<ToolPtr> retval;
    std::map<QString, RobotToolPtr>::iterator it = map.begin();
    for(; it!= map.end(); ++it)
    {
        retval.push_back(it->second);
    }
    return retval;
}

RobotTrackingSystemService::RobotTrackingSystemService(Ur5RobotPtr robot) :
    mState(Tool::tsNONE),
    mTimer(0),
    mUr5Robot(robot),
    tps(0.1)
{
    if(robot == NULL)
        return;

    connect(mUr5Robot.get(), &Ur5Robot::startTracking, this, &RobotTrackingSystemService::startTracking);
    connect(mUr5Robot.get(), &Ur5Robot::stopTracking, this, &RobotTrackingSystemService::stopTracking);
}

RobotTrackingSystemService::~RobotTrackingSystemService()
{
    this->deconfigure();
}

Tool::State RobotTrackingSystemService::getState() const
{
    return mState;
}

void RobotTrackingSystemService::setState(const Tool::State val)
{
    if (mState==val)
        return;

    if (val > mState) // up
    {
        if (val == Tool::tsTRACKING)
            this->startTracking();
        else if (val == Tool::tsINITIALIZED)
            this->initialize();
        else if (val == Tool::tsCONFIGURED)
            this->configure();
    }
    else // down
    {
        if (val == Tool::tsINITIALIZED)
            this->stopTracking();
        else if (val == Tool::tsCONFIGURED)
            this->uninitialize();
        else if (val == Tool::tsNONE)
            this->deconfigure();
    }
}

void RobotTrackingSystemService::startTracking()
{
    this->configure();
    this->internalSetState(Tool::tsTRACKING);
    connect(mUr5Robot.get(), &Ur5Robot::transform, this, &RobotTrackingSystemService::receiveTransform);
}

void RobotTrackingSystemService::stopTracking()
{
    disconnect(mUr5Robot.get(), &Ur5Robot::transform, this, &RobotTrackingSystemService::receiveTransform);
    this->deconfigure();
}

std::vector<ToolPtr> RobotTrackingSystemService::getTools()
{
    return toVector(mTools);
}

TrackerConfigurationPtr RobotTrackingSystemService::getConfiguration()
{
    TrackerConfigurationPtr retval;
    return retval;
}

ToolPtr RobotTrackingSystemService::getReference()
{
    return mReference;
}

void RobotTrackingSystemService::setLoggingFolder(QString loggingFolder)
{
}

void RobotTrackingSystemService::internalSetState(Tool::State state)
{
    mState = state;
    emit stateChanged();
}

void RobotTrackingSystemService::serverIsConnected()
{
    this->internalSetState(Tool::tsINITIALIZED);
}

void RobotTrackingSystemService::serverIsDisconnected()
{
    this->internalSetState(Tool::tsCONFIGURED);
}

void RobotTrackingSystemService::receiveTransform(QString devicename, Transform3D transform, double timestamp)
{
    if(mTimer==0 || timestamp>mTimer+tps)
    {
    RobotToolPtr tool = this->getTool(devicename);
    tool->toolTransformAndTimestampSlot(transform,timestamp);
    tool->tps(1/(timestamp-mTimer));
    mTimer = timestamp;
    }
}

void RobotTrackingSystemService::configure()
{
    this->serverIsConfigured();
}

void RobotTrackingSystemService::deconfigure()
{
    mTools.clear();
    mReference.reset();
    this->serverIsDeconfigured();
}

void RobotTrackingSystemService::initialize()
{

}

void RobotTrackingSystemService::uninitialize()
{

}

void RobotTrackingSystemService::serverIsConfigured()
{
    this->internalSetState(Tool::tsCONFIGURED);
}

void RobotTrackingSystemService::serverIsDeconfigured()
{
    this->internalSetState(Tool::tsNONE);
}


RobotToolPtr RobotTrackingSystemService::getTool(QString devicename)
{
    RobotToolPtr retval;
    std::map<QString, RobotToolPtr>::iterator it = mTools.find(devicename);
    if(it == mTools.end())
    {
        retval = RobotToolPtr(new RobotTool(devicename, mUr5Robot));
        mTools[devicename] = retval;
        //todo: will this work?
        emit stateChanged();
    }
    else
    {
        retval = it->second;
    }

    return retval;
}

} // namespace cx
