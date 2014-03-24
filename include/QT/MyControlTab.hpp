#ifndef MYCONTROLTAB_HPP
#define MYCONTROLTAB_HPP

#include "QT/ControlTab.hpp"
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>

class MyGLWidget;

class MyControlTab : public ControlTab{
  Q_OBJECT
  public:
  MyControlTab(MyGLWidget* glWidget, QWidget* parent=0);
  virtual ~MyControlTab();
  
protected:
  QSpinBox* treeDepthSpin1;
  QSpinBox* treeDepthSpin2;
  QSpinBox* treeDepthSpin3;
  QSpinBox* treeDepthSpin4;
  QSpinBox* treeDepthSpin5;
  
  QCheckBox* boxCheckBox1;
  QCheckBox* boxCheckBox2;
  QCheckBox* depthCheckBox;
  
  QSlider*   boxSizeSlider;
  
  QPushButton* updateButton;
  QPushButton* perturbButton;

  QSpinBox* displX;
  QSpinBox* displY;
  QSpinBox* displZ;
};

#endif/*MYCONTROLTAB_HPP*/
