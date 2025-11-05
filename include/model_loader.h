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

// Armas
#define MODEL_THOMPSON_GUN   25

// Inimigos


// Projéteis


// Ambiente


// Grid (já definidos no shader)


// ============================================================================
// FUNÇÃO PRINCIPAL - CARREGA TODOS OS MODELOS DO JOGO
// ============================================================================

void LoadAllGameModels(std::map<std::string, SceneObject>& scene);

#endif // MODEL_LOADER_H
