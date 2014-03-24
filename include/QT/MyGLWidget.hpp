#ifndef MYGLWIDGET_HPP
#define MYGLWIDGET_HPP

#include "QT/GLWidget.hpp"
#include "BVH.hpp"

class BVHVisual{
public:
  BVHVisual(BVH* bvh, Collisions* c);
  ~BVHVisual();

  void drawNode(int node, int grandParent, int depth, bool drawBoxes);

  void drawTree(int depth=0, bool drawBoxes = false);

  void drawBBox(int node);

  void drawBBox(const Box& box, bool visibility=false);

  void drawAllCollidingFaces(bool drawBoxes);
  void drawCollidingFace(int face, bool drawBoxes);
  
  void drawCollidingVertexFace(int vertex, bool drawBoxes, Vector& displ);
  void drawCollidingEdgeEdge(int edge, bool drawBoxes, Vector& displ);

  Vector* nodeColors;

  BVH* bvh;
  Collisions* col;

  bool depthBuffer;
};


class MyGLWidget : public GLWidget{
  Q_OBJECT
  public:
  MyGLWidget(const QGLFormat& fmt, BVH* bvh, Collisions* c, 
	     QWidget* parent = 0);
  MyGLWidget(BVH* bvh, Collisions* c, QWidget* parent = 0);
  virtual ~MyGLWidget();
    virtual void paintGL();
  virtual void initializeGL();
public slots:
  void setTreeDepth1(int);
  void setTreeDepth2(int);
  void setTreeDepth3(int);
  void setTreeDepth4(int);
  void setTreeDepth5(int);
  void drawBoundingBoxes1(int);
  void drawBoundingBoxes2(int);
  void enableDepthBuffer(int);
  void setBoxSize(int);
  void update();
  void perturb();
  void setDisplacementX(int);
  void setDisplacementY(int);
  void setDisplacementZ(int);
protected:
  int treeDepth1;
  int treeDepth2;
  int treeDepth3;
  int treeDepth4;
  int treeDepth5;
  bool drawBoxes1;
  bool drawBoxes2;
  int  boxSize;
  
  BVH* bvh;
  Collisions* c;
  
  BVHVisual* bvhVisual;
  Vector displacement;
};

#endif/*MYGLWIDGET_HPP*/
