#include "QT/ControlTab.hpp"
#include "QT/ControlTab.moc"
#include <QGridLayout>
#include <QLabel>

ControlTab::ControlTab(GLWidget* glw, QWidget* parent):QTabWidget(parent){
  glWidget = glw;
  QWidget* centralWidget = new QWidget();
  //QHBoxLayout* innerLayout = new QHBoxLayout();
  //QWidget* innerWidget = new QWidget();
  
  //QVBoxLayout* centralLayout = new QVBoxLayout();
  QGridLayout* centralLayout = new QGridLayout();

  //centralLayout->addWidget(glWidgetArea, 0, 0);
  //centralLayout->addWidget(new ControlTab(), 0, 1);
  //centralLayout->addWidget(pixmapLabelArea, 0, 1);
  xSlider = createSlider(0, 0);
  ySlider = createSlider(0, 0);
  zSlider = createSlider(0, 0);
  
  togglePerspectiveButton = new QPushButton(tr("Perspective"));
  togglePerspectiveButton->setCheckable(true);
  togglePerspectiveButton->setChecked(true);
  
  zoomSlider = new QSlider(Qt::Horizontal);
  zoomSlider->setRange(1, 1000);
  zoomSlider->setValue(500);
  
  //zoomSlider->setSingleStep(1);
  //zoomSlider->setTickPosition(QSlider::TicksRight);
  
  ///////////////////////////////////////////////////////////////////////
  
  connect(xSlider, SIGNAL(valueChanged(int)),
	  glWidget, SLOT(setXRotation(int)));
  connect(glWidget, SIGNAL(xRotationChanged(int)),
	  xSlider, SLOT(setValue(int)));
  
  connect(ySlider, SIGNAL(valueChanged(int)),
	  glWidget, SLOT(setYRotation(int)));
  connect(glWidget, SIGNAL(yRotationChanged(int)),
	  ySlider, SLOT(setValue(int)));
  
  connect(zSlider, SIGNAL(valueChanged(int)),
	  glWidget, SLOT(setZRotation(int)));
  connect(glWidget, SIGNAL(zRotationChanged(int)),
	  zSlider, SLOT(setValue(int)));
  
  connect(togglePerspectiveButton, SIGNAL(toggled(bool)),
	  glWidget, SLOT(setPerspectiveRendering(bool)));
  
  connect(zoomSlider, SIGNAL(valueChanged(int)),
	  glWidget, SLOT(setZoomFactor(int)));
  
  ////////////////////////////////////////////////////////////////////////
  
  centralLayout->addWidget(new QLabel("X-axis"), 0, 0);
  centralLayout->addWidget(xSlider, 0, 1);
  centralLayout->addWidget(new QLabel("Y-axis"), 1, 0);
  centralLayout->addWidget(ySlider, 1, 1);
  centralLayout->addWidget(new QLabel("Z-axis"), 2, 0);
  centralLayout->addWidget(zSlider, 2, 1);
  centralLayout->addWidget(new QLabel("Persp/Ortho"), 3, 0);
  centralLayout->addWidget(togglePerspectiveButton, 3, 1);
  centralLayout->addWidget(new QLabel("Zoom"), 4, 0);
  centralLayout->addWidget(zoomSlider, 4, 1);
  centralLayout->setRowStretch(5, 1);
  centralLayout->setColumnStretch(1, 1);

  
  centralWidget->setLayout(centralLayout);
  
  //innerLayout->addWidget(centralWidget);
  //innerLayout->addStretch(1);
  
  //innerWidget->setLayout(innerLayout);
  
  addTab(centralWidget, "Camera");
  
  setMaximumWidth(250);
}

ControlTab::~ControlTab(){
  
}

QSlider* ControlTab::createSlider(const char* changedSignal,
				  const char* setterSlot){
  QSlider* slider = new QSlider(Qt::Horizontal);
  slider->setRange(0, 360 * 16);
  slider->setSingleStep(16);
  slider->setPageStep(15 * 16);
  slider->setTickInterval(15 * 16);
  slider->setTickPosition(QSlider::TicksRight);
  //connect(slider, SIGNAL(valueChanged(int)),
  //	  glWidget, setterSlot);
  
  //connect(glWidget, changedSignal, slider, SLOT(setValue(int)));
  
  return slider;
}
