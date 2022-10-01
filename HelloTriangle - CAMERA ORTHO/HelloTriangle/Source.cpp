/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 01/08/2022
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
float Xscale = 100.0f, Yscale = 100.0f, Xpos = 50.0f, Ypos = 500.0f;
float CorR=0,CorG=0,CorB=0;
bool ScalePos = true;
int QtdCubos = 0;

glm::mat4 model[100];

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções

int setupGeometry();
// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;



// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- JONATHAN-FARIAS", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
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
	
	//TODA LARGURA E ALTURA DA MINHA TELA
	//glViewport(width/2, height/2, width/2, height/2);


	// Compilando e buildando o programa de shader
	Shader shader("../shaders/hello_triangle.vs","../shaders/hello_triangle.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();


	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	

	glUseProgram(shader.ID);


	// aqui estamos inicializando a matriz como a matriz identidade
	glm::mat4 projection = glm::mat4(1);

	// esse comando vai funcionar como uma escala
	// EX)1
	//projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
	//EX)2
	projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

	//passando para o Shader a informação da matrix
	shader.setMat4("projection", glm::value_ptr(projection));

	//matriz de viwer
	glm::mat4 View = glm::mat4(1);
	View = glm::translate(View, glm::vec3(0.0f, 0.0f, -3.0f));
	shader.setMat4("View", glm::value_ptr(View));

	//Habilita teste de profundidade
	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(10);

		//primeiro Viewport
		glViewport(0, 0, width, height );

		//////////////////////////////////////////MATRIZES////////////////////////////////////////////////////

		
		model[0] = glm::mat4(1);
		//(1) transladando a posição do triangulo
		model[0] = glm::translate(model[0], glm::vec3(Xpos+100, Ypos-50, 0.0f));
		
		//(2)rotação
		model[0] = glm::rotate(model[0], /*glm::radians(90.0f)*/(float)glfwGetTime(), glm::vec3(0, 1, 0));
		//(3)aplicando a escala
		model[0] = glm::scale(model[0], glm::vec3(100.0f, 100.0f, 100.0f));

		//passando para o shader o valor dessa matriz
		shader.setMat4("model", glm::value_ptr(model[0]));
		//////////////////////////////////////////////////////////////////////////////////////////////


		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		//glUniform4f(colorLoc, 0.5f, 0.5f, 1.0f, 1.0f); //(cor do preenchimento do TRIANGULO ou a forma que for)enviando cor para variável uniform inputColor
		//shader.setVec4("inputColor", 1.0f, 0.5f, 0.0f, 1.0f);---------------------------------------
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//-----------------------------PAINT-------------------------------------------------------
		
		if (QtdCubos > 0) 
		{
			for (int i = 1; i < size(model); i++)
			{
				
				shader.setMat4("model", glm::value_ptr(model[i]));

				//shader.setVec4("inputColor", 1.0f, 0.0f, 0.0f, 1.0f);-------------------------------
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		


		//-----------------------------------------------------------------------------------
		glBindVertexArray(0);


		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}


// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_A)
	{
		Xpos -= 50;
	}
	if (key == GLFW_KEY_D)
	{
		Xpos += 50;
	}
	if (key == GLFW_KEY_S)
	{
		Ypos -= 50;
	}
	if (key == GLFW_KEY_W)
	{
		Ypos += 50;
	}
	if (key == GLFW_KEY_SPACE)
	{
		QtdCubos++;
		model[QtdCubos] = glm::mat4(1);
		model[QtdCubos] = glm::translate(model[QtdCubos], glm::vec3(Xpos, Ypos, 0.0f));
		model[QtdCubos] = glm::scale(model[QtdCubos], glm::vec3(50.0f, 50.0f, 1.0f));
	}



}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {//EX 3-LISTA 2

		//////FRENTE ////////////////////-------------FRENTE
		//1-metade quadrado   // R-G-B
		-0.5f, 0.5f, 0.0f,	  1.0, 1.0, 0.0,//B
		  0.5f , -0.5f, 0.0f, 0.0, 1.0, 1.0,//C
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 //2-metade quadrado
		 -0.5f, 0.5f, 0.0f,   1.0, 1.0, 0.0,//B
		 0.5f, 0.5f, 0.0f,    0.0, 1.0, 1.0,//D
		 0.5f, -0.5f, 0.0f,   1.0, 0.0, 1.0,//C
		 /////////////////////////////////////
		 //////TOPO ////////////////////---------------TOPO
		 //1-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, 0.5f, 0.0f,    0.0, 1.0, 1.0,//D
		 -0.5f, 0.5f, 0.0f,   1.0, 0.0, 1.0,//B
		 //2-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, 0.5f, -0.5f,   0.0, 1.0, 1.0,//4(D)
		 0.5f, 0.5f, 0.0f,    1.0, 0.0, 1.0,//D
		 /////////////////////////////////////
		 //////COSTAS ////////////////////----------COSTAS
		 //1-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, -0.5f, -0.5f,  1.0, 1.0, 0.0,//3(C)
		 -0.5f, -0.5f, -0.5f, 1.0, 1.0, 1.0,//1(A)
		 //2-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 1.0, 0.0,//2(B)
		  0.5f, 0.5f, -0.5f,  1.0, 1.0, 0.0,//4(D)
		  0.5f, -0.5f, -0.5f, 1.0, 1.0, 0.0,//3(C)
		  /////////////////////////////////////
		 //////ESQUERDA ////////////////////---------------ESQUERDA
		 //1-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 0.0, 1.0,//2(B)
		 -0.5f, -0.5f, -0.5f, 1.0, 0.0, 1.0,//1(A)
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 //2-METADE
		 -0.5f, 0.5f, -0.5f,  1.0, 0.0, 1.0,//2(B)
		 -0.5f, 0.5f, 0.0f,   1.0, 0.0, 1.0,//B
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 /////////////////////////////////////
		 //////PISO-BASE ////////////////////------------BASE
		 //1-METADE
		 -0.5f, -0.5f, -0.5f, 1.0, 1.0, 0.0,//1(A)
		 0.5f, -0.5f, 0.0f,   0.0, 1.0, 1.0,//C
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 //2-METADE TOPO
		 -0.5f, -0.5f, -0.5f, 1.0, 1.0, 0.0,//1(A)
		 0.5f, -0.5f, -0.5f,  0.0, 1.0, 1.0,//3(C)
		 0.5f, -0.5f, 0.0f,   1.0, 0.0, 1.0,//C
		 ////////////////////////////////////
		 //////DIREITA ////////////////////---------------DIREITA
		 //1-METADE
		 0.5f, 0.5f, 0.0f,   0.0, 0.0, 1.0,//D
		 0.5f, -0.5f, -0.5f, 0.0, 0.0, 1.0,//3(C)
		 0.5f, -0.5f, 0.0f,  0.0, 0.0, 1.0,//C
		 //2-METADE TOPO
		 0.5f, 0.5f, 0.0f,   0.0, 0.0, 1.0,//D
		 0.5f, 0.5f, -0.5f,  0.0, 0.0, 1.0,//4(D)
		 0.5f, -0.5f, -0.5f, 0.0, 0.0, 1.0,//3(C)

	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

