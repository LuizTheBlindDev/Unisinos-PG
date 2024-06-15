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

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int setupShader();
int setupGeometry();
int loadTexture(string path);

class triangulo {
public:
	GLfloat vertice1[6];
	GLfloat vertice2[6];
	GLfloat vertice3[6];
	GLfloat vertices[18];
	GLfloat position[3];
	GLfloat color[3];
	GLfloat size;

	triangulo(GLfloat position[3], GLfloat size, GLfloat color[3]) {
		this->position[0] = position[0];
		this->position[1] = position[1];
		this->position[2] = position[2];

		this->size = size;

		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
	};

			void verticeUpdate() {

				vertice1[0] = position[0] + -size / 2;
				vertice1[1] = position[1] + -size / 2;
				vertice1[2] = position[2];
				vertice1[3] = color[0];
				vertice1[4] = color[1];
				vertice1[5] = color[2];

				vertice2[0] = position[0] + -size / 2;
				vertice2[1] = position[1] + size / 2;
				vertice2[2] = position[2];
				vertice2[3] = color[0];
				vertice2[4] = color[1];
				vertice2[5] = color[2];

				vertice3[0] = position[0] + size / 2;
				vertice3[1] = position[1] + -size / 2;
				vertice3[2] = position[2];
				vertice3[3] = color[0];
				vertice3[4] = color[1];
				vertice3[5] = color[2];

				vertices[0] = vertice1[1];
				vertices[1] = vertice1[2];
				vertices[2] = vertice1[3];
				vertices[3] = vertice1[4];
				vertices[4] = vertice1[5];
				vertices[5] = vertice1[6];

				vertices[6] = vertice2[0];
				vertices[7] = vertice2[1];
				vertices[8] = vertice2[2];
				vertices[9] = vertice2[3];
				vertices[10] = vertice2[4];
				vertices[11] = vertice2[5];

				vertices[12] = vertice3[0];
				vertices[13] = vertice3[1];
				vertices[14] = vertice3[2];
				vertices[15] = vertice3[3];
				vertices[16] = vertice3[4];
				vertices[17] = vertice3[5];
			};

	void updateTriangulo() {
		GLfloat vertices1[] = {
		-0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
			-0.5, -0.5, 0.5, 1.0, 0.0, 0.0,
			0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
		};


		for (int i = 0; i < 18; i++) {
			vertices[i] = vertices1[i];
		};
	};

	void setPosition(GLfloat x, GLfloat y, GLfloat z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;

		updateTriangulo();
	};


};

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

bool rotateX=false, rotateY=false, rotateZ=false, pressW=false, pressA = false, pressS = false, pressD = false, pressQ = false, pressE = false;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Luiz Silva!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
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
	GLuint shaderID = setupShader();

	GLuint texID = loadTexture("../textures/Logosheep.png");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();
	glUseProgram(shaderID);
	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	//
	//model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

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

		float angle = (GLfloat)glfwGetTime();
		float scaleAmount = static_cast<float>(glfwGetTime());

		model = glm::mat4(1); 
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}
		
		if(pressW) {
			model = glm::translate(model, glm::vec3(0.0f, scaleAmount, 0.0f));
		} 

		else if (pressS) {
			model = glm::translate(model, glm::vec3(0.0f, -scaleAmount, 0.0f));
		}

		else if (pressA) {
			model = glm::translate(model, glm::vec3(-scaleAmount, 0.0f, 0.0f));
		}

		else if (pressD) {
			model = glm::translate(model, glm::vec3(scaleAmount, 0.0f, 0.0f));
		}

		else if (pressQ) {
			model = glm::scale(model, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
		}

		else if (pressE) {
			model = glm::scale(model, glm::vec3(1 / scaleAmount, 1 / scaleAmount, 1 / scaleAmount));
		}
		

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 48);

		glBindVertexArray(0); //unbind - desconecta
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 48);

		glDrawArrays(GL_POINTS, 0, 48);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (rotateX == true) {
			rotateX = false;
			glfwSetTime(0);
		} else rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		if (rotateY == true) {
			rotateY = false;
			glfwSetTime(0);
		}
		else rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		if (rotateZ == true) {
			rotateZ = false;
			glfwSetTime(0);
		}
		else rotateZ = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}

		pressW = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		pressW = false; 
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}

		pressS = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		pressS = false;
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}

		pressA = true;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		pressA = false;
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}

		pressD = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		pressD = false;
		glfwSetTime(0);
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}
		glfwSetTime(1);
		pressQ = true;
	}

	if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
		pressQ = false;
		glfwSetTime(1);
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		if (rotateX == true || rotateY == true || rotateZ == true) {
			rotateX = false;
			rotateY = false;
			rotateZ = false;
		}
		glfwSetTime(1);
		pressE = true;
	}
	if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		pressE = false;
		glfwSetTime(1);
	}

}

//Esta fun��o est� basntante hardcoded - objetivo � compilar e "buildar" um programa de
// shader simples e �nico neste exemplo de c�digo
// O c�digo fonte do vertex e fragment shader est� nos arrays vertexShaderSource e
// fragmentShader source no ini�io deste arquivo
// A fun��o retorna o identificador do programa de shader
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

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados


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

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
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


