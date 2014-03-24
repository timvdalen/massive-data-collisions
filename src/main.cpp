#include "BVH.hpp"

#include <iostream>

#include <QApplication>
#include <QDesktopWidget>

#include "QT/MyGLWidget.hpp"
#include "QT/MyControlTab.hpp"
#include "QT/MainWindow.hpp"

int main(int argc, char** argv){
  if(argc == 2){

    BVH* bvh = new BVH(argv[1]);
    int maxSize = 5000;
    Collisions* collisions = new Collisions(bvh, maxSize);

    Vector displ;
    displ.x = displ.y = displ.z = 0;

    bvh->checkCollisions(collisions, displ);
    
    QApplication app(argc, argv);
    QDesktopWidget* desktop = QApplication::desktop();
    QGLFormat format;
    format.setDoubleBuffer(true);
    format.setDepth(true);
    format.setAlpha(true);
    format.setSampleBuffers(true);
    format.setSamples(8);
    
    MyGLWidget* glw = new MyGLWidget(format, bvh, collisions, desktop);
    MyControlTab* tab = new MyControlTab(glw, desktop);
    MainWindow window(glw, tab, desktop);
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int width = 250;
    int height = 150;
    
    int x = (screenWidth  - width ) / 2;
    int y = (screenHeight - height) / 2;
    
    window.resize(width, height);
    window.move(x, y);
    
    window.setWindowTitle("Collisions");
    window.show();
    
    app.exec();
    
    delete collisions;
    delete bvh;
  }else{
    std::cout << "Please specify an input file." << std::endl;
  }
  return 0;
}
