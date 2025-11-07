#include "model_loader.h"
#include "tiny_obj_loader.h"
#include <cstdio>
#include <vector>
#include <stdexcept>

// ============================================================================
// ESTRUTURAS (mesmas do main.cpp)
// ============================================================================

struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!warn.empty())
            fprintf(stderr, "AVISO: %s\n", warn.c_str());

        if (!err.empty())
            fprintf(stderr, "ERRO: %s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");
    }
};

// Funções externas do main.cpp
extern void BuildTrianglesAndAddToVirtualScene(ObjModel* model);
extern void ComputeNormals(ObjModel* model);

// ============================================================================
// FUNÇÃO AUXILIAR - CARREGA UM MODELO
// ============================================================================

static int g_ModelsLoaded = 0;
static int g_ModelsFailed = 0;

void LoadSingleModel(const char* filepath, const char* name) {
    printf("  -> %s", name);
    fflush(stdout);
    
    try {
        ObjModel* model = new ObjModel(filepath);
        ComputeNormals(model);
        BuildTrianglesAndAddToVirtualScene(model);
        printf(" OK\n");
        g_ModelsLoaded++;
    } catch (...) {
        printf(" ERRO (arquivo nao encontrado)\n");
        g_ModelsFailed++;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL - CARREGA TODOS OS MODELOS
// ============================================================================

void LoadAllGameModels(std::map<std::string, SceneObject>& scene) {
    printf("\n=======================================================\n");
    printf("     CARREGANDO MODELOS DO TOWER DEFENSE\n");
    printf("=======================================================\n");
    
    // ===== TORRES =====
    printf("\n[TORRES]\n");
    LoadSingleModel("../../data/models/towers/Low_Poly_Chicken.obj", "chicken_tower");
    
    // ===== ARMAS =====
    printf("\n[ARMAS]\n");
    LoadSingleModel("../../data/models/guns/M1A1_thompson.obj", "thompson_gun");
    
    // ===== INIMIGOS =====
    printf("\n[INIMIGOS]\n");
    
    
    // ===== PROJETEIS =====
    printf("\n[PROJETEIS]\n");
    
    
    // ===== AMBIENTE =====
    printf("\n[AMBIENTE]\n");
    
    
    // ===== RESUMO =====
    printf("\n=======================================================\n");
    if (g_ModelsFailed == 0) {
        printf("  OK: Todos os modelos carregados! (%d/%d)\n", 
               g_ModelsLoaded, g_ModelsLoaded + g_ModelsFailed);
    } else {
        printf("  AVISO: Carregados: %d | Falharam: %d\n", 
               g_ModelsLoaded, g_ModelsFailed);
        printf("  (Arquivos faltando serao substituidos por\n");
        printf("   placeholders: sphere/bunny)\n");
    }
    printf("=======================================================\n\n");
}
