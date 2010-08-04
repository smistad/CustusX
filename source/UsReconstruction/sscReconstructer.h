/*
 *  sscReconstructer.h
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTER_H_
#define SSCRECONSTRUCTER_H_

#include <QObject>
#include <math.h>
#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "probeXmlConfigParser.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscXmlOptionItem.h"
//#include "sscStringWidgets.h"

namespace ssc
{
//class StringOptionItem;

typedef boost::shared_ptr<class Reconstructer> ReconstructerPtr;
/**
 * Used coordinate systems:
 * u  = raw input Ultrasound frames (in x, y. Origin lower left.)
 * t  = Tool space for probe as defined in ssc:Tool (z in ray direction, y to the left)
 * s  = probe localizer Sensor.
 * pr = Patient Reference localizer sensor.
 * d  = Output Data space
 *
 */
class Reconstructer : public QObject
{
  Q_OBJECT
public:
  Reconstructer(XmlOptionFile settings, QString shaderPath);
  virtual ~Reconstructer();

  void readFiles(QString mhdFileName, QString calFilesPath);
  void reconstruct(); // assumes readFiles has already been called

  ImagePtr reconstruct(QString mhdFileName, QString calFilesPath); // do everything
  ImagePtr getOutput();
  ImagePtr getInput();
  XmlOptionFile getSettings() const { return mSettings; }
  StringDataAdapterXmlPtr mOrientationAdapter;
  StringDataAdapterXmlPtr mAlgorithmAdapter;
  std::vector<DataAdapterPtr> mAlgoOptions;
  StringDataAdapterXmlPtr mMaskReduce;//Reduce mask size in % in each direction

  OutputVolumeParams getOutputVolumeParams() const;
  void setOutputVolumeParams(const OutputVolumeParams& par);
  void setOutputRelativePath(QString path);
  void setOutputBasePath(QString path);

public slots:
  void setSettings();

signals:
  void paramsChanged();

private:
  ImagePtr mUsRaw;///<All imported US data framed packed into one image
  std::vector<TimedPosition> mFrames;
  std::vector<TimedPosition> mPositions;
  ImagePtr mMask;///< Clipping mask for the input data

  OutputVolumeParams mOutputVolumeParams;
  ReconstructAlgorithmPtr mAlgorithm;
  ProbeXmlConfigParser::Configuration mConfiguration;
  XmlOptionFile mSettings;
  //QDomDocument mSettings;
  //QString mSettingsFilename;
  QString mLastAppliedOrientation; ///< the orientation algorithm used for the currently calculated extent.
  QString mCalFileName; ///< Name of calibration file
  QString mCalFilesPath; ///< Path to calibration files
  QString mFilename; ///< filename used for current data read
  QString mCalFilename; /// filename used for current cal read
  ImagePtr mOutput;///< Output image from reconstruction
  QString mOutputRelativePath;///< Relative path to the output image
  QString mOutputBasePath;///< Global path where the relative path starts, for the output image
  QString mShaderPath; ///< name of shader folder
  QString mLastAppliedMaskReduce;///< The last used mask reduction

  void readUsDataFile(QString mhdFileName);
  void readTimeStampsFile(QString fileName, std::vector<TimedPosition>* timedPos);
  void readPositionFile(QString posFile, bool alsoReadTimestamps);
  ImagePtr createMaskFromConfigParams();
  ImagePtr generateMask();
  ImagePtr readMaskFile(QString mhdFileName);
  ssc::Transform3D applyOutputOrientation();
  void findExtentAndOutputTransform();

  QString changeExtension(QString name, QString ext);
  Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
  Transform3D readTransformFromFile(QString fileName);
  void calibrateTimeStamps();
  void calibrateTimeStamps(double timeOffset, double scale);
  void interpolatePositions();
  void calibrate(QString calFilesPath);
  std::vector<ssc::Vector3D> generateInputRectangle();
  ImagePtr generateOutputVolume();
  //StringOptionItem getNamedSetting(const QString& uid);
  void clearAll();
  void clearOutput();
  //void saveSettings();
  void createAlgorithm();
};

}//namespace
#endif //SSCRECONSTRUCTER_H_
