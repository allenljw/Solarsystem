#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

struct centerstruct { float x = 0.0f, y = 0.0f, z = 0.0f; };


// Read file contents
char* readFile(const char *filename) {
	// Open File
	std::ifstream input(filename);

	// Check file is open
	if (!input.good()) {
		// Print Error
		std::cerr << "Error: Could not open " << filename << std::endl;

		// Return Error
		return 0;
	}

	// Find end of file
	input.seekg(0, std::ios::end);

	// Calculate Size
	size_t size = input.tellg();

	// Allocate required memory
	char *data = new char[size + 1];

	// Rewind to beginning
	input.seekg(0, std::ios::beg);

	// Read file into memory
	input.read(data, size);

	// Append '\0'
	data[size] = '\0';

	// Close file
	input.close();

	// Return file contents
	return data;
}

GLuint checkShader(GLuint shader) {
	// Compile status
	GLint status = 0;

	// Check compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	// Error detected
	if (status != GL_TRUE) {
		// Get error message length
		int size;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

		// Get error message
		char *message = new char[size];
		glGetShaderInfoLog(shader, size, &size, message);

		// Print error message
		std::cerr << message << std::endl;

		// Delete message
		delete[] message;

		// Return error
		return GL_FALSE;
	}

	// Return success
	return GL_TRUE;
}

// Load and Compile Shader from source file
GLuint loadShader(GLuint type, const char *filename) {
	// Read the shader source from file
	char *source = readFile(filename);

	// Check shader source
	if (source == 0) {
		// Return Error
		return 0;
	}

	// Create the OpenGL Shaders
	GLuint shader = glCreateShader(type);

	// Load the source into the shaders
	glShaderSource(shader, 1, &source, NULL);

	// Compile the Shaders
	glCompileShader(shader);

	// Check shaders for errors
	if (checkShader(shader) == GL_TRUE) {
		// Log
		std::cout << "Loaded: " << filename << std::endl;
	}
	else {
		// Print Error
		std::cerr << "Error: could not compile " << filename << std::endl;

		// Delete shader source
		delete[] source;

		// Return Error
		return 0;
	}

	// Delete shader source
	delete[] source;

	// Return shader
	return shader;
}

// Check the status of a Program
GLuint checkProgram(GLuint program) {
	// Link status
	GLint status = 0;

	// Check link status
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	// Error detected
	if (status != GL_TRUE) {
		// Get error message length
		int size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);

		// Get error message
		char *message = new char[size];
		glGetProgramInfoLog(program, size, &size, message);

		// Print error message
		std::cerr << message << std::endl;

		// Delete message
		delete[] message;

		// Return error
		return GL_FALSE;
	}

	// Return success
	return GL_TRUE;
}

GLuint loadProgram(const char *vert_file, const char *ctrl_file, const char *eval_file, const char *geom_file, const char *frag_file) {
	// Create new OpenGL program
	GLuint program = glCreateProgram();

	// Shader Handles
	GLuint vert_shader = 0;
	GLuint ctrl_shader = 0;
	GLuint eval_shader = 0;
	GLuint geom_shader = 0;
	GLuint frag_shader = 0;

	// Load Shaders
	if (vert_file != NULL) vert_shader = loadShader(GL_VERTEX_SHADER, vert_file);
	if (ctrl_file != NULL) ctrl_shader = loadShader(GL_TESS_CONTROL_SHADER, ctrl_file);
	if (eval_file != NULL) eval_shader = loadShader(GL_TESS_EVALUATION_SHADER, eval_file);
	if (geom_file != NULL) geom_shader = loadShader(GL_GEOMETRY_SHADER, geom_file);
	if (frag_file != NULL) frag_shader = loadShader(GL_FRAGMENT_SHADER, frag_file);

	// Attach shaders
	if (vert_shader != 0) glAttachShader(program, vert_shader);
	if (ctrl_shader != 0) glAttachShader(program, ctrl_shader);
	if (eval_shader != 0) glAttachShader(program, eval_shader);
	if (geom_shader != 0) glAttachShader(program, geom_shader);
	if (frag_shader != 0) glAttachShader(program, frag_shader);

	// Check Vertex Shader
	if (vert_shader == 0) {
		// Print Error
		std::cerr << "Error: program missing vertex shader." << std::endl;

		// Delete Shaders
		if (vert_shader != 0) glDeleteShader(vert_shader);
		if (ctrl_shader != 0) glDeleteShader(ctrl_shader);
		if (eval_shader != 0) glDeleteShader(eval_shader);
		if (geom_shader != 0) glDeleteShader(geom_shader);
		if (frag_shader != 0) glDeleteShader(frag_shader);

		// Return Error
		return 0;
	}

	// Check Fragment Shader
	if (frag_shader == 0) {
		// Print Error
		std::cerr << "Error: program missing fragment shader." << std::endl;

		// Delete Shaders
		if (vert_shader != 0) glDeleteShader(vert_shader);
		if (ctrl_shader != 0) glDeleteShader(ctrl_shader);
		if (eval_shader != 0) glDeleteShader(eval_shader);
		if (geom_shader != 0) glDeleteShader(geom_shader);
		if (frag_shader != 0) glDeleteShader(frag_shader);

		// Return Error
		return 0;
	}

	// Link program
	glLinkProgram(program);

	// Delete Shaders (no longer needed)
	if (vert_shader != 0) glDeleteShader(vert_shader);
	if (ctrl_shader != 0) glDeleteShader(ctrl_shader);
	if (eval_shader != 0) glDeleteShader(eval_shader);
	if (geom_shader != 0) glDeleteShader(geom_shader);
	if (frag_shader != 0) glDeleteShader(frag_shader);

	// Check program for errors
	if (checkProgram(program) == GL_TRUE) {
		// Print Log
		std::cout << "Loaded: program" << std::endl;
	}
	else {
		// Print Error
		std::cerr << "Error: could not link program" << std::endl;

		// Return Error
		return 0;
	}

	// Return program
	return program;
}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<unsigned int> & out_indexes
	//std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	//std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<unsigned int> vertexIndices, colorIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_colors;
	//std::vector<glm::vec2> temp_uvs;
	//std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "c") == 0) {
			glm::vec3 color;
			fscanf(file, "%f %f %f\n", &color.x, &color.y, &color.z);
			temp_colors.push_back(color);
		}
		//else if (strcmp(lineHeader, "vt") == 0) {
		//	glm::vec2 uv;
		//	fscanf(file, "%f %f\n", &uv.x, &uv.y);
		//	uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
		//	temp_uvs.push_back(uv);
		//}
		/*else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}*/
		else if (strcmp(lineHeader, "f") == 0) {
			//std::string vertex1, vertex2, vertex3;
			//unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			unsigned int vertexIndex[3], colorIndex[3];
			//int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			int matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &colorIndex[0], &vertexIndex[1], &colorIndex[1], &vertexIndex[2], &colorIndex[2]);
			if (matches != 6) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			colorIndices.push_back(colorIndex[0]);
			colorIndices.push_back(colorIndex[1]);
			colorIndices.push_back(colorIndex[2]);

			/*uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);*/
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int colorIndex = colorIndices[i];
		//unsigned int uvIndex = uvIndices[i];
		//unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec3 color = temp_colors[colorIndex - 1];

		//glm::vec2 uv = temp_uvs[uvIndex - 1];
		//glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_vertices.push_back(color);
		out_indexes.push_back(i);
		//out_uvs.push_back(uv);
		//out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}


void createSphere(
	std::vector<glm::vec3> & out_vertices,
	std::vector<unsigned int> & out_indexes, centerstruct center, float radius
){

	std::vector<glm::vec3> temp_sphere;
	std::vector<glm::vec3> temp_color;

	float x, y, z, xy;                              // vertex position
	//radius = 0.08f;
	unsigned int sectorCount = 36, stackCount = 18;
	//float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
	//float s, t;                                     // texCoord
	//center.y = 0.08f;

	float sectorStep = 2 * float(M_PI) / sectorCount;
	float stackStep = float(M_PI) / stackCount;
	float sectorAngle, stackAngle;

	for (unsigned int i = 0; i <= stackCount; ++i)
	{
		glm::vec3 temp;

		stackAngle = float(M_PI) / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = center.z + radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal
		for (unsigned int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position
			x = center.x + xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = center.y + xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			temp.x = x;
			temp.y = y;
			temp.z = z;

			out_vertices.push_back(temp);

			temp.x = 0.0f;
			temp.y = 1.0f;
			temp.z = 0.0f;

			out_vertices.push_back(temp);


			// normalized vertex normal
			/*nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			addNormal(nx, ny, nz);*/

			// vertex tex coord between [0, 1]
			/*s = (float)j / sectorCount;
			t = (float)i / stackCount;
			addTexCoord(s, t);*/
		}
	}

	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	unsigned int k1, k2;
	for (unsigned int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				//addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1

				out_indexes.push_back(k1);
				out_indexes.push_back(k2);
				out_indexes.push_back(k1 + 1);

			}

			if (i != (stackCount - 1))
			{
				//addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1

				out_indexes.push_back(k1 + 1);
				out_indexes.push_back(k2);
				out_indexes.push_back(k2 + 1);
			}

		}
	}

}


int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Playground", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

	//enable depth test
	glEnable(GL_DEPTH_TEST);

	// Create and compile our GLSL program from the shaders
	GLuint programID = loadProgram("vert.glsl", NULL, NULL, NULL, "frag.glsl");

	
	//generate the ground vertices

	std::vector<glm::vec3> vertices;
	unsigned int vertices_number = 0;

	glm::vec3 temp_ground;
	temp_ground.x = -1.5f;  temp_ground.y = -1.3f;   temp_ground.z = -0.8f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);

	temp_ground.x = -1.5f;  temp_ground.y = -1.3f;   temp_ground.z = 0.0f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);

	temp_ground.x = 1.5f;  temp_ground.y = -1.3f;   temp_ground.z = 0.0f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);

	temp_ground.x = 1.5f;  temp_ground.y = -1.3f;   temp_ground.z = -0.8f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);
	
	temp_ground.x = -1.5f;  temp_ground.y = -1.3f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);

	temp_ground.x = 1.5f;  temp_ground.y = -1.3f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);
	temp_ground.x = 0.8f;  temp_ground.y = 0.8f;   temp_ground.z = 0.8f;
	vertices.push_back(temp_ground);

	//index start number after the ground index, it should be index starting point for the next object
	vertices_number = vertices_number+6;


	// Triangle Indexes
	std::vector<unsigned int> indexes;
	
	indexes.push_back(0);
	indexes.push_back(1);
	indexes.push_back(2);

	indexes.push_back(0);
	indexes.push_back(2);
	indexes.push_back(3);

	indexes.push_back(1);
	indexes.push_back(2);
	indexes.push_back(4);

	indexes.push_back(4);
	indexes.push_back(5);
	indexes.push_back(2);

		
	//generate the vertice of the Cylinder
	GLfloat segments = 360.0f;
	float bottom = -1.1f;
	float top = 0.0f;

	// give the center point of the cylinder
	centerstruct center;
	
	float r = 0.05f;
	GLuint n;
	for (n = 0; n <= segments; ++n)
	{
		
		glm::vec3 temp_cylinder, temp_color;

		GLfloat const t0 = 2 * float(M_PI) * (float)n / (float)segments;
		GLfloat const t1 = 2 * float(M_PI) * (float)(n + 1) / (float)segments;
		//quad vertex 0
		temp_cylinder.x = center.x + sin(t0) * r;
		temp_cylinder.y = top;
		temp_cylinder.z = center.z + cos(t0) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		//quad vertex 1
		temp_cylinder.x = center.x + sin(t0) * r;
		temp_cylinder.y = bottom;
		temp_cylinder.z = center.z + cos(t0) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		//quad vertex 2
		temp_cylinder.x = center.x + sin(t1) * r;
		temp_cylinder.y = top;
		temp_cylinder.z = center.z + cos(t1) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		//quad vertex 3
		temp_cylinder.x = center.x + sin(t1) * r;
		temp_cylinder.y = bottom;
		temp_cylinder.z = center.z + cos(t1) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		// generate the index to draw the triangle
		indexes.push_back(vertices_number + 4*n);
		indexes.push_back(vertices_number + 4*n + 1);
		indexes.push_back(vertices_number + 4*n + 2);

		indexes.push_back(vertices_number+ 4*n + 1);
		indexes.push_back(vertices_number+ 4*n+ 2);
		indexes.push_back(vertices_number+ 4*n+ 3);
				
	}

	//the index start number for second cylinder
	vertices_number = vertices_number + n * 4;

	bottom = -1.3f;
	top = -1.1f;
	
	r = 0.02f;

	for (n = 0; n <= segments; ++n)
	{
		glm::vec3 temp_cylinder, temp_color;

		GLfloat const t0 = 2 * float(M_PI) * (float)n / (float)segments;
		GLfloat const t1 = 2 * float(M_PI) * (float)(n + 1) / (float)segments;
		//quad vertex 0
		temp_cylinder.x = center.x + sin(t0) * r;
		temp_cylinder.y = top;
		temp_cylinder.z = center.z + cos(t0) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		//quad vertex 1
		temp_cylinder.x = center.x + sin(t0) * r;
		temp_cylinder.y = bottom;
		temp_cylinder.z = center.z + cos(t0) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		//quad vertex 2
		temp_cylinder.x = center.x + sin(t1) * r;
		temp_cylinder.y = top;
		temp_cylinder.z = center.z + cos(t1) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);



		//quad vertex 3
		temp_cylinder.x = center.x + sin(t1) * r;
		temp_cylinder.y = bottom;
		temp_cylinder.z = center.z + cos(t1) * r;

		temp_color.x = 0.0f;
		temp_color.y = 0.0f;
		temp_color.z = 1.0f;

		vertices.push_back(temp_cylinder);
		vertices.push_back(temp_color);

		indexes.push_back(vertices_number + 4 * n);
		indexes.push_back(vertices_number + 4 * n + 1);
		indexes.push_back(vertices_number + 4 * n + 2);

		indexes.push_back(vertices_number + 4 * n + 1);
		indexes.push_back(vertices_number + 4 * n + 2);
		indexes.push_back(vertices_number + 4 * n + 3);
	}

	vertices_number = vertices_number + n * 4;


	// Vertex Array Objects
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// Vertex Buffer Object (VBO)
	GLuint vbo = 0;

	// Element Buffer Object (EBO)
	GLuint ebo = 0;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Load Vertex Data
	//glBufferData(GL_ARRAY_BUFFER, sizeof(buffer) * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// Load Element Data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int), &indexes[0], GL_STATIC_DRAW);


	// Set Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

	// Enable Vertex Attribute Arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		
	//draw a sphere

	std::vector<glm::vec3> sphere_vertices;
	std::vector<unsigned int> sphere_indexes;
	float radius = 0.08f;
	center.y = 0.08f;

	createSphere(sphere_vertices, sphere_indexes, center, radius);
	
	// Vertex Array Objects
	GLuint v_sphere_oject = 0;
	glGenVertexArrays(1, &v_sphere_oject);
	glBindVertexArray(v_sphere_oject);


	// Vertex Buffer Object (VBO)
	GLuint vbo2 = 0;

	// Element Buffer Object (EBO)
	GLuint ebo2 = 0;

	glGenBuffers(1, &vbo2);
	glGenBuffers(1, &ebo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);

	// Load Vertex Data
	//glBufferData(GL_ARRAY_BUFFER, sizeof(buffer) * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(glm::vec3), &sphere_vertices[0], GL_STATIC_DRAW);

	// Load Element Data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indexes.size() * sizeof(unsigned int), &sphere_indexes[0], GL_STATIC_DRAW);


	// Set Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

	// Enable Vertex Attribute Arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

	

	// Read our .obj file to get the vertices and colors for the flag including the indexes of the triangle
	std::vector<glm::vec3> flag_vertices;
	std::vector<unsigned int> flag_indexes;
	bool res = loadOBJ("vertexstore.obj", flag_vertices, flag_indexes);

	if (res == false)
	{
		fprintf(stderr, "Failed to load flag object file\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Vertex Array Objects
	GLuint v_flag_object = 0;
	glGenVertexArrays(1, &v_flag_object);
	glBindVertexArray(v_flag_object);


	// Vertex Buffer Object (VBO)
	GLuint vbo3 = 0;

	// Element Buffer Object (EBO)
	GLuint ebo3 = 0;

	glGenBuffers(1, &vbo3);
	glGenBuffers(1, &ebo3);

	glBindBuffer(GL_ARRAY_BUFFER, vbo3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo3);

	// Load Vertex Data
	//glBufferData(GL_ARRAY_BUFFER, sizeof(buffer) * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, flag_vertices.size() * sizeof(glm::vec3), &flag_vertices[0], GL_STATIC_DRAW);

	// Load Element Data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, flag_indexes.size() * sizeof(unsigned int), &flag_indexes[0], GL_STATIC_DRAW);


	// Set Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));

	// Enable Vertex Attribute Arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// use the program
	glUseProgram(programID);

	// retrieve the matrix uniform locations
	GLuint projectionLoc = glGetUniformLocation(programID, "u_Projection");
		
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// normally the projection don't change in the main loop, therefore, put it outside the main loop
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				

		// Use Program
		glUseProgram(programID);

		// retrieve the matrix uniform locations
		GLuint modelLoc = glGetUniformLocation(programID, "u_Model");
		GLuint viewLoc = glGetUniformLocation(programID, "u_View");
		
		// generate the number with time change
		float t = float(glfwGetTime());

		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);

		// rotate the camera in a circle looking at the vertex (0.0f, 0.0f, 0.0f)
		float radius = 4.0f;
		float camX = sin(0.1*t) * radius;
		float camZ = cos(0.1*t) * radius;
		view = glm::lookAt(glm::vec3(camX, 1.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, 3.0f));
		//view = glm::lookAt(
		//	glm::vec3(0, 3, 3), // Camera is at (4,3,3), in World Space
		//	glm::vec3(0, 0, 0), // and looks at the origin
		//	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		//);
				
		// pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
				

		for (n = 0; n < flag_vertices.size(); n=n+2)
		{
			//make the z coordinate change to implement simple sine wave animation
			flag_vertices[n].z = flag_vertices[n].x * 0.5 *(sin(0.8*t + 3* flag_vertices[n].x));
			
			//printf("%s%f", "z point value", flag_vertices[n].z);
		}
		
		// bind the first vertex array object to draw the triangles
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, NULL);

		glBindVertexArray(0);

		// bind the second vertex array object to draw the triangles
		glBindVertexArray(v_sphere_oject);
		glDrawElements(GL_TRIANGLES, sphere_indexes.size() , GL_UNSIGNED_INT, NULL);

		glBindVertexArray(0);

		// bind the third vertex array object to draw the triangles
		glBindVertexArray(v_flag_object);
		glBindBuffer(GL_ARRAY_BUFFER, vbo3);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo3);
		glBufferSubData(GL_ARRAY_BUFFER, 0, flag_vertices.size() * sizeof(glm::vec3), &flag_vertices[0]);
		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, flag_indexes.size() * sizeof(unsigned int), &flag_indexes[0]);
		
		glDrawElements(GL_TRIANGLES, flag_indexes.size(), GL_UNSIGNED_INT, NULL);

		glBindVertexArray(0);

		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	// Delete VAO, VBO & EBO
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	glDeleteVertexArrays(1, &v_sphere_oject);
	glDeleteBuffers(1, &vbo2);
	glDeleteBuffers(1, &ebo2);

	glDeleteVertexArrays(1, &v_flag_object);
	glDeleteBuffers(1, &vbo3);
	glDeleteBuffers(1, &ebo3);

	// Delete Programs
	glDeleteProgram(programID);
	
	// Stop receiving events for the window and free resources; this must be
	// called from the main thread and should not be invoked from a callback
	glfwDestroyWindow(window);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

