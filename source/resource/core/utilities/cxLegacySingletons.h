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
#ifndef CXLEGACYSINGLETONS_H
#define CXLEGACYSINGLETONS_H

#include "boost/shared_ptr.hpp"

namespace cx
{
class DataManager;
class ToolManager;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/** 
 *
 *
 * \ingroup cx
 * \date 23.02.2014, 2014
 * \author christiana
 */
DataManager* dataManager();
ToolManager* toolManager();
SpaceProviderPtr spaceProvider();

} // namespace cx


#endif // CXLEGACYSINGLETONS_H
