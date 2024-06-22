/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"

#include "Mesh.h"

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int setupShader();
int setupGeometry();
int loadTexture(string path);
glm::mat4 doAction(string action, glm::mat4 model);
int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// C�digo fonte do Vertex Shader (em GLSL): ainda hardcoded
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

//C�difo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"in vec2 texCoord;\n"
"out vec4 color;\n"
"uniform sampler2D tex_buffer;\n"
"void main()\n"
"{\n"
"color = texture(tex_buffer, texCoord);\n"
"}\n\0";

string keyAction = "";

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Luiz Silva!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	// GLuint shaderID = setupShader();
	 
	Shader shader("vertex.vs", "fragment.fs");

	GLuint texID = loadTexture("../textures/Logosheep.png");

	int nVerts;
	GLuint VAO = loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts);


	Mesh suzanne;
	suzanne.initialize(VAO, nVerts, &shader, glm::vec3(-2.75, 0.0, 0.0));


	glUseProgram(shaderID);

	glEnable(GL_DEPTH_TEST);


	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		model = glm::mat4(1); 
		model = doAction(keyAction, model);

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 48);

		glBindVertexArray(0); //unbind - desconecta
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura
	

		glDrawArrays(GL_POINTS, 0, 48);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}


glm::mat4 doAction(string keyAction, glm::mat4 model) {

	float angle = (GLfloat)glfwGetTime();
	float scaleAmount = static_cast<float>(glfwGetTime());

	if (keyAction == "rotateX"){
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	else if (keyAction == "rotateY"){
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	else if (keyAction == "rotateZ"){
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

// Fun��o de callback de teclado 
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

int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compila��o (exibi��o via log no terminal)
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
	// Checando erros de compila��o (exibi��o via log no terminal)
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

		//Base da pir�mide: 2 tri�ngulos
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
	
	//Atributo posi��o (x, y, z)
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

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os par�metros de wrapping e filtering
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

	nVerts = vbuffer.size() / 11; //Provis�rio

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do v�rtice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);


	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;

}
