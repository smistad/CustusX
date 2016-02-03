#ifndef UR5PLANNEDMOVE_H
#define UR5PLANNEDMOVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTextEdit>


namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5PlannedMoveTab : public QWidget
{
    Q_OBJECT
public:
    Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5PlannedMoveTab();


private slots:
    void runVTKfileSlot();
    void openVTKfileSlot();
    void blendRadiusChangedSlot();
    void runVelocityVTKSlot();

    void startLoggingSlot();
    void stopLoggingSlot();
    void clearPoseQueueSlot();
    void moveToInitialPositionButtonSlot();

    void moveToPointSlot();
    void moveToFrameSlot();
    void followActiveToolSlot();
    void startFollowingActiveToolSlot(Transform3D matrix, double timestamp);

private:
    QPushButton *runVTKButton, *openVTKButton, *runVelocityVTKButton;
    QPushButton *startLoggingButton, *stopLoggingButton;
    QPushButton *clearPoseQueueButton;
    QPushButton *moveToInitialPositionButton;

    QPushButton *moveToPointButton, *moveToFrameButton, *followActiveToolButton;

    QLineEdit *vtkLineEdit;

    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit, *blendRadiusLineEdit;


    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;

    void setMoveVTKWidget(QVBoxLayout *parent);
    void setMoveSettingsWidget(QVBoxLayout *parent);
    void setTextEditorWidget(QVBoxLayout *parent);

    VisServicesPtr mServices;

    Transform3D prevMatrix;
};

} // cx

#endif // UR5PLANNEDMOVE_H
