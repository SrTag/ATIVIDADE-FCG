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
//------------------------VARIAVEIS DO GBA-----------------------------------------------
#define MAX_X 4
#define MAX_Y 4
#define MAX_Z 4
#define MAX_CORES 10

bool BlocoPintado = false;
int taPintado[MAX_X][MAX_Y][MAX_Z];
int CursorPosition[MAX_X][MAX_Y][MAX_Z];
int Grid[MAX_X][MAX_Y][MAX_Z];
int indiceCor=2; // inicializando a cor com vermelho

glm::vec3 posVoxelCursor = glm::vec3(0, 0, 0);
glm::vec4 paletaCores[MAX_CORES]
{
	glm::vec4(0.0f,0.0f,0.0f,1.0f),//preto
	glm::vec4(1.0f,1.0f,1.0f,1.0f),//branco
	glm::vec4(1.0f,0.0f,0.0f,1.0f),//vermelho
	glm::vec4(0.0f,1.0f,0.0f,1.0f),//verde
	glm::vec4(0.0f,0.0f,1.0f,1.0f),//azul
	glm::vec4(1.0f,1.0f,0.0f,1.0f),//amarelo
	glm::vec4(0.0f,1.0f,1.0f,1.0f),//ciano
	glm::vec4(1.0f,0.0f,1.0f,1.0f),//magenta
	glm::vec4(0.5f,0.5f,0.5f,1.0f),//cinza
	glm::vec4(1.0f,1.0f,1.0f,0.3f),//cor Voxel vazio
};

GLuint VAO_Voxel;
// Compilando e buildando o programa de shader
Shader shader;

void desenharCursor();
void inicializarGrid();
void desenharGrid();
void addVox();


float  Xpos = 0.0f, Ypos = 0.0f, Zpos = 3.0f;

bool ScalePos = true;
int QtdCubos = 0;


const GLuint WIDTH = 800, HEIGHT = 600;

//VARIAVEIS DA CAMERA--------------------------------------------------
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
//para calcular o quanto que o mouse deslocou
float yaw = -90.0, pitch = 0.0; //rotação em x e y
//---------------------------------------------------------------

glm::mat4 model;

// Protótipo da função de callback de teclado e de mouse
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateCameraPos(GLFWwindow* window);

// Protótipos das funções

int setupGeometry();
// Dimensões da janela (pode ser alterado em tempo de execução)




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

	glfwSetCursorPosCallback(window, mouse_callback);

	//Desabilitando o cursor do mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	
	shader = Shader("../shaders/hello_triangle.vs", "../shaders/hello_triangle.fs");
	
	// Gerando um buffer simples, com a geometria de um triângulo
	VAO_Voxel = setupGeometry();

	//HABILITANDO TRANSPARENCIA
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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

	//MATRIZ DE PROJEÇÃO PERSPECTIVA
	projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	//passando para o Shader a informação da matrix
	shader.setMat4("projection", glm::value_ptr(projection));
	
	//Matriz de VIWERs
	glm::mat4 View = glm::mat4(1);

	View = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),//POSIÇÃO PONTO
		glm::vec3(0.0f, 0.0f, 0.0f), //TARGET(PONTO,NÃO VETOR) -> DIR = TARGET - POS
		glm::vec3(0.0f, 1.0f, 0.0f));//UP(VETOR)

	shader.setMat4("View", glm::value_ptr(View));

	//Habilita teste de profundidade
	glEnable(GL_DEPTH_TEST);

	//tamanho da linha e pontos
	glLineWidth(10);
	glPointSize(10);

	//chamando a inicialização da grid
	inicializarGrid();

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		//CHAMA A CAMERA PERSPECTIVE PARA MOVIMENTAR
		updateCameraPos(window);

		// atualizando a matriz de viwer com as novas posições
		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("View", glm::value_ptr(View));

		// Limpa o buffer de cor
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//primeiro Viewport
		glViewport(0, 0, width, height );

		//chamada do DESENHO----------------------------------------------------------
		
		desenharGrid();
		//desenharCursor();
		Grid[(int)Xpos][(int)Ypos][(int)Zpos] = indiceCor;
		
		glBindVertexArray(0);


		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO_Voxel);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}


void inicializarGrid()
{

	for (int x = 0; x < MAX_X; x++)
	{
		for (int y = 0; y < MAX_Y; y++)
		{
			for (int z = 0; z < MAX_Z; z++)
			{
				Grid[x][y][z] = -1;
				taPintado[x][y][z] = 0;
			}
		}
	}
}

void desenharGrid()
{
	
	float LarguraVoxel = 1.0;
	float AlturaVoxel = 1.0;
	float ProfundVoxel = 1.0;

	glBindVertexArray(VAO_Voxel);

	for (int x = 0; x < MAX_X; x++)
	{
		for (int y = 0; y < MAX_Y; y++)
		{
			for (int z = 0; z < MAX_Z; z++)
			{
				float dx = x * LarguraVoxel;
				float dy = y * AlturaVoxel;
				float dz = z * ProfundVoxel;

				glm::mat4 model = glm::mat4(1);
				//transladar
				model = glm::translate(model,glm::vec3(dx, dy, dz));
				//mandar pro shader
				shader.setMat4("model", glm::value_ptr(model));

				

				if (Grid[x][y][z] == -1) //voxel vazio
				{
					//passa para o shader a cor do voxel vazio na paleta
					shader.setVec4("inputColor", paletaCores[9].r, paletaCores[9].g, paletaCores[9].b, paletaCores[9].a);
				}
				else 
				{
					glm::vec4 corVoxel;
					int indiceCorVoxel = Grid[x][y][z];
					corVoxel.r = paletaCores[indiceCorVoxel].r;
					corVoxel.g = paletaCores[indiceCorVoxel].g;
					corVoxel.b = paletaCores[indiceCorVoxel].b;
					corVoxel.a = paletaCores[indiceCorVoxel].a;

					shader.setVec4("inputColor", corVoxel.r, corVoxel.g, corVoxel.b, corVoxel.a);
				}
				//desenhar os voxels
				glDrawArrays(GL_TRIANGLES, 0, 36);

				shader.setVec4("inputColor", 0.0f, 0.0f, 0.0f, 1.0f);
				glDrawArrays(GL_LINE_LOOP, 0, 36);
			}
		}
	}

}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS) 
	{
		//LIMITANDO A GRID
		if (Xpos < 0 || Xpos > MAX_X){ Xpos = 1;}
		if (Ypos < 0 || Ypos > MAX_Y) { Ypos = 1; }
		if (Zpos < 0 || Zpos > MAX_Z) { Zpos = 1; }


		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{

			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Ypos++;
			
		}
		else
		{
			Ypos++;
			if (taPintado[(int)Xpos][(int)Ypos-1][(int)Zpos] == 0)
			{
				Grid[(int)Xpos][(int)Ypos-1][(int)Zpos] = -1;
			}


		}
		BlocoPintado = false;
		
		
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{
			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Ypos--;
		}
		else
		{
			Ypos--;
			if (taPintado[(int)Xpos][(int)Ypos+1][(int)Zpos] == 0)
			{
				Grid[(int)Xpos][(int)Ypos+1][(int)Zpos] = -1;
			}


		}
		BlocoPintado = false;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{
			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Xpos++;
		}
		else
		{
			Xpos++;
			if (taPintado[(int)Xpos - 1][(int)Ypos][(int)Zpos] == 0)
			{
				Grid[(int)Xpos - 1][(int)Ypos][(int)Zpos] = -1;
			}
			
		
		}
		BlocoPintado = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{
			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Xpos--;
		}
		else
		{
			Xpos--;
			if (taPintado[(int)Xpos+1][(int)Ypos][(int)Zpos] == 0)
			{
				Grid[(int)Xpos+1][(int)Ypos][(int)Zpos] = -1;
			}


		}
		BlocoPintado = false;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{
			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Zpos++;
		}
		else
		{
			Zpos++;
			if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos-1] == 0)
			{
				Grid[(int)Xpos][(int)Ypos][(int)Zpos-1] = -1;
			}


		}
		BlocoPintado = false;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos] > 0)
		{
			Grid[(int)Xpos][(int)Ypos][(int)Zpos] = taPintado[(int)Xpos][(int)Ypos][(int)Zpos];
			Zpos--;
		}
		else
		{
			Zpos--;
			if (taPintado[(int)Xpos][(int)Ypos][(int)Zpos+1] == 0)
			{
				Grid[(int)Xpos][(int)Ypos][(int)Zpos+1] = -1;
			}


		}
		BlocoPintado = false;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		
		BlocoPintado = true;
		posVoxelCursor = glm::vec3(Xpos, Ypos, Zpos);
		taPintado[(int)Xpos][(int)Ypos][(int)Zpos] = indiceCor;
		addVox();
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		indiceCor = 7;//verde
	}
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		indiceCor = 8;//verde
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		indiceCor = 6;//verde
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		indiceCor = 5;//verde
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		indiceCor = 4;//verde
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		indiceCor =3;//verde
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		indiceCor = 2;//verde
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		indiceCor = 1;//verde
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
		 -0.5f, 0.5f, -1.0f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, 0.5f, 0.0f,    0.0, 1.0, 1.0,//D
		 -0.5f, 0.5f, 0.0f,   1.0, 0.0, 1.0,//B
		 //2-METADE
		 -0.5f, 0.5f, -1.0f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, 0.5f, -1.0f,   0.0, 1.0, 1.0,//4(D)
		 0.5f, 0.5f, 0.0f,    1.0, 0.0, 1.0,//D
		 /////////////////////////////////////
		 //////COSTAS ////////////////////----------COSTAS
		 //1-METADE
		 -0.5f, 0.5f, -1.0f,  1.0, 1.0, 0.0,//2(B)
		 0.5f, -0.5f, -1.0f,  1.0, 1.0, 0.0,//3(C)
		 -0.5f, -0.5f, -1.0f, 1.0, 1.0, 1.0,//1(A)
		 //2-METADE
		 -0.5f, 0.5f, -1.0f,  1.0, 1.0, 0.0,//2(B)
		  0.5f, 0.5f, -1.0f,  1.0, 1.0, 0.0,//4(D)
		  0.5f, -0.5f, -1.0f, 1.0, 1.0, 0.0,//3(C)
		  /////////////////////////////////////
		 //////ESQUERDA ////////////////////---------------ESQUERDA
		 //1-METADE
		 -0.5f, 0.5f, -1.0f,  1.0, 0.0, 1.0,//2(B)
		 -0.5f, -0.5f, -1.0f, 1.0, 0.0, 1.0,//1(A)
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 //2-METADE
		 -0.5f, 0.5f, -1.0f,  1.0, 0.0, 1.0,//2(B)
		 -0.5f, 0.5f, 0.0f,   1.0, 0.0, 1.0,//B
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 /////////////////////////////////////
		 //////PISO-BASE ////////////////////------------BASE
		 //1-METADE
		 -0.5f, -0.5f, -1.0f, 1.0, 1.0, 0.0,//1(A)
		 0.5f, -0.5f, 0.0f,   0.0, 1.0, 1.0,//C
		 -0.5f, -0.5f, 0.0f,  1.0, 0.0, 1.0,//A
		 //2-METADE TOPO
		 -0.5f, -0.5f, -1.0f, 1.0, 1.0, 0.0,//1(A)
		 0.5f, -0.5f, -1.0f,  0.0, 1.0, 1.0,//3(C)
		 0.5f, -0.5f, 0.0f,   1.0, 0.0, 1.0,//C
		 ////////////////////////////////////
		 //////DIREITA ////////////////////---------------DIREITA
		 //1-METADE
		 0.5f, 0.5f, 0.0f,   0.0, 0.0, 1.0,//D
		 0.5f, -0.5f, -1.0f, 0.0, 0.0, 1.0,//3(C)
		 0.5f, -0.5f, 0.0f,  0.0, 0.0, 1.0,//C
		 //2-METADE TOPO
		 0.5f, 0.5f, 0.0f,   0.0, 0.0, 1.0,//D
		 0.5f, 0.5f, -1.0f,  0.0, 0.0, 1.0,//4(D)
		 0.5f, -0.5f, -1.0f, 0.0, 0.0, 1.0,//3(C)

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
void updateCameraPos(GLFWwindow* window)
{
	
	float cameraSpeed = 0.05f; // adjust accordingly

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
void addVox()
{
	Grid[(int)posVoxelCursor.x][(int)posVoxelCursor.y][(int)posVoxelCursor.z] = indiceCor;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	//Precisamos também atualizar o cameraUp!! Pra isso, usamos o Up do
	//mundo (y), recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(cameraFront,
		glm::vec3(0.0, 1.0, 0.0)));
	cameraUp = glm::normalize(glm::cross(right, cameraFront));
}
void desenharCursor() 
{
	

	glm::mat4 model = glm::mat4(1);
	//transladar
	model = glm::translate(model, glm::vec3(Xpos, Ypos, Zpos));
	
	//mandar pro shader
	shader.setMat4("model", glm::value_ptr(model));

	shader.setVec4("inputColor", 1,1, 0, 1.0f);

	//desenhar os voxels
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


