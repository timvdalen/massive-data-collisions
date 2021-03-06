#include "BVH.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <math.h>
#include <ctime>
#include "cuPrintf.itiscu"
#include <sys/time.h>
#include <cassert>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
	   if (code != cudaSuccess) 
		      {
			            fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
				          if (abort) exit(code);
					     }
}
void vectorDot(float& r, const Vector& a, const Vector& b){
  r = a.x * b.x + a.y * b.y + a.z * b.z;
}

void vectorCross(Vector& r, const Vector& a, const Vector& b){
  r.x = a.y * b.z - a.z * b.y;
  r.y = a.z * b.x - a.x * b.z;
  r.z = a.x * b.y - a.y * b.x;
}

__host__ __device__ void vectorAdd(Vector& r, const Vector& a, const Vector& b){
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  r.z = a.z + b.z;
}

void vectorSub(Vector& r, const Vector& a, const Vector& b){
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  r.z = a.z - b.z;
}

void vectorMul(Vector& r, const Vector&a, float s){
  r.x = a.x * s;
  r.y = a.y * s;
  r.z = a.z * s;
}

void vectorDiv(Vector& r, const Vector&a, float s){
  r.x = a.x / s;
  r.y = a.y / s;
  r.z = a.z / s;
}

void vectorNormalize(Vector& r, const Vector& a){
  float length;
  vectorDot(length, a, a);
  vectorDiv(r, a, sqrt(length));
}

float maxf(float a, float b){
  if(a>b){
	return a;
  }
  return b;
}


float minf(float a, float b){
  if(a<b){
	return a;
  }
  return b;
}

Mesh::Mesh(int nv, int ne, int nf){
  nVertices = nv;
  nEdges = ne;
  nFaces = nf;  

  vertices = new Vertex[nVertices];
  edges    = new Edge[nEdges];
  faces    = new Face[nFaces];
}

Mesh::~Mesh(){
  delete[] vertices;
  delete[] edges;
  delete[] faces;
}

void Mesh::perturbVertices(float magnitude, Vector& center){
  Vertex* displacement = new Vertex[nVertices];
  int*    count        = new int[nVertices];
  /*Reset displacement*/
  for(int i=0;i<nVertices;i++){
	displacement[i].x = 0;
	displacement[i].y = 0;
	displacement[i].z = 0;
	count[i]          = 0;
  }
  
  /*Displace faces along its distance vector from the center of the
	object*/
  for(int i=0;i<nFaces;i++){
	float perturbation;
	perturbation = magnitude*(rand()%RAND_MAX)/((float)RAND_MAX)-magnitude/2.25;

	Vector p1, p2, p3;
	Vector faceCenter;
	int* indices = faces[i].vertices;

	p1 = vertices[indices[0]];
	p2 = vertices[indices[1]];
	p3 = vertices[indices[2]];

	/*Compute center of face*/
	vectorAdd(faceCenter, p1, p2);
	vectorAdd(faceCenter, faceCenter, p3);
	vectorDiv(faceCenter, faceCenter, 3.0);

	vectorSub(faceCenter, faceCenter, center);

	vectorMul(faceCenter, faceCenter, perturbation);

	for(int j=0;j<3;j++){
	  vectorAdd(displacement[indices[j]], displacement[indices[j]], 
		faceCenter);
	  count[indices[j]]++;
	}
  }

  /*Displace vertices in model*/
  for(int i=0;i<nVertices;i++){
	vectorDiv(displacement[i], displacement[i], (float)count[i]);
	vectorAdd(vertices[i], displacement[i], vertices[i]);
  }

  delete[] displacement;
}

BVH::BVH(const char* fileName){
  mesh = 0;
  tree = 0;
  boxes = 0;
  adjacentFaces = 0;
  faceNodeMap = 0;

  std::ifstream in(fileName);
  
  if(in.is_open() ) {
	/*Read triangular mesh from file*/

	int n_vertices, n_edges, n_faces;
	
	in >> n_vertices >> n_edges >> n_faces;
	 
	mesh = new Mesh(n_vertices, n_edges, n_faces);
	
	for(int i=0;i<n_vertices;i++){
	  in >> mesh->vertices[i].x
	 >> mesh->vertices[i].y
	 >> mesh->vertices[i].z;
	}

	for(int i=0;i<n_edges;i++){
	  in >> mesh->edges[i].vertices[0] >> mesh->edges[i].vertices[1];
	}

	for(int i=0;i<n_faces;i++){
	  in >> mesh->faces[i].vertices[0]
	 >> mesh->faces[i].vertices[1]
	 >> mesh->faces[i].vertices[2]
	 >> mesh->faces[i].edges[0]
	 >> mesh->faces[i].edges[1]
	 >> mesh->faces[i].edges[2];
	}
	
	/*Read BVH*/
	in >> rootNode;
	in >> nNodes;
	in >> maxAdjacency;

	tree = new BHVNode[nNodes];
	boxes = new Box[nNodes];
	adjacentFaces = new int[nNodes * (maxAdjacency + 1)];

	faceNodeMap = new int[n_faces];

	/*Read nodes*/
	for(int i=0;i<nNodes;i++){
	  in >> tree[i].faceId 
	 >> tree[i].parent
	 >> tree[i].childs[0]
	 >> tree[i].childs[1]
	 >> tree[i].level;      
	  
	  if(tree[i].faceId != -1){
	faceNodeMap[tree[i].faceId] = i;
	  }
	}

	/*Read adjacency information*/
	int offset = maxAdjacency + 1;
	for(int i=0;i<nNodes;i++){
	  /*Read ammount of adjacent faces*/
	  in >> adjacentFaces[i*offset];

	  /*Read adjacent faces*/
	  for(int j=0;j<adjacentFaces[i*offset];j++){
	in >> adjacentFaces[i*offset+j+1];
	  }
	}

	/*Update BBox and visibility*/
	Vector displacement;
	displacement.x = displacement.y = displacement.z = 0;

	update(displacement);
  }else{
	std::cout << "Could not open file:: " << fileName << std::endl;
  }
}

BVH::~BVH(){
  if(mesh){
	delete mesh;
  }
  if(tree){
	delete[] tree;
  }
  if(adjacentFaces){
	delete[] adjacentFaces;
  }
  if(faceNodeMap){
	delete[] faceNodeMap;
  }
  if(boxes){
	delete[] boxes;
  }
}

Box::Box(){
  reset();
}

bool Box::intersects(const Box& b)const{
  if(!( (b.min.x <= max.x) && (b.max.x >= min.x) ))
	return false;
  
  if(!( (b.min.y <= max.y) && (b.max.y >= min.y) ))
	return false;
  
  if(!( (b.min.z <= max.z) && (b.max.z >= min.z) ))
	return false;
  
  return true;
}

void Box::reset(){
  min.x = min.y = min.z =  1E100;
  max.x = max.y = max.z = -1E100;
}

void Box::addPoint(Vector& p){
  if(min.x > p.x){
	min.x = p.x;
  }
  if(max.x < p.x){
	max.x = p.x;
  }

  if(min.y > p.y){
	min.y = p.y;
  }
  if(max.y < p.y){
	max.y = p.y;
  }

  if(min.z > p.z){
	min.z = p.z;
  }
  if(max.z < p.z){
	max.z = p.z;
  }
}

int binarySearch(int* A, int key, int imin, int imax){
  while(imax >= imin){
	int imid = imin + ((imax - imin) / 2);

	if(A[imid] < key){
	  imin = imid + 1;
	}else if(A[imid] > key){
	  imax = imid - 1;
	}else{
	  return imid;
	}
  }

  return -1;
}

void BVH::update(const Vector& displacement){
  /*Update all bounding boxes given a displacement*/
  updateNodeBBox(rootNode, displacement);

  /*Update visibility-bits*/
  updateNodeVisibility(rootNode);
}

/*Recursively update bounding-boxes*/
void BVH::updateNodeBBox(int curNode, const Vector& displacement){
  if(curNode == -1){
	return;
  }
  
  if(tree[curNode].faceId == -1){
	/*First update bounding box of children*/
	
	updateNodeBBox(tree[curNode].childs[0], displacement);
	updateNodeBBox(tree[curNode].childs[1], displacement);
	
	/*Add min and max points of child boxes to current box*/
	boxes[curNode].reset();
	int* childs = tree[curNode].childs; 
	if(childs[0] != -1){
	  boxes[curNode].addPoint(boxes[childs[0]].min);
	  boxes[curNode].addPoint(boxes[childs[0]].max);
	}
	
	if(childs[1] != -1){
	  boxes[curNode].addPoint(boxes[childs[1]].min);
	  boxes[curNode].addPoint(boxes[childs[1]].max);
	}
  }else{
	/*Update the bounding box according to the stored geometry*/
	boxes[curNode].reset();
	int* vertices = mesh->faces[tree[curNode].faceId].vertices;
	boxes[curNode].addPoint(mesh->vertices[vertices[0]]);
	boxes[curNode].addPoint(mesh->vertices[vertices[1]]);
	boxes[curNode].addPoint(mesh->vertices[vertices[2]]);

	/*Add displacement*/
	Vector p1, p2;
	vectorAdd(p1, boxes[curNode].min, displacement);
	vectorAdd(p2, boxes[curNode].max, displacement);

	boxes[curNode].addPoint(p1);
	boxes[curNode].addPoint(p2);
  }  
}

/*Recursively update visibility-bits*/
void BVH::updateNodeVisibility(int curNode){
  if(curNode == -1){
	return;
  }

  tree[curNode].visibility = 0;

  if(tree[curNode].faceId != -1){
	/*Determine visibility of a triangle*/

	int* vertices = mesh->faces[tree[curNode].faceId].vertices;

	Vector faceNormal;
	Vector ba;
	Vector ca;

	vectorSub(ba, mesh->vertices[vertices[1]], mesh->vertices[vertices[0]]);
	vectorSub(ca, mesh->vertices[vertices[2]], mesh->vertices[vertices[0]]);

	vectorCross(faceNormal, ba, ca);

	for(int i=0;i<3;i++){
	  for(int j=0;j<3;j++){
	for(int k=0;k<3;k++){
	  if((i == 1) && (j == 1) && (k == 1)){
		/*Skip, center*/
	  }else{
		Vector testVector;
		testVector.x = 1-i;
		testVector.y = 1-j;
		testVector.z = 1-k;

		float dot;
		vectorDot(dot, testVector, faceNormal);

		if(dot >= 0){
		  tree[curNode].visibility |= 1 << (i*9+j*3+k);
		}
	  }
	}
	  }
	}
  }else{
	/*Determine visibility by AND-ing visibility-bits of childs*/

	int* childs = tree[curNode].childs;
	updateNodeVisibility(childs[0]);
	updateNodeVisibility(childs[1]);

	if(childs[0] != -1 && childs[1] != -1){
	  tree[curNode].visibility = (tree[childs[0]].visibility &
				  tree[childs[1]].visibility);
	}else{
	  if(childs[0] != -1){
	tree[curNode].visibility = tree[childs[0]].visibility;
	  }
	  if(childs[1] != -1){
	tree[curNode].visibility = tree[childs[1]].visibility;
	  }
	}
  }
}

bool BVH::checkAdjacency(int node, int query){
  int index = binarySearch(adjacentFaces + node*(maxAdjacency+1), query, 
			   1, adjacentFaces[node*(maxAdjacency+1)] + 1);

  if(index == -1){
	return false;
  }
  /*Query-node is adjacent to node*/
  return true;
}

/*Finds all nodes in the tree that may intersect with the given face*/
bool BVH::checkCollisionsFace(const int curNode, const int queryNode, 
				  Collisions* candidates){

  /*A face can't collide with itself*/
  if(curNode == queryNode){
	return false;
  }

  /*Check if the query is valid*/
  if(tree[queryNode].faceId == -1){
	return false;
  }
  
  if(boxes[curNode].intersects(boxes[queryNode])){
	/*Possible collision, since bounding boxes intersect*/

	int child = queryNode;

	/*Traverse the tree to the root until the current node has no
	  parent*/
	while(true){
	  if(child == curNode){
	if(tree[curNode].visibility){
	  return false;
	}
	break;
	  }
	  if(child == -1){
	/*child is the root of an object (in case of multiple objects,
	  this is not the root of the tree)*/
	break;
	  }
	  if(tree[curNode].level == tree[child].level){
	if(checkAdjacency(curNode, child)){
	  if(tree[curNode].visibility & tree[child].visibility){
		return false;
	  }
	}else{
	  /*Nodes are not adjacent*/
	}
	break;
	  }

	  child = tree[child].parent;
	}
	
	if(tree[curNode].faceId != -1){
	  /*Store result in candidates*/
	  candidates->storeFaceFaceResult(tree[queryNode].faceId, 
					  tree[curNode].faceId); 
	}else{
	  bool c1 = false;
	  bool c2 = false;
	  if(tree[curNode].childs[0] != -1){
	c1 = checkCollisionsFace(tree[curNode].childs[0], queryNode, 
				 candidates);
	  }
	  if(tree[curNode].childs[1] != -1){
	c2 = checkCollisionsFace(tree[curNode].childs[1], queryNode, 
				 candidates);
	  }
	  return c1 || c2;
	}
  }
  return false;
}

void BVH::checkFaceFaceCollisions(Collisions* c){
  for(int i=0;i<mesh->nFaces;i++){ 
	int queryNode = faceNodeMap[i];
	checkCollisionsFace(rootNode, queryNode, c); 
  }
}

void BVH::checkCollisions(Collisions* c, const Vector& displacement){
  /*Initialize / clear the stored collisions*/
  c->initialize();
  
  checkFaceFaceCollisions(c);


  c->breakDown(this, displacement);


}

Collisions::Collisions(const BVH* b, int max){
  maxSize = max;

  nFaces    = b->mesh->nFaces;
  nVertices = b->mesh->nVertices;
  nEdges    = b->mesh->nEdges;

  if(b->mesh){  
	potentialFaceFace     = new int[nFaces * maxSize];
	nPotentialFaces       = new int[nFaces];

	potentialVertexFace   = new int[nVertices * maxSize];
	nPotentialVertexFaces = new int[nVertices];
	
	potentialEdgeEdge     = new int[nEdges * maxSize];
	nPotentialEdgeEdges   = new int[nEdges];
  }else{
	potentialFaceFace     = 0;
	nPotentialFaces       = 0;

	potentialVertexFace   = 0;
	nPotentialVertexFaces = 0;

	potentialEdgeEdge     = 0;
	nPotentialEdgeEdges   = 0;
  }
}

Collisions::~Collisions(){
  if(potentialFaceFace){
	delete[] potentialFaceFace;
	delete[] nPotentialFaces;
	delete[] potentialVertexFace;
	delete[] nPotentialVertexFaces;
	delete[] potentialEdgeEdge;
	delete[] nPotentialEdgeEdges;
  }
}

/*Resets all collisions*/
void Collisions::initialize(){  
  for(int i=0;i<nFaces;i++){
	nPotentialFaces[i] = 0;
  }
  
  for(int i=0;i<nVertices;i++){
	nPotentialVertexFaces[i] = 0;
  }

  for(int i=0;i<nEdges;i++){
	nPotentialEdgeEdges[i] = 0;
  }
}

void Collisions::storeFaceFaceResult(int f1, int f2){
  int index = nPotentialFaces[f1];

  if(index >= maxSize){
	/*Array is to small to store the results*/
	//extendStorage();
	std::cout << "maxSize is too small for storing all results" << std::endl;
  }else{
	potentialFaceFace[f1*maxSize + index] = f2;
	nPotentialFaces[f1]++;
  }
}

void Collisions::storeVertexFaceResult(int v, int f){
  int index = nPotentialVertexFaces[v];

  if(index >= maxSize){
	/*Array is to small to store the results*/
	//extendStorage();
	std::cout << "maxSize is too small for storing all results" << std::endl;
  }else{  
	potentialVertexFace[v*maxSize + index] = f;
	nPotentialVertexFaces[v]++;
  }
}

void Collisions::storeEdgeEdgeResult(int e1, int e2){
  int index = nPotentialEdgeEdges[e1];

  if(index >= maxSize){
	/*Array is to small to store the results*/
	//extendStorage();
	std::cout << "maxSize is too small for storing all results" << std::endl;
  }else{ 
	potentialEdgeEdge[e1*maxSize + index] = e2;
	nPotentialEdgeEdges[e1]++;
  }
}


__global__ void breakDownDeel1(int nFaces, int maxSize, int* nPotFace, int* potFaceFace, int* nVFOutput, int* nEEOutput, Vector* displacement, Vertex* vertices, Edge* edges, Face* faces, Box* boxes, int* VFOutput, int* EEOutput, int splitfactor, bool last)
{
	int faceA = (blockDim.x * blockIdx.x + threadIdx.x);
	int j = (blockDim.y * blockIdx.y + threadIdx.y);
	//cuPrintf("blockDim.x: %d, blockIdx.x: %d, threadIdx.x: %d\n", blockDim.x, blockIdx.x, threadIdx.x);
	//cuPrintf("blockDim.y: %d, blockIdx.y : %d, threadIdx.y : %d\n", blockDim.y, blockIdx.y, threadIdx.y);
	//cuPrintf("faceA : %d, j : %d\n", faceA, j);
    if ((faceA < nFaces/splitfactor && !last) || (faceA < nFaces%splitfactor && last)) {
		int nPairs = nPotFace[faceA];
		if(j < nPairs) {
			int faceB = potFaceFace[faceA * maxSize + j];
			
			int* localv = faces[faceA].vertices;
			int* localeA = faces[faceA].edges;
			int* localeB = faces[faceB].edges;
			
			for(int vertex=0; vertex<3; vertex++) {
				Box vertexBox;
				Vector v0 = vertices[localv[vertex]];
				vertexBox.addPoint(v0);
				Vector p1;
				vectorAdd(p1, v0, *displacement);
				vertexBox.addPoint(p1);

				if(vertexBox.intersects(boxes[faceB])){
					int index = atomicAdd(&nVFOutput[localv[vertex]],1);
					
					if(index < maxSize) {
						VFOutput[localv[vertex]*maxSize + index] = faceB;
					}
				}
			}
			
			for(int edgeA=0; edgeA<3; edgeA++) {
				int* verticesEdgeA = edges[localeA[edgeA]].vertices;
	
				Box edgeBoxA;

				Vector v0 = vertices[verticesEdgeA[0]];
				Vector v1 = vertices[verticesEdgeA[1]];

				edgeBoxA.addPoint(v0);
				edgeBoxA.addPoint(v1);

				Vector p1, p2;
				vectorAdd(p1, v0, *displacement);
				vectorAdd(p2, v1, *displacement);
				edgeBoxA.addPoint(p1);
				edgeBoxA.addPoint(p2);
	
				for(int edgeB=0;edgeB<3;edgeB++){
					if((localeA[edgeA] < localeB[edgeB] && (localeA[edgeA] + localeB[edgeB]) % 2 == 0) ||
					(localeA[edgeA] >= localeB[edgeB] && (localeA[edgeA] + localeB[edgeB]) % 2 == 1)){
						int* verticesEdgeB = edges[localeB[edgeB]].vertices;

						Box edgeBoxB;
						Vector v2 = vertices[verticesEdgeB[0]];
						Vector v3 = vertices[verticesEdgeB[1]];

						edgeBoxB.addPoint(v2);
						edgeBoxB.addPoint(v3);
	    
						vectorAdd(p1, v2, *displacement);
						vectorAdd(p2, v3, *displacement);

						edgeBoxB.addPoint(p1);
						edgeBoxB.addPoint(p2);

						if(edgeBoxA.intersects(edgeBoxB)){
							int index = atomicAdd(&nEEOutput[localeA[edgeA]],1);
							
							if(index < maxSize) {
								EEOutput[localeA[edgeA]*maxSize + index] = localeB[edgeB];
							}
						}
					}
				}
			}
		}
	}   
}

void Collisions::breakDown(const BVH* bvh, const Vector& displacement){  
    // preprocess bounding boxes to save space
	Box* faceBoxes = new Box[nFaces];
	for(int i=0; i < nFaces; i++){
		faceBoxes[i].min.x = bvh->boxes[bvh->faceNodeMap[i]].min.x;
		faceBoxes[i].min.y = bvh->boxes[bvh->faceNodeMap[i]].min.y;
		faceBoxes[i].min.z = bvh->boxes[bvh->faceNodeMap[i]].min.z;
		faceBoxes[i].max.x = bvh->boxes[bvh->faceNodeMap[i]].max.x;
		faceBoxes[i].max.y = bvh->boxes[bvh->faceNodeMap[i]].max.y;
		faceBoxes[i].max.z = bvh->boxes[bvh->faceNodeMap[i]].max.z;
	}
	
	size_t avail;
    size_t total;
    cudaMemGetInfo(&avail, &total);
   	size_t req_constant = sizeof(Vector) + nVertices*sizeof(Vertex) + nEdges*sizeof(Edge) + nFaces*sizeof(Face) + nFaces*sizeof(Box) + nVertices*maxSize*sizeof(int) + nVertices*sizeof(int) + nEdges*maxSize*sizeof(int) + nEdges*sizeof(int); 
	size_t required = req_constant + nFaces*sizeof(int) + nFaces*maxSize*sizeof(int);
   
	assert(req_constant <= avail); //If the constant doesn't fit, this isn't going to get anywhere

	int splitfactor = 1;
	while(required > avail) {
        printf("Memory is not going to fit in device memory, thus split: %d\n", splitfactor);
		splitfactor *= 2;
		required = req_constant + 
		// the input can be split over batches of parallel calculations
		(nFaces*sizeof(int) + nFaces*maxSize*sizeof(int)) / splitfactor;
	}
    
    cudaPrintfInit();

    // Allocate vectors in device memory
    int* nPotFace;
    gpuErrchk(cudaMalloc(&nPotFace, nFaces*sizeof(int)));
    int* potFaceFace;
    gpuErrchk(cudaMalloc(&potFaceFace, nFaces*maxSize*sizeof(int)));
    Vector* disp;
    gpuErrchk(cudaMalloc(&disp, sizeof(Vector)));
	
    Vertex* vCuda;
    gpuErrchk(cudaMalloc(&vCuda, nVertices*sizeof(Vertex)));
    Edge* eCuda;
    gpuErrchk(cudaMalloc(&eCuda, nEdges*sizeof(Edge)));
    Face* fCuda;
    gpuErrchk(cudaMalloc(&fCuda, nFaces*sizeof(Face)));
    Box* bCuda;
    gpuErrchk(cudaMalloc(&bCuda, nFaces*sizeof(Box)));
	
    int* VFOutput;
    gpuErrchk(cudaMalloc(&VFOutput, nVertices*maxSize*sizeof(int)));
    int* nVFOutput;
    gpuErrchk(cudaMalloc(&nVFOutput, nVertices*sizeof(int)));
    int* EEOutput;
    gpuErrchk(cudaMalloc(&EEOutput, nEdges*maxSize*sizeof(int)));
    int* nEEOutput;
    gpuErrchk(cudaMalloc(&nEEOutput, nEdges*sizeof(int)));
	
    // Copy vectors from host memory to device memory
    gpuErrchk(cudaMemcpy(nVFOutput, nPotentialVertexFaces, nVertices*sizeof(int), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(nEEOutput, nPotentialEdgeEdges, nEdges*sizeof(int), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(disp, &displacement, sizeof(Vector), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(vCuda, bvh->mesh->vertices, nVertices*sizeof(Vertex), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(eCuda, bvh->mesh->edges, nEdges*sizeof(Edge), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(fCuda, bvh->mesh->faces, nFaces*sizeof(Face), cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(bCuda, faceBoxes, nFaces*sizeof(Box), cudaMemcpyHostToDevice));
    
    // Invoke kernel
    dim3 threadsPerBlock(16, 16);  // 256 threads
    dim3 numBlocks(nFaces/threadsPerBlock.x, maxSize/threadsPerBlock.y); 
    //int threadsPerBlock = 1;//maxSize;	// deze moeten nog verbeterd
    //int numBlocks = 1;//nFaces;		// allebei dus
    
    struct timeval before;
    struct timeval after;
	
	int indexmult = nFaces/splitfactor;
        gettimeofday(&before, NULL);
	for(int i = 0; i<splitfactor; i++) {
	    if(splitfactor != 1 && i == splitfactor-1) {
			gpuErrchk(cudaMemcpy(nPotFace, nPotentialFaces+i*indexmult, nFaces%splitfactor*sizeof(int), cudaMemcpyHostToDevice));
			gpuErrchk(cudaMemcpy(potFaceFace, potentialFaceFace+i*indexmult*maxSize, nFaces%splitfactor*maxSize*sizeof(int), cudaMemcpyHostToDevice));
		
			breakDownDeel1<<<numBlocks, threadsPerBlock>>>(nFaces, maxSize, nPotFace, potFaceFace, nVFOutput, nEEOutput, disp, vCuda, eCuda, fCuda, bCuda, VFOutput, EEOutput, splitfactor, true);
	    } else {
			gpuErrchk(cudaMemcpy(nPotFace, nPotentialFaces+i*indexmult, nFaces/splitfactor*sizeof(int), cudaMemcpyHostToDevice));
			gpuErrchk(cudaMemcpy(potFaceFace, potentialFaceFace+i*indexmult*maxSize, nFaces/splitfactor*maxSize*sizeof(int), cudaMemcpyHostToDevice));
		
			breakDownDeel1<<<numBlocks, threadsPerBlock>>>(nFaces, maxSize, nPotFace, potFaceFace, nVFOutput, nEEOutput, disp, vCuda, eCuda, fCuda, bCuda, VFOutput, EEOutput, splitfactor, false);
	    }
            cudaDeviceSynchronize();
	}
   
    gettimeofday(&after, NULL);

    unsigned long long time_before = (unsigned long long)(before.tv_sec) * 1000000 + (unsigned long long)(before.tv_usec);
    unsigned long long time_after = (unsigned long long)(after.tv_sec) * 1000000 + (unsigned long long)(after.tv_usec);

    printf("%llu\n", time_after - time_before);

   gpuErrchk(cudaPeekAtLastError());
   gpuErrchk(cudaDeviceSynchronize());


    // Copy result from device memory to host memory
    // Output vectors contain the result in host memory
    cudaMemcpy(potentialVertexFace, VFOutput, nVertices*maxSize*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(nPotentialVertexFaces, nVFOutput, nVertices*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(potentialEdgeEdge, EEOutput, nEdges*maxSize*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(nPotentialEdgeEdges, nEEOutput, nEdges*sizeof(int), cudaMemcpyDeviceToHost);
	

	// free device memory
	cudaFree(nPotFace);
	cudaFree(potFaceFace);
	cudaFree(disp);
	cudaFree(vCuda);
	cudaFree(eCuda);
	cudaFree(fCuda);
	cudaFree(bCuda);
	cudaFree(VFOutput);
	cudaFree(nVFOutput);
	cudaFree(EEOutput);
	cudaFree(nEEOutput);
	
	cudaPrintfDisplay();
	cudaPrintfEnd();
}

/*Not used anymore*/
void Collisions::symmetrize(){
  /*In order to symmetrize, sort potentialFaceFace and make a copy of
	nPotentialFaceFaces.*/
  int* tmpNPotentialFaceFaces = new int[nFaces];

  memcpy(tmpNPotentialFaceFaces, nPotentialFaces, 
	 sizeof(int) * nFaces);

  /*Sort each array*/
  for(int i=0;i<nFaces;i++){
	std::sort(potentialFaceFace + i*maxSize, 
		  potentialFaceFace + i*maxSize + tmpNPotentialFaceFaces[i]); 
  }

  for(int faceA=0;faceA<nFaces;faceA++){
	for(int indexB=0;indexB<nPotentialFaces[faceA];indexB++){
	  int faceB = potentialFaceFace[faceA * maxSize + indexB];
	  
	  /*Check if faceA exists in array of faceB*/
	  /*We use tmpNPotentialFaceFaces here, since array
	potentialFaceFace will be updated after the part which is
	sorted. The copy just contains the number of elements in the
	old situation, which are sorted.*/
	  int indexA = binarySearch(potentialFaceFace + faceB*maxSize,
				faceA, 0, tmpNPotentialFaceFaces[faceB]);

	  if(indexA == -1){
	/*Face A is not found in the array of faceB*/
	storeFaceFaceResult(faceB, faceA); //Also updates nPotentialFaces
	  }
	}
  }

  delete[] tmpNPotentialFaceFaces;
}

void Collisions::extendStorage(){
  int newMaxSize = maxSize*2;

  int* tmppotentialFaceFace     = new int[nFaces * newMaxSize];
  int* tmppotentialVertexFace   = new int[nVertices * newMaxSize];    
  int* tmppotentialEdgeEdge     = new int[nEdges * newMaxSize];

  /*Copy data*/
  for(int i=0;i<nFaces;i++){
	memcpy(tmppotentialFaceFace + i * newMaxSize, 
	   potentialFaceFace + i * maxSize, sizeof(int)*maxSize);
  }

  for(int i=0;i<nVertices;i++){
	memcpy(tmppotentialVertexFace + i * newMaxSize, 
	   potentialVertexFace + i * maxSize, sizeof(int)*maxSize);
  }

  for(int i=0;i<nEdges;i++){
	memcpy(tmppotentialEdgeEdge + i * newMaxSize, 
	   potentialEdgeEdge + i * maxSize, sizeof(int)*maxSize);
  }

  maxSize = newMaxSize;
  
  delete[] potentialFaceFace;
  delete[] potentialVertexFace;
  delete[] potentialEdgeEdge;

  potentialFaceFace = tmppotentialFaceFace;
  potentialVertexFace = tmppotentialVertexFace;
  potentialEdgeEdge = tmppotentialEdgeEdge;
}
