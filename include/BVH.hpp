#ifndef BVH_HPP
#define BVH_HPP

class Vertex{
public:
  float x, y, z;
};

typedef class Vertex Vector;

void vectorDot(float& r, const Vector& a, const Vector& b);

void vectorCross(Vector& r, const Vector& a, const Vector& b);

void vectorAdd(Vector& r, const Vector& a, const Vector& b);

void vectorSub(Vector& r, const Vector& a, const Vector& b);

void vectorMul(Vector& r, const Vector& a, float s);

void vectorDiv(Vector& r, const Vector& a, float s);

void vectorNormalize(Vector& r, const Vector& a);

class Edge{
public:
  int vertices[2];
};

/*edges are un ordered, faces are ordered*/
class Face{
public:
  int vertices[3];
  int edges[3];
};

class Mesh{
public:
  Mesh(int nv, int ne, int nf);
  ~Mesh();

  void perturbVertices(float magnitude, Vector& center);

  int nVertices;
  int nEdges;
  int nFaces;

  Vertex* vertices;
  Edge*   edges;
  Face*   faces;
};

class Box{
public:
  Box();
  Vector min, max;
  bool intersects(const Box& b)const;
  void addPoint(Vector& p);
  void reset();
};

class BHVNode{
public:
  int faceId;       /*Face associated with this node. -1 if node is an
		      internal node*/
  int parent;       /*Parent of the node*/
  int childs[2];    /*Childs of the node*/
  int visibility;   /*Stores the visibility bits*/
  int level;        /*Level in the tree*/
};

class Collisions;

class BVH{
public:
  /*Loads a ply file, creates a mesh and a Bounding Volume Hierarchy*/
  BVH(const char* meshFile);
  ~BVH();
  
  /*Updates the bounding volumes*/
  void update(const Vector& displacement);

  void updateNodeBBox(int curNode, const Vector& displacement);

  void updateNodeVisibility(int curNode);

  void checkFaceFaceCollisions(Collisions* c);

  bool checkCollisionsFace(const int curNode, const int queryNode,
			   Collisions* candidates);

  void checkCollisions(Collisions* c, const Vector& displacement);

  bool checkAdjacency(int node, int query);

  int rootNode;

  int* adjacentFaces; /*Stores per node all adjacent nodes*/
  int maxAdjacency;
  int nNodes;
  int* faceNodeMap; /*Mapping from vertexid to node id*/

  BHVNode* tree;       /*A full balanced tree containing all nodes in
			 breadth-first order*/
  Mesh* mesh;          /*mesh containing all vertices, edges and
			 faces*/
  Box* boxes;          /*All bboxes*/
};

enum CollisionType{VertexFace, EdgeEdge};

class CollisionPair{
public:
  CollisionType type;
  int objectA; /*If type is VertexFace, objectA represents the id of
		 the vertex. If type is EdgeEdge, it represents the id
		 of the first edge.*/
  int objectB; /*If type is VertexFace, objectB represents the id of
		 the face. If type is EdgeEdge, it represents the id
		 of the second edge.*/
};

class Collisions{
public:
  Collisions(const BVH* b, int max);
  ~Collisions();

  void initialize();
  void storeFaceFaceResult(int f1, int f2);
  void storeVertexFaceResult(int v1, int f1);
  void storeEdgeEdgeResult(int e1, int e2);

  void symmetrize();

  void breakDown(const BVH* bvh, const Vector& displacement);

  void extendStorage();

  int* potentialFaceFace;   /*Stores per face, all close-by faces*/
  int* nPotentialFaces;     /*Stores per face the ammount of close-by faces*/

  /*Break-down face-face into vertex-face and edge-edge pairs*/
  int* potentialVertexFace;  /*Stores per vertex all close-by faces*/
  int* nPotentialVertexFaces;/*Stores per vertex the ammount of close-by faces*/

  int* potentialEdgeEdge;   /*Stores per edge all close-by edges*/
  int* nPotentialEdgeEdges; /*Stores per edge the ammount of close-by edges*/

  CollisionPair* collisions;
  int nCollisions;
  int maxSize;

  int nFaces;
  int nVertices;
  int nEdges;
};



#endif/*BVH_HPP*/
