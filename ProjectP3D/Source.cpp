#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#define GLEW_STATIC
#pragma region Includes GL, GLFW, GLM
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#pragma endregion

void init(void);

#define WIDTH 800
#define HEIGHT 600

GLfloat ZOOM = 10.0f;

float rotationX = 0.0f, rotationY = 0.0f;
float yaw = 0.0f, pitch = 0.0f;
float newXPos, newYPos;
bool isFirstFrame = true;

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	// Se faz zoom in
	if (yoffset == 1) {
		// Incremento no zoom, varia com a distância da câmara
		ZOOM += fabs(ZOOM) * 0.1f;  // abs - valor absoluto -> significa que quanto mais longe o objeto estiver, maior é a capacidade de zoom
	}
	// Senão, se faz zoom out
	else if (yoffset == -1) {
		// Incremento no zoom, varia com a distância da câmara
		ZOOM -= fabs(ZOOM) * 0.1f;
	}
	std::cout << "ZOOM = " << ZOOM << std::endl;
}

void cursorCallback(GLFWwindow *window, double xPos, double yPos) {

	if (!isFirstFrame) {
		yaw = (xPos - newXPos) * 0.01f;
		pitch = (yPos - newYPos) * 0.01f;
		rotationX += yaw;
		rotationY += pitch;
		if (rotationY > 90.0f) {
			rotationY = 90.0f;
		}
		else if (rotationY < -90.0f) {
			rotationY = -90.0f;
		}
	}

	newXPos = xPos;
	newYPos = yPos;
}

void loadOBJ(const char *path, std::vector <glm::vec3> &out_vertices, std::vector <glm::vec2> &out_textures, std::vector <glm::vec3> &out_normals) {
	std::vector< unsigned int > vertexIndices, textureIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_text;
	std::vector< glm::vec3 > temp_normals;

	std::string vertice = "v";
	std::string texture = "vt";
	std::string normal = "vn";
	std::string face = "f";

	std::string line;
	std::ifstream file(path);

	if (!file.is_open())
		std::cout << "Erro" << std::endl;

	while (!file.eof()) {
		std::getline(file, line);

		std::string coordType;
		float coordX;
		float coordY;
		float coordZ;
		
		std::stringstream data(line);

		data >> coordType;

		if (coordType == "v") {
			data >> coordX >> coordY >> coordZ;
			temp_vertices.push_back(glm::vec3(coordX, coordY, coordZ));
		}

		if (coordType == "vt") {
			data >> coordX >> coordY >> coordZ;

			temp_text.push_back(glm::vec3(coordX, coordY, coordZ));
		}

		if (coordType == "vn") {
			data >> coordX >> coordY >> coordZ;

			temp_normals.push_back(glm::vec3(coordX, coordY, coordZ));

		}

		if (coordType == "f") {

			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			for (int i = 0; i < 3; i++) {
				data >> vertexIndex[i];
				if (data.peek() == '/') {
					data.ignore();
				}
				data >> uvIndex[i];
				if (data.peek() == '/') {
					data.ignore();
				}
				data >> normalIndex[i];
			}

			std::cout << uvIndex[1];

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			textureIndices.push_back(uvIndex[0]);
			textureIndices.push_back(uvIndex[1]);
			textureIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
}

std::vector<glm::vec3> Load3DModel(void) {
	// 6 faces * 4 vértices por face
	glm::vec3 point[6 * 4] = {
		// Frente
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		// Trás
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		// Direita
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		// Esquerda
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		// Cima
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		// Baixo
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f)
	};

	std::vector<glm::vec3> ret;
	for (auto i : point)
		ret.push_back(i);

	return ret;
}

void showCube(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | ->operação bit a bit

	float *vertex_stream = static_cast<float*>(glm::value_ptr(obj.front()));

	std::vector<glm::vec3> colors{
		glm::vec3(1.0f, 0.0f, 0.0f), // Red
		glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
		glm::vec3(0.0f, 1.0f, 0.0f), // Green
		glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
		glm::vec3(0.0f, 0.0f, 1.0f), // Blue
		glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	// Desenha quad em modo imediato
	glBegin(GL_QUADS);
	for (int nv = 0; nv < 6 * 4 * 3; nv += 3) {
		glColor3f(colors[nv / (4 * 3)].r, colors[nv / (4 * 3)].g, colors[nv / (4 * 3)].b);
		glm::vec4 vertex = glm::vec4(vertex_stream[nv], vertex_stream[nv + 1], vertex_stream[nv + 2], 1.0f);
		// Cálculo das coordenadas de recorte
		glm::vec4 transformed_vertex = mvp * vertex;
		// Divisão de Perspetiva
		glm::vec4 normalized_vertex = transformed_vertex / transformed_vertex.w;
		// Desenho do vértice
		glVertex3f(normalized_vertex.x, normalized_vertex.y, normalized_vertex.z);
	}
	glEnd();
}

int main(void) {
	std::vector<glm::vec3> obj = Load3DModel();

	GLFWwindow *window;

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "P3D - Trabalho Pratico 1 (Part #1)", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	init();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, cursorCallback);

	glfwSetScrollCallback(window, scrollCallback);

	char path[128] = "C:\\Users\\VitorHugodeCastroeMe\\Desktop\\P3D-TP\\Iron_Man\\Iron_Man.obj";
	std::vector <glm::vec3> out_vertices;
	std::vector <glm::vec2> out_textures;
	std::vector <glm::vec3> out_normals;
	loadOBJ(path, out_vertices, out_textures, out_normals);

	// Projection
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	while (!glfwWindowShouldClose(window)) {
		// View
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, ZOOM),	// Posição da câmara no mundo
			glm::vec3(0.0f, 0.0f, -1.0f),	// Direção para a qual a câmara esta apontada
			glm::vec3(0.0f, 1.0f, 0.0f)		// Vector vertical -> camera.up
		);

		view = glm::rotate(view, glm::radians(rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(rotationY), glm::vec3(1.0f, 0.0f, 0.0f));

		// Model
		glm::mat4 model = glm::mat4(1.0f);

		// MVP
		glm::mat4 mvp = projection * view * model;

		showCube(obj, mvp);

		glfwSwapBuffers(window);
		glfwPollEvents();

		isFirstFrame = false;
	}

	glfwTerminate();
	return 0;
}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}