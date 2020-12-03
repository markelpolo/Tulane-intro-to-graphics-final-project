#include "common.h"







bool VoxelGrid::loadVoxels(const char * path){

  //decode
  unsigned error = voxelgrid_decode(volume, width, height, depth, path);

  //if there's an error, display it
  if(error){
      std::cout << "decoder error " << error;
      std::cout << ": " << voxelgrid_error_text(error) << std::endl;
    return false;}
    
  
  std::cout << "Volume loaded: " << width << " x " << height << " x " << depth << std::endl;
  std::cout << (width*height*depth) << " voxels.\n";
  std::cout << "Volume has " << volume.size()/(width*height*depth) << "color values per voxel.\n";

  vec3 center = vec3(-(float)width/2.0, -(float)height/2.0, -(float)depth/2.0);
  double max_dim = (std::max)(width, (std::max)(height, depth));

  model_view = RotateX(-45)*
               Scale(1.0/max_dim,
                     1.0/max_dim,
                     1.0/max_dim)*
                     Translate(center);  //Orient Model About Center

  return true;

}


//Create a single triangulated cube at the given position and add it
//to the vertices array. You may want to use this function (or add
//similar functions to the VoxelGrid class) to organize your code.
void VoxelGrid::addCube(vec3 pos) {
	//4TH ELEMENT IS 1 FOR VERTICES
	//Back face
	//Triangle 1	
	vertices.push_back(vec4(0, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 0, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(0, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 0, 1) + vec4(pos, 0));

	//Front face
	//Triangle 1
	vertices.push_back(vec4(0, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 1, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(0, 1, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 1, 1) + vec4(pos, 0));


	//Top Face
	//Triangle 1
	vertices.push_back(vec4(0, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 0, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(0, 1, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 1, 1) + vec4(pos, 0));

	//Bottom Face
	//Triangle 1
	vertices.push_back(vec4(0, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 0, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(0, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 1, 1) + vec4(pos, 0));

	//Right Face
	//Triangle 1
	vertices.push_back(vec4(1, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 0, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(1, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(1, 1, 1, 1) + vec4(pos, 0));

	//Left Face
	//Triangle 1
	vertices.push_back(vec4(0, 0, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 0, 1) + vec4(pos, 0));
	//Triangle 2
	vertices.push_back(vec4(0, 0, 1, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 0, 1) + vec4(pos, 0));
	vertices.push_back(vec4(0, 1, 1, 1) + vec4(pos, 0));
}

//TODO
//Create a triangulated version of the voxel grid for rendering and populate
//the vertices array.
void VoxelGrid::createMesh(){
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int z = 0; z < depth; z++) {
				if (volume[(x + y * width + z * width*height) * 4 + 3] != 0) {
					addCube(vec3(x, y, z));
				}
			}
		}
	}
}

//TODO
//Populate the normal array with vertice normals for the triangle mesh
void VoxelGrid::createNormals(){
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int z = 0; z < depth; z++) {
				if (volume[(x + y * width + z * width*height) * 4 + 3] != 0) {
					//Back face
					//Triangle 1
					normals.push_back(vec3(0, 0, -1));
					normals.push_back(vec3(0, 0, -1));
					normals.push_back(vec3(0, 0, -1));
					//Triangle 2
					normals.push_back(vec3(0, 0, -1));
					normals.push_back(vec3(0, 0, -1));
					normals.push_back(vec3(0, 0, -1));

					//Front face
					//Triangle 1
					normals.push_back(vec3(0, 0, 1));
					normals.push_back(vec3(0, 0, 1));
					normals.push_back(vec3(0, 0, 1));
					//Triangle 2
					normals.push_back(vec3(0, 0, 1));
					normals.push_back(vec3(0, 0, 1));
					normals.push_back(vec3(0, 0, 1));

					//Top face
					//Triangle 1
					normals.push_back(vec3(0, 1, 0));
					normals.push_back(vec3(0, 1, 0));
					normals.push_back(vec3(0, 1, 0));
					//Triangle 2
					normals.push_back(vec3(0, 1, 0));
					normals.push_back(vec3(0, 1, 0));
					normals.push_back(vec3(0, 1, 0));

					//Bottom face
					//Triangle 1
					normals.push_back(vec3(0, -1, 0));
					normals.push_back(vec3(0, -1, 0));
					normals.push_back(vec3(0, -1, 0));
					//Triangle 2
					normals.push_back(vec3(0, -1, 0));
					normals.push_back(vec3(0, -1, 0));
					normals.push_back(vec3(0, -1, 0));

					//Right face
					//Triangle 1
					normals.push_back(vec3(1, 0, 0));
					normals.push_back(vec3(1, 0, 0));
					normals.push_back(vec3(1, 0, 0));
					//Triangle 2
					normals.push_back(vec3(1, 0, 0));
					normals.push_back(vec3(1, 0, 0));
					normals.push_back(vec3(1, 0, 0));

					//Left face
					//Triangle 1
					normals.push_back(vec3(-1, 0, 0));
					normals.push_back(vec3(-1, 0, 0));
					normals.push_back(vec3(-1, 0, 0));
					//Triangle 2
					normals.push_back(vec3(-1, 0, 0));
					normals.push_back(vec3(-1, 0, 0));
					normals.push_back(vec3(-1, 0, 0));
				}
			}
		}
	}

}

//TODO
//Populate the color array with vertice colors for the triangle mesh
void VoxelGrid::createColors(){
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int z = 0; z < depth; z++) {
				if (volume[(x + y * width + z * width*height) * 4 + 3] != 0) {
					int red = volume[(x + y * width + z * height*width) * 4];
					int green = volume[(x + y * width + z * height*width) * 4 + 1];
					int blue = volume[(x + y * width + z * height*width) * 4 + 2];
					
					vec3 color = vec3(red / 255.0, green / 255.0, blue / 255.0);
					
					//Add a color for each vertex
					for (unsigned int i = 0; i < 36; i++) {
						colors.push_back(color);
					}
				}
			}
		}
	}
}
