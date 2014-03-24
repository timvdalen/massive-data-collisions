#include <QtGui>
#include <QtOpenGL>

#include "QT/GLWidget.hpp"
#include "QT/GLWidget.moc"

#define PI 3.14159265359

float sqrf(float a){
  return a*a;
}

GLWidget::GLWidget(const QGLFormat& format,
		   QWidget* parent):QGLWidget(format, parent){
  xRot = yRot = zRot = 0;
  xPos = yPos = 0;
  perspective = true;
  zoomFactor = 1;
}

GLWidget::GLWidget(QWidget* parent):QGLWidget(parent){
  xRot = yRot = zRot = 0;
  xPos = yPos = 0;
  perspective = true;
  zoomFactor = 1;
}

GLWidget::~GLWidget(){
  makeCurrent();
}

void GLWidget::setXRotation(int angle){
  normalizeAngle(&angle);
  if(angle != xRot){
    xRot = angle;
    emit xRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setYRotation(int angle){
  normalizeAngle(&angle);
  if(angle != yRot){
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setZRotation(int angle){
  normalizeAngle(&angle);
  if(angle != zRot){
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setXPosition(int pos){
  if(pos != xPos){
    xPos = pos;
    emit xPositionChanged(pos);
    updateGL();
  }
}

void GLWidget::setYPosition(int pos){
  if(pos != yPos){
    yPos = pos;
    emit yPositionChanged(pos);
    updateGL();
  }
}

void GLWidget::setZPosition(int pos){
  if(pos != zPos){
    zPos = pos;
    emit zPositionChanged(pos);
      updateGL();
  }
}

void GLWidget::setPerspectiveRendering(bool b){
  perspective = b;
  resizeGL(width(), height());
  updateGL();
}

void GLWidget::setZoomFactor(int f){
  zoomFactor = sqrf(f/500.0f);
  resizeGL(width(), height());
  updateGL();
}

void GLWidget::initializeGL(){
  static const GLfloat lightPos[4] = {5.0f, 5.0f, 10.0f, 1.0f};
  
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  
  glEnable(GL_NORMALIZE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

#if 0
void GLWidget::paintGL(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glPushMatrix();
  glTranslatef(xPos/32.0, -yPos/32.0, zPos/32.0);
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);  
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
  
  drawGear(gear1, -3.0, -2.0, 0.0, gear1Rot / 16.0);
  drawGear(gear2, +3.1, -2.0, 0.0, -2.0 * (gear1Rot / 16.0) - 9.0);
  
  glRotated(+90.0, 1.0, 0.0, 0.0);
  drawGear(gear3, -3.1, -1.8, -2.2, 2.0 * (gear1Rot / 16.0) - 2.0);
  
  glPopMatrix();
}
#endif

void GLWidget::resizeGL(int w, int h){
  //int side = qMin(w, h);
  float aspect = (float)w/(float)h;
  if(perspective){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0*aspect*zoomFactor, 1.0*aspect*zoomFactor, 
	      -1.0*zoomFactor, 1.0*zoomFactor, 5.0, 600.0);
  }else{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5*aspect*zoomFactor, 5*aspect*zoomFactor, 
	    -5*zoomFactor, 5*zoomFactor, 5, 600);
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.0,0.0, -40.0);
}

void GLWidget::mousePressEvent(QMouseEvent* event){
  lastPos = event->pos();
  event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event){
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();
  
  if(event->buttons() & Qt::LeftButton){
    /*Rotate model*/
    setXRotation(xRot + 8 * dy);
    setYRotation(yRot + 8 * dx);
  }else if(event->buttons() & Qt::RightButton){
    /*Rotate model*/
    setXRotation(xRot + 8 * dy);
    setZRotation(zRot + 8 * dx);
  }else if(event->buttons() & Qt::MidButton){
    if(event->modifiers() == Qt::ControlModifier){
      
    }else{
      setXPosition(xPos + dx);
      setYPosition(yPos + dy);
    }
  }
  lastPos = event->pos();
  event->accept();
}

void GLWidget::wheelEvent(QWheelEvent* event){
  setZPosition(zPos + event->delta());
  event->accept();
}

void GLWidget::normalizeAngle(int* angle){
  while(*angle<0){
    *angle += 360 * 16;
  }
  while(*angle > 360 * 16){
    *angle -= 360 * 16;
  }
}
