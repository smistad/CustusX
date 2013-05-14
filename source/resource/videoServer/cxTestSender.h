#ifndef CXTESTSENDER_H_
#define CXTESTSENDER_H_

#include "cxGrabberSender.h"
#include <boost/shared_ptr.hpp>

namespace cxtest {

/*
 * TestSender.h
 *
 * \date May 13, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TestSender : public cx::Sender
{
	Q_OBJECT
public:
	TestSender() {}
	virtual ~TestSender() {}

	virtual bool isReady() const;

	virtual void send(cx::PackagePtr package);

	cx::PackagePtr getPackage();

signals:
	void newPackage();

private:
	cx::PackagePtr mPackage;

};
typedef boost::shared_ptr<TestSender> TestSenderPtr;

} /* namespace cxtest */
#endif /* CXTESTSENDER_H_ */
