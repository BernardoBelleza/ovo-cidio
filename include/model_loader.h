#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <map>
#include <string>

// Forward declarations
struct SceneObject;

// ============================================================================
// IDs DOS MODELOS (para usar com glUniform1i)
// ============================================================================

// Planos do grid
#define CELL_EMPTY_PLANE   10
#define CELL_PATH_PLANE    11
#define CELL_BLOCKED_PLANE 12
#define CELL_BASE_PLANE    13

// Torres
#define MODEL_CHICKEN_TOWER  20
#define MODEL_BEAGLE_TOWER   21

// Armas
#define MODEL_THOMPSON_GUN   25
#define MODEL_AK47           26

// Inimigos


// Projéteis


// Ambiente

#define MODEL_CHICKEN_COOP   40


// Grid (já definidos no shader)


// ============================================================================
// FUNÇÃO PRINCIPAL - CARREGA TODOS OS MODELOS DO JOGO
// ============================================================================

void LoadAllGameModels(std::map<std::string, SceneObject>& scene);

#endif // MODEL_LOADER_H
