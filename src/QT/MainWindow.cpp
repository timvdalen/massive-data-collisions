#include <QtGui>
#include <QtOpenGL>

#include "QT/MainWindow.hpp"
#include "QT/ControlTab.hpp"
#include "QT/GLWidget.hpp"

#include "QT/MainWindow.moc"

MainWindow::MainWindow(GLWidget* glw, ControlTab* tab, 
		       QWidget* parent):QMainWindow(parent), 
					glWidget(glw), 
					controlTab(tab){
  centralWidget = new QSplitter(Qt::Horizontal);
  setCentralWidget(centralWidget);
  
  glWidgetArea = new QScrollArea();
  glWidgetArea->setWidget(glWidget);
  glWidgetArea->setWidgetResizable(true);
  glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  glWidgetArea->setSizePolicy(QSizePolicy::Ignored, 
			      QSizePolicy::Ignored);
  
  glWidgetArea->setMinimumSize(500, 500);
    
  createActions();
  createMenus();
  
  QVBoxLayout* centralLayout = new QVBoxLayout();
  
  centralLayout->setSpacing(0);
  centralLayout->setContentsMargins(0, 0, 0, 0);
  
  centralWidget->addWidget(glWidgetArea);
  centralWidget->addWidget(controlTab);
  
  setWindowTitle(tr("Collisions"));
  resize(400, 300); 
}

MainWindow::~MainWindow(){
  
}

void MainWindow::about(){
  QMessageBox::about(this, tr("About"), tr("About"));
}

void MainWindow::createActions(){
  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
  
  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  
  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus(){
  fileMenu = menuBar()->addMenu(tr("&File"));

  fileMenu->addAction(exitAct);
  
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

