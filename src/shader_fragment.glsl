#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define CELL_EMPTY_PLANE    10
#define CELL_PATH_PLANE     11
#define CELL_BLOCKED_PLANE  12
#define CELL_BASE_PLANE     13
#define CELL_START_PLANE    14


// Tower Defense Models
#define MODEL_CHICKEN_TOWER 20
#define MODEL_BEAGLE_TOWER  21
#define MODEL_THOMPSON_GUN  25
#define MODEL_AK47          26
#define MODEL_CHICKEN_COOP  40

// Enemies
#define MODEL_HAWK          30
#define MODEL_FOX           31
#define MODEL_WOLF          32
#define MODEL_RAT           33

// Tower Range Circle
#define TOWER_RANGE_CIRCLE  99

uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D texture_grass;
uniform sampler2D texture_path;
uniform sampler2D texture_chicken;
uniform sampler2D texture_thompson;
uniform sampler2D texture_beagle;
uniform sampler2D texture_ak47;
uniform sampler2D texture_chicken_coop;
uniform sampler2D texture_hawk;
uniform sampler2D texture_fox;
uniform sampler2D texture_wolf;
uniform sampler2D texture_rat;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 light_source_point = vec4(0.0,15.0,0.0,1.0);
    vec4 l = normalize(light_source_point - p);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Half-vector do blinn-phong
    vec4 h = normalize(l + v);
    
    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    // Se nada for definido, o padrão da luz é nulo no objeto
    vec3 Kd0 = vec3(0.0, 0.0, 0.0); 
    vec3 Ks0 = vec3(0.0, 0.0, 0.0); 
    float q = 1000.0; // Para ser ignorável deve ser muito alto
    vec3 I = vec3(1.0, 1.0, 1.0); // Intensidade da luz branca
   
    if ( object_id == MODEL_CHICKEN_TOWER )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_chicken, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_THOMPSON_GUN )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_thompson, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_BEAGLE_TOWER )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_beagle, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_AK47 )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_ak47, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_CHICKEN_COOP )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_chicken_coop, vec2(U,V)).rgb; 
        Ks0 = vec3(1.0, 1.0, 1.0);
        q = 10.0;

        Kd0 = vec3(0.7, 0.55, 0.35);
    }
    else if ( object_id == MODEL_HAWK )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_hawk, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_FOX )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_fox, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_WOLF )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_wolf, vec2(U, V)).rgb;
    }
    else if ( object_id == MODEL_RAT )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_rat, vec2(U, V)).rgb;
    }
    else if ( object_id == CELL_EMPTY_PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_grass, texcoords).rgb;
    }
    else if ( object_id == CELL_PATH_PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = texture(texture_path, texcoords).rgb;
    }
    else if ( object_id == CELL_BLOCKED_PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = vec3(0.3, 0.3, 0.3); // Cinza escuro
    }
    else if ( object_id == CELL_BASE_PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = vec3(0.2, 0.4, 0.9); // Azul
    }
    else if ( object_id == CELL_START_PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;
        Kd0 = vec3(1.0, 0.0, 0.0); // Vermelho
    }
    else if ( object_id == TOWER_RANGE_CIRCLE )
    {
        Kd0 = vec3(1.0, 1.0, 0.0); // Amarelo
    }

    // Equação de Iluminação
    float lambert = max(0,dot(n,l));
    
    vec3 phong_specular_term  = Ks0 * I * pow(max(dot(n, h), 0.0),q); 
    color.rgb = Kd0 * I * (lambert + 0.01) + phong_specular_term;

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

