// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#include "cxDataMetricRep.h"

#include "cxGraphicalPrimitives.h"
#include "cxView.h"
#include "cxDataMetric.h"
#include "cxLogger.h"
#include "cxVtkHelperClasses.h"

namespace cx
{

DataMetricRep::DataMetricRep(const QString& uid, const QString& name) :
	RepImpl(uid, name),
	mGraphicsSize(1),
	mShowLabel(false),
	mLabelSize(2.5),
	mShowAnnotation(true)
//	mView(NULL)
{
}

void DataMetricRep::setDataMetric(DataMetricPtr value)
{
    if (mMetric)
	{
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
		disconnect(mMetric.get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
	}

    mMetric = value;

    if (mMetric)
	{
		connect(mMetric.get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
	}

    this->clear();
	this->setModified();
}

DataMetricPtr DataMetricRep::getDataMetric()
{
    return mMetric;
}

void DataMetricRep::setShowLabel(bool on)
{
	mShowLabel = on;
	this->setModified();
}

void DataMetricRep::setGraphicsSize(double size)
{
	mGraphicsSize = size;
	this->setModified();
}

void DataMetricRep::setLabelSize(double size)
{
	mLabelSize = size;
	this->setModified();
}

void DataMetricRep::setShowAnnotation(bool on)
{
	mShowAnnotation = on;
	this->setModified();
}

void DataMetricRep::clear()
{
    mText.reset();
}

void DataMetricRep::addRepActorsToViewRenderer(View *view)
{
//    mView = view;

//	vtkRendererPtr renderer = mView->getRenderer();
//	renderer->AddObserver(vtkCommand::StartEvent, this->mCallbackCommand, 1.0);

    this->clear();
	this->setModified();
}

void DataMetricRep::removeRepActorsFromViewRenderer(View *view)
{
//	vtkRendererPtr renderer = mView->getRenderer();
//	renderer->RemoveObserver(this->mCallbackCommand);

//    mView = NULL;
    this->clear();
}

void DataMetricRep::drawText()
{
	if (!this->getView())
        return;

    QString text = this->getText();

    if (text.isEmpty())
    {
        mText.reset();
        return;
    }

	if (!mText)
	{
		mText.reset(new CaptionText3D(this->getRenderer()));
	}
	mText->setColor(mMetric->getColor());
    mText->setText(text);
    mText->setPosition(mMetric->getRefCoord());
    mText->setSize(mLabelSize / 100);
}

QString DataMetricRep::getText()
{
	if (!mShowAnnotation)
		return "";
	QStringList text;
	if (mShowLabel)
		text << mMetric->getName();
	if (mMetric->showValueInGraphics())
		text << mMetric->getValueAsString();
	return text.join(" = ");
}

Vector3D DataMetricRep::getColorAsVector3D() const
{
	if (!mMetric)
		return Vector3D(1,1,1);
//	QColor color = mMetric->getColor();
//	Vector3D retval(color.redF(), color.greenF(), color.blueF());
//	return retval;
	return cx::getColorAsVector3D(mMetric->getColor());
}

}