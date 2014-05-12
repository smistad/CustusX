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

#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "cxDataManager.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include "cxDataLocations.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include <QFileInfo>
#include <QDir>
#include "cxXmlOptionItem.h"

#include "catch.hpp"

#include "cxLogger.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxElastixExecuter.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "cxElastixSingleThreadedRunner.h"
#include "cxTypeConversions.h"
#include "cxElastixParameters.h"
#include "cxLogicManager.h"

namespace cxtest
{

/** 
 *
 *
 * \ingroup cx
 * \date 6/7/2013, 2013
 * \author christiana
 */

class ElastiXFixture
{
public:
	ElastiXFixture()
	{
//		cx::Reporter::initialize();
//		cx::cxDataManager::initialize();
		cx::LogicManager::initialize();
	}

	~ElastiXFixture()
	{
		cx::LogicManager::shutdown();
//		cx::DataManager::shutdown();
//		cx::Reporter::shutdown();
	}
	bool compareTransforms(cx::Transform3D result, cx::Transform3D solution)
	{
		std::cout << "result\n" << result << std::endl;
		std::cout << "solution\n" << solution << std::endl;

		cx::Transform3D diff = solution * result.inv();

		std::cout << "diff\n" << diff << std::endl;

		cx::Vector3D t_delta = diff.matrix().block<3, 1>(0, 3);
		Eigen::AngleAxisd angleAxis = Eigen::AngleAxisd(diff.matrix().block<3, 3>(0, 0));
		double angle = angleAxis.angle();

		cx::Vector3D shift = diff.coord(cx::Vector3D(0,0,0));


		QString res = QString(""
			"Shift vector (r):\t%1\n"
			"Accuracy |v|:\t%2mm\n"
			"Angle:       \t%3*\n"
			"")
			.arg(qstring_cast(shift))
			.arg(shift.length(), 6, 'f', 2)
			.arg(angle / M_PI * 180.0, 6, 'f', 2);

		std::cout << res << std::endl;

		return (fabs(angle/M_PI*180.0) < 0.1) && (shift.length() < 0.1);
	}
};



TEST_CASE("ElastiX should register kaisa to a translated+resampled version of same", "[pluginRegistration][integration][not_win32][not_win64]")
{
	ElastiXFixture fixture;

	QString kaisa_padded_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_padded.mhd";
	QString kaisa_resliced_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_resliced.mhd";
	QString kaisa_resliced_linear_fname = cx::DataLocations::getTestDataPath() + "/testing/elastiX/kaisa_resliced_linear.mhd";

	std::cout << "------" << kaisa_padded_fname << std::endl;
	cx::DataPtr kaisa_resliced_linear = cx::dataManager()->loadData("source_"+kaisa_resliced_linear_fname, kaisa_resliced_linear_fname);
	cx::DataPtr kaisa_padded = cx::dataManager()->loadData("source_"+kaisa_padded_fname, kaisa_padded_fname);
	cx::DataPtr kaisa_resliced = cx::dataManager()->loadData("source_"+kaisa_resliced_fname, kaisa_resliced_fname);

	REQUIRE(kaisa_resliced_linear.get());
	REQUIRE(kaisa_padded.get());
	QString elastixPreset = "elastix/p_Rigid"; // this is an autogenerated preset

	cx::Transform3D solution;
	solution.matrix() <<
				1, 0 ,0, 10,
				0, 1, 0,  5,
				0, 0, 1, -2,
				0, 0, 0,  1;

	cx::Transform3D result = cx::Transform3D::Identity();

	cx::ElastixParametersPtr parameters(new cx::ElastixParameters(cx::XmlOptionFile()));
	parameters->getCurrentPreset()->setValue(elastixPreset);

	cx::ElastixSingleThreadedRunner runner;
	REQUIRE(runner.registerLinear(kaisa_padded, kaisa_resliced_linear, parameters, &result));

	REQUIRE(fixture.compareTransforms(result, solution) == true);
}


} // namespace cxtest

