#include "sscImage.h"

#include <QDomDocument>
#include <vtkImageAccumulate.h>
#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageLuminance.h>

#include "sscImageTF3D.h"
#include "sscBoundingBox3D.h"
#include "sscImageLUT2D.h"
#include "sscRegistrationTransform.h"
#include "sscLandmark.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

namespace ssc
{
  
Image::~Image()
{}

Image::Image(const std::string& uid, const vtkImageDataPtr& data,
    const std::string& name) :
  Data(uid, name),
	mBaseImageData(data)
{
  mShading.on = false;
  mShading.ambient = 0.2;
  mShading.diffuse = 0.9;
  mShading.specular = 0.3;
  mShading.specularPower = 15.0;
  
  this->resetTransferFunctions();
}
  
void Image::resetTransferFunctions()
{
  //messageManager()->sendDebug("Image::reset called");
  
  if(!mBaseImageData)
  {
    messageManager()->sendWarning("ssc::Image has no image data");
    return;
  }
  
  mBaseImageData->Update();
	mBaseImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
  
  if(mImageTransferFunctions3D)
    disconnect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()),
               this, SIGNAL(transferFunctionsChanged()));
  if(mImageLookupTable2D)
    disconnect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()),
               this, SIGNAL(transferFunctionsChanged()));
  
  mImageTransferFunctions3D.reset(new ImageTF3D(mBaseImageData));
	mImageLookupTable2D.reset(new ImageLUT2D(mBaseImageData));
  
	// Add initial values to the 3D transfer functions
	mImageTransferFunctions3D->addAlphaPoint(this->getMin(), 0);
	mImageTransferFunctions3D->addAlphaPoint(this->getMax(), this->getMaxAlphaValue());
	mImageTransferFunctions3D->addColorPoint(this->getMin(), Qt::black);
	mImageTransferFunctions3D->addColorPoint(this->getMax(), Qt::white);
  
  connect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()),
          this, SIGNAL(transferFunctionsChanged()));
  connect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()),
					this, SIGNAL(transferFunctionsChanged()));
}
  
//void Image::set_rMd(Transform3D rMd)
//{
//	bool changed = !similar(rMd, m_rMd);
//
//	Data::set_rMd(rMd);
//	//std::cout << "Image::setTransform(): \n" << rMd << std::endl;
//	if (!changed)
//	{
//		return;
//	}
//}

void Image::transformChangedSlot()
{
	if (mReferenceImageData)
	{
		Transform3D rMd = get_rMd();
		mOrientatorMatrix->DeepCopy(rMd.inv().matrix());
		mReferenceImageData->Update();		
	}
  //std::cout << "Image::transformChangedSlot()\n" << rMd << std::endl;
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	//messageManager()->sendDebug("Image::setVtkImageData()");
	mBaseImageData = data;
	mBaseGrayScaleImageData = NULL;
	
	if (mOrientator)
	{
		mOrientator->SetInput(mBaseImageData);		
	}

  this->resetTransferFunctions();
	emit vtkImageDataChanged();
}
vtkImageDataPtr Image::getGrayScaleBaseVtkImageData()
{
	if (mBaseGrayScaleImageData)
	{
		return mBaseGrayScaleImageData;
	}
	
	mBaseGrayScaleImageData = getBaseVtkImageData();
	
	// if the volume is color, run it through a luminance filter in order to get a
	// finning grayscale representation.
	if (mBaseGrayScaleImageData->GetNumberOfScalarComponents()>2) // color
	{
		vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		luminance->SetInput(mBaseGrayScaleImageData);
		mBaseGrayScaleImageData = luminance->GetOutput();		
	}
	
	mBaseGrayScaleImageData->Update();
	return mBaseGrayScaleImageData;
}
ImageTF3DPtr Image::getTransferFunctions3D()
{
	return mImageTransferFunctions3D;
}
ImageLUT2DPtr Image::getLookupTable2D()
{
	return mImageLookupTable2D;
}
//See ssc::Data
/*REGISTRATION_STATUS Image::getRegistrationStatus() const
{
	return mRegistrationStatus;
}*/
vtkImageDataPtr Image::getBaseVtkImageData()
{
	return mBaseImageData;
}
vtkImageDataPtr Image::getRefVtkImageData()
{
	if (!mReferenceImageData) // optimized: dont init it if you dont need it.
	{
		// provide a resampled volume for algorithms requiring that (such as proberep)
		mOrientatorMatrix = vtkMatrix4x4Ptr::New();
		mOrientator = vtkImageReslicePtr::New();
		mOrientator->SetInput(mBaseImageData);
		mOrientator->SetInterpolationModeToLinear();
		mOrientator->SetOutputDimensionality( 3);
		mOrientator->SetResliceAxes(mOrientatorMatrix);
		mOrientator->AutoCropOutputOn();
		mReferenceImageData = mOrientator->GetOutput();

		mReferenceImageData->Update();		

		this->transformChangedSlot(); // update transform
	}
	
	return mReferenceImageData;
}

LandmarkMap Image::getLandmarks()
{
	return mLandmarks;
}
void Image::setLandmark(Landmark landmark)
{
  //std::cout << "Image::setLandmark" << std::endl;
  mLandmarks[landmark.getUid()] = landmark;
  emit landmarkAdded(landmark.getUid());
}
void Image::removeLandmark(std::string uid)
{
  mLandmarks.erase(uid);
  emit landmarkRemoved(uid);
}
/** If index is found, it's treated as an edit operation, else
 * it's an add operation.
 * @param x
 * @param y
 * @param z
 * @param index
 */
/*void Image::addLandmarkSlot(double x, double y, double z, unsigned int index)
{
	double addLandmark[4] = {x, y, z, (double)index};

	int numberOfLandmarks = mLandmarks->GetNumberOfTuples();
	//if index exists, we treat it as an edit operation
	for(int i=0; i<= numberOfLandmarks-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		if(landmark[3] == index)
		{
			mLandmarks->SetTupleValue(i, addLandmark);
			emit landmarkAdded(x, y, z, index);
			return;
		}
	}
	//else it's an add operation
	mLandmarks->InsertNextTupleValue(addLandmark);
	emit landmarkAdded(x, y, z, index);
}*/
/** If index is found that tuple(landmark) is removed from the array, else
 * it's just ignored.
 * @param x
 * @param y
 * @param z
 * @param index
 */
/*void Image::removeLandmarkSlot(double x, double y, double z, unsigned int index)
{
	int numberOfLandmarks = mLandmarks->GetNumberOfTuples();
	for(int i=0; i<= numberOfLandmarks-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		if(landmark[3] == index)
		{
			mLandmarks->RemoveTuple(i);
			emit landmarkRemoved(x, y, z, index);
		}
	}
}*/
//
//void Image::transferFunctionsChangedSlot()
//{
//	emit vtkImageDataChanged();
//}
void Image::printLandmarks()
{
	std::cout << "Landmarks: " << std::endl;
	/*for(int i=0; i<= mLandmarks->GetNumberOfTuples()-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		std::stringstream stream;
		stream << i << ": (";
		stream << landmark[0];
		stream << ",";
		stream << landmark[1];
		stream << ",";
		stream << landmark[2];
		stream << ",";
		stream << landmark[3];
		stream << ")";
		std::cout << stream.str() << std::endl;
	}*/
}
DoubleBoundingBox3D Image::boundingBox() const
{
  mBaseImageData->UpdateInformation();
	DoubleBoundingBox3D bounds(mBaseImageData->GetBounds());
	return bounds;
}
vtkImageAccumulatePtr Image::getHistogram()
{
	if (mHistogramPtr.GetPointer() == NULL)
	{
		mHistogramPtr = vtkImageAccumulatePtr::New();
		mHistogramPtr->SetInput(mBaseImageData);
		mHistogramPtr->IgnoreZeroOn(); // required for Sonowand CT volumes, where data are placed between 31K and 35K.
		// Set up only a 1D histogram for now, so y and z values are set to 0
		mHistogramPtr->SetComponentExtent(this->getMin(), this->getMax(),0,0,0,0);
		mHistogramPtr->SetComponentOrigin(this->getMin(), 0, 0);
		mHistogramPtr->SetComponentSpacing(1, 0, 0);
	}
	mHistogramPtr->Update();
	return mHistogramPtr;
}
int Image::getMax()
{
	// Alternatively create max from histogram
	//IntIntMap::iterator iter = this->getHistogram()->end();
	//iter--;
	//return (*iter).first;
	return (int)mImageTransferFunctions3D->getScalarMax();
}
int Image::getMin()
{
	// Alternatively create min from histogram
	//IntIntMap::iterator iter = this->getHistogram()->begin();
	//return (*iter).first;
	return (int)mImageTransferFunctions3D->getScalarMin();
}
int Image::getPosMax()
{
	return (int)mImageTransferFunctions3D->getScalarMax() - getMin();
}
int Image::getPosMin()
{
	return 0;
}
  
int Image::getRange()
{
	return this->getMax() - this->getMin();
}
int Image::getMaxAlphaValue()
{
	return 255;
}
void Image::addXml(QDomNode& parentNode)
{
//  Data::addXml(parentNode);

  QDomDocument doc = parentNode.ownerDocument();
  QDomElement imageNode = doc.createElement("image");
  parentNode.appendChild(imageNode);

  m_rMd_History->addXml(imageNode); //TODO: should be in the superclass

  QDomElement uidNode = doc.createElement("uid");
  uidNode.appendChild(doc.createTextNode(mUid.c_str()));
  imageNode.appendChild(uidNode);

  QDomElement nameNode = doc.createElement("name");
  nameNode.appendChild(doc.createTextNode(mName.c_str()));
  imageNode.appendChild(nameNode);
  
  QDomElement filePathNode = doc.createElement("filePath");
  filePathNode.appendChild(doc.createTextNode(mFilePath.c_str()));
  imageNode.appendChild(filePathNode);
  
  mImageTransferFunctions3D->addXml(imageNode);
  mImageLookupTable2D->addXml(imageNode);

  QDomElement shadingNode = doc.createElement("shading");
  shadingNode.appendChild(doc.createTextNode(qstring_cast(mShading.on)));
  imageNode.appendChild(shadingNode);
  std::cout << "created shading" << std::endl;
  
  QDomElement shadingAmbientNode = doc.createElement("shadingAmbient");
  shadingAmbientNode.appendChild(doc.createTextNode(qstring_cast(mShading.ambient)));
  imageNode.appendChild(shadingAmbientNode);
  
  QDomElement shadingDiffuseNode = doc.createElement("shadingDiffuse");
  shadingDiffuseNode.appendChild(doc.createTextNode(qstring_cast(mShading.diffuse)));
  imageNode.appendChild(shadingDiffuseNode);
  
  QDomElement shadingSpecularNode = doc.createElement("shadingSpecular");
  shadingSpecularNode.appendChild(doc.createTextNode(qstring_cast(mShading.specular)));
  imageNode.appendChild(shadingSpecularNode);
  
  QDomElement shadingSpecularPowerNode = doc.createElement("shadingSpecularPower");
  shadingSpecularPowerNode.appendChild(doc.createTextNode(qstring_cast(mShading.specularPower)));
  imageNode.appendChild(shadingSpecularPowerNode);

  QDomElement landmarksNode = doc.createElement("landmarks");
  LandmarkMap::iterator it = mLandmarks.begin();
  for(; it != mLandmarks.end(); ++it)
  {
    QDomElement landmarkNode = doc.createElement("landmark");
    it->second.addXml(landmarkNode);
    landmarksNode.appendChild(landmarkNode);
  }
  imageNode.appendChild(landmarksNode);

}

void Image::parseXml(QDomNode& dataNode)
{
  //Data::parseXml(dataNode);

  // image node must be parsed in the data manager to create this Image object
  // Only subnodes are parsed here

	if (dataNode.isNull())
		return;
	
  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
  m_rMd_History->parseXml(registrationHistory);

	//transferefunctions
	QDomNode transferfunctionsNode = dataNode.namedItem("transferfunctions");
	if (!transferfunctionsNode.isNull())
		mImageTransferFunctions3D->parseXml(transferfunctionsNode);
	else
	{
		std::cout << "Warning: Image::parseXml() found no transferfunctions";
		std::cout << std::endl;
	}

	mImageLookupTable2D->parseXml(dataNode.namedItem("lookuptable2D"));

	mShading.on = dataNode.namedItem("shading").toElement().text().toInt();
  //Assign default values if the shading nodes don't exists to allow backward compability
  if(!dataNode.namedItem("shadingAmbient").isNull())
    mShading.ambient = dataNode.namedItem("shadingAmbient").toElement().text().toDouble();
  //else
  //  mShading.ambient = 0.2;
  if(!dataNode.namedItem("shadingDiffuse").isNull())
    mShading.diffuse = dataNode.namedItem("shadingDiffuse").toElement().text().toDouble();
  //else
  //  mShading.diffuse = 0.9;
  if(!dataNode.namedItem("shadingSpecular").isNull())
    mShading.specular = dataNode.namedItem("shadingSpecular").toElement().text().toDouble();
  //else
  //  mShading.specular = 0.3;
  if(!dataNode.namedItem("shadingSpecularPower").isNull())
    mShading.specularPower = dataNode.namedItem("shadingSpecularPower").toElement().text().toDouble();
  //else
  //  mShading.specularPower = 15.0;
  
	QDomNode landmarksNode = dataNode.namedItem("landmarks");
	QDomElement landmarkNode = landmarksNode.firstChildElement("landmark");
	for (; !landmarkNode.isNull(); landmarkNode = landmarkNode.nextSiblingElement("landmark"))
	{
	  Landmark landmark;
	  landmark.parseXml(landmarkNode);
	  this->setLandmark(landmark);
  }
}

void Image::setShadingOn(bool on)
{
  mShading.on = on;
  emit transferFunctionsChanged();
}

bool Image::getShadingOn() const
{
  return mShading.on;
}

void Image::setShadingAmbient(double ambient)
{
  mShading.ambient = ambient;
  emit transferFunctionsChanged();
}
  
void Image::setShadingDiffuse(double diffuse)
{
  mShading.diffuse = diffuse;
  emit transferFunctionsChanged();
}
  
void Image::setShadingSpecular(double specular)
{
  mShading.specular = specular;
  emit transferFunctionsChanged();
}

void Image::setShadingSpecularPower(double specularPower)
{
  mShading.specularPower = specularPower;
  emit transferFunctionsChanged();
}

double Image::getShadingAmbient()
{return mShading.ambient;}
double Image::getShadingDiffuse()
{return mShading.diffuse;}
double Image::getShadingSpecular()
{return mShading.specular;}
double Image::getShadingSpecularPower()
{return mShading.specularPower;}  

Image::shadingStruct Image::getShading()
{
  return mShading;
}

void Image::setShading(Image::shadingStruct shading )
{
  mShading = shading;
  emit transferFunctionsChanged();
}
  
} // namespace ssc
