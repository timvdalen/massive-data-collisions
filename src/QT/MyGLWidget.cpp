#include "QT/MyGLWidget.moc"
#include <iostream>
#include <cstdlib>

BVHVisual::BVHVisual(BVH* b, Collisions* c):bvh(b), col(c), 
					    depthBuffer(false){
  nodeColors = new Vector[bvh->nNodes];

  for(int i=0;i<bvh->nNodes;i++){
    nodeColors[i].x = (rand()%256)/(256.0f);
    nodeColors[i].y = (rand()%256)/(256.0f);
    nodeColors[i].z = (rand()%256)/(256.0f);
  }
}

BVHVisual::~BVHVisual(){
  delete [] nodeColors;
}

void BVHVisual::drawNode(int node, int grandParent, int depth, bool drawBoxes){
  if(node != -1){
    int level = bvh->tree[node].level;
    if(level == depth){
      grandParent = node;
      if(drawBoxes){
	glDisable(GL_LIGHTING);
	drawBBox(node);
	glEnable(GL_LIGHTING);
      }
    }

    if(bvh->tree[node].faceId != -1){
      /*Draw triangle*/
      int* vertices = bvh->mesh->faces[bvh->tree[node].faceId].vertices;

      Vector faceNormal;
      Vector v0 = bvh->mesh->vertices[vertices[0]];
      Vector v1 = bvh->mesh->vertices[vertices[1]];
      Vector v2 = bvh->mesh->vertices[vertices[2]];

      Vector ba;
      Vector ca;
      
      vectorSub(ba, v1, v0);
      vectorSub(ca, v2, v0);
      
      vectorCross(faceNormal, ba, ca);     

      Vector color;
      if(grandParent != -1){
	color = nodeColors[grandParent];
      }else{
	color.x = color.y = color.z = 1;
      }

      glBegin(GL_TRIANGLES);
      glColor3f(color.x, color.y, color.z);
      glNormal3f(faceNormal.x, faceNormal.y, faceNormal.z);      
      glVertex3f(v0.x, v0.y, v0.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glEnd();
    }else{
      int* childs = bvh->tree[node].childs;
      drawNode(childs[0], grandParent, depth, drawBoxes);
      drawNode(childs[1], grandParent, depth, drawBoxes);
    }
  }
}

void BVHVisual::drawBBox(const Box& box, bool visibility){
  Vector min, max;
  min = box.min;
  max = box.max;
  
  if(visibility){
    glColor3f(0,1,0);
  }else{
    glColor3f(1,0,0);
  }

  //glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  
  /*Bottom square*/
  glVertex3f(min.x, min.y, min.z); glVertex3f(max.x, min.y, min.z);
  glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, max.y, min.z);
  glVertex3f(max.x, max.y, min.z); glVertex3f(min.x, max.y, min.z);
  glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, min.y, min.z);
  
  /*Top square*/
  glVertex3f(min.x, min.y, max.z); glVertex3f(max.x, min.y, max.z);
  glVertex3f(max.x, min.y, max.z); glVertex3f(max.x, max.y, max.z);
  glVertex3f(max.x, max.y, max.z); glVertex3f(min.x, max.y, max.z);
  glVertex3f(min.x, max.y, max.z); glVertex3f(min.x, min.y, max.z);
  
  /*Connect squares*/
  /*Bottom square*/
  glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, min.y, max.z);
  glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, min.y, max.z);
  glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, max.y, max.z);
  glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, max.y, max.z);
  
  glEnd();
  //glEnable(GL_LIGHTING);
}

void BVHVisual::drawBBox(int node){
  bool hasVector = false;
  if(bvh->tree[node].visibility){
    hasVector = true;
  }
  
  drawBBox(bvh->boxes[node], hasVector);
}

void BVHVisual::drawTree(int depth, bool drawBoxes){
  drawNode(bvh->rootNode, -1, depth, drawBoxes);
}

void BVHVisual::drawAllCollidingFaces(bool drawBoxes){
  glDisable(GL_LIGHTING);
  if(depthBuffer == false){
    glDisable(GL_DEPTH_TEST);
  }

  for(int i=0;i<bvh->mesh->nFaces;i++){
    int nFaces = col->nPotentialFaces[i];
    if(nFaces != 0){
      for(int j=0;j<nFaces;j++){
	int colFace = col->potentialFaceFace[i * col->maxSize + j];
	//draw colFace

	if(drawBoxes){
	  glLineWidth(1);
	  drawBBox(bvh->faceNodeMap[colFace]);
	  glLineWidth(5);
	}
		
	int* vertices = bvh->mesh->faces[colFace].vertices;
	Vector v0 = bvh->mesh->vertices[vertices[0]];
	Vector v1 = bvh->mesh->vertices[vertices[1]];
	Vector v2 = bvh->mesh->vertices[vertices[2]];
	
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v0.x, v0.y, v0.z);
	glEnd();
      }

      if(drawBoxes){
	glLineWidth(1);
	drawBBox(bvh->faceNodeMap[i]);
	glLineWidth(5);
      }
      
      glColor3f(1,0,0);
      //draw face
      
      int* vertices = bvh->mesh->faces[i].vertices;
      Vector v0 = bvh->mesh->vertices[vertices[0]];
      Vector v1 = bvh->mesh->vertices[vertices[1]];
      Vector v2 = bvh->mesh->vertices[vertices[2]];
      
      glBegin(GL_LINES);
      glVertex3f(v0.x, v0.y, v0.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v0.x, v0.y, v0.z);
      glEnd();
    }
  }
  if(depthBuffer == false){
    glEnable(GL_DEPTH_TEST);
  }
  glEnable(GL_LIGHTING);
}

void BVHVisual::drawCollidingFace(int colFace, bool drawBoxes){
  int face = -1;

  int curFace = -1;
  /*Not very efficient*/
  for(int i=0;i<bvh->mesh->nFaces;i++){
    if(col->nPotentialFaces[i] != 0){
      curFace++;

      if(curFace == colFace){
	face = i;
	break;
      }
    }
  }
  

  if(face != -1 && face < bvh->mesh->nFaces){
    int nFaces = col->nPotentialFaces[face];

    glDisable(GL_LIGHTING);
    if(depthBuffer == false){
      glDisable(GL_DEPTH_TEST);
    }

    for(int i=0;i<nFaces;i++){
      int colFace = col->potentialFaceFace[face * col->maxSize + i];
      //draw colFace

      if(drawBoxes){
	glLineWidth(1);
	drawBBox(bvh->faceNodeMap[colFace]);
	glLineWidth(5);
      }
      
      int* vertices = bvh->mesh->faces[colFace].vertices;
      Vector v0 = bvh->mesh->vertices[vertices[0]];
      Vector v1 = bvh->mesh->vertices[vertices[1]];
      Vector v2 = bvh->mesh->vertices[vertices[2]];

      glColor3f(0,1,0); 
     
      glBegin(GL_LINES);
      glVertex3f(v0.x, v0.y, v0.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v0.x, v0.y, v0.z);
      glEnd();
    }

    if(drawBoxes){
      glLineWidth(1);
      drawBBox(bvh->faceNodeMap[face]);
      glLineWidth(5);
    }

    glColor3f(1,0,0);
    //draw face

    int* vertices = bvh->mesh->faces[face].vertices;
    Vector v0 = bvh->mesh->vertices[vertices[0]];
    Vector v1 = bvh->mesh->vertices[vertices[1]];
    Vector v2 = bvh->mesh->vertices[vertices[2]];
    
    glBegin(GL_LINES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v0.x, v0.y, v0.z);
    glEnd();

    if(depthBuffer == false){
      glEnable(GL_DEPTH_TEST);
    }
    glEnable(GL_LIGHTING);
  }
}

void BVHVisual::drawCollidingVertexFace(int colVertex, bool drawBoxes, 
					Vector& displacement){
  int vertex = -1;

  int curVertex = -1;
  /*Not very efficient*/
  for(int i=0;i<bvh->mesh->nVertices;i++){
    if(col->nPotentialVertexFaces[i] != 0){
      curVertex++;

      if(curVertex == colVertex){
	vertex = i;
	break;
      }
    }
  }

  if(vertex != -1 && vertex < bvh->mesh->nFaces){
    int nFaces = col->nPotentialVertexFaces[vertex];

    glDisable(GL_LIGHTING);
    if(depthBuffer == false){
      glDisable(GL_DEPTH_TEST);
    }
    
    for(int i=0;i<nFaces;i++){
      int colFace = col->potentialVertexFace[vertex * col->maxSize + i];
      //draw colFace

      if(drawBoxes){
	glLineWidth(1);
	drawBBox(bvh->faceNodeMap[colFace]);
	glLineWidth(5);
      }
      
      int* vertices = bvh->mesh->faces[colFace].vertices;
      Vector v0 = bvh->mesh->vertices[vertices[0]];
      Vector v1 = bvh->mesh->vertices[vertices[1]];
      Vector v2 = bvh->mesh->vertices[vertices[2]];

      glColor3f(0,1,0); 
     
      glBegin(GL_LINES);
      glVertex3f(v0.x, v0.y, v0.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v0.x, v0.y, v0.z);
      glEnd();
    }

    Vector v0 = bvh->mesh->vertices[vertex];

    if(drawBoxes){
      glLineWidth(1);
      Box box;
      box.reset();
      box.addPoint(v0);
      Vector p1;
      vectorAdd(p1, v0, displacement);
      box.addPoint(p1);

      drawBBox(box);
      glLineWidth(5);
    }

    glColor3f(1,0,0);
    //draw vertex

    glBegin(GL_POINTS);
    glVertex3f(v0.x, v0.y, v0.z);
    glEnd();

    if(depthBuffer == false){
      glEnable(GL_DEPTH_TEST);
    }
    glEnable(GL_LIGHTING);
  }
}

void BVHVisual::drawCollidingEdgeEdge(int colEdge, bool drawBoxes, 
					Vector& displacement){
  int edge = -1;

  int curEdge = -1;
  /*Not very efficient*/
  for(int i=0;i<bvh->mesh->nEdges;i++){
    if(col->nPotentialEdgeEdges[i] != 0){
      curEdge++;

      if(curEdge == colEdge){
	edge = i;
	break;
      }
    }
  }

  if(edge != -1 && edge < bvh->mesh->nEdges){
    int nEdges = col->nPotentialEdgeEdges[edge];

    glDisable(GL_LIGHTING);
    if(depthBuffer == false){
      glDisable(GL_DEPTH_TEST);
    }
    
    for(int i=0;i<nEdges;i++){
      int cEdge = col->potentialEdgeEdge[edge * col->maxSize + i];
      //draw colFace
      
      int* vertices = bvh->mesh->edges[cEdge].vertices;
      Vector v0 = bvh->mesh->vertices[vertices[0]];
      Vector v1 = bvh->mesh->vertices[vertices[1]];

      if(drawBoxes){
	glLineWidth(1);
	Box box;
	box.reset();
	box.addPoint(v0);
	box.addPoint(v1);
	
	Vector p1, p2;
	vectorAdd(p1, v0, displacement);
	vectorAdd(p2, v1, displacement);
	box.addPoint(p1);
	box.addPoint(p2);

	drawBBox(box);
	glLineWidth(5);
      }
 
      glColor3f(0,1,0); 
     
      glBegin(GL_LINES);
      glVertex3f(v0.x, v0.y, v0.z);
      glVertex3f(v1.x, v1.y, v1.z);
      glEnd();
    }

    int* vertices = bvh->mesh->edges[edge].vertices;
    Vector v0 = bvh->mesh->vertices[vertices[0]];
    Vector v1 = bvh->mesh->vertices[vertices[1]];

    if(drawBoxes){
      glLineWidth(1);
      Box box;
      box.reset();
      box.addPoint(v0);
      box.addPoint(v1);
      Vector p1, p2;
      vectorAdd(p1, v0, displacement);
      vectorAdd(p2, v1, displacement);
      box.addPoint(p1);
      box.addPoint(p2);

      drawBBox(box);
      glLineWidth(5);
    }

    glColor3f(1,0,0);
    //draw vertex
    
    glBegin(GL_LINES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glEnd();
    
    if(depthBuffer == false){
      glEnable(GL_DEPTH_TEST);
    }
    glEnable(GL_LIGHTING);
  }
}


MyGLWidget::MyGLWidget(const QGLFormat& format,
		       BVH* b, Collisions* col,
		       QWidget* parent):GLWidget(format, parent), 
					treeDepth1(0),
					treeDepth2(-1),
					treeDepth3(0),
					treeDepth4(-1),
					treeDepth5(-1),
					drawBoxes1(false),
					drawBoxes2(false),
					bvh(b),
					c(col){
  boxSize = 500;
  bvhVisual = new BVHVisual(bvh, col);
					}

MyGLWidget::MyGLWidget(BVH* b, Collisions* col,
		       QWidget* parent):GLWidget(parent), 
					treeDepth1(0),
					treeDepth2(-1),
					treeDepth3(0),
					treeDepth4(-1),
					treeDepth5(-1),
					drawBoxes1(false),
					drawBoxes2(false),
					bvh(b),
					c(col){
  boxSize = 500;
  bvhVisual = new BVHVisual(bvh, col);
					}

MyGLWidget::~MyGLWidget(){
  delete bvhVisual;
}

void MyGLWidget::setTreeDepth1(int d){
  treeDepth1 = d;
  updateGL();
}

void MyGLWidget::setTreeDepth2(int d){
  treeDepth2 = d;
  updateGL();
}
void MyGLWidget::setTreeDepth3(int d){
  treeDepth3 = d;
  updateGL();
}
void MyGLWidget::setTreeDepth4(int d){
  treeDepth4 = d;
  updateGL();
}
void MyGLWidget::setTreeDepth5(int d){
  treeDepth5 = d;
  updateGL();
}

void MyGLWidget::drawBoundingBoxes1(int state){
  if(state == 0){
    drawBoxes1 = false;
  }else{
    drawBoxes1 = true;
  }
  updateGL();
}

void MyGLWidget::drawBoundingBoxes2(int state){
  if(state == 0){
    drawBoxes2 = false;
  }else{
    drawBoxes2 = true;
  }
  updateGL();
}

void MyGLWidget::enableDepthBuffer(int state){
  if(state == 0){
    bvhVisual->depthBuffer = false;
  }else{
    bvhVisual->depthBuffer = true;
  }
  updateGL();
}

void MyGLWidget::setBoxSize(int size){
  boxSize = size;
  updateGL();
}

void MyGLWidget::update(){
  bvh->update(displacement);
  bvh->checkCollisions(c, displacement);

  updateGL();
}

void MyGLWidget::perturb(){
  float magnitude = boxSize/100000.0f;
  Vector center = bvh->boxes[bvh->rootNode].min;
  vectorAdd(center, center, bvh->boxes[bvh->rootNode].max);
  vectorDiv(center, center, 2.0);

  bvh->mesh->perturbVertices(magnitude, center);
  
  updateGL();
}

void MyGLWidget::setDisplacementX(int displ){
  displacement.x = displ/1000.0f;
}

void MyGLWidget::setDisplacementY(int displ){
  displacement.y = displ/1000.0f;
}

void MyGLWidget::setDisplacementZ(int displ){
  displacement.z = displ/1000.0f;
}

void MyGLWidget::paintGL(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glPushMatrix();
  glTranslatef(xPos/32.0, -yPos/32.0, zPos/32.0);
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);  
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

  bvhVisual->drawTree(treeDepth3, drawBoxes1);

  glLineWidth(5);
  glPointSize(10);

  if(treeDepth4 >= 0){
    bvhVisual->drawCollidingFace(treeDepth4, drawBoxes2);
  }
  if(treeDepth4 == -2){
    bvhVisual->drawAllCollidingFaces(drawBoxes2);
  } 

  if(treeDepth2 >= 0){
    bvhVisual->drawCollidingVertexFace(treeDepth2, drawBoxes2, displacement);
  }
  if(treeDepth2 == -2){
    //bvhVisual->drawAllCollidingFaces(drawBoxes2);
  }

  if(treeDepth5 >= 0){
    bvhVisual->drawCollidingEdgeEdge(treeDepth5, drawBoxes2, displacement);
  }
  if(treeDepth5 == -2){
    //bvhVisual->drawAllCollidingFaces(drawBoxes2);
  }

  glLineWidth(1);
  glPopMatrix();
}

float white[] = {0.7,0.7,0.7,1};
float ambient[] = {0.2,0.2,0.2,2};
float poss[] = {-1200,1200,1200,1};
float specular[] = {0.7,0.7,0.7};
float shininess[] = {2,2,2,2};

void MyGLWidget::initializeGL(){
  GLWidget::initializeGL();
  glClearColor(0.2, 0.2, 0.2, 0.0);
  glShadeModel(GL_SMOOTH);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_MULTISAMPLE);
  
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, poss);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  
  glMaterialf(GL_FRONT, GL_SHININESS, 16);
  
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  
  glEnable(GL_LIGHTING);
}

  
