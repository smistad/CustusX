#include "cxSeansVesselRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxStateMachineManager.h"
#include "cxTimedAlgorithm.h"
#include "cxPatientData.h"

namespace cx
{

SeansVesselRegistrationWidget::SeansVesselRegistrationWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mLTSRatioSpinBox(new QSpinBox()),
    mRegisterButton(new QPushButton("Register")),
    mFixedImageLabel(new QLabel("Fixed image:")),
    mMovingImageLabel(new QLabel("Moving image:"))
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

  connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

  mLTSRatioSpinBox->setSingleStep(1);
  mLTSRatioSpinBox->setValue(80);

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mFixedImageLabel, 0, 0);
  layout->addWidget(mMovingImageLabel, 1, 0);
  layout->addWidget(new QLabel("LTS Ratio: "), 2, 0);
  layout->addWidget(mLTSRatioSpinBox, 2, 1);
  layout->addWidget(mRegisterButton, 3, 0, 1, 2);
}

SeansVesselRegistrationWidget::~SeansVesselRegistrationWidget()
{}

QString SeansVesselRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Seans Vessel Registration.</h3>"
      "<p>Select two datasets you want to registere to eachother, adjust the input parameters.</p>"
      "<p><i>Adjust the parameters and click the register button.</i></p>"
      "</html>";
}

void SeansVesselRegistrationWidget::fixedImageSlot(QString uid)
{
  ssc::DataPtr fixedImage = registrationManager()->getFixedData();
  if(!fixedImage)
    return;
  mFixedImageLabel->setText(qstring_cast("Fixed data: <b>"+fixedImage->getName()+"</b>"));
  mFixedImageLabel->update();
}

void SeansVesselRegistrationWidget::movingImageSlot(QString uid)
{
  ssc::DataPtr movingImage = registrationManager()->getMovingData();
  if(!movingImage)
    return;
  mMovingImageLabel->setText(qstring_cast("Moving data: <b>"+movingImage->getName()+"</b>"));
  mMovingImageLabel->update();
}

void SeansVesselRegistrationWidget::registerSlot()
{
  int lts_ratio = mLTSRatioSpinBox->value();
  double stop_delta = 0.001; //TODO, add user interface
  double lambda = 0; //TODO, add user interface
  double sigma = 1.0; //TODO, add user interface
  bool lin_flag = 1; //TODO, add user interface
  int sample = 1; //TODO, add user interface
  int single_point_thre = 1; //TODO, add user interface
  bool verbose = 1; //TODO, add user interface

  ssc::messageManager()->sendDebug("Using lts_ratio: "+qstring_cast(lts_ratio));
  QString logPath = stateManager()->getPatientData()->getActivePatientFolder()+"/Logs/";

  registrationManager()->doVesselRegistration(lts_ratio, stop_delta, lambda, sigma, lin_flag, sample, single_point_thre, verbose, logPath);
}

}//namespace cx
