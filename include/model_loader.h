#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <map>
#include <string>

// Forward declarations
struct SceneObject;

// ============================================================================
// IDs DOS MODELOS (para usar com glUniform1i)
// ============================================================================

// Modelos existentes
#define MODEL_SPHERE 0
#define MODEL_BUNNY  1
#define MODEL_PLANE  2

// Torres
#define MODEL_CHICKEN_TOWER  20
#define MODEL_ROOSTER_TOWER  21
#define MODEL_SCARECROW      22
#define MODEL_EGG_CANNON     23

// Inimigos
#define MODEL_FOX     30
#define MODEL_RAT     31
#define MODEL_WEASEL  32
#define MODEL_HAWK    33

// Projéteis
#define MODEL_EGG     40
#define MODEL_CORN    41
#define MODEL_FEATHER 42

// Ambiente
#define MODEL_TREE      50
#define MODEL_FENCE     51
#define MODEL_HENHOUSE  52
#define MODEL_GRASS     53

// Grid (já definidos no shader)
#define MODEL_CELL_EMPTY   10
#define MODEL_CELL_PATH    11
#define MODEL_CELL_BLOCKED 12
#define MODEL_CELL_BASE    13

// ============================================================================
// FUNÇÃO PRINCIPAL - CARREGA TODOS OS MODELOS DO JOGO
// ============================================================================

void LoadAllGameModels(std::map<std::string, SceneObject>& scene);

#endif // MODEL_LOADER_H
