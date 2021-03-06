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


#include "cxVideoSourceSHM.h"

#include <boost/array.hpp>
#include <vector>

#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkPNGReader.h>


#include <qtimer.h>

namespace cx
{

VideoSourceSHM::VideoSourceSHM(int width, int height, int depth)
	: mImageWidth(width), mImageHeight(height), mImageColorDepth(depth), mImageImport(vtkImageImportPtr::New())
{
	mImportInitialized = false;
	mStartWhenConnected = false;

	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(3);

	mConnected = false;
	mStreaming = false;

	mPollTimer = new QTimer(this);
	mPollTimer->setInterval(40);	// Polling interval (currently set @ 25 fps)
	mTimeStamp = 0;


	mImageData = mImageImport->GetOutput();
}

VideoSourceSHM::~VideoSourceSHM()
{
	// Disconnect
	disconnectServer();
}

QString VideoSourceSHM::getUid()
{
	return mSource.key();
}

QString VideoSourceSHM::getName()
{
	return "SHMLink";
}

vtkImageDataPtr VideoSourceSHM::getVtkImageData()
{
	return mImageImport->GetOutput();
}

double VideoSourceSHM::getTimestamp()
{
	return (double) mSource.timestamp().toMSecsSinceEpoch();
}

/// Returns a short info message
QString VideoSourceSHM::getInfoString() const
{
	return "NOT YET IMPLEMENTED";
}

/// Returns a short status message
QString VideoSourceSHM::getStatusString() const
{
	return QString("%1 and %2").arg(isConnected() ? "Connected" : "Not connected").arg(isStreaming() ? "streaming" : "not streaming");;
}

void VideoSourceSHM::start()
{
	if (!isConnected())
	{
		mStartWhenConnected = true;
		return;
	}
	mStartWhenConnected = false;
	if (!mStreaming)
	{
		mPollTimer->start();
		// If all is well - tell the system we're streaming
		mStreaming = true;

		emit streaming(mStreaming);
	}
}

void VideoSourceSHM::stop()
{
	mStartWhenConnected = false;
	if (mStreaming)
	{
		mPollTimer->stop();
		// If all is well - tell the system we've stopped streaming
		mStreaming = false;

		emit streaming(mStreaming);
	}

}

bool VideoSourceSHM::validData() const
{
	// return (isConnected() && isStreaming()); // Currently only check available
	return isConnected() && mImportInitialized;
}

bool VideoSourceSHM::isConnected() const
{
	return mConnected;
}

bool VideoSourceSHM::isStreaming() const
{
	return (isConnected() && mStreaming);
}

/**
 * Grabs latest video source frame (if it exists) and signals a new frame
 * Must call release on this when buffer no longer is needed
 */
void VideoSourceSHM::update()
{
	unsigned char* buffer = (unsigned char*) mSource.isNew(); // Fetch new data from server - NULL if no new data present
	if (!buffer)
		return;

	mTimeStamp = mSource.timestamp().currentMSecsSinceEpoch();

	int numChannels = mImageColorDepth/sizeof(uchar);
	Q_UNUSED(numChannels);

	mImageImport->SetWholeExtent(0, mImageWidth - 1, 0, mImageHeight - 1, 0, 0);
	mImageImport->SetDataExtentToWholeExtent();

	mImageImport->SetImportVoidPointer(buffer);
	mImageImport->Update();
	mImportInitialized = true;

	emit newFrame();
}

/**
 * Connects to a shared memory server end, described by a unique key string.
 * Connects signals and slots on success.
 */
void VideoSourceSHM::connectServer(const QString& key)
{
	mConnected = mSource.attach(key);

	if (mConnected)
	{
		connect(mPollTimer, SIGNAL(timeout()), this, SLOT(serverPollSlot()));
		serverPollSlot(); // Pull in a new frame here, even if we may no be started yet to initialize the image import
	}
	if (mStartWhenConnected)
	{
		start();
	}
}

/**
 * Disconnects from current shared memory server end.
 * Disconnects signals and slots.
 */
void VideoSourceSHM::disconnectServer()
{
	stop();

	if (mConnected)
	{
		disconnect(mPollTimer, SIGNAL(timeout()), this, SLOT(serverPollSlot()));
		mSource.release();
	}

	mSource.detach();
	mConnected = false;
}

/**
 * Slot: calls update on this
 */
void VideoSourceSHM::serverPollSlot()
{
	this->update();
}

void VideoSourceSHM::setResolution(double resolution)
{
	mImageImport->SetDataSpacing(resolution, resolution, 1);
}
} // end namespace
