#include "BVH.hpp"

#include <iostream>

int main(int argc, char** argv){
  if(argc == 2){

    BVH* bvh = new BVH(argv[1]);
    int maxSize = 5000;
    Collisions* collisions = new Collisions(bvh, maxSize);

    Vector displ;
    displ.x = displ.y = displ.z = 0;

    bvh->checkCollisions(collisions, displ);
    
    delete collisions;
    delete bvh;
  }else{
    std::cout << "Please specify an input file." << std::endl;
  }
  return 0;
}
