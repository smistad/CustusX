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
#include "cxDataLocations.h"

#include <iostream>
#include <QApplication>
#include <QDir>
#include "cxConfig.h"
#include "cxSettings.h"
#include "cxFileHelpers.h"
#include "cxTypeConversions.h"

namespace cx
{

//---------------------------------------------------------
bool DataLocations::mTestMode = false;
//---------------------------------------------------------

void DataLocations::setTestMode()
{
	mTestMode = true;
	cx::removeNonemptyDirRecursively(getTestDataPath() + "/temp");
//	cx::removeNonemptyDirRecursively(getTestDataPath() + "/temp/settings");
}

QString DataLocations::getTestDataPath()
{
	QString settingsPath = cx::DataLocations::getRootConfigPath() + "/settings";
	QString dataRootConfigFile = settingsPath + "/data_root_location.txt";
	if (QFileInfo(dataRootConfigFile).exists())
	{
		return readTestDataPathFromFile(dataRootConfigFile);
	}
	else
	{
		return CX_DATA_ROOT;
	}
}

QString DataLocations::getLargeTestDataPath()
{
	QString settingsPath = cx::DataLocations::getRootConfigPath() + "/settings";
	QString dataRootConfigFile = settingsPath + "/large_data_root_location.txt";
	if (QFileInfo(dataRootConfigFile).exists())
	{
		return readTestDataPathFromFile(dataRootConfigFile);
	}
	else
	{
		return CX_LARGE_DATA_ROOT;
	}
}

QString DataLocations::getExistingTestData(QString pathRelativeToTestDataRoot, QString filename)
{
	QString path;

	path = QString("%1/%2/%3").arg(getTestDataPath()).arg(pathRelativeToTestDataRoot).arg(filename);
	if (QFileInfo(path).exists())
		return path;

	path = QString("%1/%2/%3").arg(getLargeTestDataPath()).arg(pathRelativeToTestDataRoot).arg(filename);
	if (QFileInfo(path).exists())
		return path;

	return "";
}


QString DataLocations::readTestDataPathFromFile(QString filename)
{
	QFile file(filename);
	file.open(QFile::ReadOnly);
	QString cxDataRoot(file.readAll());
	return cxDataRoot;
}

QString DataLocations::getSettingsPath()
{
	QString retval = cx::DataLocations::getRootConfigPath() + "/settings";
	if (mTestMode)
		retval = getTestDataPath() + "/temp/settings";
	return retval;
}

QString DataLocations::getBundlePath()
{
#ifdef __APPLE__
  QString path(qApp->applicationDirPath()+"/../../..");
  QString bundle = QDir(qApp->applicationDirPath()+"/../..").canonicalPath();
//  std::cout << "check bundle: " << bundle << ", isbundle=" << QFileInfo(bundle).isBundle() << std::endl;
  if (QFileInfo(bundle).isBundle())
	  return path;
  else
	  return qApp->applicationDirPath();
#else
  QString path(qApp->applicationDirPath());
  return path;
#endif
}

QStringList DataLocations::getDefaultPluginsPath()
{
	QStringList retval;
	QString bundlePath = DataLocations::getBundlePath();
	QString appPath(qApp->applicationDirPath());

//	QString buildLocation = appPath + "/plugins";
	QString buildLocation = bundlePath + "/plugins";
	if (QFile(buildLocation).exists())
		retval <<  buildLocation;

//	QString installLocation = bundlePath + "/plugins";
	QString installLocation = appPath + "/plugins";
	if (QFile(installLocation).exists())
		retval << installLocation;

	QString fallbackInstallLocation = appPath;
	if (QFile(fallbackInstallLocation).exists())
		retval << fallbackInstallLocation;

	return retval;
}

QString DataLocations::getRootConfigPath()
{
	QStringList paths = getRootConfigPaths();
	if (paths.empty())
		return "";
	return paths.front();
}

QStringList DataLocations::getRootConfigPaths()
{
  QString path = getBundlePath() + "/" + CX_CONFIG_ROOT_RELATIVE_INSTALLED; // look for installed location
  if (QDir(path).exists())
	return QStringList() << QDir(path).canonicalPath();

  QStringList retval;
  if (QDir(CX_CONFIG_ROOT).exists()) // look for folder in source code
	retval << QDir(CX_CONFIG_ROOT).canonicalPath();
  if (QDir(CX__OPTIONAL_CONFIG_ROOT).exists()) // look for folder in source code
	retval << QDir(CX__OPTIONAL_CONFIG_ROOT).canonicalPath();

  return retval;
}

QString DataLocations::getDocPath()
{
  QString path = getBundlePath() + "/" + CX_DOC_ROOT_RELATIVE_INSTALLED; // look for installed location
  if (QDir(path).exists())
	return QDir(path).canonicalPath();

  if (QDir(CX_DOC_ROOT).exists()) // look for folder in source code
	return QDir(CX_DOC_ROOT).canonicalPath();

  return "";
}

QStringList DataLocations::appendStringToAllElements(QStringList root, QString suffix)
{
	QStringList retval;
	for (int i=0; i<root.size(); ++i)
		retval << root[i] + suffix;
	return retval;
}

QStringList DataLocations::getToolsPaths()
{
	QString suffix("/tool/Tools/");
	QStringList root = getRootConfigPaths();
	return appendStringToAllElements(root, "/tool/Tools/");
}
  
QStringList DataLocations::getApplicationToolConfigPaths()
{
	QString suffix("/tool/" + settings()->value("globalApplicationName").toString());
	QStringList root = getRootConfigPaths();
	return appendStringToAllElements(root, suffix);
}
  
QString DataLocations::getToolConfigFilePath()
{
	QString relPath("/tool/" + settings()->value("globalApplicationName").toString());
	QString filename = settings()->value("toolConfigFile").toString();
	return getExistingConfigPath(relPath, "", filename);
}

QString DataLocations::getAudioConfigFilePath()
{
  QString path(getRootConfigPath()+"/audio/");
  return path;
}
  
QString DataLocations::getShaderPath()
{
  QString path = getRootConfigPath()+"/shaders";
  if (QDir(path).exists())
    return path;
  return "";
}

namespace
{
QString changeExtension(QString name, QString ext)
{
  QStringList splitName = name.split(".");
  splitName[splitName.size()-1] = ext;
  return splitName.join(".");
}
}

QString DataLocations::getXmlSettingsFile()
{
	return getSettingsPath() + "/settings.xml";
}

QString DataLocations::getCachePath()
{
	QString path(getRootConfigPath()+"/cache/");
    return path;
}

QString DataLocations::getExistingConfigPath(QString pathRelativeToConfigRoot, QString alternativeAbsolutePath, QString filename)
{
	foreach (QString root, getRootConfigPaths())
	{
		QString path = root + "/" + pathRelativeToConfigRoot + "/" + filename;
		if (QFileInfo(path).exists())
			return path;
	}

	if (QFileInfo(alternativeAbsolutePath + "/" + filename).exists())
		return alternativeAbsolutePath;

	return "";
}

} // namespace cx
