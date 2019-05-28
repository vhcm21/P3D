#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#define GLFW_USE_DWM_SWAP_INTERVAL

#define GLEW_STATIC
#pragma region Includes GL, GLFW, GLM
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#pragma endregion
#include "LoadShaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void init(void);

#define WIDTH 800
#define HEIGHT 600

#define NumVAOs 1
#define NumBuffers 3 // V�rtices, Cores
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

GLuint programa;

GLfloat ZOOM = 10.0f;

float rotationX = 0.0f, rotationY = 0.0f;
float yaw = 0.0f, pitch = 0.0f;
float newXPos, newYPos;
bool isFirstFrame = true;

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	// Se faz zoom in
	if (yoffset == 1) {
		// Incremento no zoom, varia com a dist�ncia da c�mara
		ZOOM += fabs(ZOOM) * 0.1f;  // abs - valor absoluto -> significa que quanto mais longe o objeto estiver, maior � a capacidade de zoom
	}
	// Sen�o, se faz zoom out
	else if (yoffset == -1) {
		// Incremento no zoom, varia com a dist�ncia da c�mara
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

		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			out_vertices.push_back(vertex);

			unsigned int textureIndex = textureIndices[i];
			glm::vec2 text = temp_text[textureIndex - 1];
			out_textures.push_back(text);

			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = temp_normals[normalIndex - 1];
			out_normals.push_back(normal);
		}
	}
}

std::vector<glm::vec3> Load3DModel(void) {
	// 6 faces * 4 v�rtices por face
	glm::vec3 point[6 * 4] = {
		// Frente
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		// Tr�s
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

void init_two_VBO_Packed_Arrays(std::vector <glm::vec3> &vertices, 
	std::vector <glm::vec2> &textures, std::vector <glm::vec3> &normals) {
	// ****************************************************
	// VAOs - Vertex Array Objects
	// ****************************************************

	// Gerar 'NumVAOs' nomes para VAOs.
	// Neste caso, como NumVAOS = 1, gera apenas 1 nome.
	glGenVertexArrays(NumVAOs, VAOs);
	// Faz bind do VAO, cujo nome est� definido em 'VAOs[0]', com o contexto do OpenGL.
	// Um VAO � criado no primero bind que lhe seja feito.
	// Este VAO passa a estar ativo at� que seja feito o bind a outro VAO, ou seja feito o bind com valor 0.
	glBindVertexArray(VAOs[0]);

	// ****************************************************
	// VBOs - Vertex Buffer Objects
	// ****************************************************

	// Gera 'NumBuffers' nomes para VBOs.
	// Neste caso, como NumBuffers = 2, gera 2 nomes
	// Esta fun��o pode ser chamada antes da cria��o de VAOs.
	glGenBuffers(NumBuffers, Buffers);
	// Para cada nome de VBO
	for (int i = 0; i < NumBuffers; i++) {
		// Faz bind do VBO ao buffer GL_ARRAY_BUFFER.
		// GL_ARRAY_BUFFER � utilizado para dados de atributos de v�rtices.
		// Um VBO � criado no primeiro bind que lhe seja feito.
		// Este VBO passa a estar ativo at� que seja feito o bind a outro VBO ou seja feito o bind com valor 0.
		// Para alocar a mem�ria para os BO, n�o � necess�rio que j� esteja vinculado um VAO.
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[i]);
		if (i == 0) {
			// Inicializa o VBO (que est� ativo) com mem�ria de tamanho imut�vel.
			glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertices) /*2 * 6 * sizeof(float)*/, &vertices.front(), 0);
			// Inicializa o VBO (que est� ativo) com mem�ria de tamanho mut�vel.
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		}
		else if (i == 1) {
			// Inicializa o VBO (que est� ativo) com mem�ria de tamanho imut�vel.
			glBufferStorage(GL_ARRAY_BUFFER, sizeof(textures) /*3 * 6 * sizeof(float)*/, &textures.front(), 0);
			// Inicializa o VBO (que est� ativo) com mem�ria de tamanho mut�vel.
			//glBufferData(GL_ARRAY_BUFFER, sizeof(cores), vertices, GL_STATIC_DRAW);
		}
		else {
			glBufferStorage(GL_ARRAY_BUFFER, sizeof(normals) /*2 * 6 * sizeof(float)*/, &normals.front(), 0);
		}
	}

	// ****************************************************
	// Shaders
	// ****************************************************

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER,   "shaders/vertShader.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/fragShader.frag" },
		{ GL_NONE, NULL }
	};

	programa = LoadShaders(shaders);
	if (!programa) exit(EXIT_FAILURE);
	glUseProgram(programa);

	// ****************************************************
	// Ligar Atributos aos Shaders
	// ****************************************************

	// As localiza��es de atributos fazem parte do programa shader, e s�o completamente independentes do VAO.
	// Podemos, assim, obter as localiza��es de atributos sem que antes se tenha criado qualquer VAO.
	// ------------------------------------------------------------------------------------------------------
	// Obt�m a localiza��o do atributo 'vPosition' no 'programa'.
	//GLint coordsId = glGetAttribLocation(programa, "vPosition"); // Para vers�es anteriores � 4.3
	GLint coordsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vPosition"); // Para vers�es >= 4.3
	GLint uvsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vTextureCoords");

	//glBindVertexArray(VAOs[0]); // N�o � necess�rio fazer o bind do VAO, pois ele j� � o que est� ativo no contexto do OpenGL.

	// Ativa o VBO 'Buffers[0]'.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	// Liga a localiza��o do atributo 'vPosition' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui j� � necess�rio ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// � neste ponto que o VBO fica associado ao VAO.
	// Especifica tamb�m como � que a informa��o do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo ir� receber, por v�rtice, 2 elementos do tipo float.
	glVertexAttribPointer(coordsId, 3 /*2 elementos por v�rtice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);

	// Ativa o VBO 'Buffers[1]'.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	// Liga a localiza��o do atributo 'vColors' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui j� � necess�rio ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// � neste ponto que o VBO fica associado ao VAO.
	// Especifica tamb�m como � que a informa��o do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo ir� receber, por v�rtice, 3 elementos do tipo float.
	glVertexAttribPointer(uvsId, 2 /*3 elementos por v�rtice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);

	// Habitita o atributo com localiza��o 'coresId' para o VAO ativo.
	// Os atributos de um VAO est�o desativados por defeito.
	glEnableVertexAttribArray(coordsId);
	// Habitita o atributo com localiza��o 'coresId' para o VAO ativo.
	// Os atributos de um VAO est�o desativados por defeito.
	glEnableVertexAttribArray(uvsId);
}

void display(std::vector <glm::vec3> &vertices) {
	static const GLfloat black[] = {
		0.0f, 0.0f, 0.0f, 0.0f
	};
	// Limpa o buffer de cor
	glClearBufferfv(GL_COLOR, 0, black);

	// Vincula (torna ativo) o VAO
	glBindVertexArray(VAOs[0]);
	// Envia comando para desenho de primitivas GL_TRIANGLES, que utilizar� os dados do VAO vinculado.
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void load_texture(void) {
	GLuint textureName = 0;

	// Gera um nome de textura
	glGenTextures(1, &textureName);

	// Ativa a Unidade de Textura #0
	// A Unidade de Textura 0 j� est� ativa por defeito.
	// S� uma Unidade de Textura est� ativa a cada momento.
	glActiveTexture(GL_TEXTURE0);

	// Vincula esse nome de textura ao target GL_TEXTURE_2D da Unidade de Textura ativa.
	glBindTexture(GL_TEXTURE_2D, textureName);

	// Define os par�metros de filtragem (wrapping e ajuste de tamanho)
	// para a textura que est� vinculada ao target GL_TEXTURE_2D da Unidade de Textura ativa.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Leitura/descompress�o do ficheiro com imagem de textura
	int width, height, nChannels;
	// Ativa a invers�o vertical da imagem, aquando da sua leitura para mem�ria.
	stbi_set_flip_vertically_on_load(true);
	// Leitura da imagem para mem�ria do CPU
	unsigned char *imageData = stbi_load("C:\\Users\\VitorHugodeCastroeMe\\Desktop\\P3D-TP\\Iron_Man\\Iron_Man_D.tga", &width, &height, &nChannels, 0);
	if (imageData) {
		// Carrega os dados da imagem para o Objeto de Textura vinculado ao target GL_TEXTURE_2D da Unidade de Textura ativa.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// Gera o Mipmap para essa textura
		glGenerateMipmap(GL_TEXTURE_2D);

		// Liberta a imagem da mem�ria do CPU
		stbi_image_free(imageData);
	}
	else {
		std::cout << "Error loading texture!" << std::endl;
	}
}


int main(void) {
	//std::vector<glm::vec3> obj = Load3DModel();

	std::vector <glm::vec3> vertices;
	std::vector <glm::vec2> textures;
	std::vector <glm::vec3> normals;

	GLFWwindow *window;

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "P3D - Trabalho Pratico 1 (Part #1)", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	load_texture();

	init();

	init_two_VBO_Packed_Arrays(vertices, textures, normals);

	// Indica��o da Unidade de Textura a ligar ao sampler 'TexSampler'.
	//glUniform1i(glGetUniformLocation(programa, "TexSampler"), 0 /* Unidade de Textura #0 */);
	GLint locationTexSampler = glGetProgramResourceLocation(programa, GL_UNIFORM, "TexSampler");
	glProgramUniform1i(programa, locationTexSampler, 0 /* Unidade de Textura #0 */);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, cursorCallback);

	glfwSetScrollCallback(window, scrollCallback);

	char path[128] = "C:\\Users\\VitorHugodeCastroeMe\\Desktop\\P3D-TP\\Iron_Man\\Iron_Man.obj";
	loadOBJ(path, vertices, textures, normals);

	// Projection
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	while (!glfwWindowShouldClose(window)) {

		// View
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, ZOOM),	// Posi��o da c�mara no mundo
			glm::vec3(0.0f, 0.0f, -1.0f),	// Dire��o para a qual a c�mara esta apontada
			glm::vec3(0.0f, 1.0f, 0.0f)		// Vector vertical -> camera.up
		);

		view = glm::rotate(view, glm::radians(rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(rotationY), glm::vec3(1.0f, 0.0f, 0.0f));

		// Model
		glm::mat4 model = glm::mat4(1.0f);

		// MVP
		glm::mat4 mvp = projection * view * model;

		display(vertices);

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