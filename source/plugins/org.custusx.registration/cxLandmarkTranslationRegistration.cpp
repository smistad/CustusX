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
#include "cxLandmarkTranslationRegistration.h"

/*=========================================================================

 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: IterativeClosestPoint1.cxx,v $
 Language:  C++
 Date:      $Date: 2009-12-04 03:25:22 $
 Version:   $Revision: 1.7 $

 Copyright (c) Insight Software Consortium. All rights reserved.
 See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

// Software Guide : BeginLatex
//
// This example illustrates how to perform Iterative Closest Point (ICP)
// registration in ITK. The main class featured in this section is the
// \doxygen{EuclideanDistancePointMetric}.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkTranslationTransform.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
// Software Guide : EndCodeSnippet

#include <iostream>
#include <fstream>

class CommandIterationUpdate: public itk::Command
{
public:
	typedef CommandIterationUpdate Self;
	typedef itk::Command Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	itkNewMacro( Self );

protected:
	CommandIterationUpdate() {}

public:
	typedef itk::LevenbergMarquardtOptimizer OptimizerType;
	typedef const OptimizerType * OptimizerPointer;

	void Execute(itk::Object *caller, const itk::EventObject & event)
	{
		Execute((const itk::Object *) caller, event);
	}

	void Execute(const itk::Object * object, const itk::EventObject & event)
	{
		OptimizerPointer optimizer = dynamic_cast<OptimizerPointer>(object);

		if (!itk::IterationEvent().CheckEvent(&event))
		{
			return;
		}

		std::cout << "iteration Value = " << optimizer->GetCachedValue() << std::endl;
		std::cout << "iteration Position = " << optimizer->GetCachedCurrentPosition();
		std::cout << std::endl << std::endl;

	}

};

class LandmarkTranslation
{
public:
	static const unsigned int Dimension = 3;

	typedef itk::PointSet<float, Dimension> PointSetType;

//  PointSetType::Pointer fixedPointSet  = PointSetType::New();
//  PointSetType::Pointer movingPointSet = PointSetType::New();

	typedef PointSetType::PointType PointType;
	typedef PointSetType::PointsContainer PointsContainer;

	//-----------------------------------------------------------
	// Set up  the Metric
	//-----------------------------------------------------------
	typedef itk::EuclideanDistancePointMetric<PointSetType, PointSetType> MetricType;

	typedef MetricType::TransformType TransformBaseType;
	typedef TransformBaseType::ParametersType ParametersType;
	typedef TransformBaseType::JacobianType JacobianType;

	//-----------------------------------------------------------
	// Set up a Transform
	//-----------------------------------------------------------

	typedef itk::TranslationTransform<double, Dimension> TransformType;

	// Optimizer Type
	typedef itk::LevenbergMarquardtOptimizer OptimizerType;

	// Registration Method
	typedef itk::PointSetToPointSetRegistrationMethod<PointSetType, PointSetType> RegistrationType;

	// methods:
	cx::Transform3D mResult; ///< transform of movingPointSet

	bool registerPoints(std::vector<cx::Vector3D> ref, std::vector<cx::Vector3D> target);
	bool registerPoints(PointSetType::Pointer fixedPointSet, PointSetType::Pointer movingPointSet);
	PointSetType::Pointer toItk(std::vector<cx::Vector3D> input);
	int test();

};

LandmarkTranslation::PointSetType::Pointer LandmarkTranslation::toItk(std::vector<cx::Vector3D> input)
{
	PointSetType::Pointer pointSet = PointSetType::New();
	PointsContainer::Pointer pointContainer = PointsContainer::New();
	PointType point;

	for (unsigned i = 0; i < input.size(); ++i)
	{
		point[0] = input[i][0];
		point[1] = input[i][1];
		point[2] = input[i][2];
		pointContainer->InsertElement(i, point);
	}
	pointSet->SetPoints(pointContainer);
	return pointSet;
}

bool LandmarkTranslation::registerPoints(std::vector<cx::Vector3D> ref, std::vector<cx::Vector3D> target)
{
	return registerPoints(toItk(ref), toItk(target));
}

bool LandmarkTranslation::registerPoints(PointSetType::Pointer fixedPointSet, PointSetType::Pointer movingPointSet)
{

//-----------------------------------------------------------
// Set up  the Metric
//-----------------------------------------------------------
	MetricType::Pointer metric = MetricType::New();

//-----------------------------------------------------------
// Set up a Transform
//-----------------------------------------------------------

	TransformType::Pointer transform = TransformType::New();

	// Optimizer Type
	OptimizerType::Pointer optimizer = OptimizerType::New();
	optimizer->SetUseCostFunctionGradient(false);

	// Registration Method
	RegistrationType::Pointer registration = RegistrationType::New();

	// Scale the translation components of the Transform in the Optimizer
	OptimizerType::ScalesType scales(transform->GetNumberOfParameters());
	std::cout << "transform->GetNumberOfParameters() " << transform->GetNumberOfParameters() << std::endl;
	scales.Fill(0.01);

	unsigned long numberOfIterations = 100;
	double gradientTolerance = 1e-5; // convergence criterion
	double valueTolerance = 1e-5; // convergence criterion
	double epsilonFunction = 1e-6; // convergence criterion

	optimizer->SetScales(scales);
	optimizer->SetNumberOfIterations(numberOfIterations);
	optimizer->SetValueTolerance(valueTolerance);
	optimizer->SetGradientTolerance(gradientTolerance);
	optimizer->SetEpsilonFunction(epsilonFunction);

	// Start from an Identity transform (in a normal case, the user
	// can probably provide a better guess than the identity...
	transform->SetIdentity();

	registration->SetInitialTransformParameters(transform->GetParameters());

	//------------------------------------------------------
	// Connect all the components required for Registration
	//------------------------------------------------------
	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);
	registration->SetTransform(transform);
	registration->SetFixedPointSet(fixedPointSet);
	registration->SetMovingPointSet(movingPointSet);

//  // Connect an observer
//  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
//  optimizer->AddObserver( itk::IterationEvent(), observer );

	try
	{
//		registration->StartRegistration();
		registration->Update();
	} catch (itk::ExceptionObject & e)
	{
		std::cout << e << std::endl;
		return false;
	}

	mResult = cx::Transform3D::Identity();
	for (unsigned i = 0; i < transform->GetNumberOfParameters(); ++i)
		mResult(i, 3) = transform->GetParameters()[i];

	//std::cout << "Solution = " << transform->GetParameters() << "\n" << mResult << std::endl;
	//std::cout << "Solution = " << registration->GetLastTransformParameters() << std::endl;

	return true;

}

namespace cx
{

/**
 * Creates a transform that moves target into ref space. (targetMref)
 * @param ref
 * @param target
 * @param ok
 * @return
 */
Transform3D LandmarkTranslationRegistration::registerPoints(std::vector<Vector3D> ref,
				std::vector<Vector3D> target, bool* ok)
{
	if (ref.size() != target.size() || ref.empty())
	{
		std::cout << "Different sizes in ref and target: aborting registration." << std::endl;
		*ok = false;
		return Transform3D::Identity();
	}

	// ad-hoc solution for one and two points: itk doesn't handle this for some reason.
	if (ref.size() == 1)
	{
		Vector3D t = ref[0] - target[0];
		*ok = true;
		return createTransformTranslate(t);
	}
	if (ref.size() == 2)
	{
		Vector3D rr = (ref[0] + ref[1]) / 2.0;
		Vector3D tt = (target[0] + target[1]) / 2.0;
		Vector3D t = rr - tt;
		*ok = true;
		return createTransformTranslate(t);
	}

	LandmarkTranslation registrator;
	*ok = registrator.registerPoints(ref, target);
	return registrator.mResult;
}

} // namespace cx
