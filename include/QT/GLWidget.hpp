#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include <QGLWidget>
class GLWidget : public QGLWidget{
  Q_OBJECT
  
public:
  GLWidget(QWidget* parent = 0);
  GLWidget(const QGLFormat& format, QWidget* parent = 0);
  virtual ~GLWidget();
  
  int xRotation() const{return xRot;}
  int yRotation() const{return yRot;}
  int zRotation() const{return zRot;}
  int xPosition() const{return xPos;}
  int yPosition() const{return yPos;}
  int zPosition() const{return yPos;}

public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void setXPosition(int pos);
  void setYPosition(int pos);
  void setZPosition(int pos);
  void setPerspectiveRendering(bool b);
  void setZoomFactor(int z);
  
signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  
  void xPositionChanged(int pos);
  void yPositionChanged(int pos);
  void zPositionChanged(int pos);
  
protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  void normalizeAngle(int* angle);
protected:
  int xRot;
  int yRot;
  int zRot;
  int xPos;
  int yPos;
  int zPos;
  
  bool perspective;
  float zoomFactor;
  
private:
  
  QPoint lastPos;
  
  
};

#endif/*GLWIDGET_HPP*/
