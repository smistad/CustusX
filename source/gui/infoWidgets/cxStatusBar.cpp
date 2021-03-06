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

#include "cxStatusBar.h"

#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QAction>
#include <QToolButton>
#include <QPixmap>
#include <QMetaObject>

#include "cxTrackingService.h"

#include "cxTrackingService.h"
#include "cxVideoService.h"
#include "boost/bind.hpp"
#include <QMetaMethod>
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"
#include "cxManualTool.h"
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxActiveToolProxy.h"
#include "cxLogicManager.h"
#include "cxViewService.h"

#include "cxLogMessageFilter.h"
#include "cxMessageListener.h"
#include "cxVLCRecorder.h"


namespace cx
{
StatusBar::StatusBar() :
	mRenderingFpsLabel(new QLabel(this)),
	mGrabbingInfoLabel(new QLabel(this)),
	mRecordFullscreenLabel(new QLabel(this)),
	mTpsLabel(new QLabel(this))
{
	mMessageListener = MessageListener::create();
	mMessageListener->installFilter(MessageFilterStatusBar::create());
	connect(mMessageListener.get(), &MessageListener::newMessage, this, &StatusBar::showMessageSlot);

	connect(trackingService().get(), &TrackingService::stateChanged, this, &StatusBar::resetToolManagerConnection);

	cx::TrackingServicePtr ts = cx::logicManager()->getTrackingService();
	mActiveTool = ActiveToolProxy::New(ts);
	connect(mActiveTool.get(), &ActiveToolProxy::tps, this, &StatusBar::tpsSlot);

	connect(trackingService().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(updateToolButtons()));

	connect(viewService().get(), SIGNAL(fps(int)), this, SLOT(renderingFpsSlot(int)));

	connect(videoService().get(), SIGNAL(fps(int)), this, SLOT(grabbingFpsSlot(int)));
	connect(videoService().get(), SIGNAL(connected(bool)), this, SLOT(grabberConnectedSlot(bool)));

	connect(vlc(), &VLCRecorder::stateChanged, this, &StatusBar::onRecordFullscreenChanged);

//	this->addPermanentWidget(mMessageLevelLabel);
	this->addPermanentWidget(mRenderingFpsLabel);
}

StatusBar::~StatusBar()
{
}

void StatusBar::resetToolManagerConnection()
{
	this->disconnectFromToolSignals();
	if (trackingService()->getState()>=Tool::tsCONFIGURED)
		this->connectToToolSignals();
	this->updateToolButtons();
}

void StatusBar::connectToToolSignals()
{
	this->disconnectFromToolSignals(); // avoid duplicates

	this->addPermanentWidget(mTpsLabel);
	mTpsLabel->show();

	TrackingService::ToolMap tools = trackingService()->getTools();
	for (TrackingService::ToolMap::iterator it = tools.begin(); it != tools.end(); ++it)
	{
		ToolPtr tool = it->second;
		if (tool->hasType(Tool::TOOL_MANUAL))
			continue;
		if (tool == trackingService()->getManualTool())
			continue;
		connect(tool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolButtons()));

		ToolData current;
		current.mTool = tool;
		current.mAction.reset(new QAction(tool->getName(), NULL));
		current.mAction->setToolTip("Press to set active");

		QtSignalAdapters::connect0<void()>(
			current.mAction.get(),
			SIGNAL(triggered()),
			boost::bind(&StatusBar::activateTool, this, tool->getUid()));

		current.mButton.reset(new QToolButton);
		current.mButton->setDefaultAction(current.mAction.get());
		this->addPermanentWidget(current.mButton.get());
		mToolData.push_back(current);
	}

	this->updateToolButtons();
}

void StatusBar::disconnectFromToolSignals()
{
	this->removeWidget(mTpsLabel);

	for (unsigned i = 0; i < mToolData.size(); ++i)
	{
		ToolData current = mToolData[i];

		disconnect(current.mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolButtons()));
		this->removeWidget(current.mButton.get());
	}
	mToolData.clear();
}


void StatusBar::activateTool(QString uid)
{
	trackingService()->setActiveTool(uid);
}

void StatusBar::updateToolButtons()
{
	ToolPtr activeTool = trackingService()->getActiveTool();

	for (unsigned i = 0; i < mToolData.size(); ++i)
	{
		ToolData current = mToolData[i];
		ToolPtr tool = current.mTool;
		QString color = this->getToolStyle(tool->getVisible(), tool->isInitialized(), activeTool == tool);
		current.mButton->setStyleSheet(QString("QToolButton { %1; }").arg(color));

		if (!tool->isInitialized())
			current.mAction->setToolTip("Tool is not Initialized");
		else if (activeTool == tool)
			current.mAction->setToolTip("Active Tool");
		else if (tool->getVisible())
			current.mAction->setToolTip("Tool not visible/not tracking");
		else
			current.mAction->setToolTip("Tool visible. Press to set as active");
	}
}

QString StatusBar::getToolStyle(bool visible, bool initialized, bool active)
{
	if (!initialized)
		return QString("background-color: silver");

	if (visible)
	{
		if (active)
			return QString("background-color: lime");
		else
			return QString("background-color: green");
	}

	return QString("background-color: orangered");
}

void StatusBar::renderingFpsSlot(int numFps)
{
	QString fpsString = "FPS: " + QString::number(numFps);
	mRenderingFpsLabel->setText(fpsString);
}

void StatusBar::tpsSlot(int numTps)
{
	QString tpsString = "TPS: " + QString::number(numTps);
	mTpsLabel->setText(tpsString);
}

void StatusBar::grabbingFpsSlot(int numFps)
{
	QString infoString = "VideoConnection-FPS: " + QString::number(numFps);
	mGrabbingInfoLabel->setText(infoString);
}

void StatusBar::grabberConnectedSlot(bool connected)
{
	if (connected)
	{
		this->addPermanentWidget(mGrabbingInfoLabel);
		mGrabbingInfoLabel->show();
	}
	else
		this->removeWidget(mGrabbingInfoLabel);
}

void StatusBar::onRecordFullscreenChanged()
{
	QLabel* label = mRecordFullscreenLabel;

	if (vlc()->isRecording())
	{
		label->setMargin(0);
		int size = this->height()*0.75; // fit within statusbar
		QPixmap map;
		map.load(":/icons/Video-icon_green.png");
		label->setPixmap(map.scaled(size, size, Qt::KeepAspectRatio));

		this->addPermanentWidget(mRecordFullscreenLabel);
		mRecordFullscreenLabel->show();
	}
	else
	{
		this->removeWidget(mRecordFullscreenLabel);
	}
}

void StatusBar::showMessageSlot(Message message)
{
	QString text = QString("[%1] %4")
			.arg(qstring_cast(message.getMessageLevel()))
			.arg(message.getText());

	this->showMessage(text, message.getTimeout());
}

}//namespace cx
