#include "common.h"


bool VoxelGrid::loadVoxels(const char * path){

	std::string path_string = std::string(path);
	int min_frame;

	//Count each image found in the directory
	for (const auto & entry : std::filesystem::directory_iterator(path)) {

		//Convert image name to integer to use as index

		std::filesystem::path path_e = entry.path();
		std::string path_string_e = path_e.string();
		int frame = std::stoi(path_string_e.substr(path_string.size(), path_string_e.size() - 4));
		
		if (depth == 0) {
			min_frame = frame;
		}
		else if (frame < min_frame) {
			min_frame = frame;
		}
		//Count the number of files in the directory first
		//Add to depth counter for each image added
		depth++;

	}

	//Then assign a list of image data entries 
	std::vector < std::vector < unsigned char > > images;
	images.resize(depth);
	for (const auto & entry : std::filesystem::directory_iterator(path)) {
		//decode image		
		std::vector<unsigned char> image;
		std::filesystem::path path_e = entry.path();
		unsigned error = lodepng::decode(image, width, height, path_e.string());

		//if there's an error, display it
		if (error) {
			std::cout << "decoder error " << error;
			std::cout << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}

		std::cout << "Image loaded: " << width << " x " << height << std::endl;
		std::cout << image.size() << " pixels.\n";
		std::cout << "Image has " << image.size() / (width*height) << "color values per pixel.\n";

		//Convert image name to integer to use as index
		std::string path_string_e = path_e.string();
		int frame = std::stoi(path_string_e.substr(path_string.size(), path_string_e.size() - 4));
		images[frame - min_frame] = image;
	}
	
	for (unsigned int i = 0; i < images.size(); i++) { 
		std::vector<unsigned char> image = images[i];
		for (unsigned int j = 0; j < image.size(); j++) {
			volume.push_back(image[j]);
		}
	}
	
	center = vec3(-(float)width / 2.0, -(float)height / 2.0, -(float)depth/ 2.0);
	double max_dim = float max(width, height, depth);

	model_view = RotateZ(180) * 
		Scale(1.0 / max_dim,
			1.0 / max_dim,
			1.0 / max_dim)*
		Translate(center);  //Orient Model About Center
  return true;

}

void VoxelGrid::addCube(vec3 pos) {
	//VERTICES
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

	//NORMALS
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

void VoxelGrid::createMesh() {
	for (unsigned int x = 0; x < width; x++) {
		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int z = 0; z < depth; z++) {
				int red = volume[(x + y * width + z * height*width) * 4];
				int green = volume[(x + y * width + z * height*width) * 4 + 1];
				int blue = volume[(x + y * width + z * height*width) * 4 + 2];
				Voxel *voxel = new Voxel();

				if ((red + blue + green) > 0) {
					double alpha = (red + blue + green) / 3.0;
					vec4 color = vec4(red / 255.0, green / 255.0, blue / 255.0, alpha);
					//Add a color for each vertex
					for (unsigned int i = 0; i < 36; i++) {
						colors.push_back(color);
					}

					//Adding voxels for MIP raycast
					Voxel::ShadingValues *shading = new Voxel::ShadingValues;
					shading->color = color;
					shading->Kd = 1.0;
					voxel->setShadingValues(*shading);
					voxel->setModelView(Translate(vec3(x, y, z))); //-y because as y increases, we scale down the y axis 
					voxels.push_back(voxel);


					//Only adding cube if the other cubes around it do not occlude it
					/*
					if (x !=0 && x%width != 0){
					int redx1 = volume[((x + 1) + y * width + z * height*width) * 4];
					}
					int redx2 = volume[((x + 1) + y * width + z * height*width) * 4];
					int redx1 = volume[((x + 1) + y * width + z * height*width) * 4];
					
					int redx1 = volume[((x + 1) + y * width + z * height*width) * 4];
					int redx1 = volume[((x + 1) + y * width + z * height*width) * 4];
					
					int redx1 = volume[((x + 1) + y * width + z * height*width) * 4];
					*/

					//Adding cube for visualization
					addCube(vec3(x, y, z));
				}	
				else {
					voxels.push_back(new Voxel());
				}
			}
		}
	}
}

/*
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
					double alpha = (red + blue + green) / 3.0;
					vec4 color = vec4(red / 255.0, green / 255.0, blue / 255.0, 0.0);
					//if (z == 0) {vec4 color = vec4(1.0);}
					//Add a color for each vertex
					for (unsigned int i = 0; i < 36; i++) {
						colors.push_back(color);
					}
				}
			}
		}
	}
}
*/
