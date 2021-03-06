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

#include "cxUsReconstructionFileMaker.h"

#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "vtkImageAppend.h"
#include "vtkMetaImageWriter.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxSettings.h"
#include "cxXmlOptionItem.h"
#include "cxTimeKeeper.h"
#include "cxDataReaderWriter.h"
#include "cxUSFrameData.h"
#include "cxSavingVideoRecorder.h"
#include "cxImageDataContainer.h"
#include "cxUSReconstructInputDataAlgoritms.h"
#include "cxCustomMetaImage.h"


typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace cx
{

UsReconstructionFileMaker::UsReconstructionFileMaker(QString sessionDescription) :
    mSessionDescription(sessionDescription)
{
}

UsReconstructionFileMaker::~UsReconstructionFileMaker()
{
}

USReconstructInputData UsReconstructionFileMaker::getReconstructData()
{
	return mReconstructData;
}

/**Create and return the structure that would have been read by UsReconstructFileReader,
 * if written from this object.
 *
 */
USReconstructInputData UsReconstructionFileMaker::getReconstructData(ImageDataContainerPtr imageData,
                                                                          std::vector<double> imageTimestamps,
                                                                          TimedTransformMap trackerRecordedData,
                                                                          ToolPtr tool,
                                                                          bool writeColor, Transform3D rMpr)
{
	if(trackerRecordedData.empty())
		reportWarning("No tracking data for writing to reconstruction file.");

	USReconstructInputData retval;

	retval.mFilename = mSessionDescription; // not saved yet - no filename
	retval.mUsRaw = USFrameData::create(mSessionDescription, imageData);
	retval.rMpr = rMpr;

	for (TimedTransformMap::iterator it = trackerRecordedData.begin(); it != trackerRecordedData.end(); ++it)
	{
		TimedPosition current;
		current.mTime = it->first;
		current.mPos = it->second;
		retval.mPositions.push_back(current);
	}

	std::vector<double> fts = imageTimestamps;
	for (unsigned i=0; i<fts.size(); ++i)
	{
		TimedPosition current;
		current.mTime = fts[i];
		current.mPos = Transform3D::Identity();
		// current.mPos = not written - will be found from track positions during reconstruction.
		retval.mFrames.push_back(current);
	}

	if (tool && tool->getProbe())
	{
		retval.mProbeDefinition.setData(tool->getProbe()->getProbeDefinition());
	}

	if (tool)
		retval.mProbeUid = tool->getUid();

	this->fillFramePositions(&retval);

	return retval;
}

/** Use tool positions to generate positions for each frame, then convert the
  * position format from prMt to rMu. This makes is possible to use the frames
  * externally.
  */
void UsReconstructionFileMaker::fillFramePositions(USReconstructInputData* data) const
{
	cx::USReconstructInputDataAlgorithm::interpolateFramePositionsFromTracking(data);
	cx::USReconstructInputDataAlgorithm::transformFramePositionsTo_rMu(data);
}

bool UsReconstructionFileMaker::writeTrackerTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTimestamps(reconstructionFolder+"/"+session+".tts",
								 ts, "tracking timestamps");
}

bool UsReconstructionFileMaker::writeUSTimestamps(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTimestamps(reconstructionFolder+"/"+session+".fts",
								 ts, "frame timestamps");
}

bool UsReconstructionFileMaker::writeTimestamps(QString filename, std::vector<TimedPosition> ts, QString type)
{
	bool success = false;

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return success;
	}
	QTextStream stream(&file);

	for (unsigned i=0; i<ts.size(); ++i)
	{
		stream << qstring_cast(ts[i].mTime);
		stream << endl;
	}
	file.close();
	success = true;

	QFileInfo info(file);
	mReport << QString("%1, %2 bytes, %3 %4.")
			   .arg(info.fileName())
			   .arg(info.size())
			   .arg(ts.size())
			   .arg(type);
//	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" frame timestamps.";

	return success;
}

bool UsReconstructionFileMaker::writeUSTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".fp", ts, "frame transforms rMu");
}

bool UsReconstructionFileMaker::writeTrackerTransforms(QString reconstructionFolder, QString session, std::vector<TimedPosition> ts)
{
	return this->writeTransforms(reconstructionFolder+"/"+session+".tp", ts, "tracking transforms prMt");
}

bool UsReconstructionFileMaker::writeTransforms(QString filename, std::vector<TimedPosition> ts, QString type)
{
	bool success = false;
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return success;
	}
	QTextStream stream(&file);

	for (unsigned i=0; i<ts.size(); ++i)
	{
		Transform3D transform = ts[i].mPos;
		stream << transform(0,0) << " ";
		stream << transform(0,1) << " ";
		stream << transform(0,2) << " ";
		stream << transform(0,3);
		stream << endl;
		stream << transform(1,0) << " ";
		stream << transform(1,1) << " ";
		stream << transform(1,2) << " ";
		stream << transform(1,3);
		stream << endl;
		stream << transform(2,0) << " ";
		stream << transform(2,1) << " ";
		stream << transform(2,2) << " ";
		stream << transform(2,3);
		stream << endl;
	}
	file.close();
	success = true;

	QFileInfo info(file);
	mReport << info.fileName()+", "+qstring_cast(info.size())+" bytes, "+qstring_cast(ts.size())+" " + type + ".";

	return success;
}

/**
 * Write probe configuration to file. This works even for configs not saved to the ProbeCalibConfigs.xml file.
 */
void UsReconstructionFileMaker::writeProbeConfiguration(QString reconstructionFolder, QString session, ProbeDefinition data, QString uid)
{
	XmlOptionFile file = XmlOptionFile(reconstructionFolder + "/" + session + ".probedata.xml");
	data.addXml(file.getElement("configuration"));
	file.getElement("tool").toElement().setAttribute("toolID", uid);
	file.save();
}

QString UsReconstructionFileMaker::createUniqueFolder(QString patientFolder, QString sessionDescription)
{
	QString retval("");
	QDir patientDir(patientFolder + "/US_Acq");

	QString subfolder = sessionDescription;
	QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
	QString newPathName = subfolderAbsolutePath;
	int i=1;
	while(!findNewSubfolder(newPathName))
	{
		newPathName = subfolderAbsolutePath+"_"+QString::number(i++);
	}
	patientDir.mkpath(newPathName);
	patientDir.cd(newPathName);

	retval = patientDir.absolutePath();
	return retval;
}

QString UsReconstructionFileMaker::createFolder(QString patientFolder, QString sessionDescription)
{
	QString retval("");
	QDir patientDir(patientFolder + "/US_Acq");

	QString subfolder = sessionDescription;
	QString subfolderAbsolutePath = patientDir.absolutePath()+"/"+subfolder;
	QString newPathName = subfolderAbsolutePath;
	patientDir.mkpath(newPathName);
	patientDir.cd(newPathName);

	retval = patientDir.absolutePath();
	return retval;
}

bool UsReconstructionFileMaker::findNewSubfolder(QString subfolderAbsolutePath)
{
	QDir dir;
	if(dir.exists(subfolderAbsolutePath))
		return false;

	return true;
}

void UsReconstructionFileMaker::report()
{
	foreach(QString string, mReport)
	{
		reportSuccess(string);
	}
}

void UsReconstructionFileMaker::writeUSImages(QString path, ImageDataContainerPtr images, bool compression, std::vector<TimedPosition> pos)
{
	CX_ASSERT(images->size()==pos.size());
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

	for (unsigned i=0; i<images->size(); ++i)
	{
		vtkImageDataPtr currentImage = images->get(i);
		QString filename = QString("%1/%2_%3.mhd").arg(path).arg(mSessionDescription).arg(i);

		writer->SetInputData(currentImage);
		writer->SetFileName(cstring_cast(filename));
		writer->SetCompression(compression);
		{
			StaticMutexVtkLocker lock;
			writer->Write();
		}

		CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
		customReader->setTransform(pos[i].mPos);
		customReader->setModality("US");
		customReader->setImageType(mSessionDescription);
	}
}

void UsReconstructionFileMaker::writeMask(QString path, QString session, vtkImageDataPtr mask)
{
	QString filename = QString("%1/%2.mask.mhd").arg(path).arg(session);
	if (!mask)
	{
		reportWarning(QString("No mask found, ignoring write to %1").arg(filename));
		return;
	}

	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInputData(mask);
	writer->SetFileName(cstring_cast(filename));
	writer->SetCompression(false);
	writer->Write();
}


void UsReconstructionFileMaker::writeREADMEFile(QString reconstructionFolder, QString session)
{
	QString text = ""
"* ==== Format description												\n"
"* Refer to 																		    \n"
"*		http://custusx.org/uploads/developer_doc/nightly/org_custusx_acquisition.html   \n"
"* for a description of the files.                                                      \n"
"*/																		\n";

	QFile file(reconstructionFolder+"/"+session+".README.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportError("Cannot open "+file.fileName());
		return;
	}
	QTextStream stream(&file);
	stream << text;
	file.close();
}

QString UsReconstructionFileMaker::writeToNewFolder(QString path, bool compression)
{
	TimeKeeper timer;
	mReconstructData.mFilename = path+"/"+mSessionDescription+".fts"; // use fts since this is a single unique file.

	mReport.clear();
	mReport << "Made reconstruction folder: " + path;
	QString session = mSessionDescription;

	this->writeTrackerTimestamps(path, session, mReconstructData.mPositions);
	this->writeTrackerTransforms(path, session, mReconstructData.mPositions);
	this->writeUSTimestamps(path, session, mReconstructData.mFrames);
	this->writeUSTransforms(path, session, mReconstructData.mFrames);
	this->writeProbeConfiguration(path, session, mReconstructData.mProbeDefinition.mData, mReconstructData.mProbeUid);
	this->writeMask(path, session, mReconstructData.getMask());
	this->writeREADMEFile(path, session);

	ImageDataContainerPtr imageData = mReconstructData.mUsRaw->getImageContainer();
	if (imageData)
		this->writeUSImages(path, imageData, compression, mReconstructData.mFrames);
	else
		mReport << "failed to find frame data, save failed.";

	int time = std::max(1, timer.getElapsedms());
	mReport << QString("Completed save to %1. Spent %2s, %3fps").arg(mSessionDescription).arg(time/1000).arg(imageData->size()*1000/time);

	this->report();
	mReport.clear();

	return mReconstructData.mFilename;
}



}//namespace cx
