#include "QT/MyControlTab.moc"
#include "QT/MyGLWidget.hpp"
#include <QGridLayout>
#include <QLabel>

MyControlTab::MyControlTab(MyGLWidget* glw, QWidget* parent):
  ControlTab(glw, parent){
  
  treeDepthSpin1 = new QSpinBox(this);
  treeDepthSpin1->setMinimum(0);
  treeDepthSpin1->setMaximum(20000);
  
  treeDepthSpin2 = new QSpinBox(this);
  treeDepthSpin2->setMinimum(-2);
  treeDepthSpin2->setMaximum(20000);
  treeDepthSpin2->setValue(-1);    
  
  treeDepthSpin3 = new QSpinBox(this);
  treeDepthSpin3->setMinimum(0);
  treeDepthSpin3->setMaximum(20000);
  
  treeDepthSpin4 = new QSpinBox(this);
  treeDepthSpin4->setMinimum(-2);
  treeDepthSpin4->setMaximum(20000);
  treeDepthSpin4->setValue(-1);
  
  treeDepthSpin5 = new QSpinBox(this);
  treeDepthSpin5->setMinimum(-2);
  treeDepthSpin5->setMaximum(20000);
  treeDepthSpin5->setValue(-1);
  
  boxCheckBox1 = new QCheckBox(tr("All"), this);
  boxCheckBox2 = new QCheckBox(tr("Colliding pairs"), this);
  depthCheckBox = new QCheckBox(tr("Depth buffer"), this);
  
  boxSizeSlider = new QSlider(Qt::Horizontal);
  boxSizeSlider->setRange(1, 1000);
  boxSizeSlider->setValue(500);
  
  updateButton = new QPushButton(tr("Update"), this);

  perturbButton = new QPushButton(tr("Perturb vertices"), this);
  perturbButton->setAutoRepeat(true);
  perturbButton->setAutoRepeatDelay(100);
  

  displX = new QSpinBox(this);
  displX->setMinimum(-100);
  displX->setMaximum( 100);
  displX->setValue(0);

  displY = new QSpinBox(this);
  displY->setMinimum(-100);
  displY->setMaximum( 100);
  displY->setValue(0);

  displZ = new QSpinBox(this);
  displZ->setMinimum(-100);
  displZ->setMaximum( 100);
  displZ->setValue(0);
  
  connect(treeDepthSpin1, SIGNAL(valueChanged(int)),
	  glw, SLOT(setTreeDepth1(int)));
  
  connect(treeDepthSpin2, SIGNAL(valueChanged(int)),
	  glw, SLOT(setTreeDepth2(int)));
  
  connect(treeDepthSpin3, SIGNAL(valueChanged(int)),
	  glw, SLOT(setTreeDepth3(int)));
  
  connect(treeDepthSpin4, SIGNAL(valueChanged(int)),
	  glw, SLOT(setTreeDepth4(int)));
  
  connect(treeDepthSpin5, SIGNAL(valueChanged(int)),
	  glw, SLOT(setTreeDepth5(int)));
  
  connect(boxCheckBox1, SIGNAL(stateChanged(int)),
	  glw, SLOT(drawBoundingBoxes1(int)));
  
  connect(boxCheckBox2, SIGNAL(stateChanged(int)),
	  glw, SLOT(drawBoundingBoxes2(int)));

  connect(depthCheckBox, SIGNAL(stateChanged(int)),
	  glw, SLOT(enableDepthBuffer(int)));
  
  connect(boxSizeSlider, SIGNAL(valueChanged(int)),
	  glWidget, SLOT(setBoxSize(int)));
  
  connect(updateButton, SIGNAL(released()),
	  glw, SLOT(update()));

  connect(perturbButton, SIGNAL(released()),
	  glw, SLOT(perturb()));

  connect(displX, SIGNAL(valueChanged(int)),
	  glw, SLOT(setDisplacementX(int)));

  connect(displY, SIGNAL(valueChanged(int)),
	  glw, SLOT(setDisplacementY(int)));

  connect(displZ, SIGNAL(valueChanged(int)),
	  glw, SLOT(setDisplacementZ(int)));
  
  /*Create new tab*/
  QWidget* appTab = new QWidget();
  QGridLayout* appLayout = new QGridLayout();
  
  appLayout->addWidget(new QLabel("depth 1"), 0 ,0);
  appLayout->addWidget(treeDepthSpin1, 0, 1);
  appLayout->addWidget(new QLabel("Vertex-Face"), 1 ,0);
  appLayout->addWidget(treeDepthSpin2, 1, 1);
  appLayout->addWidget(new QLabel("BVH depth"), 2 ,0);
  appLayout->addWidget(treeDepthSpin3, 2, 1);
  appLayout->addWidget(new QLabel("Face-Face"), 3 ,0);
  appLayout->addWidget(treeDepthSpin4, 3, 1);
  appLayout->addWidget(new QLabel("Edge-Edge"), 4 ,0);
  appLayout->addWidget(treeDepthSpin5, 4, 1);
  appLayout->addWidget(new QLabel("Draw bbox"), 5 ,0);
  appLayout->addWidget(boxCheckBox1, 5, 1);
  appLayout->addWidget(new QLabel("Draw bbox"), 6 ,0);
  appLayout->addWidget(boxCheckBox2, 6, 1);
  appLayout->addWidget(new QLabel("Depth buffer"), 7, 0);
  appLayout->addWidget(depthCheckBox, 7, 1);
  appLayout->addWidget(new QLabel("Displacement"), 8 ,0);
  appLayout->addWidget(boxSizeSlider, 8, 1);
  appLayout->addWidget(new QLabel("Update tree"), 9 ,0);
  appLayout->addWidget(updateButton, 9, 1);
  appLayout->addWidget(new QLabel("Perturb mesh"), 10 ,0);
  appLayout->addWidget(perturbButton, 10, 1);

  appLayout->addWidget(new QLabel("Displ X"), 11 ,0);
  appLayout->addWidget(displX, 11, 1);
  appLayout->addWidget(new QLabel("Displ Y"), 12 ,0);
  appLayout->addWidget(displY, 12, 1);
  appLayout->addWidget(new QLabel("Displ Z"), 13 ,0);
  appLayout->addWidget(displZ, 13, 1);

  appLayout->setRowStretch(14, 1);
  appLayout->setColumnStretch(2, 1);
  appTab->setLayout(appLayout);
  
  addTab(appTab, "Collisions");
}

MyControlTab::~MyControlTab(){
  
}
