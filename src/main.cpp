#include "BVH.hpp"

#include <iostream>
#include <cstdio>

int main(int argc, char** argv){
	if(argc == 2){

		BVH* bvh = new BVH(argv[1]);
		int maxSize = 5000;
		Collisions* collisions = new Collisions(bvh, maxSize);

		Vector displ;
		displ.x = displ.y = displ.z = 0;

		int sum = 0;
		int arr_length = collisions->nVertices;
		for(int i=0; i < arr_length; i++){
			sum += collisions->nPotentialVertexFaces[i];
		}
		printf("Before: %d, %d\n", arr_length, sum);

		bvh->checkCollisions(collisions, displ);

		sum = 0;
		for(int i=0; i < arr_length; i++){
			sum += collisions->nPotentialVertexFaces[i];
		}
		printf("After:%d, %d\n", arr_length, sum);
	
		delete collisions;
		delete bvh;
	}else{
		std::cout << "Please specify an input file." << std::endl;
	}
	return 0;
}
