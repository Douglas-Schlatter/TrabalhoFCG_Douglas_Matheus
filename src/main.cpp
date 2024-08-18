//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informсtica
//       Departamento de Informсtica Aplicada
//
//    INF01047 Fundamentos de Computaчуo Grсfica
//               Prof. Eduardo Gastal
//
//                   LABORATгRIO 4
//

// Arquivos "headers" padrѕes de C podem ser incluэdos em um
// programa C++, sendo necessсrio somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo sуo especэficos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criaчуo de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criaчуo de janelas do sistema operacional

// Headers da biblioteca GLM: criaчуo de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "collisions.h"

#define PI 3.14159
#define NUM_JOGOS 3

#define MAXTEMPODASH 1.0


//Estado da capivara no jogo DESVIECAPIVARA
typedef enum {
    ESPERA = 0,
    ATENCAO = 1,
    CORRE = 2
} ESTADO_CAPIVARA;

//Qual jogo estс ativo no momento
typedef enum {
    NONE = 0,
    CAPIVARAIMPOSTORA = 1,
    DESVIECAPIVARA = 2,
    ANGRYCAP = 3
} JOGO;

typedef enum {
    AGUARDE = 0,
    EM_JOGO = 1,
    VITORIA = 6,
    DERROTA = 7
} ESTADO_JOGO;

struct VAR_CAP_IMPOSTORA {
    glm::vec3 luz_pos;
    float countCapPos;
    float prev_time;
    float speed;
    float speedCount;
    int rng;
};

struct VAR_ANGRY_CAP {
    glm::vec4 camera_position_c;
    float prev_time;
    float speed;
    float capSpeed;
    ESTADO_CAPIVARA estado;
    float tempoEstado;
    glm::vec3 capPos;
    glm::vec3 capPrevPos;
    glm::vec3 capNextPos;
    glm::vec4 capView;
    float angulo;
    float tempoDash;
};

struct VAR_DESVIE_CAP {
    glm::vec4 camera_position_c;
    float prev_time;
    float speed;
    ESTADO_CAPIVARA estado;
    float tempoEstado;
    glm::vec2 capPos;
    glm::vec2 capPrevPos;
    glm::vec2 capNextPos;
    glm::vec4 capView;
    float angulo;
    float tempoDash;
};

// Estrutura que representa um modelo geomщtrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lъ o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, entуo setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretѓrio dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};


// Declaraчуo de funчѕes utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declaraчуo de vсrias funчѕes utilizadas em main().  Essas estуo definidas
// logo apѓs a definiчуo de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrѓi representaчуo de um ObjModel como malha de triтngulos para renderizaчуo
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso nуo existam.
void LoadShadersFromFiles(); // Carrega os shaders de vщrtice e fragmento, criando um programa de GPU
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
void LoadTextureImage(const char* filename);
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Funчуo utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Funчуo para debugging

// Declaraчуo de funчѕes auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funчѕes estуo definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

void TextRendering_ShowEstado(GLFWwindow* window, ESTADO_CAPIVARA estado, float tempo);
void TextRendering_ShowCapPos(GLFWwindow* window, glm::vec2 capPos, glm::vec2 capPrevPos, glm::vec2 capNextPos);
void TextRendering_ShowCapAngulo(GLFWwindow* window, glm::vec4 capView, float angulo);
void TextRendering_ShowCollision(GLFWwindow* window, glm::vec4 minx, glm::vec4 maxx, glm::vec4 bboxmin, glm::vec4 bboxmax);

// Funчѕes abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informaчѕes do programa. Definidas apѓs main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funчѕes callback para comunicaчуo com o sistema operacional e interaчуo do
// usuсrio. Veja mais comentсrios nas definiчѕes das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
//Individual game related
void CapivaraImpostora(GLFWwindow *window, VAR_CAP_IMPOSTORA *variaveis, ESTADO_JOGO *estado);
void DesvieCapivara(GLFWwindow *window, VAR_DESVIE_CAP *variaveis, ESTADO_JOGO *estado);
void LogicaCapivara(ESTADO_CAPIVARA *estado, float *tempo, glm::vec2 capPos, glm::vec2 *capPrevPos, glm::vec2 *capNextPos, glm::vec4 *camera_position_c, float *tempoDash);
float easing(float tempo);
void DrawHUD(float tempo);
void AngryCap(GLFWwindow *window, VAR_ANGRY_CAP *variaveis);
//Game state related
JOGO TrocaDeJogo(ESTADO_JOGO *estado, VAR_CAP_IMPOSTORA *jogoCapImpostora, VAR_DESVIE_CAP *jogoDesvieCap,VAR_ANGRY_CAP *jogoAngryCap);
void DrawResult(GLFWwindow *window, ESTADO_JOGO *result);


// Definimos uma estrutura que armazenarс dados necessсrios para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // Эndice do primeiro vщrtice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Nњmero de эndices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterizaчуo (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estуo armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

// Abaixo definimos variсveis globais utilizadas em vсrias funчѕes do cѓdigo.

// A cena virtual щ uma lista de objetos nomeados, guardados em um dicionсrio
// (map).  Veja dentro da funчуo BuildTrianglesAndAddToVirtualScene() como que sуo incluэdos
// objetos dentro da variсvel g_VirtualScene, e veja na funчуo main() como
// estes sуo acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardarс as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razуo de proporчуo da janela (largura/altura). Veja funчуo FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Тngulos de Euler que controlam a rotaчуo de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuсrio estс com o botуo esquerdo do mouse
// pressionado no momento atual. Veja funчуo MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Anсlogo para botуo direito do mouse
bool g_MiddleMouseButtonPressed = false; // Anсlogo para botуo do meio do mouse

bool keyW = false;
bool keyA = false;
bool keyS = false;
bool keyD = false;
bool keySPACE = false;


JOGO jogoAtual = NONE;
float tempoJogo = 0;

#define TEMPOCAPIMP 10
#define TEMPODESVIECAP 30

// Variсveis que definem a cтmera em coordenadas esfщricas, controladas pelo
// usuсrio atravщs do mouse (veja funчуo CursorPosCallback()). A posiчуo
// efetiva da cтmera щ calculada dentro da funчуo main(), dentro do loop de
// renderizaчуo.
float g_CameraTheta = 0.0f; // Тngulo no plano ZX em relaчуo ao eixo Z
float g_CameraPhi = 0.0f;   // Тngulo em relaчуo ao eixo Y
float g_CameraDistance = 3.5f; // Distтncia da cтmera para a origem


// Variсveis que controlam rotaчуo do antebraчo
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Variсveis que controlam translaчуo do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Variсvel que controla o tipo de projeчуo utilizada: perspectiva ou ortogrсfica.
bool g_UsePerspectiveProjection = true;

// Variсvel que controla se o texto informativo serс mostrado na tela.
bool g_ShowInfoText = true;

// Variсveis que definem um programa de GPU (shaders). Veja funчуo LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_coords;
GLint g_gameID;
GLint g_tempo;

GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

GLuint g_NumLoadedTextures = 0;

int main(int argc, char* argv[])
{
    std::srand(glfwGetTime());
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressуo de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versуo 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto щ, utilizaremos somente as
    // funчѕes modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com tэtulo "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047 - Trabalho Final - Douglas e Matheus", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a funчуo de callback que serс chamada sempre que o usuсrio
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botѕes do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverуo renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funчѕes definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a funчуo de callback que serс chamada sempre que a janela for
    // redimensionada, por consequъncia alterando o tamanho do "framebuffer"
    // (regiуo de memѓria onde sуo armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forчamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informaчѕes sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vщrtices e de fragmentos que serуo utilizados
    // para renderizaчуo. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    LoadTextureImage("../../data/capivara.jpg");      // TextureImage0
    LoadTextureImage("../../data/chao.jpg");      // TextureImage1
    LoadTextureImage("../../data/parede.jpg");      // TextureImage2
    LoadTextureImage("../../data/moon.jpg");      // TextureImage3

    // Construэmos a representaчуo de objetos geomщtricos atravщs de malhas de triтngulos
    ObjModel spheremodel("../../data/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel("../../data/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);

    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel capivaramodel("../../data/capivara.obj");
    //ComputeNormals(&capivaramodel);
    BuildTrianglesAndAddToVirtualScene(&capivaramodel);

    ObjModel hudTimermodel("../../data/timerHUD.obj");
    BuildTrianglesAndAddToVirtualScene(&hudTimermodel);

    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o cѓdigo para renderizaчуo de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    VAR_CAP_IMPOSTORA jogoCapImpostora;
    VAR_DESVIE_CAP jogoDesvieCap;
    VAR_ANGRY_CAP jogoAngryCap;

    ESTADO_JOGO estadoAtual = AGUARDE;

    // Ficamos em um loop infinito, renderizando, atщ que o usuсrio feche a janela
    while (!glfwWindowShouldClose(window))
    {

        if ((estadoAtual == VITORIA || estadoAtual == DERROTA)) {
            DrawResult(window, &estadoAtual);
        }
        if (jogoAtual == NONE) jogoAtual = TrocaDeJogo(&estadoAtual, &jogoCapImpostora, &jogoDesvieCap, &jogoAngryCap);
        if (jogoAtual == DESVIECAPIVARA) DesvieCapivara(window, &jogoDesvieCap, &estadoAtual);
        if (jogoAtual == CAPIVARAIMPOSTORA) CapivaraImpostora(window, &jogoCapImpostora, &estadoAtual);
        if (jogoAtual == ANGRYCAP) AngryCap(window, &jogoAngryCap);
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

void CapivaraImpostora(GLFWwindow *window, VAR_CAP_IMPOSTORA *variaveis, ESTADO_JOGO *estado) {
// Aqui executamos as operaчѕes de renderizaчуo

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor щ
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto щ:
        // Vermelho, Verde, Azul, Alpha (valor de transparъncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminaчуo.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e tambщm resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vщrtice e fragmentos).
        glUseProgram(g_GpuProgramID);

        glUniform1i(g_gameID, CAPIVARAIMPOSTORA);

        // Computamos a posiчуo da cтmera utilizando coordenadas esfщricas.  As
        // variсveis g_CameraDistance, g_CameraPhi, e g_CameraTheta sуo
        // controladas pelo mouse do usuсrio. Veja as funчѕes CursorPosCallback()
        // e ScrollCallback().
        //float r = g_CameraDistance;
        //float y = r*sin(g_CameraPhi);
        //float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        //float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varсveis que efetivamente definem a cтmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::vec4 camera_position_c  = glm::vec4(0.0f,3.0f,-1.0f,1.0f); // Ponto "c", centro da cтmera
        glm::vec4 camera_lookat_l    = glm::vec4(0.0f,-10.0f,1.0f,1.0f); // Ponto "l", para onde a cтmera (look-at) estarс sempre olhando
        glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a cтmera estс virada
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "cщu" (eito Y global)

        // Computamos a matriz "View" utilizando os parтmetros da cтmera para
        // definir o sistema de coordenadas da cтmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Projeчуo.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da cтmera, os planos near e far
        // estуo no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posiчуo do "near plane"
        float farplane  = -10.0f; // Posiчуo do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Projeчуo Perspectiva.
            // Para definiчуo do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Projeчуo Ortogrсfica.
            // Para definiчуo dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEЧУO ORTOGRСFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortogrсfico, computamos o valor de "t"
            // utilizando a variсvel g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glm::mat4 model = Matrix_Identity(); // Transformaчуo identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vэdeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas sуo
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        float cur_time = (float)glfwGetTime();
        float delta_t = cur_time - variaveis->prev_time;
        variaveis->prev_time = cur_time;
        variaveis->countCapPos += delta_t * variaveis->speedCount;
        //luz_x = cos(luz);
        //luz_y = sin(luz);

        tempoJogo -= delta_t;
        if (tempoJogo <= 0)
            *estado = DERROTA;

        if (keyW) variaveis->luz_pos.y += variaveis->speed * delta_t;
        if (keyA) variaveis->luz_pos.x += variaveis->speed * delta_t;
        if (keyS) variaveis->luz_pos.y -= variaveis->speed * delta_t;
        if (keyD) variaveis->luz_pos.x -= variaveis->speed * delta_t;

        glUniform2f(g_coords, variaveis->luz_pos.x, variaveis->luz_pos.y);

        #define SPHERE 0
        #define BUNNY  1
        #define PLANE  2
        #define CAPIVARA 3
        #define CAPIVARA2 4

        std::vector <glm::vec4> posCap = {glm::vec4(0,0,0,0),
                            glm::vec4(0,0,0,0),
                            glm::vec4(0,0,0,0),
                            glm::vec4(0,0,0,0)};

        for (int i = 0; i < 4; i++) {
            model = Matrix_Translate(cos(PI/2 * i+ variaveis->countCapPos),-0.6f,sin(PI/2 * i+ variaveis->countCapPos))
                  * Matrix_Rotate_X(-PI/2)
                  * Matrix_Rotate_Z(-PI/2*i + 3*PI/2 - variaveis->countCapPos);
            posCap[i] = glm::vec4(cos(PI/2 * i + variaveis->countCapPos), -0.6f, sin(PI/2 * i + variaveis->countCapPos), 0);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            if (variaveis->rng == i) {
                glUniform1i(g_object_id_uniform, CAPIVARA2);
            }
            else {
                glUniform1i(g_object_id_uniform, CAPIVARA);

            }
            DrawVirtualObject("object_0");
            DrawVirtualObject("object_1");
            DrawVirtualObject("object_2");
            DrawVirtualObject("object_3");
        }

        // Desenhamos o modelo do chуo
        model = Matrix_Translate(0.0f, -1.0f, 0.0f)
              * Matrix_Scale(2, 1, 2);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_plane");

        if (keySPACE) {
            for (int i = 0; i < 4; i++) {
                if (EsferaEsfera(glm::vec4(variaveis->luz_pos.x, -0.6f, variaveis->luz_pos.y, 0), 0.1, posCap[i], 0.5)) {
                    if (i == variaveis->rng) {
                        *estado = VITORIA;
                    } else {
                        *estado = DERROTA;
                    }
                }
            }
        }

        DrawHUD(tempoJogo/TEMPOCAPIMP);

        // Imprimimos na tela os тngulos de Euler que controlam a rotaчуo do
        // terceiro cubo.
        //TextRendering_ShowEulerAngles(window);

        // Imprimimos na informaчуo sobre a matriz de projeчуo sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informaчуo sobre o nњmero de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operaчѕes de renderizaчуo nуo
        // щ o mesmo que estс sendo mostrado para o usuсrio, caso contrсrio
        // seria possэvel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuсrio
        // tudo que foi renderizado pelas funчѕes acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interaчуo do
        // usuсrio (teclado, mouse, ...). Caso positivo, as funчѕes de callback
        // definidas anteriormente usando glfwSet*Callback() serуo chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
}

void DesvieCapivara(GLFWwindow *window, VAR_DESVIE_CAP *variaveis, ESTADO_JOGO *estado) {
// Aqui executamos as operaчѕes de renderizaчуo

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor щ
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto щ:
        // Vermelho, Verde, Azul, Alpha (valor de transparъncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminaчуo.
        //
        //           R     G     B     A
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e tambщm resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vщrtice e fragmentos).
        glUseProgram(g_GpuProgramID);

        glUniform1i(g_gameID, DESVIECAPIVARA);

        // Computamos a posiчуo da cтmera utilizando coordenadas esfщricas.  As
        // variсveis g_CameraDistance, g_CameraPhi, e g_CameraTheta sуo
        // controladas pelo mouse do usuсrio. Veja as funчѕes CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varсveis que efetivamente definem a cтmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //camera_position_c  = glm::vec4(0.0f,3.0f,-1.0f,1.0f); // Ponto "c", centro da cтmera
        //glm::vec4 camera_lookat_l    = glm::vec4(0.0f,-10.0f,1.0f,1.0f); // Ponto "l", para onde a cтmera (look-at) estarс sempre olhando
        glm::vec4 camera_view = glm::vec4(x,y,z,0.0f); // Vetor "view", sentido para onde a cтmera estс virada
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "cщu" (eito Y global)

        // Computamos a matriz "View" utilizando os parтmetros da cтmera para
        // definir o sistema de coordenadas da cтmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(variaveis->camera_position_c, camera_view, camera_up_vector);

        // Agora computamos a matriz de Projeчуo.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da cтmera, os planos near e far
        // estуo no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posiчуo do "near plane"
        float farplane  = -15.0f; // Posiчуo do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Projeчуo Perspectiva.
            // Para definiчуo do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Projeчуo Ortogrсfica.
            // Para definiчуo dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEЧУO ORTOGRСFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortogrсfico, computamos o valor de "t"
            // utilizando a variсvel g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glm::mat4 model = Matrix_Identity(); // Transformaчуo identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vэdeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas sуo
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        //glm::vec4 w = -glm::vec4(camera_view.x, camera_view.y, camera_view.z, 0);
        glm::vec4 w = -glm::vec4(camera_view.x, 0, camera_view.z, 0);
        w = w/norm(w);
        glm::vec4 u = crossproduct(camera_up_vector, w);
        u = u/norm(u);


        float cur_time = (float)glfwGetTime();
        float delta_t = cur_time - variaveis->prev_time;
        variaveis->prev_time = cur_time;
        variaveis->tempoEstado -= 1 * delta_t;

        tempoJogo -= delta_t;
        if (tempoJogo <= 0)
            *estado = VITORIA;

        if (keyW) variaveis->camera_position_c += -w * variaveis->speed * delta_t;
        if (keyA) variaveis->camera_position_c += -u * variaveis->speed * delta_t;
        if (keyS) variaveis->camera_position_c += +w * variaveis->speed * delta_t;
        if (keyD) variaveis->camera_position_c += +u * variaveis->speed * delta_t;

        if (variaveis->tempoEstado < 0.0)
            LogicaCapivara(&variaveis->estado, &variaveis->tempoEstado, variaveis->capPos, &variaveis->capPrevPos, &variaveis->capNextPos, &variaveis->camera_position_c, &variaveis->tempoDash);

        if (variaveis->estado == CORRE) {
            //Capivara vai da posiчуo Prev atщ a posiчуo Next
            variaveis->tempoDash = std::min(variaveis->tempoDash + delta_t, (float)MAXTEMPODASH);
            variaveis->capPos.x = variaveis->capPrevPos.x + (variaveis->capNextPos.x - variaveis->capPrevPos.x) * easing(variaveis->tempoDash/MAXTEMPODASH);
            variaveis->capPos.y = variaveis->capPrevPos.y + (variaveis->capNextPos.y - variaveis->capPrevPos.y) * easing(variaveis->tempoDash/MAXTEMPODASH);
        }

        #define SPHERE 0
        #define WALL  1
        #define PLANE  2
        #define CAPIVARA 3
        #define CAPIVARA2 4
        #define HUD 5

        if (variaveis->estado == ATENCAO) {
            //Capivara olha para a cтmera
            glm::vec4 capViewNew = variaveis->camera_position_c - glm::vec4(variaveis->capPos.x, variaveis->camera_position_c.y, variaveis->capPos.y, 1.0f);
            capViewNew = capViewNew/norm(capViewNew);

            //https://stackoverflow.com/questions/5188561/signed-angle-between-two-3d-vectors-with-same-origin-within-the-same-plane
            variaveis->angulo += std::atan2(dotproduct(crossproduct(variaveis->capView, capViewNew), glm::vec4(0,1,0,0)), dotproduct(variaveis->capView, capViewNew));

            variaveis->capView = capViewNew;

            model = Matrix_Translate(variaveis->capPos.x,-0.6f,variaveis->capPos.y)
                  * Matrix_Rotate_X(-PI/2)
                  * Matrix_Rotate_Z(variaveis->angulo);
            }
        else {
            model = Matrix_Translate(variaveis->capPos.x,-0.6f,variaveis->capPos.y)
                * Matrix_Rotate_X(-PI/2)
                * Matrix_Rotate_Z(variaveis->angulo);
        }
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, CAPIVARA);
        DrawVirtualObject("object_0");
        DrawVirtualObject("object_1");
        DrawVirtualObject("object_2");
        DrawVirtualObject("object_3");

        // Guardar model da capivara
        PushMatrix(model);

        #define TAMANHO_SALA_X 3
        #define TAMANHO_SALA_Y 3

        // Desenhamos o modelo do chуo
        model = Matrix_Translate(0.0f, -1.0f, 0.0f)
              * Matrix_Scale(TAMANHO_SALA_X, 1, TAMANHO_SALA_Y);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_plane");

        // Lua
        model = Matrix_Translate(2,10,-2)
              * Matrix_Scale(0.5,0.5,0.5);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, SPHERE);
        DrawVirtualObject("the_sphere");

        // Parede
        model = Matrix_Translate(TAMANHO_SALA_X,0,0)
              * Matrix_Scale(1,1,TAMANHO_SALA_Y)
              * Matrix_Rotate_Z(PI/2)
              * Matrix_Rotate_Y(PI/2);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        //capivara -> parede +X
        PushMatrix(model);

        model = Matrix_Translate(-TAMANHO_SALA_X,0,0)
              * Matrix_Scale(1,1,TAMANHO_SALA_Y)
              * Matrix_Rotate_Z(-PI/2)
              * Matrix_Rotate_Y(-PI/2);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        //capivara -> parede +X -> parede -X
        PushMatrix(model);

        model = Matrix_Translate(0,0,TAMANHO_SALA_Y)
              * Matrix_Scale(TAMANHO_SALA_X,1,1)
              * Matrix_Rotate_X(-PI/2);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        //capivara -> parede +X -> parede -X -> parede +Z
        PushMatrix(model);

        model = Matrix_Translate(0,0,-TAMANHO_SALA_Y)
              * Matrix_Scale(TAMANHO_SALA_X,1,1)
              * Matrix_Rotate_X(PI/2)
              * Matrix_Rotate_Y(PI);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        //capivara -> parede +X -> parede -X -> parede +Z -> parede -Z
        PushMatrix(model);

        DrawHUD(tempoJogo/TEMPODESVIECAP);


        //capivara -> parede +X -> parede -X -> parede +Z -> { parede -Z }
        PopMatrix(model);

        glm::vec4 normPlano = glm::vec4(0,1,0,0);

        glm::vec4 normParedeMZ = model * normPlano;
        normParedeMZ = normParedeMZ/norm(normParedeMZ);

        //capivara -> parede +X -> parede -X -> { parede +Z }
        PopMatrix(model);
        glm::vec4 normParedeZ = model * normPlano;
        normParedeZ = normParedeZ/norm(normParedeZ);

        //capivara -> parede +X -> { parede -X }
        PopMatrix(model);
        glm::vec4 normParedeMX = model * normPlano;
        normParedeMX = normParedeMX/norm(normParedeMX);

        //capivara -> { parede +X }
        PopMatrix(model);
        glm::vec4 normParedeX = model * normPlano;
        normParedeX = normParedeX/norm(normParedeX);

        //{ capivara }
        PopMatrix(model);

        // Da pra pegar a bounding box da capivara com g_VirtualScene["object_0"].bbox_max
        // Mas a capivara sуo 4 objetos dferentes, entуo aqui tem os nњmeros do modelo total

        //          0.18          -0.18
        // max =    0.48  min =   -0.67
        //          0.31          -0.41

        glm::vec4 bbox_cap_min = model * glm::vec4(-0.18, -0.67, -0.41, 1);
        glm::vec4 bbox_cap_max = model * glm::vec4(0.18, 0.48, 0.31, 1);

        // Hitbox do jogador
        glm::vec4 maxHitbox = variaveis->camera_position_c + glm::vec4(0.2,10,0.2,0);
        glm::vec4 minHitbox = variaveis->camera_position_c + glm::vec4(-0.2,-10,-0.2,0);


        // Se a capivara encostar no jogador, ele perde
        if (CuboCubo(minHitbox, maxHitbox, bbox_cap_min, bbox_cap_max)) {
            *estado = DERROTA;
        }

        // Se a capivara estiver fora das paredes, ela para
        if (CuboPlano(bbox_cap_min, bbox_cap_max, glm::vec4(TAMANHO_SALA_X,0,0,1), normParedeX)) {
            variaveis->estado = ESPERA;
            variaveis->tempoDash = 0;
            variaveis->tempoEstado = 3.0;
            variaveis->capPos.x += normParedeX.x * 0.3;
            variaveis->capPos.y += normParedeX.z * 0.3;
        }
        if (CuboPlano(bbox_cap_min, bbox_cap_max, glm::vec4(-TAMANHO_SALA_X,0,0,1), normParedeMX)) {
            variaveis->estado = ESPERA;
            variaveis->tempoDash = 0;
            variaveis->tempoEstado = 3.0;
            variaveis->capPos.x += normParedeMX.x * 0.3;
            variaveis->capPos.y += normParedeMX.z * 0.3;
        }
        if (CuboPlano(bbox_cap_min, bbox_cap_max, glm::vec4(0,0,TAMANHO_SALA_Y,1), normParedeZ)) {
            variaveis->estado = ESPERA;
            variaveis->tempoDash = 0;
            variaveis->tempoEstado = 3.0;
            variaveis->capPos.x += normParedeZ.x * 0.3;
            variaveis->capPos.y += normParedeZ.z * 0.3;
        }
        if (CuboPlano(bbox_cap_min, bbox_cap_max, glm::vec4(0,0,-TAMANHO_SALA_Y,1), normParedeMZ)) {
            variaveis->estado = ESPERA;
            variaveis->tempoDash = 0;
            variaveis->tempoEstado = 3.0;
            variaveis->capPos.x += normParedeMZ.x * 0.3;
            variaveis->capPos.y += normParedeMZ.z * 0.3;
        }

        // Se jogador encostar na parede, ele para
        if (CuboPlano(minHitbox, maxHitbox, glm::vec4(TAMANHO_SALA_X,0,0,1), normParedeX)) {
            variaveis->camera_position_c += normParedeX * glm::vec4(0.1, 0.1, 0.1, 0);
        }
        if (CuboPlano(minHitbox, maxHitbox, glm::vec4(-TAMANHO_SALA_X,0,0,1), normParedeMX)) {
            variaveis->camera_position_c += normParedeMX * glm::vec4(0.1, 0.1, 0.1, 0);
        }
        if (CuboPlano(minHitbox, maxHitbox, glm::vec4(0,0,TAMANHO_SALA_Y,1), normParedeZ)) {
            variaveis->camera_position_c += normParedeZ * glm::vec4(0.1, 0.1, 0.1, 0);
        }
        if (CuboPlano(minHitbox, maxHitbox, glm::vec4(0,0,-TAMANHO_SALA_Y,1), normParedeMZ)) {
            variaveis->camera_position_c += normParedeMZ * glm::vec4(0.1, 0.1, 0.1, 0);
        }

        // Imprimimos na tela os тngulos de Euler que controlam a rotaчуo do
        // terceiro cubo.
        TextRendering_ShowEstado(window, variaveis->estado, variaveis->tempoEstado);
        TextRendering_ShowCapPos(window, variaveis->capPos, variaveis->capPrevPos, variaveis->capNextPos);
        //TextRendering_ShowCapAngulo(window, variaveis->capView, variaveis->angulo);
        TextRendering_ShowCollision(window, bbox_cap_min, bbox_cap_max, minHitbox, maxHitbox);

        // Imprimimos na informaчуo sobre a matriz de projeчуo sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informaчуo sobre o nњmero de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operaчѕes de renderizaчуo nуo
        // щ o mesmo que estс sendo mostrado para o usuсrio, caso contrсrio
        // seria possэvel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuсrio
        // tudo que foi renderizado pelas funчѕes acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interaчуo do
        // usuсrio (teclado, mouse, ...). Caso positivo, as funчѕes de callback
        // definidas anteriormente usando glfwSet*Callback() serуo chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
}

void AngryCap(GLFWwindow *window, VAR_ANGRY_CAP *variaveis) {
// Aqui executamos as operaУЇУЕes de renderizaУЇУЃo

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor УЉ
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto УЉ:
        // Vermelho, Verde, Azul, Alpha (valor de transparУЊncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de IluminaУЇУЃo.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e tambУЉm resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vУЉrtice e fragmentos).
        glUseProgram(g_GpuProgramID);

        glUniform1i(g_gameID,ANGRYCAP);

        // Computamos a posiУЇУЃo da cУЂmera utilizando coordenadas esfУЉricas.  As
        // variУЁveis g_CameraDistance, g_CameraPhi, e g_CameraTheta sУЃo
        // controladas pelo mouse do usuУЁrio. Veja as funУЇУЕes CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varУЁveis que efetivamente definem a cУЂmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //camera_position_c  = glm::vec4(0.0f,3.0f,-1.0f,1.0f); // Ponto "c", centro da cУЂmera
        //glm::vec4 camera_lookat_l    = glm::vec4(0.0f,-10.0f,1.0f,1.0f); // Ponto "l", para onde a cУЂmera (look-at) estarУЁ sempre olhando
        glm::vec4 camera_view = glm::vec4(x,y,z,0.0f); // Vetor "view", sentido para onde a cУЂmera estУЁ virada
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "cУЉu" (eito Y global)

        // Computamos a matriz "View" utilizando os parУЂmetros da cУЂmera para
        // definir o sistema de coordenadas da cУЂmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(variaveis->camera_position_c, camera_view, camera_up_vector);

        // Agora computamos a matriz de ProjeУЇУЃo.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da cУЂmera, os planos near e far
        // estУЃo no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // PosiУЇУЃo do "near plane"
        float farplane  = -10.0f; // PosiУЇУЃo do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // ProjeУЇУЃo Perspectiva.
            // Para definiУЇУЃo do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // ProjeУЇУЃo OrtogrУЁfica.
            // Para definiУЇУЃo dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJEУУO ORTOGRУFICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortogrУЁfico, computamos o valor de "t"
            // utilizando a variУЁvel g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glm::mat4 model = Matrix_Identity(); // TransformaУЇУЃo identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vУ­deo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas sУЃo
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        //glm::vec4 w = -glm::vec4(camera_view.x, camera_view.y, camera_view.z, 0);
        glm::vec4 w = -glm::vec4(camera_view.x, 0, camera_view.z, 0);
        w = w/norm(w);
        glm::vec4 u = crossproduct(camera_up_vector, w);
        u = u/norm(u);


        float cur_time = (float)glfwGetTime();
        float delta_t = cur_time - variaveis->prev_time;
        variaveis->prev_time = cur_time;
        variaveis->tempoEstado -= 1 * delta_t;


        if (keyW) variaveis->camera_position_c += -w * variaveis->speed * delta_t;
        if (keyA) variaveis->camera_position_c += -u * variaveis->speed * delta_t;
        if (keyS) variaveis->camera_position_c += +w * variaveis->speed * delta_t;
        if (keyD) variaveis->camera_position_c += +u * variaveis->speed * delta_t;

        //if (variaveis->tempoEstado < 0.0)
        //    LogicaCapivara(&variaveis->estado, &variaveis->tempoEstado, variaveis->capPos, &variaveis->capPrevPos, &variaveis->capNextPos, &variaveis->camera_position_c, &variaveis->tempoDash);

        //AQUI ESTA MOVENDO A CAP
        /*
        if (variaveis->estado == CORRE) {
            //Capivara vai da posiУЇУЃo Prev atУЉ a posiУЇУЃo Next
            variaveis->tempoDash = std::min(variaveis->tempoDash + delta_t, (float)MAXTEMPODASH);
            variaveis->capPos.x = variaveis->capPrevPos.x + (variaveis->capNextPos.x - variaveis->capPrevPos.x) * easing(variaveis->tempoDash/MAXTEMPODASH);
            variaveis->capPos.y = variaveis->capPrevPos.y + (variaveis->capNextPos.y - variaveis->capPrevPos.y) * easing(variaveis->tempoDash/MAXTEMPODASH);
        }
        */
        variaveis->capPrevPos = variaveis->capPos;
        if (keySPACE) {
            variaveis->capNextPos.x  = variaveis->capNextPos.x + (camera_view.x);
            variaveis->capNextPos.y  = variaveis->capNextPos.y + (camera_view.y);
            variaveis->capNextPos.z  = variaveis->capNextPos.z + (camera_view.z);
            //variaveis->capNextPos.z = variaveis->capNextPos.z + (variaveis->camera_position_c.z+4);
            //variaveis->tempoDash = 0.1;
            //Capivara vai da posiУЇУЃo Prev atУЉ a posiУЇУЃo Next
            //variaveis->tempoDash = std::min(variaveis->tempoDash + delta_t, (float)MAXTEMPODASH);
            //variaveis->capPos.x = variaveis->capPrevPos.x + (variaveis->capNextPos.x - variaveis->capPrevPos.x) * easing(variaveis->tempoDash/MAXTEMPODASH);
            variaveis->capPos = variaveis->capPrevPos + (variaveis->capNextPos - variaveis->capPrevPos)*variaveis->capSpeed*delta_t;

            //variaveis->capPos.z = variaveis->capPrevPos.z + (variaveis->capNextPos.z - variaveis->capPrevPos.z)*1.0*delta_t;
            //variaveis->capPos.z = variaveis->capPrevPos.z + (variaveis->capNextPos.z - variaveis->capPrevPos.z) * easing(variaveis->tempoDash/MAXTEMPODASH);
        }
        else
        {
           variaveis->capPos =   variaveis->camera_position_c;
           //variaveis->capPrevPos = variaveis->capPos;
           variaveis->capNextPos = glm::vec3(variaveis->camera_position_c.x, variaveis->camera_position_c.y,variaveis->camera_position_c.z);
        }

        #define SPHERE 0
        #define WALL  1
        #define PLANE  2
        #define CAPIVARA 3
        #define CAPIVARA2 4
        #define HUD 5
        /* OLD ANGLE CODE
        if (variaveis->estado == ATENCAO) {
            //Capivara olha para a cУЂmera
            glm::vec4 capViewNew = variaveis->camera_position_c - glm::vec4(variaveis->capPos.x, variaveis->camera_position_c.y, variaveis->capPos.y, 1.0f);
            capViewNew = capViewNew/norm(capViewNew);

            //https://stackoverflow.com/questions/5188561/signed-angle-between-two-3d-vectors-with-same-origin-within-the-same-plane
            variaveis->angulo += std::atan2(dotproduct(crossproduct(variaveis->capView, capViewNew), glm::vec4(0,1,0,0)), dotproduct(variaveis->capView, capViewNew));

            variaveis->capView = capViewNew;

            model = Matrix_Translate(variaveis->capPos.x,-0.6f,variaveis->capPos.y)
                  * Matrix_Rotate_X(-PI/2)
                  * Matrix_Rotate_Z(variaveis->angulo);
            }
        else {
            model = Matrix_Translate(variaveis->capPos.x,-0.6f,variaveis->capPos.y)
                * Matrix_Rotate_X(-PI/2)
                * Matrix_Rotate_Z(variaveis->angulo);
        }
        */

            //Capivara olhando para a mesma direção que a camera FALAR COM O MATHEUS SOBRE
            glm::vec4 capViewNew = camera_view;//variaveis->camera_position_c - glm::vec4(variaveis->capPos.x, variaveis->camera_position_c.y, variaveis->capPos.y, 1.0f);
            capViewNew = capViewNew/norm(capViewNew);

            //https://stackoverflow.com/questions/5188561/signed-angle-between-two-3d-vectors-with-same-origin-within-the-same-plane
            variaveis->angulo += std::atan2(dotproduct(crossproduct(variaveis->capView, capViewNew), glm::vec4(0,1,0,0)), dotproduct(variaveis->capView, capViewNew));

            variaveis->capView = capViewNew;





        model = Matrix_Translate(variaveis->capPos.x,variaveis->capPos.y,variaveis->capPos.z)
        * Matrix_Rotate_X(-PI/2)
        * Matrix_Rotate_Z(variaveis->angulo);

        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, CAPIVARA);
        DrawVirtualObject("object_0");
        DrawVirtualObject("object_1");
        //glUniform1i(g_object_id_uniform, 999);
        DrawVirtualObject("object_2");
        //glUniform1i(g_object_id_uniform, CAPIVARA);
        DrawVirtualObject("object_3");

        #define TAMANHO_SALA_X 3
        #define TAMANHO_SALA_Y 3

        // Desenhamos o modelo do chУЃo
        model = Matrix_Translate(0.0f, -1.0f, 0.0f)
              * Matrix_Scale(TAMANHO_SALA_X, 1, TAMANHO_SALA_Y);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_plane");

        DrawHUD(variaveis->tempoEstado/3);

        // Imprimimos na tela os УЂngulos de Euler que controlam a rotaУЇУЃo do
        // terceiro cubo.
        TextRendering_ShowEstado(window, variaveis->estado, variaveis->tempoEstado);
        TextRendering_ShowCapPos(window, variaveis->capPos, variaveis->capPrevPos, variaveis->capNextPos);
        //TextRendering_ShowCapPos(window, variaveis->capPos, variaveis->capPrevPos, variaveis->capNextPos);
        TextRendering_ShowCapAngulo(window, variaveis->capView, variaveis->angulo);

        // Imprimimos na informaУЇУЃo sobre a matriz de projeУЇУЃo sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informaУЇУЃo sobre o nУКmero de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operaУЇУЕes de renderizaУЇУЃo nУЃo
        // УЉ o mesmo que estУЁ sendo mostrado para o usuУЁrio, caso contrУЁrio
        // seria possУ­vel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuУЁrio
        // tudo que foi renderizado pelas funУЇУЕes acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interaУЇУЃo do
        // usuУЁrio (teclado, mouse, ...). Caso positivo, as funУЇУЕes de callback
        // definidas anteriormente usando glfwSet*Callback() serУЃo chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
}

void LogicaCapivara(ESTADO_CAPIVARA *estado, float *tempo, glm::vec2 capPos, glm::vec2 *capPrevPos, glm::vec2 *capNextPos, glm::vec4 *camera_position_c, float *tempoDash) {
    if (*estado == ESPERA) {
        *estado = ATENCAO;
        *tempo = 3.0;
    } else if (*estado == ATENCAO) {
        *estado = CORRE;
        *tempo = 3.0;
        *capPrevPos = capPos;
        *capNextPos = glm::vec2(camera_position_c->x, camera_position_c->z);
    } else if (*estado == CORRE) {
        *estado = ATENCAO;
        *tempo = 3.0;
        *tempoDash = 0.0;
    }
}

float easing(float tempo) {
    //easeOutExpo
    if (tempo == 1) {
        return 1;
    } else {
        return 1 - std::pow(2,-10*tempo);
    }
}

JOGO TrocaDeJogo(ESTADO_JOGO *estado, VAR_CAP_IMPOSTORA *jogoCapImpostora, VAR_DESVIE_CAP *jogoDesvieCap, VAR_ANGRY_CAP  *jogoAngryCap) {
    *estado = EM_JOGO;
    int rng = rand() % NUM_JOGOS + 1; // para aleatorio deixe essa linha
    //int rng = ANGRYCAP; // para deixar o jogo sempre o mesmo
    float prev_time = (float) glfwGetTime();

    switch (rng) {
    case CAPIVARAIMPOSTORA:
        jogoCapImpostora->luz_pos = glm::vec3(0.0f, 0.0f, 0.0f);
        jogoCapImpostora->countCapPos = 0;
        jogoCapImpostora->prev_time = prev_time;
        jogoCapImpostora->speed = 2.0f;
        jogoCapImpostora->speedCount = 1.0f;
        jogoCapImpostora->rng = rand() % 4;
        tempoJogo = TEMPOCAPIMP;
        return CAPIVARAIMPOSTORA;
        break;

    case DESVIECAPIVARA:
        jogoDesvieCap->camera_position_c = glm::vec4(0.0f, -0.45f, 1.0f, 1.0f);
        jogoDesvieCap->estado = ESPERA;
        jogoDesvieCap->tempoEstado = 3.0;
        jogoDesvieCap->capPos = glm::vec2(0, -1);
        jogoDesvieCap->capNextPos = glm::vec2(0, 0);
        jogoDesvieCap->capPrevPos = glm::vec2(0, 0);
        jogoDesvieCap->capView = glm::vec4(0.0, 0.0, 1.0, 0.0);
        jogoDesvieCap->capView = jogoDesvieCap->capView/norm(jogoDesvieCap->capView);
        jogoDesvieCap->angulo = 0;
        jogoDesvieCap->tempoDash = 0.0f;
        jogoDesvieCap->speed = 2.0f;
        jogoDesvieCap->prev_time = prev_time;
        g_CameraPhi = 0;
        g_CameraTheta = PI;
        tempoJogo = TEMPODESVIECAP;
        return DESVIECAPIVARA;
        break;
    case ANGRYCAP:
        jogoAngryCap->camera_position_c = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        jogoAngryCap->estado = ESPERA;
        jogoAngryCap->tempoEstado = 3.0;
        jogoAngryCap->capPos = glm::vec3(0, -0.3,0);
        jogoAngryCap->capNextPos = glm::vec3(0, 0,0);
        jogoAngryCap->capPrevPos = glm::vec3(0, 0,0);
        jogoAngryCap->capView = glm::vec4(0.0, 0.0, 1.0, 0.0);
        jogoAngryCap->capView = jogoAngryCap->capView/norm(jogoAngryCap->capView);
        jogoAngryCap->angulo = 0;
        jogoAngryCap->tempoDash = 0.0f;
        jogoAngryCap->speed = 2.0f;
        jogoAngryCap->capSpeed = 0.01f;
        jogoAngryCap->prev_time = prev_time;
        g_CameraPhi = 0;
        g_CameraTheta = PI;
        return ANGRYCAP;
        break;
    default:
        return NONE;
    }

}

void DrawResult(GLFWwindow *window, ESTADO_JOGO *result) {
    if (*result == VITORIA) {
        printf("Ganhou!\n\n");
    }
    if (*result == DERROTA) {
        printf("Perdeu :(\n\n");
    }
    *result = AGUARDE;
    jogoAtual = NONE;
}

void DrawHUD(float tempo) {
    glm::mat4 model;

    model = Matrix_Translate(0,0.9,0)
          * Matrix_Scale(tempo,0.05,1);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glUniform1f(g_tempo, tempo);
    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(g_object_id_uniform, HUD);
    DrawVirtualObject("the_hud");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

// Funчуo que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
   stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parтmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Funчуo que desenha um objeto armazenado em g_VirtualScene. Veja definiчуo
// dos objetos na funчуo BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vщrtices apontados pelo VAO criado pela funчуo BuildTrianglesAndAddToVirtualScene(). Veja
    // comentсrios detalhados dentro da definiчуo de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Setamos as variсveis "bbox_min" e "bbox_max" do fragment shader
    // com os parтmetros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os vщrtices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definiчуo de
    // g_VirtualScene[""] dentro da funчуo BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentaчуo da funчуo glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operaчѕes posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Funчуo que carrega os shaders de vщrtices e de fragmentos que serуo
// utilizados para renderizaчуo. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estуo fixados, sendo que assumimos a existъncia
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereчo das variсveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variсveis para enviar dados para a placa de vэdeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variсvel da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variсvel da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variсvel da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variсvel "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");
    g_coords = glGetUniformLocation(g_GpuProgramID, "pos_luz");
    g_gameID = glGetUniformLocation(g_GpuProgramID, "game_ID");
    g_tempo = glGetUniformLocation(g_GpuProgramID, "tempo");

        // Variсveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage3"), 3);
    glUseProgram(0);
}

// Funчуo que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Funчуo que remove a matriz atualmente no topo da pilha e armazena a mesma na variсvel M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

// Funчуo que computa as normais de um ObjModel, caso elas nуo tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIТNGULOS.
    // Segundo, computamos as normais dos VЩRTICES atravщs do mщtodo proposto
    // por Gouraud, onde a normal de cada vщrtice vai ser a mщdia das normais de
    // todas as faces que compartilham este vщrtice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            const glm::vec4  n = crossproduct(b-a, c-a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

// Constrѓi triтngulos para futura renderizaчуo a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o cѓdigo da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel щ
                // comparando se o эndice retornado щ -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    //const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    //const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    //texture_coefficients.push_back( u );
                    //texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name           = model->shapes[shape].name;
        theobject.first_index    = first_index; // Primeiro эndice
        theobject.num_indices    = last_index - first_index + 1; // Nњmero de indices
        theobject.rendering_mode = GL_TRIANGLES;       // Эndices correspondem ao tipo de rasterizaчуo GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora щ GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operaчѕes posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definiчуo de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // serс aplicado nos vщrtices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definiчуo de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // serс aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Funчуo auxilar, utilizada pelas duas funчѕes acima. Carrega cѓdigo de GPU de
// um arquivo GLSL e faz sua compilaчуo.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variсvel "filename"
    // e colocamos seu conteњdo em memѓria, apontado pela variсvel
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o cѓdigo do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o cѓdigo do shader GLSL (em tempo de execuчуo)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilaчуo
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memѓria para guardar o log de compilaчуo.
    // A chamada "new" em C++ щ equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilaчуo
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ щ equivalente ao "free()" do C
    delete [] log;
}

// Esta funчуo cria um programa de GPU, o qual contщm obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definiчуo dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memѓria para guardar o log de compilaчуo.
        // A chamada "new" em C++ щ equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ щ equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleчуo apѓs serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Definiчуo da funчуo que serс chamada sempre que a janela do sistema
// operacional for redimensionada, por consequъncia alterando o tamanho do
// "framebuffer" (regiуo de memѓria onde sуo armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda regiуo do framebuffer. A
    // funчуo "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa щ a operaчуo de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos tambщm a razуo que define a proporчуo da janela (largura /
    // altura), a qual serс utilizada na definiчуo das matrizes de projeчуo,
    // tal que nуo ocorra distorчѕes durante o processo de "Screen Mapping"
    // acima, quando NDC щ mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float щ necessсrio pois nњmeros inteiros sуo arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variсveis globais que armazenam a њltima posiчуo do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Funчуo callback chamada sempre que o usuсrio aperta algum dos botѕes do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuсrio pressionou o botуo esquerdo do mouse, guardamos a
        // posiчуo atual do cursor nas variсveis g_LastCursorPosX e
        // g_LastCursorPosY.  Tambщm, setamos a variсvel
        // g_LeftMouseButtonPressed como true, para saber que o usuсrio estс
        // com o botуo esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuсrio soltar o botуo esquerdo do mouse, atualizamos a
        // variсvel abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuсrio pressionou o botуo esquerdo do mouse, guardamos a
        // posiчуo atual do cursor nas variсveis g_LastCursorPosX e
        // g_LastCursorPosY.  Tambщm, setamos a variсvel
        // g_RightMouseButtonPressed como true, para saber que o usuсrio estс
        // com o botуo esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuсrio soltar o botуo esquerdo do mouse, atualizamos a
        // variсvel abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuсrio pressionou o botуo esquerdo do mouse, guardamos a
        // posiчуo atual do cursor nas variсveis g_LastCursorPosX e
        // g_LastCursorPosY.  Tambщm, setamos a variсvel
        // g_MiddleMouseButtonPressed como true, para saber que o usuсrio estс
        // com o botуo esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuсrio soltar o botуo esquerdo do mouse, atualizamos a
        // variсvel abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Funчуo callback chamada sempre que o usuсrio movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botуo esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o њltimo
    // instante de tempo, e usamos esta movimentaчуo para atualizar os
    // parтmetros que definem a posiчуo da cтmera dentro da cena virtual.
    // Assim, temos que o usuсrio consegue controlar a cтmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parтmetros da cтmera com os deslocamentos
        g_CameraTheta -= 0.01f*dx;
        g_CameraPhi   -= 0.01f*dy;

        // Em coordenadas esfщricas, o тngulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as variсveis globais para armazenar a posiчуo atual do
        // cursor como sendo a њltima posiчуo conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parтmetros da antebraчo com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;

        // Atualizamos as variсveis globais para armazenar a posiчуo atual do
        // cursor como sendo a њltima posiчуo conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parтmetros da antebraчo com os deslocamentos
        g_TorsoPositionX += 0.01f*dx;
        g_TorsoPositionY -= 0.01f*dy;

        // Atualizamos as variсveis globais para armazenar a posiчуo atual do
        // cursor como sendo a њltima posiчуo conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Funчуo callback chamada sempre que o usuсrio movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distтncia da cтmera para a origem utilizando a
    // movimentaчуo da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma cтmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela estс olhando, pois isto gera problemas de divisуo por zero na
    // definiчуo do sistema de coordenadas da cтmera. Isto щ, a variсvel abaixo
    // nunca pode ser zero. Versѕes anteriores deste cѓdigo possuэam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definiчуo da funчуo que serс chamada sempre que o usuсrio pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ====================
    // Nуo modifique este loop! Ele щ utilizando para correчуo automatizada dos
    // laboratѓrios. Deve ser sempre o primeiro comando desta funчуo KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ====================

    // Se o usuсrio pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O cѓdigo abaixo implementa a seguinte lѓgica:
    //   Se apertar tecla X       entуo g_AngleX += delta;
    //   Se apertar tecla shift+X entуo g_AngleX -= delta;
    //   Se apertar tecla Y       entуo g_AngleY += delta;
    //   Se apertar tecla shift+Y entуo g_AngleY -= delta;
    //   Se apertar tecla Z       entуo g_AngleZ += delta;
    //   Se apertar tecla shift+Z entуo g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }


    // Se o usuУЁrio apertar a tecla espaУЇo, resetamos os УЂngulos de Euler para zero.
    if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS) {
            keySPACE = true;
        } else if (action == GLFW_RELEASE) {
            keySPACE  = false;
        }
    }
    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS) {
            keyW = true;
        } else if (action == GLFW_RELEASE) {
            keyW  = false;
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            keyA = true;
        } else if (action == GLFW_RELEASE) {
            keyA = false;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            keyS = true;
        } else if (action == GLFW_RELEASE) {
            keyS = false;
        }
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            keyD = true;
        } else if (action == GLFW_RELEASE) {
            keyD = false;
        }

    }

    // Se o usuсrio apertar a tecla P, utilizamos projeчуo perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        //g_UsePerspectiveProjection = true;
        printf("max\n");
        PrintVector(glm::vec4(g_VirtualScene["object_0"].bbox_max,0));
        PrintVector(glm::vec4(g_VirtualScene["object_1"].bbox_max,0));
        PrintVector(glm::vec4(g_VirtualScene["object_2"].bbox_max,0));
        PrintVector(glm::vec4(g_VirtualScene["object_3"].bbox_max,0));

        printf("min\n");
        PrintVector(glm::vec4(g_VirtualScene["object_0"].bbox_min,0));
        PrintVector(glm::vec4(g_VirtualScene["object_1"].bbox_min,0));
        PrintVector(glm::vec4(g_VirtualScene["object_2"].bbox_min,0));
        PrintVector(glm::vec4(g_VirtualScene["object_3"].bbox_min,0));
        //          18          -18
        // max =    48  min =   -67
        //          31          -41
    }

    // Se o usuсrio apertar a tecla O, utilizamos projeчуo ortogrсfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        //g_UsePerspectiveProjection = false;
        jogoAtual = NONE;
    }

    // Se o usuсrio apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Se o usuсrio apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }
}

// Definimos o callback para impressуo de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta funчуo recebe um vщrtice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformaчѕes.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os тngulos de Euler definidos nas variсveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
/*
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}*/

void TextRendering_ShowEstado(GLFWwindow* window, ESTADO_CAPIVARA estado, float tempo)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Estado: %d \ttempo: %f\n", estado, tempo);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

void TextRendering_ShowCapPos(GLFWwindow* window, glm::vec2 capPos, glm::vec2 capPrevPos, glm::vec2 capNextPos)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "pos = (%.2f, %.2f) prev = (%.2f, %.2f) next = (%.2f, %.2f)\n", capPos.x, capPos.y, capPrevPos.x, capPrevPos.y, capNextPos.x, capNextPos.y);

    TextRendering_PrintString(window, buffer, -1.0f+pad/5, -1.0f+pad, 1.0f);
}

void TextRendering_ShowCapAngulo(GLFWwindow* window, glm::vec4 capView, float angulo)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "capView: (%.2f, %.2f, %.2f) -> %.2f", capView.x, capView.y, capView.z, angulo);

    TextRendering_PrintString(window, buffer, -1.0f+pad/5, -1.0f+pad*5, 1.0f);
}

void TextRendering_ShowCollision(GLFWwindow* window, glm::vec4 minx, glm::vec4 maxx , glm::vec4 bboxmin, glm::vec4 bboxmax)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[160];
    snprintf(buffer, 160, "bbox: (%.5f, %.5f, %.5f) BBOX: (%.5f, %.5f, %.5f) bbox2: (%.2f, %.2f, %.2f) BBOX2: (%.2f, %.2f, %.2f) ", minx.x, minx.y, minx.z, maxx.x, maxx.y, maxx.z,
                                                                                                                                    bboxmin.x, bboxmin.y, bboxmin.z, bboxmax.x, bboxmax.y, bboxmax.z);

    TextRendering_PrintString(window, buffer, -1.0f+pad/5, -1.0f+pad*5, 1.0f);
}


// Escrevemos na tela qual matriz de projeчуo estс sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o nњmero de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variсveis estсticas (static) mantщm seus valores entre chamadas
    // subsequentes da funчуo!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o nњmero de segundos que passou desde a execuчуo do programa
    float seconds = (float)glfwGetTime();

    // Nњmero de segundos desde o њltimo cсlculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

// Funчуo para debugging: imprime no terminal todas informaчѕes de um modelo
// geomщtrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

