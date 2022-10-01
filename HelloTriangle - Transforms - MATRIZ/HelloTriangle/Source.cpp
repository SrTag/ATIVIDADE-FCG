/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 01/08/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;
//--------------------------------BIBLIOTECA--------------------------
// GLAD
#include <glad/glad.h>
#include "Shader.h"
// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//-----------------------------------------------------------------------
float Xscale = 100.0f, Yscale = 100.0f, Xpos = 400.0f, Ypos = 300.0f;
bool ScalePos = true;
// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es

int setupGeometry();
// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;



// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- JONATHAN-FARIAS", nullptr, nullptr);
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
	
	//TODA LARGURA E ALTURA DA MINHA TELA
	//glViewport(width/2, height/2, width/2, height/2);


	// Compilando e buildando o programa de shader
	Shader shader("../shaders/hello_triangle.vs","../shaders/hello_triangle.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();


	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a vari�veis do tipo uniform em GLSL para armazenar esse tipo de info
	// que n�o est� nos buffers
	

	glUseProgram(shader.ID);


	// aqui estamos inicializando a matriz como a matriz identidade
	glm::mat4 projection = glm::mat4(1);

	// esse comando vai funcionar como uma escala
	// EX)1
	//projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
	//EX)2
	projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

	//passando para o Shader a informa��o da matrix
	shader.setMat4("projection", glm::value_ptr(projection));

	
	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(10);

		//primeiro Viewport
		glViewport(0, 0, width, height );

		//////////////////////////////////////////MATRIZES////////////////////////////////////////////////////

		//DEFINI��O DA MATRIZ MODELO (transf. na geometria)
		glm::mat4 model = glm::mat4(1); // matriz identidade

		//(1) transladando a posi��o do triangulo
		model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
		//(2)rota��o
		model = glm::rotate(model, /*glm::radians(90.0f)*/(float)glfwGetTime(), glm::vec3(0, 0, 1));
		//(3)aplicando a escala
		model = glm::scale(model, glm::vec3(300.0f, 300.0f, 1.0f));

		//passando para o shader o valor dessa matriz
		shader.setMat4("model", glm::value_ptr(model));
		//////////////////////////////////////////////////////////////////////////////////////////////


		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		//glUniform4f(colorLoc, 0.5f, 0.5f, 1.0f, 1.0f); //(cor do preenchimento do TRIANGULO ou a forma que for)enviando cor para vari�vel uniform inputColor
		shader.setVec4("inputColor", 1.0f, 0.5f, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		// PONTOS - GL_POINTS
		//glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f); //(cor dos PONTOS OU LINHAS)enviando cor para vari�vel uniform inputColor
		shader.setVec4("inputColor", 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);

		/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////SEGUNDA MATRIZ////////////////////////////////////////////////////

		//DEFINI��O DA MATRIZ MODELO (transf. na geometria)
		model = glm::mat4(1); // matriz identidade

		//(1) transladando a posi��o do triangulo
		model = glm::translate(model, glm::vec3(600.0f, 300.0f, 0.0f));
		//(2)rota��o
		//model = glm::rotate(model, /*glm::radians(90.0f)*/(float)glfwGetTime(), glm::vec3(0, 0, 1));
		//(3)aplicando a escala
		if (Xscale >= 300.0f || Yscale >= 300.0f) 
		{
			ScalePos = false;
		}
		if (Xscale <= 0.0f || Yscale <= 0.0f)
		{
			ScalePos = true;
		}
		if (ScalePos == true) 
		{
			Xscale += glfwGetTime()/300;
			Yscale += glfwGetTime()/300;
			model = glm::scale(model, glm::vec3(Xscale, Yscale, 1.0f));
		}
		if (ScalePos == false)
		{
			Xscale -= glfwGetTime()/300;
			Yscale -= glfwGetTime()/300;
			model = glm::scale(model, glm::vec3(Xscale, Yscale, 1.0f));
		}
		
		

		//passando para o shader o valor dessa matriz
		shader.setMat4("model", glm::value_ptr(model));

		shader.setVec4("inputColor", 0.0f, 0.5f, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		shader.setVec4("inputColor", 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
		//////////////////////////////////////////////////////////////////////////////////////////////
		/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////TERCEIRA MATRIZ////////////////////////////////////////////////////

	
		
		//DEFINI��O DA MATRIZ MODELO (transf. na geometria)
		model = glm::mat4(1); // matriz identidade
		model = glm::translate(model, glm::vec3(Xpos, Ypos, 0.0f));


		//(1) transladando a posi��o do triangulo
		
		//(2)rota��o
		//model = glm::rotate(model, /*glm::radians(90.0f)*/(float)glfwGetTime(), glm::vec3(0, 0, 1));
		//(3)aplicando a escala
		model = glm::scale(model, glm::vec3(300.0f, 300.0f, 1.0f));

		//passando para o shader o valor dessa matriz
		shader.setMat4("model", glm::value_ptr(model));

		shader.setVec4("inputColor", 0.0f, 0.0f, 0.5f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		shader.setVec4("inputColor", 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
		//////////////////////////////////////////////////////////////////////////////////////////////
		
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

	if (key == GLFW_KEY_A) 
	{
		Xpos -= glfwGetTime();
	}
	if (key == GLFW_KEY_D)
	{
		Xpos += glfwGetTime();
	}
	if (key == GLFW_KEY_S)
	{
		Ypos -= glfwGetTime();
	}
	if (key == GLFW_KEY_W)
	{
		Ypos += glfwGetTime();
	}
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
	GLfloat vertices[] = {//EX 3-LISTA 2

		  0.5f , -0.5f, 0.0f,
		 -0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f,

	};

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

