#ifndef CONTROLTAB_HPP
#define CONTROLTAB_HPP

#include <QTabWidget>
#include <QSlider>
#include <QPushButton>

#include "QT/GLWidget.hpp"

class ControlTab : public QTabWidget{
  Q_OBJECT
public:
  ControlTab(GLWidget* glWidget, QWidget* parent = 0);
  virtual ~ControlTab();
  
protected:
  QSlider* createSlider(const char* changedSignal,
			const char* setterSlot);
  
  QSlider* xSlider;
  QSlider* ySlider;
  QSlider* zSlider;
  
  QPushButton* togglePerspectiveButton;
  
  QSlider* zoomSlider;
  
  GLWidget* glWidget;
};

#endif/*CONTROLTAB_HPP*/
