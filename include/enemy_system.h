#ifndef ENEMY_SYSTEM_H
#define ENEMY_SYSTEM_H

#include <glm/vec3.hpp>
#include <vector>
#include "game_attributes.h"

extern std::vector<Enemy> g_Enemies;
extern std::vector<glm::vec3> g_PathWaypoints;

struct EnemySpawn {
    EnemyType type;
    float spawnTime;
};

struct Wave {
    std::vector<EnemySpawn> spawns;
    float duration;
};

void InitializeEnemySystem();
void FindPathWaypoints();
void SpawnEnemy(EnemyType type);
void UpdateAllEnemies(float deltaTime);
void DrawAllEnemies();

glm::vec3 CalculateBezierPoint(const glm::vec3& p0, const glm::vec3& p1, 
                               const glm::vec3& p2, const glm::vec3& p3, float t);
glm::vec3 GetBezierControlPoint(int waypointIndex, bool isP1);


static void InitializeWaves();
static void GenerateWave(int waveNumber); 
void StartWave(int waveNumber);
void UpdateWaveSystem(float deltaTime);
bool IsWaveActive();
bool IsWaveComplete();
int GetCurrentWaveNumber();



const EnemyRenderInfo& GetEnemyRenderInfo(EnemyType type); 
const EnemyAttributes& GetEnemyAttributes(EnemyType type);

#endif // ENEMY_SYSTEM_H
