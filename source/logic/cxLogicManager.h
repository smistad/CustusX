/*
 * cxLogicManager.h
 *
 *  \date Jun 1, 2011
 *      \author christiana
 */

#ifndef CXLOGICMANAGER_H_
#define CXLOGICMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <QObject>

namespace cx
{
/**
* \file
* \addtogroup cxLogic
* @{
*/

typedef boost::shared_ptr<class ServiceController> ServiceControllerPtr;

/**\brief Responsible for the entire logic layer.
 * \ingroup cxLogic
 *
 * All controllers in this layers are embedded in this class.
 * The service layer are managed via the ServiceController aggregate member.
 *
 */
class LogicManager : public QObject
{
	Q_OBJECT
public:
  static LogicManager* getInstance();

  static void initialize();
  static void shutdown();

//  ServiceControllerPtr getServiceController();

private:
  static LogicManager* mInstance;
  static void setInstance(LogicManager* instance);

  LogicManager();
	virtual ~LogicManager();

	LogicManager(LogicManager const&); // not implemented
	LogicManager& operator=(LogicManager const&); // not implemented

	ServiceControllerPtr mServiceController;
};

//LogicManager* logicManager(); // if necessary

/**
* @}
*/
}

#endif /* CXLOGICMANAGER_H_ */
