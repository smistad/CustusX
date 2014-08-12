#include "cxTestSenderController.h"

#include "catch.hpp"

TestSenderController::TestSenderController(QObject* parent) :
	QObject(parent),
	mImageReceived(false),
	mStatusReceived(false)
{
}

void TestSenderController::initialize(cx::DirectlyLinkedSenderPtr grabberBridge)
{
	mGrabberBridge = grabberBridge;
	connect(mGrabberBridge.get(), SIGNAL(newImage()), this, SLOT(newImageSlot()), Qt::DirectConnection);
	connect(mGrabberBridge.get(), SIGNAL(newUSStatus()), this, SLOT(newUSStatusSlot()), Qt::DirectConnection);

}

bool TestSenderController::verify()
{
	CHECK(mImageReceived);
	CHECK(mStatusReceived);
	return mImageReceived && mStatusReceived;
}

void TestSenderController::newImageSlot()
{
//	this->addImageToQueue(mGrabberBridge->popImage());
//	std::cout << "TestGrabberSenderController::newImageSlot()" << std::endl;
	mImageReceived = true;
}

void TestSenderController::newUSStatusSlot()
{
//	this->addSonixStatusToQueue(mGrabberBridge->popUSStatus());
//	std::cout << "TestGrabberSenderController::newUSStatusSlot()" << std::endl;
	mStatusReceived = true;
}