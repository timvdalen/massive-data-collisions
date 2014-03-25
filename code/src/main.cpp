#include "BVH.hpp"

#include <iostream>

void printMetrics(const Collisions* collisions){
	int sumVertexFace = 0;
	int sumEdgeEdge = 0;
	for(int i=0; i < collisions->nVertices; i++){
		sumVertexFace += collisions->nPotentialVertexFaces[i];
		sumEdgeEdge += collisions->nPotentialEdgeEdges[i];
	}

	std::cout << "Total number of vertex-face: " << sumVertexFace << std::endl << "Total number of edge-edge: " << sumEdgeEdge << std::endl;
}

int main(int argc, char** argv){
	if(argc == 2){

		BVH* bvh = new BVH(argv[1]);
		int maxSize = 5000;
		Collisions* collisions = new Collisions(bvh, maxSize);

		Vector displ;
		displ.x = displ.y = displ.z = 0;

		bvh->checkCollisions(collisions, displ);

		printMetrics(collisions);

		delete collisions;
		delete bvh;
	}else{
		std::cout << "Please specify an input file." << std::endl;
	}
	return 0;
}
