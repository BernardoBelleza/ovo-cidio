#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
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
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "resource_loader.h"
#include "game_attributes.h"
#include "tower_system.h"
#include "chicken_coop_system.h"
#include "hud.h"
#include "enemy_system.h"

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);

// Função para mostrar FPS
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Função para desenhar o grid do mapa
void DrawMapGrid();

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Detecção de clique duplo
double g_LastClickTime = 0.0;
int g_LastClickGridX = -1;
int g_LastClickGridZ = -1;
const double DOUBLE_CLICK_TIME = 0.5; // 500ms para clique duplo

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.5f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 15.0f; // Distância da câmera para a origem

// Variáveis da câmera livre (WASD)
bool g_UseFreeCamera = false; // true = câmera livre, false = câmera look-at
glm::vec4 g_CameraPosition = glm::vec4(0.0f, 2.0f, 5.0f, 1.0f);
glm::vec4 g_CameraView = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
float g_CameraPhi_Free = 0.0f;
float g_CameraTheta_Free = M_PI; // Começa olhando para -Z

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

// Objeto de física do chicken tower (testes)
PhysicsObject g_ChickenPhysics = {
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    1.0f,
    0.3f,
    false
};

// Objeto de física do beagle tower (testes)
PhysicsObject g_BeaglePhysics = {
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    1.0f,
    0.3f,
    false
};

CellType g_MapGrid[MAP_HEIGHT][MAP_WIDTH];

// Função para inicializar o mapa
void InitializeMap() {
    
    int layout[MAP_HEIGHT][MAP_WIDTH] = {
        {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
        {2,4,1,1,1,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
        {2,0,0,0,0,1,1,1,1,1,1,1,0,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,1,0,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,1,0,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,1,0,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,1,0,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,1,1,0,2},
        {2,0,0,0,0,0,0,0,0,0,0,0,1,3,2},
        {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
    };

    InitializeChickenCoops();
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            g_MapGrid[i][j] = (CellType)layout[i][j];
            if (g_MapGrid[i][j] == CELL_BASE)
                AddChickenCoop(j, i);
            
        }
    }
}

// Converte coordenada de grid para mundo 3D (centro da célula)
glm::vec3 GridToWorld(int gridX, int gridZ) {
    float worldX = gridX - MAP_WIDTH/2.0f + 0.5f;
    float worldZ = gridZ - MAP_HEIGHT/2.0f + 0.5f;
    return glm::vec3(worldX, 0.0f, worldZ);
}

// Converte coordenada de mundo para grid
glm::ivec2 WorldToGrid(glm::vec3 worldPos) {
    int gridX = (int)(worldPos.x + MAP_WIDTH/2.0f);
    int gridZ = (int)(worldPos.z + MAP_HEIGHT/2.0f);
    return glm::ivec2(gridX, gridZ);
}

// Funções de inicialização e renderização
GLFWwindow* InitializeWindow();
void SetupCallbacks(GLFWwindow* window);
void InitializeOpenGL();
void LoadGameResources();
void UpdateCameras(glm::mat4& view, glm::mat4& projection);
void RenderScene(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection);

// Retorna a altura do terreno baseada no tipo de célula
float GetGroundHeight(int gridX, int gridZ) {
    if (gridX < 0 || gridX >= MAP_WIDTH || gridZ < 0 || gridZ >= MAP_HEIGHT)
        return 0.0f;
    
    CellType cell = g_MapGrid[gridZ][gridX];

    switch(cell) {
        case CELL_EMPTY:    return 0.0f;   // Chão normal
        case CELL_PATH:     return -0.1f;  // Caminho levemente mais baixo
        case CELL_BLOCKED:  return 0.5f;   // Paredes elevadas
        case CELL_BASE:     return 0.2f;   // Base levemente elevada
        default:            return 0.0f;
    }
}

int main(int argc, char* argv[])
{
    // Inicializa janela e contexto GLFW/OpenGL
    GLFWwindow* window = InitializeWindow();

    // Configura callbacks de input
    SetupCallbacks(window);

    // Inicializa OpenGL e imprime informações da GPU
    InitializeOpenGL();

    LoadGameResources();

    InitializeMap();

    InitializeHUD();

    InitializeTowers();
    
    InitializeEnemySystem();

    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o MSAA (Multisample Anti-Aliasing)
    glEnable(GL_MULTISAMPLE);

    // Habilitamos o Backface Culling. Veja slides 8-13 do documento Aula_02_Fundamentos_Matematicos.pdf, slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float prevTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        // Calcula deltaTime para física
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;
        
        UpdatePhysics(g_ChickenPhysics, deltaTime);
        UpdatePhysics(g_BeaglePhysics, deltaTime);
        UpdateAllTowersPhysics(deltaTime);
        UpdateAllEnemies(deltaTime);
        UpdateWaveSystem(deltaTime);
        
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calcula matrizes de câmera
        glm::mat4 view, projection;
        UpdateCameras(view, projection);

        RenderScene(window, view, projection);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função que converte clique do mouse em posição do grid usando raycast
// Funciona tanto para câmera livre quanto look-at
bool GetGridPositionFromMouse(GLFWwindow* window, double mouseX, double mouseY, int& outGridX, int& outGridZ)
{
    // Pega dimensões da janela
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Normaliza mouse para [-1, 1]
    float x = (2.0f * mouseX) / width - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / height;
    
    // Ray em clip space
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Converte para view space
    float fov = M_PI / 3.0f;
    glm::mat4 proj = Matrix_Perspective(fov, g_ScreenRatio, -0.1f, -50.0f);
    glm::vec4 rayEye = glm::inverse(proj) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // Calcula posição e direção da câmera baseado no modo atual
    glm::vec4 cameraPos, cameraView;
    
    if (g_UseFreeCamera) {
        // CÂMERA LIVRE: Usa variáveis globais diretas
        cameraPos = g_CameraPosition;
        cameraView = g_CameraView;
    } else {
        // CÂMERA LOOK-AT: Calcula posição esférica
        float r = g_CameraDistance;
        float y_cam = r * sin(g_CameraPhi);
        float z_cam = r * cos(g_CameraPhi) * cos(g_CameraTheta);
        float x_cam = r * cos(g_CameraPhi) * sin(g_CameraTheta);
        
        cameraPos = glm::vec4(x_cam, y_cam, z_cam, 1.0f);
        glm::vec4 lookat = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        cameraView = lookat - cameraPos;
    }
    
    // Converte para world space
    glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::mat4 view = Matrix_Camera_View(cameraPos, cameraView, up);
    glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    glm::vec3 rayOrigin = glm::vec3(cameraPos);
    
    // Intersecção raio-plano (Y=0)
    if (glm::abs(rayDir.y) < 0.001f) return false;
    float t = -rayOrigin.y / rayDir.y;
    if (t < 0.0f) return false;
    
    // Ponto de hit
    glm::vec3 hitPoint = rayOrigin + rayDir * t;
    glm::ivec2 grid = WorldToGrid(hitPoint);
    
    outGridX = grid.x;
    outGridZ = grid.y;
    
    return (outGridX >= 0 && outGridX < MAP_WIDTH && outGridZ >= 0 && outGridZ < MAP_HEIGHT);
}

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // CLIQUE ESQUERDO: Seleciona torre OU abre menu de compra (duplo clique)
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Usa raycast para encontrar célula do grid
        int gridX, gridZ;
        if (GetGridPositionFromMouse(window, xpos, ypos, gridX, gridZ)) {
            double currentTime = glfwGetTime();
            double timeSinceLastClick = currentTime - g_LastClickTime;
            
            // Verifica se é clique duplo na mesma posição
            if (timeSinceLastClick < DOUBLE_CLICK_TIME && 
                gridX == g_LastClickGridX && gridZ == g_LastClickGridZ) {
                // CLIQUE DUPLO: Abre menu de compra
                printf("[INPUT] Clique duplo detectado em (%d, %d)\n", gridX, gridZ);
                OpenTowerMenu(gridX, gridZ);
                
                // Reset para evitar triplo clique
                g_LastClickTime = 0.0;
                g_LastClickGridX = -1;
                g_LastClickGridZ = -1;
            } else {
                // CLIQUE SIMPLES: Seleciona torre
                int towerIndex = SelectTowerAtPosition(gridX, gridZ);
                if (towerIndex >= 0) {
                    g_SelectedTowerIndex = towerIndex;
                    ShowTowerInfo(towerIndex);
                    printf("[SELECAO] Torre #%d selecionada\n", towerIndex + 1);
                } else {
                    g_SelectedTowerIndex = -1;
                    printf("[SELECAO] Nenhuma torre nessa posicao\n");
                }
                
                // Salva tempo e posição do clique
                g_LastClickTime = currentTime;
                g_LastClickGridX = gridX;
                g_LastClickGridZ = gridZ;
            }
        }
        
        // Guarda estado do mouse
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // BOTÃO DIREITO: Desseleciona torre ou fecha menu
        if (g_ShowTowerMenu) {
            CloseTowerMenu();
        } else {
            g_SelectedTowerIndex = -1;
            printf("[SELECAO] Torre desselecionada\n");
        }
        
        // Guarda estado do mouse
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;
    
        if (g_UseFreeCamera)
        {
            // ===== CÂMERA LIVRE =====
            g_CameraTheta_Free -= 0.003f * dx;
            g_CameraPhi_Free   -= 0.003f * dy;
            
            // Limita ângulo vertical
            float phimax = M_PI_2 - 0.01f;
            float phimin = -phimax;
            
            if (g_CameraPhi_Free > phimax)
                g_CameraPhi_Free = phimax;
            if (g_CameraPhi_Free < phimin)
                g_CameraPhi_Free = phimin;
        }
        else
        {
            // ===== CÂMERA LOOK-AT =====
            // Atualizamos parâmetros da câmera com os deslocamentos
            g_CameraTheta -= 0.01f*dx;
            g_CameraPhi   += 0.01f*dy;
        
            // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
            float phimax = M_PI_2;
            float phimin = -phimax;
        
            if (g_CameraPhi > phimax)
                g_CameraPhi = phimax;
        
            if (g_CameraPhi < phimin)
                g_CameraPhi = phimin;
        }
    
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // Se o menu de compra estiver aberto, fecha ele
        if (g_ShowTowerMenu) {
            CloseTowerMenu();
        } else {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    // Sistema de compra de torres
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (g_ShowTowerMenu) {
            BuyTower(TOWER_CHICKEN);
        }
    }
    
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        if (g_ShowTowerMenu) {
            BuyTower(TOWER_BEAGLE);
        }
    }

    // Tecla C: Alterna entre câmera livre e câmera look-at
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        g_UseFreeCamera = !g_UseFreeCamera;
        printf("Camera: %s\n", g_UseFreeCamera ? "LIVRE (WASD)" : "LOOK-AT");
    }

    // Tecla G: Reset da gravidade (testes)
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        // Reseta posição e velocidade do chicken (testes)
        g_ChickenPhysics.position.y = 5.0f;
        g_ChickenPhysics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ChickenPhysics.onGround = false;

        // Reseta posição e velocidade do beagle (testes)
        g_BeaglePhysics.position.y = 5.0f;
        g_BeaglePhysics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        g_BeaglePhysics.onGround = false;
    }
    
    // Tecla E: Spawna um lobo (teste de inimigos)
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        SpawnEnemy(ENEMY_WOLF);
        printf("[TESTE] Lobo spawnado!\n");
    }
    
    // Tecla ENTER: Inicia próxima wave
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        if (!IsWaveActive()) {
            int nextWave = GetCurrentWaveNumber() + 1;
            StartWave(nextWave);
        } else {
            printf("[WAVE] Wave ja esta ativa!\n");
        }
    }
    
    // Tecla H: Spawna um gavião (teste de inimigos)
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        SpawnEnemy(ENEMY_HAWK);
        printf("[TESTE] Gaviao spawnado!\n");
    }
    
    // Tecla F: Spawna uma raposa (teste de inimigos)
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        SpawnEnemy(ENEMY_FOX);
        printf("[TESTE] Raposa spawnada!\n");
    }
    
    // Tecla R: Spawna um rato (teste de inimigos)
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        SpawnEnemy(ENEMY_RAT);
        printf("[TESTE] Rato spawnado!\n");
    }

    // ===== CONTROLES DA CÂMERA LIVRE (WASD) =====
    if (g_UseFreeCamera)
    {
        // Calcula vetores de direção
        glm::vec4 camera_up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        glm::vec4 w = -g_CameraView / norm(g_CameraView);
        glm::vec4 u = crossproduct(camera_up, w) / norm(crossproduct(camera_up, w));

        float speed = 0.3f;

        // W - Frente
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
            g_CameraPosition -= w * speed;

        // S - Trás
        if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
            g_CameraPosition += w * speed;

        // A - Esquerda
        if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            g_CameraPosition -= u * speed;

        // D - Direita
        if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            g_CameraPosition += u * speed;
    }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
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

// Inicializa janela GLFW e contexto OpenGL
GLFWwindow* InitializeWindow()
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Ativamos antialiasing (MSAA) com 4 amostras por pixel
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "Ovocídio - O massacre da granja", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    return window;
}

void SetupCallbacks(GLFWwindow* window)
{
    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);
    
    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.
}

void InitializeOpenGL()
{
    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);
}

void LoadGameResources()
{
    LoadShadersFromFiles();

    // Carregamos as imagens para serem utilizadas como textura
    LoadTextureImage("../../data/textures/grid/grass.jpg");
    LoadTextureImage("../../data/textures/grid/path.jpg");
    LoadTextureImage("../../data/textures/towers/chicken.png");
    LoadTextureImage("../../data/textures/guns/thompson.png");
    LoadTextureImage("../../data/textures/towers/beagle.png");
    LoadTextureImage("../../data/textures/guns/ak47.jpg");
    LoadTextureImage("../../data/textures/enemies/hawk.png");
    LoadTextureImage("../../data/textures/enemies/fox.png");
    LoadTextureImage("../../data/textures/enemies/wolf.png");
    LoadTextureImage("../../data/textures/enemies/rat.png");

    ObjModel planemodel("../../data/models/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    // Carrega todos os modelos do Tower Defense
    LoadAllGameModels(g_VirtualScene);
}

void UpdateCameras(glm::mat4& view, glm::mat4& projection)
{
    glm::vec4 camera_position_c;
    glm::vec4 camera_view_vector;
    glm::vec4 camera_up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f);

    if (g_UseFreeCamera)
    {
        // Câmera livre
        g_CameraView.x = cos(g_CameraPhi_Free) * sin(g_CameraTheta_Free);
        g_CameraView.y = sin(g_CameraPhi_Free);
        g_CameraView.z = cos(g_CameraPhi_Free) * cos(g_CameraTheta_Free);

        camera_position_c = g_CameraPosition;
        camera_view_vector = g_CameraView;
    }
    else
    {
        // Câmera look-at
        float r = g_CameraDistance;
        float y = r * sin(g_CameraPhi);
        float z = r * cos(g_CameraPhi) * cos(g_CameraTheta);
        float x = r * cos(g_CameraPhi) * sin(g_CameraTheta);

        camera_position_c = glm::vec4(x, y, z, 1.0f);
        glm::vec4 camera_lookat_l = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        camera_view_vector = camera_lookat_l - camera_position_c;
    }

    // Computamos a matriz "View" utilizando os parâmetros da câmera para
    // definir o sistema de coordenadas da câmera.
    view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;   // Posição do "near plane"
    float farplane  = -100.0f; // Posição do "far plane"

    // Projeção Perspectiva.
    // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    float field_of_view = M_PI / 3.0f;
    projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
}

void RenderScene(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection)
{
    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(g_GpuProgramID);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo
    // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
    // efetivamente aplicadas em todos os pontos.
    glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

    // Desenhamos o grid do mapa (Tower Defense)
    DrawMapGrid();

    // Desenhamos todos os objetos do jogo
    DrawAllTowers();
    DrawChickenCoops();
    DrawTowerRangeCircle();
    DrawAllEnemies();

    // Imprimimos na tela informação sobre o número de quadros renderizados
    // por segundo (frames per second).
    TextRendering_ShowFramesPerSecond(window);

    // Renderiza o HUD (dinheiro e mensagens do console)
    int screenWidth, screenHeight;
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    RenderHUD(window, screenWidth, screenHeight);
}

void DrawMapGrid()
{    
    for (int z = 0; z < MAP_HEIGHT; z++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            glm::vec3 worldPos = GridToWorld(x, z);
            
            // Matriz de transformação para cada célula
            glm::mat4 model = Matrix_Translate(worldPos.x, 0.0f, worldPos.z)
                            * Matrix_Scale(0.48f, 1.0f, 0.48f); // 0.48 para deixar espaço entre células
            
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            
            // Define a cor baseada no tipo de célula
            CellType cellType = g_MapGrid[z][x];
            
            switch(cellType) {
                case CELL_EMPTY:
                    glUniform1i(g_object_id_uniform, CELL_EMPTY_PLANE);
                    break;
                case CELL_PATH:
                    glUniform1i(g_object_id_uniform, CELL_PATH_PLANE);
                    break;
                case CELL_BLOCKED:
                    glUniform1i(g_object_id_uniform, CELL_BLOCKED_PLANE);
                    break;
                case CELL_BASE:
                    glUniform1i(g_object_id_uniform, CELL_BASE_PLANE);
                    break;
                case CELL_START:
                    glUniform1i(g_object_id_uniform, CELL_START_PLANE);
                    break;
            }
            
            DrawVirtualObject("the_plane");
        }
    }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

