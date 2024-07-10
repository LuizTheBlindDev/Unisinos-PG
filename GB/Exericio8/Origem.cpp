#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD e GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Mesh.h"
#include "Hermite.h"
#include "Bezier.h"
#include "CatmullRom.h"

// Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();
int loadTexture(string path);
glm::mat4 doAction(string action, glm::mat4 model);
void drawCube(int VAO);
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color = glm::vec3(1.0, 0.0, 1.0));
void drawOBJ(GLuint VAO, int nVertices, Shader shader, glm::vec3 position, int texID);
void key_callbackFP(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
vector<glm::vec3> generateControlPointsSet();


// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

string keyAction = "";

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05;
float pitch = 0.0, yaw = -90.0;


// Função MAIN
int main()
{
	// Inicialização da GLFW e configuração de janela
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Luiz Silva!", nullptr, nullptr);
	glfwMakeContextCurrent(window);


	// callback e config para teclado e mouse
	glfwSetKeyCallback(window, key_callbackFP);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("shaders/vertex.vs", "shaders/fragment.fs");
	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "sp"), 0);

	//Matriz de view -- posição e orientação da câmera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	
	int nVerts;
	GLuint VAOd = loadSimpleOBJ("../../3D_models/Naves/Destroyer05.obj", nVerts);
	GLuint texDest = loadTexture("C:/RepositorioVS/3D_Models/Naves/Texture/T_Spase_64.png");

	//int nVerts;
	//GLuint VAOl = loadSimpleOBJ("../../3D_models/Naves/LightCruiser05.obj", nVerts);
	//GLuint texLight = loadTexture("C:/RepositorioVS/3D_Models/Naves/Texture/T_Spase_Blue.png");

	std::vector<glm::vec3> controlPoints = generateControlPointsSet();

	Bezier bezier;
	bezier.setControlPoints(controlPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(2000);

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		//Atualizando a posição e orientação da câmera
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", glm::value_ptr(view));

		//Atualizando o shader com a posição da câmera
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		i = (i + 1) % nbCurvePoints;
		glm::vec3 position = bezier.getPointOnCurve(i);

		//drawOBJ(VAOl, nVerts, shader, position, texLight);
		drawOBJ(VAOd, nVerts, shader, position, texDest);

		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAOd);
	//glDeleteVertexArrays(1, &VAOl);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}


// Função de callback de teclado 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (keyAction == "rotateX") {
			keyAction = " ";
			glfwSetTime(0);
		}
		else keyAction = "rotateX";
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (keyAction == "rotateY") {
			keyAction = " ";
			glfwSetTime(0);
		}
		else keyAction = "rotateY";
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (keyAction == "rotateZ") {
			keyAction = " ";
			glfwSetTime(0);
		}
		else keyAction = "rotateZ";
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		 keyAction = "pressW";
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		keyAction = "pressS";
	}

	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		keyAction = "pressA";
	}

	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		keyAction = "pressD";
	}

	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		glfwSetTime(1);
		keyAction = "pressQ";
	}

	if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(1);
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		glfwSetTime(1);
		keyAction = "pressE";
	}

	if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		keyAction = " ";
		glfwSetTime(1);
	}
}

//Função de ação correpondente a tecla pressionada
glm::mat4 doAction(string keyAction, glm::mat4 model) {

	float angle = (GLfloat)glfwGetTime();
	float scaleAmount = static_cast<float>(glfwGetTime());

	if (keyAction == "rotateX") {
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	else if (keyAction == "rotateY") {
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	else if (keyAction == "rotateZ") {
		model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (keyAction == "pressW") {
		model = glm::translate(model, glm::vec3(0.0f, scaleAmount, 0.0f));
	}

	else if (keyAction == "pressS") {
		model = glm::translate(model, glm::vec3(0.0f, -scaleAmount, 0.0f));
	}

	else if (keyAction == "pressA") {
		model = glm::translate(model, glm::vec3(-scaleAmount, 0.0f, 0.0f));
	}

	else if (keyAction == "pressD") {
		model = glm::translate(model, glm::vec3(scaleAmount, 0.0f, 0.0f));
	}

	else if (keyAction == "pressQ") {
		model = glm::scale(model, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
	}

	else if (keyAction == "pressE") {
		model = glm::scale(model, glm::vec3(1 / scaleAmount, 1 / scaleAmount, 1 / scaleAmount));
	}

	return model;
}

//Configuração de shader(!!refactored!!)
int setupShader(){

	// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
	const GLchar* vertexShaderSource = "#version 450\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"layout(location = 2) in vec2 tex_coord;\n"
		"uniform mat4 model;\n"
		"out vec4 finalColor;\n"
		"out vec2 texCoord;\n"
		"void main()\n"
		"{\n"
		"gl_Position = model * vec4(position, 1.0);\n"
		"finalColor = vec4(color, 1.0);\n"
		"texCoord = vec2(tex_coord.x, tex_coord.y);\n"
		"}\0";

	//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
	const GLchar* fragmentShaderSource = "#version 450\n"
		"in vec4 finalColor;\n"
		"in vec2 texCoord;\n"
		"out vec4 color;\n"
		"uniform sampler2D tex_buffer;\n"
		"void main()\n"
		"{\n"
		"color = texture(tex_buffer, texCoord);\n"
		"}\n\0";

	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

//Geometria de um cubo com textura
int setupGeometry(){
	GLfloat vertices[] = {

		//Base da pirâmide: 2 triângulos
		//x     y      z     r    g    b
		-0.5 , -0.5 , -0.5, 1.0, 0.0, 0.0, 1.0, 1.0,
		-0.5 , -0.5 ,  0.5, 1.0, 0.0, 0.0, 1.0, 0.0,
		 0.5 , -0.5 , -0.5, 1.0, 0.0, 0.0, 0.0, 1.0,

		 -0.5 , -0.5 ,  0.5, 1.0, 0.0, 0.0, 1.0, 0.0,
		  0.5 , -0.5 ,  0.5, 1.0, 0.0, 0.0, 0.0, 0.0,
		  0.5 , -0.5 , -0.5, 1.0, 0.0, 0.0, 0.0, 0.0,

		 //
		 -0.5 , -0.5 , -0.5, 0.0, 1.0, 0.0, 1.0, 1.0,
		 -0.5 , 0.5 ,  -0.5, 0.0, 1.0, 0.0, 1.0, 0.0,
		  0.5 , -0.5 , -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,

		 -0.5 ,  0.5 ,  -0.5, 0.0, 1.0, 0.0, 1.0, 0.0,
		  0.5 ,  0.5 ,  -0.5, 0.0, 1.0, 0.0, 0.0, 0.0,
		  0.5 , -0.5 ,  -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,

		  -0.5 , -0.5 , -0.5, 1.0, 1.0, 0.0, 1.0, 1.0,
		  -0.5 ,  0.5 , -0.5, 1.0, 1.0, 0.0, 1.0, 0.0,
		  -0.5 , -0.5 ,  0.5, 1.0, 1.0, 0.0, 0.0, 1.0,

		  -0.5 ,  0.5 , -0.5, 1.0, 1.0, 0.0, 1.0, 0.0,
		  -0.5 ,  0.5 ,  0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
		  -0.5 , -0.5 ,  0.5, 1.0, 1.0, 0.0, 0.0, 1.0,

		 -0.5 , -0.5 ,  0.5,  0.0, 0.0, 1.0, 1.0, 1.0,
		 -0.5 ,  0.5 ,  0.5,  0.0, 0.0, 1.0, 1.0, 0.0,
		  0.5 , -0.5 ,  0.5,  0.0, 0.0, 1.0, 0.0, 1.0,

		 -0.5 ,  0.5 ,  0.5, 0.0, 0.0, 1.0, 1.0, 0.0,
		  0.5 ,  0.5 ,  0.5, 0.0, 0.0, 1.0, 0.0, 0.0,
		  0.5 , -0.5 ,  0.5, 0.0, 0.0, 1.0, 0.0, 1.0,


		  0.5 , -0.5 , -0.5, 1.0, 0.0, 1.0, 1.0, 1.0,
		  0.5 ,  0.5 , -0.5, 1.0, 0.0, 1.0, 1.0, 0.0,
		  0.5 , -0.5 , 0.5,  1.0, 0.0, 1.0, 0.0, 1.0,

		  0.5 ,  0.5 , -0.5, 1.0, 0.0, 1.0, 1.0, 0.0,
		  0.5 ,  0.5 ,  0.5, 1.0, 0.0, 1.0, 0.0, 0.0,
		  0.5 , -0.5 ,  0.5, 1.0, 0.0, 1.0, 0.0, 1.0,

		-0.5 , 0.5 , -0.5, 1.0, 0.0, 0.5, 1.0, 1.0,
		-0.5 , 0.5 ,  0.5, 1.0, 0.0, 0.5, 1.0, 0.0,
		 0.5 , 0.5 , -0.5, 1.0, 0.0, 0.5, 0.0, 1.0,

		 -0.5 , 0.5 ,  0.5, 1.0, 0.0, 0.5, 1.0, 0.0,
		  0.5 , 0.5 ,  0.5, 1.0, 0.0, 0.5, 0.0, 0.0,
		  0.5 , 0.5 , -0.5, 1.0, 0.0, 0.5, 0.0, 1.0,

	};

	unsigned int indices[] = {
		0, 1, 3, // primeiro triangulo
		1, 2, 3  // segundo triangulo
	};

	GLuint VBO, VAO, EBO;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo textura (s , t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

//carrega textura
int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

//Função para desenhar um cubo
void drawCube(int VAO) {
	GLuint texID = loadTexture("../textures/Logosheep.png");


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 48);

	glBindVertexArray(0); //unbind - desconecta
	glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura


	glDrawArrays(GL_POINTS, 0, 48);
	glBindVertexArray(0);
}

//Função para carregar OBJ
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color)
{
	vector <glm::vec3> vertices;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	ifstream inputFile;
	inputFile.open(filepath.c_str());
	if (inputFile.is_open())
	{
		char line[100];
		string sline;



		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;

			istringstream ssline(line);
			ssline >> word;

			//cout << word << " ";
			if (word == "v")
			{
				glm::vec3 v;

				ssline >> v.x >> v.y >> v.z;

				vertices.push_back(v);
			}
			if (word == "vt")
			{
				glm::vec2 vt;

				ssline >> vt.s >> vt.t;

				texCoords.push_back(vt);
			}
			if (word == "vn")
			{
				glm::vec3 vn;

				ssline >> vn.x >> vn.y >> vn.z;

				normals.push_back(vn);
			}
			if (word == "f")
			{
				string tokens[3];

				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++)
				{
					//Recuperando os indices de v
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;
					indices.push_back(index);

					vbuffer.push_back(vertices[index].x);
					vbuffer.push_back(vertices[index].y);
					vbuffer.push_back(vertices[index].z);
					vbuffer.push_back(color.r);
					vbuffer.push_back(color.g);
					vbuffer.push_back(color.b);

					//Recuperando os indices de vts
					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;

					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					//Recuperando os indices de vns
					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;

					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}

		}

	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}
	inputFile.close();

	GLuint VBO, VAO;

	nVerts = vbuffer.size() / 11; //Provisório

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do vértice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);


	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;

}

//Função para Desenhar OBJ
void drawOBJ(GLuint VAO, int nVertices,Shader shader, glm::vec3 position, int texID) {

	Mesh obj;
	obj.initialize(VAO, nVertices, &shader, position, glm::vec3(0.5,0.5,0.5), 180.0);

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", -2.0, 10.0, 2.0);
	shader.setVec3("lightColor", 1.0, 1.0, 1.0);

	// Chamada de desenho - drawcall
	shader.setFloat("q", 1.0);
	obj.update();
	obj.draw(texID);
}

//Função callback do teclado para primeira pessoa
void key_callbackFP(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float cameraSpeed = 1.00;

	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

//Função callback do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//cout << xpos << " " << ypos << endl;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetx *= sensitivity;
	offsety *= sensitivity;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

vector<glm::vec3> generateControlPointsSet()
{
	vector <glm::vec3> controlPoints;

	controlPoints.push_back(glm::vec3(-200.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(-180.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(-160.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(-140.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(-120.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(-100.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( -80.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( -60.0, -0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( -40.0, -0.6, -50.0 ));
	controlPoints.push_back(glm::vec3( -20.0, -0.2, -50.0 ));
	controlPoints.push_back(glm::vec3(   0.0,  0.0, -50.0 ));
	controlPoints.push_back(glm::vec3(  20.0,  0.2, -50.0 ));
	controlPoints.push_back(glm::vec3(  40.0,  0.6, -50.0 ));
	controlPoints.push_back(glm::vec3(  60.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3(  80.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 100.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 120.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 140.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 160.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 180.0,  0.4, -50.0 ));
	controlPoints.push_back(glm::vec3( 200.0,  0.4, -50.0 ));

	return controlPoints;
}

vector<glm::vec3> generateControlPointsSetLight()
{
	vector <glm::vec3> controlPoints;

	controlPoints.push_back(glm::vec3( -200.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3( -180.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3( -160.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3( -140.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3( -120.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3( -100.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  -80.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  -60.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  -40.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  -20.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(    0.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(   20.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(   40.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(   60.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(   80.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  100.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  120.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  140.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  160.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  180.0, -30.0, -10.0));
	controlPoints.push_back(glm::vec3(  200.0, -30.0, -10.0));

	return controlPoints;
}

