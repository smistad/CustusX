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


#include "cxElastixExecuter.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include "sscMessageManager.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscTypeConversions.h"
#include "sscTime.h"

namespace cx
{

ElastixExecuter::ElastixExecuter(QObject* parent) : TimedBaseAlgorithm("ElastiX", 1000)
{
	mProcess = new QProcess(this);
	connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}

ElastixExecuter::~ElastixExecuter()
{
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	mProcess->close();
}

void ElastixExecuter::setDisplayProcessMessages(bool on)
{
	disconnect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

	if (on)
	{
		connect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
		//	mProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		mProcess->setProcessChannelMode(QProcess::MergedChannels);
		mProcess->setReadChannel(QProcess::StandardOutput);
	}
}


void ElastixExecuter::run(QString application,
				ssc::ImagePtr fixed,
				ssc::ImagePtr moving,
				QString outdir,
				QStringList parameterfiles)
{
	if (!fixed || !moving)
	{
		ssc::messageManager()->sendWarning("Failed to start elastiX registration, fixed or missing image missing.");
		return;
	}

	if (mProcess->state() != QProcess::NotRunning)
	{
		ssc::messageManager()->sendWarning("Failed to start elastiX registration, process already running");
		return;
	}

	// create output dir (required by ElastiX)
	QDir().mkpath(outdir);

	QString initFilename = this->writeInitTransformToElastixfile(fixed, moving, outdir);
	this->writeInitTransformToCalfile(fixed, moving, outdir);

	mLastOutdir = outdir;

	QStringList cmd;
	cmd << application;
	cmd << "-f" << patientService()->getPatientData()->getActivePatientFolder()+"/"+fixed->getFilePath();
	cmd << "-m" << patientService()->getPatientData()->getActivePatientFolder()+"/"+moving->getFilePath();
	cmd << "-out" << outdir;
	cmd << "-t0" << initFilename;
	for (int i=0; i<parameterfiles.size(); ++i)
		cmd << "-p" << parameterfiles[i];

	QString commandLine = cmd.join(" ");
	ssc::messageManager()->sendInfo(QString("Executing registration with command line: [%1]").arg(commandLine));

	mProcess->start(commandLine);
//	mProcess->start("elastix");
}

/**Class encapsulating the math conversions
 * between the ElastiX "EulerTransform" representation
 * and the ssc::Transform3D representation.
 */
class ElastixEulerTransform
{
public:
	ssc::Vector3D mAngles_xyz;
	ssc::Vector3D mTranslation;
	ssc::Vector3D mCenterOfRotation;

	static ElastixEulerTransform create(ssc::Vector3D angles_xyz, ssc::Vector3D translation, ssc::Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = angles_xyz;
		retval.mTranslation = translation;
		retval.mCenterOfRotation = centerOfRotation;
		return retval;
	}
	static ElastixEulerTransform create(ssc::Transform3D M, ssc::Vector3D centerOfRotation)
	{
		ElastixEulerTransform retval;
		retval.mAngles_xyz = M.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
		retval.mTranslation = M.matrix().block<3, 1> (0, 3);
//		retval.mCenterOfRotation = ssc::Vector3D(0,0,0);
		retval.mCenterOfRotation = centerOfRotation;
		return retval;
	}
	ssc::Transform3D toMatrix() const
	{
		ssc::Transform3D T = ssc::createTransformTranslate(mTranslation);
		ssc::Transform3D C = ssc::createTransformTranslate(mCenterOfRotation);

		ssc::Transform3D Rx = ssc::createTransformRotateX(mAngles_xyz[0]);
		ssc::Transform3D Ry = ssc::createTransformRotateY(mAngles_xyz[1]);
		ssc::Transform3D Rz = ssc::createTransformRotateZ(mAngles_xyz[2]);

		ssc::Transform3D R = Rx*Ry*Rz;
	//	ssc::Transform3D R = Rz*Ry*Rx;

		ssc::Transform3D Q = T*C*R*C.inv();
		return Q;
	}
private:
	ElastixEulerTransform() {}
};

void test_euler()
{
	std::cout << "==========TEST==============" << std::endl;
	ssc::Transform3D M = ssc::createTransformRotateX(M_PI/3)*ssc::createTransformRotateY(M_PI/4)*ssc::createTransformTranslate(ssc::Vector3D(0,2,3));

	ElastixEulerTransform E = ElastixEulerTransform::create(M, ssc::Vector3D(50,40,30));
//	ssc::Vector3D eulerAngles_xyz = M.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
//	ssc::Vector3D translation = M.matrix().block<3, 1> (0, 3);
//	ssc::Vector3D c(0,0,0);
//
//	ssc::Transform3D T = ssc::createTransformTranslate(translation);
//	ssc::Transform3D C = ssc::createTransformTranslate(c);
//
//	ssc::Transform3D Rx = ssc::createTransformRotateX(eulerAngles_xyz[0]);
//	ssc::Transform3D Ry = ssc::createTransformRotateY(eulerAngles_xyz[1]);
//	ssc::Transform3D Rz = ssc::createTransformRotateZ(eulerAngles_xyz[2]);
//
//	ssc::Transform3D R = Rx*Ry*Rz; // which one??????
////	ssc::Transform3D R = Rz*Ry*Rx;
//
//	ssc::Transform3D Q = T*C*R*C.inv();
	ssc::Transform3D Q = E.toMatrix();

	std::cout << "M\n" << M << std::endl;
	std::cout << "Q\n" << Q << std::endl;
	std::cout << "Q*M.inv\n" << Q*M.inv() << std::endl;
}

/**Write the initial (pre-registration) mMf transform to
 * disk as required by elastix.
 */
QString ElastixExecuter::writeInitTransformToElastixfile(
	ssc::ImagePtr fixed,
	ssc::ImagePtr moving,
	QString outdir)
{
	test_euler();
	ssc::Transform3D mMf = moving->get_rMd().inv() * fixed->get_rMd();

	ssc::Vector3D eulerAngles_xyz = mMf.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
	ssc::Vector3D translation = mMf.matrix().block<3, 1> (0, 3);

	QString elastiXText = QString(""
		"// Input transform file\n"
		"// Auto-generated by CustusX on %1\n"
		"\n"
		"(Transform \"EulerTransform\")\n"
		"(NumberOfParameters 6)\n"
		"(TransformParameters %2 %3)\n"
		"(InitialTransformParametersFileName \"NoInitialTransform\")\n"
		"(HowToCombineTransforms \"Compose\")\n"
		"\n"
		"// EulerTransform specific\n"
		"(CenterOfRotationPoint 0 0 0)\n"
		"(ComputeZYX \"false\")\n"
		"").arg(QDateTime::currentDateTime().toString(ssc::timestampSecondsFormatNice()),
			qstring_cast(eulerAngles_xyz),
			qstring_cast(translation));

	QFile initTransformFile(outdir+"/t0.txt");
	if (!initTransformFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Failed to open file %1 for writing.").arg(initTransformFile.fileName()));
	}
	initTransformFile.write(elastiXText.toAscii());
	return initTransformFile.fileName();
}

/**Write the initial (pre-registration) mMf transform to
 * disk in a .cal file that contains only the 16 matrix numbers.
 */
QString ElastixExecuter::writeInitTransformToCalfile(
	ssc::ImagePtr fixed,
	ssc::ImagePtr moving,
	QString outdir)
{
	ssc::Transform3D mMf = moving->get_rMd().inv() * fixed->get_rMd();

	QFile initTransformFile(outdir+"/moving_M_fixed_initial.cal");
	if (!initTransformFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Failed to open file %1 for writing.").arg(initTransformFile.fileName()));
	}
	initTransformFile.write(qstring_cast(mMf).toAscii());
	return initTransformFile.fileName();
}

void ElastixExecuter::processReadyRead()
{
	ssc::messageManager()->sendInfo(QString(mProcess->readAllStandardOutput()));
}

void ElastixExecuter::processError(QProcess::ProcessError error)
{
	QString msg;
	msg += "Registration process reported an error: ";

	switch (error)
	{
	case QProcess::FailedToStart:
		msg += "Failed to start";
		break;
	case QProcess::Crashed:
		msg += "Crashed";
		break;
	case QProcess::Timedout:
		msg += "Timed out";
		break;
	case QProcess::WriteError:
		msg += "Write Error";
		break;
	case QProcess::ReadError:
		msg += "Read Error";
		break;
	case QProcess::UnknownError:
		msg += "Unknown Error";
		break;
	default:
		msg += "Invalid error";
	}

	ssc::messageManager()->sendError(msg);
}

void ElastixExecuter::processFinished(int code, QProcess::ExitStatus status)
{
	ssc::messageManager()->sendInfo(QString("Registration process exited with code %1 and status %2").arg(code).arg(status));

//	this->getAffineResult();
}


void ElastixExecuter::processStateChanged(QProcess::ProcessState newState)
{
	QString msg;
	msg += "Registration process";

	if (newState == QProcess::Running)
	{
		ssc::messageManager()->sendInfo(msg + " running.");
		emit started(0);
//		this->delayedAutoConnectServer();
	}
	if (newState == QProcess::NotRunning)
	{
		emit finished();
		ssc::messageManager()->sendInfo(msg + " not running.");
	}
	if (newState == QProcess::Starting)
	{
		ssc::messageManager()->sendInfo(msg + " starting.");
	}

//
//	std::cout << "==============" << std::endl;
//	std::cout << QString(mProcess->readAllStandardError()) << std::endl;
//	std::cout << QString(mProcess->readAllStandardOutput()) << std::endl;
//	std::cout << "==============" << std::endl;
}


//ssc::Transform3D ElastixExecuter::getAffineResult_mMf(bool* ok)
//{
//	QString filename = mLastOutdir + "/TransformParameters.0.txt";
//	ElastixParameterFile file(filename);
//
//	bool useDirectionCosines = file.readParameterBool("UseDirectionCosines");
//	if (useDirectionCosines)
//	{
//		ssc::messageManager()->sendWarning("Elastix UseDirectionCosines is not supported. Result is probably wrong.");
//	}
//
//	QString transformType = file.readParameterString("Transform");
//	if (transformType=="EulerTransform")
//	{
//		if (ok)
//			*ok = true;
//		return file.readEulerTransform();
//	}
//
//	if (ok)
//		*ok = false;
//	ssc::messageManager()->sendWarning(QString("TransformType [%1] is not supported by CustusX. Registration result ignored.").arg(transformType));
//	return ssc::Transform3D::Identity();
//}

/**Find the TransformParameters.i.txt file with the
 * highest i. All other transform files can be found from
 * this one.
 */
QString ElastixExecuter::findMostRecentTransformOutputFile() const
{
	QString retval;
	for (int i=0; ; ++i)
	{
		QString filename = QString(mLastOutdir + "/TransformParameters.%1.txt").arg(i);
		if (!QFileInfo(filename).exists())
			break;
		retval = filename;
	}
	return retval;
}

/** Return the result of the latest registration as a linear transform mMf.
 *
 *  Important: The result is according to the ElastiX spec:
 *  \verbatim
   In elastix the transformation is defined as a coordinate mapping from
   the fixed image domain to the moving image domain.
 *  \endverbatim
 *
 * All transform files are read and concatenated. Those with
 * unrecognized (i.e. by CustusX) transforms are ignored with
 * a warning.
 *
 *
 */
ssc::Transform3D ElastixExecuter::getAffineResult_mMf(bool* ok)
{
	QString filename = this->findMostRecentTransformOutputFile();
	ssc::Transform3D mMf = ssc::Transform3D::Identity();

	while (true)
	{
		ElastixParameterFile file(filename);

		bool useDirectionCosines = file.readParameterBool("UseDirectionCosines");
		if (useDirectionCosines)
		{
			ssc::messageManager()->sendWarning("Elastix UseDirectionCosines is not supported. Result is probably wrong.");
		}

		QString transformType = file.readParameterString("Transform");
		if (transformType=="EulerTransform")
		{
			if (ok)
				*ok = true;
			ssc::Transform3D mQf = file.readEulerTransform();
			// concatenate transforms:
			mMf = mQf * mMf;
		}
		else
		{
			// accept invalid transforms, but emit warning.
//			if (ok)
//				*ok = false;
			ssc::messageManager()->sendWarning(QString("TransformType [%1] is not supported by CustusX. Registration result from %2 ignored.").arg(transformType).arg(filename));
		}

		filename = file.readParameterString("InitialTransformParametersFileName");
		if (filename.isEmpty() || filename=="NoInitialTransform")
			break;
	}

	return mMf;
}




// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------




ssc::Transform3D ElastixParameterFile::readEulerTransform()
{
	QString transformType = this->readParameterString("Transform");
	if (transformType!="EulerTransform")
		ssc::messageManager()->sendError("Assert failure: attempting to read EulerTransform");

	int numberOfParameters = this->readParameterInt("NumberOfParameters");
	if (numberOfParameters!=6)
	{
		ssc::messageManager()->sendWarning(QString("Expected 6 Euler parameters, got %1").arg(numberOfParameters));
		return ssc::Transform3D::Identity();
	}
	std::vector<double> transformParameters = this->readParameterDoubleVector("TransformParameters");
	std::vector<double> centerOfRotationPoint = this->readParameterDoubleVector("CenterOfRotationPoint");

	ssc::Vector3D t(transformParameters[3], transformParameters[4], transformParameters[5]);
	ssc::Transform3D T = ssc::createTransformTranslate(t);
	ssc::Vector3D c(centerOfRotationPoint[0], centerOfRotationPoint[1], centerOfRotationPoint[2]);
	ssc::Transform3D C = ssc::createTransformTranslate(c);

	ssc::Transform3D Rx = ssc::createTransformRotateX(transformParameters[0]);
	ssc::Transform3D Ry = ssc::createTransformRotateY(transformParameters[1]);
	ssc::Transform3D Rz = ssc::createTransformRotateZ(transformParameters[2]);

//	ssc::Transform3D R = Rx*Ry*Rz; // which one??????
	ssc::Transform3D R = Rz*Ry*Rx;

	return T*C*R*C.inv();
}

ElastixParameterFile::ElastixParameterFile(QString filename) : mFile(filename)
{
	if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		ssc::messageManager()->sendWarning(QString("Can't open ElastiX result file %1").arg(filename));
	}
	mText = QString(mFile.readAll());
//	std::cout << "Loaded text from " << filename << ":\n" << mText << std::endl;
}

QString ElastixParameterFile::readParameterRawValue(QString key)
{
	QStringList lines = mText.split('\n');

	QString regexpStr = QString(""
		"\\s*" // match zero or more whitespace
		"\\("  // match beginning (
		"\\s*" // match zero or more whitespace
		"%1"   // key
		"\\s+" // match one or more whitespace
		"("    // start grab value
		"[^\\)]*" // value - anything up to the closing )
		")"    // end grab value
		"\\)"  // match ending )
		"[^\\n]*" // rest of line - ignore
		).arg(key);
	QRegExp rx(regexpStr);
//	std::cout << regexpStr << std::endl;

	lines.indexOf(rx);

//	if (lines.indexOf(rx)>=0)
//	{
//		std::cout << "FOUND0 " << rx.cap(0) << std::endl;
//		std::cout << "FOUND1 " << rx.cap(1) << std::endl;
//	}

	return rx.cap(1);
}

QString ElastixParameterFile::readParameterString(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	if (retval.startsWith("\"") && retval.endsWith("\""))
	{
		retval = retval.replace(0, 1, "");
		retval = retval.replace(retval.size()-1, 1, "");
	}

	return retval;
}

bool ElastixParameterFile::readParameterBool(QString key)
{
	QString text = this->readParameterString(key);
	return (text=="true") ? true : false;
}

int ElastixParameterFile::readParameterInt(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	return retval.toInt();
}

std::vector<double> ElastixParameterFile::readParameterDoubleVector(QString key)
{
	QString retval =  this->readParameterRawValue(key);
	return convertQString2DoubleVector(retval);
}

} /* namespace cx */
