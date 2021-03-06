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

#ifndef CXSLICECOMPUTER_H_
#define CXSLICECOMPUTER_H_

#include "cxResourceExport.h"

#include "cxTransform3D.h"
#include "cxDefinitions.h"

namespace cx
{

/**\brief A 2D slice plane in 3D.
 * i,j are perpendicular unit vectors.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT SlicePlane
{
public:
	SlicePlane() {}
	SlicePlane(const Vector3D& c, const Vector3D& i, const Vector3D& j);
	Vector3D c; ///< defines the center of the plane
	Vector3D i; ///< defines the first axis of the plane. unit vector
	Vector3D j; ///< defines the second axis of the plane. unit vector
};

cxResource_EXPORT std::ostream& operator<<(std::ostream& s, const SlicePlane& val);
cxResource_EXPORT bool similar(const SlicePlane& a, const SlicePlane& b);


/**\brief Calculates a slice plane given a definition.
 * 
 * SliceComputer is set up with what image to slice and the tool to use,
 * along with what method that defines the slicing. The input produces a 
 * slice plane defined by the i,j basis vectors spanning the plane, and a 
 * center c defining the center. 
 * 
 * Most methods set the computer state, while getPlane() does all the computing.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT SliceComputer
{
public:
	SliceComputer();
	~SliceComputer();
	void setToolPosition(const Transform3D& rMt);
	void setOrientationType(ORIENTATION_TYPE val);
	void setPlaneType(PLANE_TYPE val);
	void setFixedCenter(const Vector3D& center); 
	void setFollowType(FOLLOW_TYPE val); 
	void setGravity(bool use, const Vector3D& dir);
	void setToolOffset(double val); 
	void setToolViewOffset(bool use, double viewportHeight, double viewOffset, bool useConstrainedViewOffset = false);
	void setToolViewportHeight(double viewportHeight);
	void setClinicalApplication(CLINICAL_VIEW application);
	SlicePlane getPlane() const;

	void initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset, CLINICAL_VIEW application, bool useConstrainedViewOffset = false);
	ORIENTATION_TYPE getOrientationType() const;
	PLANE_TYPE getPlaneType() const;
	Transform3D getToolPosition() const;

private:
	std::pair<Vector3D,Vector3D> generateBasisVectors() const;
	Vector3D generateFixedIJCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const;
	SlicePlane orientToGravity(const SlicePlane& base) const;
	SlicePlane applyViewOffset(const SlicePlane& base) const;
	double getViewOffsetAbsoluteFromCenter() const;

	std::pair<Vector3D,Vector3D> generateBasisVectorsNeurology() const;
	std::pair<Vector3D,Vector3D> generateBasisVectorsRadiology() const;

private:
	CLINICAL_VIEW mClinicalApplication;
	ORIENTATION_TYPE mOrientType;
	PLANE_TYPE mPlaneType;
	FOLLOW_TYPE mFollowType;
	Vector3D mFixedCenter;

	Transform3D m_rMt;
	double mToolOffset;

	bool mUseGravity;
	Vector3D mGravityDirection; 

	bool mUseViewOffset;
	double mViewportHeight;
	double mViewOffset;
	bool mUseConstrainedViewOffset;
};

} // namespace cx

#endif /*CXSLICECOMPUTER_H_*/
