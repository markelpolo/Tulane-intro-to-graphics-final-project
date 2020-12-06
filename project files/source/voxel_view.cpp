#include "common.h"
#include "SourcePath.h"


using namespace Angel;

typedef vec4 color4;

// Initialize shader lighting parameters
vec4 light(   0.0, 0.0, 10.0, 1.0 );
color4 light_ambient(  0.1, 0.1, 0.1, 0.1 );
color4 light_diffuse(  1.0, 1.0, 1.0, 1.0 );
color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

// Initialize shader material parameters
color4 material_ambient( 0.1, 0.1, 0.1, 1.0 );
color4 material_diffuse( 0.2, 0.6, 0.0, 1.0 );
color4 material_specular( 0.8, 0.8, 0.8, 1.0 );
float  material_shininess = 10;

enum{ _TOTAL_MODELS = 1};
std::vector <std::string> folder;
std::vector < VoxelGrid * > voxelgrid;
std::vector < GLuint > buffer;
std::vector < GLuint > vao;
GLuint ModelView_loc, NormalMatrix_loc, Projection_loc;
bool wireframe;
int current_model;

//Variables for perspective
int width, height;
mat4  projection;
mat4 model_view;

//==========Trackball Variables==========
static float curquat[4],lastquat[4];
/* current transformation matrix */
static float curmat[4][4];
mat4 curmat_a;
/* actual operation  */
bool scaling;
bool moving;
bool panning;
/* starting "moving" coordinates */
static int beginx, beginy;
/* ortho */
float ortho_x, ortho_y;
/* current scale factor */
static float scalefactor;
bool lbutton_down;

// Raytracing Functions
bool write_image(const char* filename, const unsigned char *Src,
	int Width, int Height, int channels) {
	unsigned bitdepth = 8;
	LodePNGColorType color_type;
	unsigned result;
	switch (channels)
	{
	case 1:
		color_type = LCT_GREY; break;
	case 2:
		color_type = LCT_GREY_ALPHA; break;
	case 3:
		color_type = LCT_RGB; break;
	default:
		color_type = LCT_RGBA; break;
	}
	result = lodepng_encode_file(filename, Src,
		static_cast<unsigned int>(Width), static_cast<unsigned int>(Height),
		color_type, bitdepth);
	if (result == 0)
		std::cerr << "finished writing " << filename << "." << std::endl;
	else
		std::cerr << "write to " << filename << " returned error code " << result << ". ("
		<< lodepng_error_text(result) << ")" << std::endl;
	return result == 0;
}



std::vector < vec4 > findRay(GLdouble x, GLdouble y) {
	
	y = height - y;

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble modelViewMatrix[16];
	GLdouble projectionMatrix[16];
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			modelViewMatrix[j * 4 + i] = voxelgrid[current_model]->model_view[i][j];
			projectionMatrix[j * 4 + i] = projection[i][j];
		}
	}


	GLdouble nearPlaneLocation[3];
	_gluUnProject(x, y, 0.0, modelViewMatrix, projectionMatrix,
		viewport, &nearPlaneLocation[0], &nearPlaneLocation[1],
		&nearPlaneLocation[2]);

	GLdouble farPlaneLocation[3];
	_gluUnProject(x, y, 1.0, modelViewMatrix, projectionMatrix,
		viewport, &farPlaneLocation[0], &farPlaneLocation[1],
		&farPlaneLocation[2]);


	vec4 ray_origin = vec4(nearPlaneLocation[0], nearPlaneLocation[1], nearPlaneLocation[2], 1.0);
	vec3 temp = vec3(farPlaneLocation[0] - nearPlaneLocation[0],
		farPlaneLocation[1] - nearPlaneLocation[1],
		farPlaneLocation[2] - nearPlaneLocation[2]);
	temp = normalize(temp);
	vec4 ray_dir = vec4(temp.x, temp.y, temp.z, 0.0);
	
	std::vector < vec4 > result(2);
	result[0] = ray_origin;
	result[1] = ray_dir;

	return result;
}

bool intersectionSort(Voxel::IntersectionValues i, Voxel::IntersectionValues j) {
	return (i.t_w < j.t_w);
}


vec4 castRay(vec4 p0, vec4 dir, Voxel *lastHitVoxel, int depth) {
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	/*
	std::vector<Voxel::IntersectionValues> intersections;
	Voxel::IntersectionValues min_intersection;
	Voxel::IntersectionValues current_intersection;
	std::vector <Voxel*> voxels = voxelgrid[current_model]->voxels;
	for (unsigned int i = 0; i < voxels.size(); i++) {
		current_intersection = voxels[i]->intersect(p0, dir);
		current_intersection.ID_ = i;
		intersections.push_back(current_intersection);

		if (i == 0) {
			min_intersection = current_intersection;
		}
		else if (intersectionSort(current_intersection, min_intersection)) {
			min_intersection = current_intersection;
		}
	}
	
	if (min_intersection.t_w != std::numeric_limits<double>::infinity()) {
		color = voxels[min_intersection.ID_]->shadingValues.color;
	}

	color.x = fmin(color.x, 1.0);
	color.y = fmin(color.y, 1.0);
	color.z = fmin(color.z, 1.0);
	color.w = fmin(color.w, 1.0);
	nearest_oxel
	return color;
	
}

void castRayDebug(vec4 p0, vec4 dir) {
	vec4 color = castRay(p0, dir, NULL, 0);
	std::cout << color << std::endl << std::endl;
	/*
  std::vector < Voxel::IntersectionValues > intersections;

  for(unsigned int i=0; i < voxelgrid[current_model].voxels.size(); i++){
	intersections.push_back(sceneVoxels[i]->intersect(p0, dir));
	intersections[intersections.size()-1].ID_ = i;
  }

  for(unsigned int i=0; i < intersections.size(); i++){
	if(intersections[i].t_w != std::numeric_limits< double >::infinity()){
	  std::cout << "Hit " << intersections[i].name << " " << intersections[i].ID_ << "\n";
	  std::cout << "P: " <<  intersections[i].P_w << "\n";
	  std::cout << "N: " <<  intersections[i].N_w << "\n";
	  vec4 L = light_position-intersections[i].P_w;
	  L  = normalize(L);
	  std::cout << "L: " << L << "\n";
	}
  }
  */
}

void rayTrace() {

	unsigned char *buffer = new unsigned char[width*height * 4];

	for (unsigned int i = 0; i < width; i++) {
		for (unsigned int j = 0; j < height; j++) {

			int idx = j * width + i;
			std::vector < vec4 > ray_o_dir = findRay(i, j);
			vec4 color = castRay(ray_o_dir[0], vec4(ray_o_dir[1].x, ray_o_dir[1].y, ray_o_dir[1].z, 0.0), NULL, 0);
			buffer[4 * idx] = color.x * 255;
			buffer[4 * idx + 1] = color.y * 255;
			buffer[4 * idx + 2] = color.z * 255;
			buffer[4 * idx + 3] = color.w * 255;
		}
	}

	write_image("output.png", buffer, width, height, 4);

	delete[] buffer;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//User interaction handler
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	/*
	if (key == GLFW_KEY_R && action == GLFW_PRESS){
	  raytrace();
	}
	*/
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		wireframe = !wireframe;
	}
}

//User interaction handler
static void mouse_click(GLFWwindow* window, int button, int action, int mods) {

	if (GLFW_RELEASE == action) {
		moving = scaling = panning = false;
		return;
	}

	if (mods & GLFW_MOD_SHIFT) {
		scaling = true;
	}
	else if (mods & GLFW_MOD_ALT) {
		panning = true;
	}
	else {
		moving = true;
		trackball(lastquat, 0, 0, 0, 0);
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	beginx = xpos; beginy = ypos;

	//Debugging the raycast function
	std::vector < vec4 > ray_o_dir = findRay(xpos, ypos);
	castRayDebug(ray_o_dir[0], vec4(ray_o_dir[1].x, ray_o_dir[1].y, ray_o_dir[1].z, 0.0));
}

//User interaction handler
void mouse_move(GLFWwindow* window, double x, double y) {

	int W, H;
	glfwGetFramebufferSize(window, &W, &H);


	float dx = (x - beginx) / (float)W;
	float dy = (beginy - y) / (float)H;

	if (panning)
	{
		ortho_x += dx;
		ortho_y += dy;

		beginx = x; beginy = y;
		return;
	}
	else if (scaling)
	{
		scalefactor *= (1.0f + dx);

		beginx = x; beginy = y;
		return;
	}
	else if (moving)
	{
		trackball(lastquat,
			(2.0f * beginx - W) / W,
			(H - 2.0f * beginy) / H,
			(2.0f * x - W) / W,
			(H - 2.0f * y) / H
		);

		add_quats(lastquat, curquat, curquat);
		build_rotmatrix(curmat, curquat);

		beginx = x; beginy = y;
		return;
	}
}


void init(){
	//Settings for muliple models
  folder.push_back("/images/");
  current_model = 0;

  std::string vshader = source_path + "/shaders/vshader.glsl";
  std::string fshader = source_path + "/shaders/fshader.glsl";
  
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vshader, vertex_shader);
  
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fshader, fragment_shader);
  
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  
  glLinkProgram(program);
  check_program_link(program);
  
  glUseProgram(program);
  
  glBindFragDataLocation(program, 0, "fragColor");

  //Per vertex attributes
  GLuint vPosition = glGetAttribLocation( program, "vPosition" );
  GLuint vColor = glGetAttribLocation( program, "vColor" );
  GLuint vNormal = glGetAttribLocation( program, "vNormal" );

  //Compute ambient, diffuse, and specular terms
  color4 ambient_product  = light_ambient * material_ambient;
  color4 diffuse_product  = light_diffuse * material_diffuse;
  color4 specular_product = light_specular * material_specular;
  
  //Retrieve and set uniform variables
  glUniform4fv( glGetUniformLocation(program, "Light"), 1, light);
  glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
  glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
  glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
  glUniform1f(  glGetUniformLocation(program, "Shininess"), material_shininess );
  
  //Matrix uniform variable locations
  ModelView_loc = glGetUniformLocation( program, "ModelView" );
  NormalMatrix_loc = glGetUniformLocation( program, "NormalMatrix" );
  Projection_loc = glGetUniformLocation( program, "Projection" );
  
  //===== Send data to GPU ======
  
  vao.resize(_TOTAL_MODELS);
  glGenVertexArrays( _TOTAL_MODELS, &vao[0] );
  
  buffer.resize(_TOTAL_MODELS);
  glGenBuffers( _TOTAL_MODELS, &buffer[0] );
  
  for(unsigned int i=0; i < _TOTAL_MODELS; i++){
    std::cout << source_path + folder[i] << std::endl;
	VoxelGrid* grid = new VoxelGrid((source_path + folder[i]).c_str());
    voxelgrid.push_back(grid);

    glBindVertexArray( vao[i] );
    glBindBuffer( GL_ARRAY_BUFFER, buffer[i] );
    unsigned int vertices_bytes = grid->vertices.size()*sizeof(vec4);
    unsigned int colors_bytes  = grid->colors.size()*sizeof(vec4);
    unsigned int normals_bytes  = grid->normals.size()*sizeof(vec3);
    
    glBufferData( GL_ARRAY_BUFFER, vertices_bytes + colors_bytes + normals_bytes, NULL, GL_STATIC_DRAW );
    unsigned int offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, vertices_bytes, &grid->vertices[0] );
    offset += vertices_bytes;
    glBufferSubData( GL_ARRAY_BUFFER, offset, colors_bytes,  &grid->colors[0] );
    offset += colors_bytes;
    glBufferSubData( GL_ARRAY_BUFFER, offset, normals_bytes,  &grid->normals[0] );
    
    glEnableVertexAttribArray( vColor );
    glEnableVertexAttribArray( vPosition );
    glEnableVertexAttribArray( vNormal );

    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    if (colors_bytes > 0)
      glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(static_cast<size_t>(vertices_bytes)) );
    if (normals_bytes > 0)
      glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(static_cast<size_t>(vertices_bytes + colors_bytes)) );

  }
  
  
  //===== End: Send data to GPU ======


  // ====== Enable some opengl capabilitions ======
  glEnable( GL_DEPTH_TEST );
  glShadeModel(GL_SMOOTH);

  //glClearColor( 0.8, 0.8, 1.0, 1.0 );
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  //===== Initalize some program state variables ======

  //Quaternion trackball variables, you can ignore
  scaling  = 0;
  moving   = 0;
  panning  = 0;
  beginx   = 0;
  beginy   = 0;
  
  matident(curmat);
  trackball(curquat , 0.0f, 0.0f, 0.0f, 0.0f);
  trackball(lastquat, 0.0f, 0.0f, 0.0f, 0.0f);
  add_quats(lastquat, curquat, curquat);
  build_rotmatrix(curmat, curquat);
  
  scalefactor = 1.0;
  
  wireframe = false;
  current_model = 0;
  
  lbutton_down = false;


  //===== End: Initalize some program state variables ======

}


int main(void){
  
  GLFWwindow* window;
  
  glfwSetErrorCallback(error_callback);
  
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  window = glfwCreateWindow(512, 512, "JPEG Volume Renderer", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  //Set key and mouse callback functions
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_click);
  glfwSetCursorPosCallback(window, mouse_move);

  
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  
  init();
  
  while (!glfwWindowShouldClose(window)){
    
    //Display as wirfram, boolean tied to keystoke 'w'
    if(wireframe){
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }else{
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    
    
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    GLfloat aspect = GLfloat(width)/height;
    
    //Projection matrix
    projection = Perspective( 45.0, aspect, 0.5, 5.0 );
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //"Camera" position
    const vec3 viewer_pos( 0.0, 0.0, 2.0 );
    
    //Track_ball rotation matrix
    mat4 track_ball =  mat4(curmat[0][0], curmat[1][0], curmat[2][0], curmat[3][0],
                            curmat[0][1], curmat[1][1], curmat[2][1], curmat[3][1],
                            curmat[0][2], curmat[1][2], curmat[2][2], curmat[3][2],
                            curmat[0][3], curmat[1][3], curmat[2][3], curmat[3][3]);
 
    //Modelview based on user interaction
    mat4 user_MV  =  Translate( -viewer_pos ) *                    //Move Camera Back to -viewer_pos
                     Translate(ortho_x, ortho_y, 0.0) *            //Pan Camera
                     track_ball *                                  //Rotate Camera
                     Scale(scalefactor,scalefactor,scalefactor);   //User Scale
    

    // ====== Draw ======
	glBindVertexArray(vao[current_model]);
	//glBindBuffer( GL_ARRAY_BUFFER, buffer[current_draw] );

	glUniformMatrix4fv(ModelView_loc, 1, GL_TRUE, user_MV*voxelgrid[current_model]->model_view);
	glUniformMatrix4fv(Projection_loc, 1, GL_TRUE, projection);
	glUniformMatrix4fv(NormalMatrix_loc, 1, GL_TRUE, transpose(invert(user_MV*voxelgrid[current_model]->model_view)));

	glDrawArrays(GL_TRIANGLES, 0, voxelgrid[current_model]->vertices.size());

    // ====== End: Draw ======

    
    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }
  
  glfwDestroyWindow(window);
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
