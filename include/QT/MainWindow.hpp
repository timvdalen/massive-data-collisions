#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QSplitter>

class QAction;
class QLabel;
class QScrollArea;
class QMenu;
class QSlider;
  
class GLWidget;
class ControlTab;

class MainWindow : public QMainWindow{
  Q_OBJECT
public:
  MainWindow(GLWidget* glw, ControlTab* ctrl, QWidget* parent = 0);
  virtual ~MainWindow();
private slots:
  void about();
  
private:
  void createActions();
  void createMenus();
  QSize getSize();
  QSplitter* centralWidget;
  QScrollArea* glWidgetArea;
   GLWidget* glWidget;
  ControlTab* controlTab;
  
  QMenu* fileMenu;
  QMenu* helpMenu;
  
  QAction* fullScreenAct;
  QAction* exitAct;
  QAction* aboutAct;
  QAction* aboutQtAct;
};

#endif/*MAINWINDOW_HPP*/
