// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#include "cxtestProbeFixture.h"

#include <QString>
#include "sscMessageManager.h"
#include "ProbeXmlConfigParserMock.h"
#include "sscProbeData.h"
#include "sscTestVideoSource.h"

#include "catch.hpp"

namespace cxtest
{

ProbeFixture::ProbeFixture()
{
	// this stuff will be performed just before all tests in this class
	cx::MessageManager::initialize();
	this->createTestProbe();
}


ProbeFixture::~ProbeFixture()
{
	// this stuff will be performed just after all tests in this class
	cx::MessageManager::shutdown();
}


void ProbeFixture::createTestProbe()
{
	this->createParameters();
	QString xmlFileName = "testXmlFileName";
	ProbeXmlConfigParserPtr mXml;
	mXml.reset(new ProbeXmlConfigParserMock(xmlFileName));
	mDefaultRtSourceName = mXml->getRtSourceList(mProbeName, mScannerName)[0];
	mProbe = cx::cxProbe::New(mProbeName, mScannerName, mXml);
}

void ProbeFixture::createParameters()
{
	mProbeName = "TestProbe";
	mScannerName = "TestScanner";
	cx::ProbeData probeData;
	mDefaultProbeDataUid = probeData.getUid(); //Uid set to "default" in ProbeData()
	mDefaultTemporalCalibration = probeData.getTemporalCalibration();
	mProbeDataUid = "TestProbeData";
}

cx::ProbeData ProbeFixture::createProbeData()
{
	cx::ProbeData probeData = mProbe->getProbeData();
	probeData.setUid(mProbeDataUid);
	mTemporalCalibration = 1000.5;
	probeData.setTemporalCalibration(mTemporalCalibration);
	return probeData;
}

} //namespace cxtest