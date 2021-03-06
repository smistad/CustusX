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

#include "cxToolImpl.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

ToolImpl::ToolImpl(const QString& uid, const QString& name) :
	Tool(uid, name),
	mPositionHistory(new TimedTransformMap()),
	m_prMt(Transform3D::Identity()),
	mTooltipOffset(0)
{
}

ToolImpl::~ToolImpl()
{

}

double ToolImpl::getTooltipOffset() const
{
	return mTooltipOffset;
}

void ToolImpl::setTooltipOffset(double val)
{
	if (similar(val, mTooltipOffset))
		return;
	mTooltipOffset = val;
	emit tooltipOffset(mTooltipOffset);
}

TimedTransformMapPtr ToolImpl::getPositionHistory()
{
	return mPositionHistory;
}

TimedTransformMap ToolImpl::getSessionHistory(double startTime, double stopTime)
{
	TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
	TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

	TimedTransformMap retval(startIt, stopIt);
	return retval;
}

Transform3D ToolImpl::get_prMt() const
{
	return m_prMt;
}

void ToolImpl::set_prMt(const Transform3D& prMt, double timestamp)
{
	if (mPositionHistory->count(timestamp))
	{
		if (similar(mPositionHistory->find(timestamp)->second, prMt))
			return;
	}

	m_prMt = prMt;
	(*mPositionHistory)[timestamp] = m_prMt;
	emit toolTransformAndTimestamp(m_prMt, timestamp);
}

void ToolImpl::resetTrackingPositionFilter(TrackingPositionFilterPtr filter)
{
    mTrackingPositionFilter = filter;
}

} // namespace cx

