#ifndef ENEMY_SYSTEM_H
#define ENEMY_SYSTEM_H

#include <glm/vec3.hpp>
#include <vector>
#include "game_attributes.h"

extern std::vector<Enemy> g_Enemies;
extern std::vector<glm::vec3> g_PathWaypoints;

void InitializeEnemySystem();
void FindPathWaypoints();
void SpawnEnemy(EnemyType type);
void UpdateAllEnemies(float deltaTime);
void DrawAllEnemies();

glm::vec3 CalculateBezierPoint(const glm::vec3& p0, const glm::vec3& p1, 
                               const glm::vec3& p2, const glm::vec3& p3, float t);
glm::vec3 GetBezierControlPoint(int waypointIndex, bool isP1);

#endif // ENEMY_SYSTEM_H
