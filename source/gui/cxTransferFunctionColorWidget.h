#ifndef CXTRANSFERFUNCTIONCOLORWIDGET_H_
#define CXTRANSFERFUNCTIONCOLORWIDGET_H_

#include <QWidget>

#include <sscImage.h>

class QRect;
class QMenu;

namespace cx
{
/**
 * \class TransferFunctionColorWidget
 *
 * \brief
 *
 * \date Apr 01, 2009
 * \author: Ole Vegard Solberg, SINTEF
 */
class TransferFunctionColorWidget : public QWidget
{
  Q_OBJECT
  
public:
  TransferFunctionColorWidget(QWidget* parent);
  ~TransferFunctionColorWidget();
  
public slots:
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed
  
protected slots:
  void removeColorSlot();
  void setColorSlot();
  
protected:
  struct ColorPoint
  {
    int position;
    QColor value;
    ColorPoint() :
    position(-1),
    value(255, 255, 255)
    {}
    void reset()
    {
      position = -1;
      value = QColor(255, 255, 255);
    }
    bool isValid()
    {
      if(position != -1)
        return true;
      return false;
    }
  }; ///< Internal placeholder for a color point
  
  QMenu* mRightClickMenu;///< Context menu for right clicking
  QAction* mCustomColorAction;///< Action for choosing custom color
  QAction* mRemoveColorAction;///< Action for removing a color
      
  QRect mFullArea;///< The full widget area
  QRect mPlotArea;///< The plot area 
  
  std::map<int, QRect> mPointRects;///< Cache with all point rectangles
  
  ColorPoint mCurrentPoint;///< The currently selected point
	bool mEndPoint;///< Current point is an endpoint
  int mColorindexSelected;
  int mCurrentClickX, mCurrentClickY;///< The x coordinate currently selected with the mouse
  int mBorder;///< The size of the border around the transferfunction. The size of the rectangles are mBorder * 2
  
  ssc::ImagePtr mCurrentImage;
  
  bool isInsideCurrentPoint();///< Checks if a screen coordinate is inside any of the point rectangles. Sets mCurrentPoint.position and mCurrentPoint.value
  void contextMenuEvent(QContextMenuEvent *event);///< Decides what happens when you rightclick in a view
  
    
  QPoint getPlotPoint(int iScalar);///< Calculate the screen point of a given scalar (x-axis) and value (y-axis)
  ColorPoint getCurrentColorPoint();///< Calculate the color point (position and color) based on clicked x coordinate
  void moveCurrentPoint();///< Move the currently selected point to the selected screen coordinate (mCurrentClickX)
  
  virtual void mousePressEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseReleaseEvent(QMouseEvent* event); ///< Reimplemented from superclass
  virtual void mouseMoveEvent(QMouseEvent* event);  ///< Reimplemented from superclass
  virtual void resizeEvent(QResizeEvent* evt);///< Reimplemented from superclass
  virtual void paintEvent(QPaintEvent* event); ///< Reimplemented from superclass. Paints the transferfunction GUI
};
}
#endif /* CXTRANSFERFUNCTIONCOLORWIDGET_H_ */
