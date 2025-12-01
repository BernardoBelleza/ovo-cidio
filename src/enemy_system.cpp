#include "enemy_system.h"
#include "game_attributes.h"
#include "matrices.h"
#include "resource_loader.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>

std::vector<Enemy> g_Enemies;
std::vector<glm::vec3> g_PathWaypoints;

static std::vector<Wave> g_Waves;
static int g_CurrentWave;
static float g_WaveTimer;
static int g_NextSpawnIndex;
static bool g_WaveActive;

static void InitializeWaves() {
    g_Waves.clear();
    
    Wave wave1;
    wave1.duration = 20.0f;
    wave1.spawns = {
        {ENEMY_RAT, 0.0f},
        {ENEMY_RAT, 3.0f},
        {ENEMY_RAT, 6.0f},
        {ENEMY_RAT, 9.0f},
        {ENEMY_RAT, 12.0f}
    };
    g_Waves.push_back(wave1);
    
    Wave wave2;
    wave2.duration = 30.0f;
    wave2.spawns = {
        {ENEMY_RAT, 0.0f},
        {ENEMY_RAT, 1.0f},
        {ENEMY_FOX, 3.0f},
        {ENEMY_RAT, 5.0f},
        {ENEMY_FOX, 7.0f},
        {ENEMY_RAT, 9.0f},
        {ENEMY_FOX, 11.0f},
        {ENEMY_RAT, 13.0f}
    };
    g_Waves.push_back(wave2);
    
    Wave wave3;
    wave3.duration = 40.0f;
    wave3.spawns = {
        {ENEMY_WOLF, 0.0f},
        {ENEMY_HAWK, 2.0f},
        {ENEMY_WOLF, 4.0f},
        {ENEMY_HAWK, 6.0f},
        {ENEMY_FOX, 8.0f},
        {ENEMY_FOX, 9.0f},
        {ENEMY_WOLF, 11.0f},
        {ENEMY_HAWK, 13.0f},
        {ENEMY_WOLF, 15.0f},
        {ENEMY_HAWK, 17.0f}
    };
    g_Waves.push_back(wave3);
    
    Wave wave4;
    wave4.duration = 50.0f;
    wave4.spawns = {
        {ENEMY_WOLF, 0.0f},
        {ENEMY_WOLF, 1.5f},
        {ENEMY_WOLF, 3.0f},
        {ENEMY_HAWK, 5.0f},
        {ENEMY_HAWK, 6.0f},
        {ENEMY_WOLF, 8.0f},
        {ENEMY_WOLF, 9.5f},
        {ENEMY_WOLF, 11.0f},
        {ENEMY_FOX, 13.0f},
        {ENEMY_FOX, 14.0f},
        {ENEMY_FOX, 15.0f},
        {ENEMY_WOLF, 17.0f},
        {ENEMY_WOLF, 18.5f},
        {ENEMY_WOLF, 20.0f}
    };
    g_Waves.push_back(wave4);
}

static glm::vec3 GridToWorld(int gridX, int gridZ) {
    float worldX = gridX - MAP_WIDTH/2.0f + 0.5f;
    float worldZ = gridZ - MAP_HEIGHT/2.0f + 0.5f;
    return glm::vec3(worldX, 0.0f, worldZ);
}

void InitializeEnemySystem() {
    g_Enemies.clear();
    g_PathWaypoints.clear();
    FindPathWaypoints();
    InitializeWaves();
    g_CurrentWave = -1;
    g_WaveTimer = 0.0f;
    g_NextSpawnIndex = 0;
    g_WaveActive = false;
}

void FindPathWaypoints() {
    int startX = -1, startZ = -1;
    for (int z = 0; z < MAP_HEIGHT; z++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (g_MapGrid[z][x] == CELL_START) {
                startX = x;
                startZ = z;
                break;
            }
        }
        if (startX != -1) break;
    }
    
    if (startX == -1) {
        printf("[ENEMY] Erro: Nao foi possivel encontrar o inicio do caminho!\n");
        return;
    }

    g_PathWaypoints.push_back(GridToWorld(startX, startZ));

    int currentX = startX;
    int currentZ = startZ;
    int lastDirX = 0;
    int lastDirZ = 0;
    
    bool pathComplete = false;
    int maxIterations = MAP_WIDTH * MAP_HEIGHT;
    int iterations = 0;
    
    while (!pathComplete && iterations < maxIterations) {
        iterations++;
        
        int dirX[] = {1, 0, -1, 0};
        int dirZ[] = {0, 1, 0, -1};
        
        bool foundNext = false;
        
        for (int i = 0; i < 4; i++) {
            int nextX = currentX + dirX[i];
            int nextZ = currentZ + dirZ[i];
            
            if (-lastDirX == dirX[i] && -lastDirZ == dirZ[i]) continue;
            if (nextX < 0 || nextX >= MAP_WIDTH || nextZ < 0 || nextZ >= MAP_HEIGHT)
                continue;
            
            CellType nextCell = g_MapGrid[nextZ][nextX];
            
            if (nextCell == CELL_PATH) {
                if (dirX[i] != lastDirX || dirZ[i] != lastDirZ) {
                    // Só adiciona se não for o primeiro movimento
                    if (lastDirX != 0 || lastDirZ != 0) {
                        g_PathWaypoints.push_back(GridToWorld(currentX, currentZ));
                    }
                }
                
                currentX = nextX;
                currentZ = nextZ;
                lastDirX = dirX[i];
                lastDirZ = dirZ[i];
                foundNext = true;
                break;
            }
            
            if (nextCell == CELL_BASE) {
                // Só adiciona se não for o primeiro movimento
                if (lastDirX != 0 || lastDirZ != 0) {
                    g_PathWaypoints.push_back(GridToWorld(currentX, currentZ));
                }
                
                g_PathWaypoints.push_back(GridToWorld(nextX, nextZ));
                pathComplete = true;
                foundNext = true;
                break;
            }
        }
        
        if (!foundNext) {
            printf("[ENEMY] Erro: Caminho bloqueado em (%d, %d)\n", currentX, currentZ);
            break;
        }
    }
    
    if (!pathComplete && g_PathWaypoints.size() > 0) {
        printf("[ENEMY] Erro: Caminho nao chegou na base!\n");
    }
    
}

const EnemyAttributes& GetEnemyAttributes(EnemyType type) {
    switch(type) {
        case ENEMY_WOLF: return WOLF_ATTRIBUTES;
        case ENEMY_HAWK: return HAWK_ATTRIBUTES;
        case ENEMY_FOX:  return FOX_ATTRIBUTES;
        case ENEMY_RAT:  return RAT_ATTRIBUTES;
        default:         return WOLF_ATTRIBUTES;
    }
}

const EnemyRenderInfo& GetEnemyRenderInfo(EnemyType type) {
    switch(type) {
        case ENEMY_WOLF: return WOLF_RENDER;
        case ENEMY_HAWK: return HAWK_RENDER;
        case ENEMY_FOX:  return FOX_RENDER;
        case ENEMY_RAT:  return RAT_RENDER;
        default:         return WOLF_RENDER;
    }
}

int GetEnemyModelID(EnemyType type) {
    switch(type) {
        case ENEMY_WOLF:
            return MODEL_WOLF;
        case ENEMY_HAWK:
            return MODEL_HAWK;
        case ENEMY_FOX:
            return MODEL_FOX;
        case ENEMY_RAT:
            return MODEL_RAT;
        default:
            return MODEL_WOLF;
    }
}

void SpawnEnemy(EnemyType type) {    
    const EnemyAttributes& attrs = GetEnemyAttributes(type);
    
    Enemy enemy;
    enemy.type = type;
    enemy.position = g_PathWaypoints[0];
    enemy.currentPathIndex = 0;
    enemy.pathProgress = 0.0f;
    enemy.active = true;
    enemy.maxHealth = attrs.maxHealth;
    enemy.moveSpeed = attrs.moveSpeed;
    enemy.health = enemy.maxHealth;
    
    g_Enemies.push_back(enemy);
}

void UpdateAllEnemies(float deltaTime) {
    for (int i = (int)g_Enemies.size() - 1; i >= 0; i--) {
        Enemy& enemy = g_Enemies[i];
        
        if (!enemy.active || enemy.health <= 0.0f) {
            g_Enemies.erase(g_Enemies.begin() + i);
            continue;
        }
        
        if (g_PathWaypoints.empty()) continue;
        
        if (enemy.currentPathIndex >= (int)g_PathWaypoints.size() - 1) {
            enemy.active = false;
            continue;
        }
        
        glm::vec3 currentWaypoint = g_PathWaypoints[enemy.currentPathIndex];
        glm::vec3 nextWaypoint = g_PathWaypoints[enemy.currentPathIndex + 1];
        glm::vec3 direction = nextWaypoint - currentWaypoint;
        float segmentLength = glm::length(direction);
        if (segmentLength > MIN_SEGMENT_LENGTH) {
            float moveDistance = enemy.moveSpeed * deltaTime;
            enemy.pathProgress += moveDistance / segmentLength;
            
            if (enemy.pathProgress >= 1.0f) {
                enemy.currentPathIndex++;
                enemy.pathProgress = 0.0f;
                
                if (enemy.currentPathIndex >= (int)g_PathWaypoints.size() - 1) {
                    enemy.position = g_PathWaypoints.back();
                    enemy.active = false;
                    printf("[ENEMY] Inimigo chegou na base!\n");
                }
            } else {
                glm::vec3 oldPos = enemy.position;

                glm::vec3 p0 = currentWaypoint;
                glm::vec3 p3 = nextWaypoint;
                glm::vec3 p1 = GetBezierControlPoint(enemy.currentPathIndex, true);
                glm::vec3 p2 = GetBezierControlPoint(enemy.currentPathIndex, false);
                enemy.position = CalculateBezierPoint(p0, p1, p2, p3, enemy.pathProgress);
                
                glm::vec3 movement = enemy.position - oldPos;
                if (glm::length(movement) > MIN_SEGMENT_LENGTH) {
                    enemy.direction = glm::normalize(movement);
                }
            }
        }
    }
}

void DrawAllEnemies() {
    for (const Enemy& enemy : g_Enemies) {
        if (!enemy.active) continue;
        
        const EnemyRenderInfo& renderInfo = GetEnemyRenderInfo(enemy.type);
        int modelID = GetEnemyModelID(enemy.type);
        
        float angle = atan2f(enemy.direction.x, enemy.direction.z);
        
        glm::mat4 model = Matrix_Translate(enemy.position.x, enemy.position.y + renderInfo.yOffset, enemy.position.z)
                        * Matrix_Rotate_Y(angle)
                        * Matrix_Scale(renderInfo.scaleX, renderInfo.scaleY, renderInfo.scaleZ);
       
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, modelID);
        DrawVirtualObject(renderInfo.meshName);
    }
}

glm::vec3 CalculateBezierPoint(const glm::vec3& p0, const glm::vec3& p1, 
                               const glm::vec3& p2, const glm::vec3& p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    // Fórmula de Bezier cúbica: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
    glm::vec3 point = uuu * p0;
    point += 3.0f * uu * t * p1;
    point += 3.0f * u * tt * p2;
    point += ttt * p3;
    
    return point;
}

glm::vec3 GetBezierControlPoint(int waypointIndex, bool isP1) {
    const glm::vec3& current = g_PathWaypoints[waypointIndex];
    const glm::vec3& next = g_PathWaypoints[waypointIndex + 1];
    glm::vec3 direction = next - current;
    float offset = glm::length(direction) * ENEMY_BEZIER_SMOOTHNESS;

    if (isP1) {
        // Control point leaving current waypoint
        if (waypointIndex > 0) {
            const glm::vec3& prev = g_PathWaypoints[waypointIndex - 1];
            glm::vec3 tangent = glm::normalize(glm::normalize(next - current) + glm::normalize(current - prev));
            return current + tangent * offset;
        }
        return current + glm::normalize(direction) * offset;
    } else {
        // Control point approaching next waypoint
        if (waypointIndex + 2 < (int)g_PathWaypoints.size()) {
            const glm::vec3& nextNext = g_PathWaypoints[waypointIndex + 2];
            glm::vec3 tangent = glm::normalize(glm::normalize(next - current) + glm::normalize(nextNext - next));
            return next - tangent * offset;
        }
        return next - glm::normalize(direction) * offset;
    }
}

void StartWave(int waveNumber) {
    if (waveNumber < 0 || waveNumber >= (int)g_Waves.size()) {
        printf("[WAVE] Aviso: Voce derrotou todas as waves!\n");
        return;
    }
    
    g_CurrentWave = waveNumber;
    g_WaveTimer = 0.0f;
    g_NextSpawnIndex = 0;
    g_WaveActive = true;
    
    printf("[WAVE] Wave %d iniciada! (%d inimigos)\n", 
           waveNumber + 1, 
           (int)g_Waves[waveNumber].spawns.size());
}

void UpdateWaveSystem(float deltaTime) {
    if (!g_WaveActive || g_CurrentWave < 0) return;
    
    g_WaveTimer += deltaTime;
    
    const Wave& currentWave = g_Waves[g_CurrentWave];
    
    while (g_NextSpawnIndex < (int)currentWave.spawns.size()) {
        const EnemySpawn& spawn = currentWave.spawns[g_NextSpawnIndex];
        
        if (g_WaveTimer >= spawn.spawnTime) {
            SpawnEnemy(spawn.type);
            g_NextSpawnIndex++;
        } else {
            break;
        }
    }
    
    if (g_NextSpawnIndex >= (int)currentWave.spawns.size() && g_Enemies.empty()) {
        g_WaveActive = false;
        printf("[WAVE] Wave %d completa!\n", g_CurrentWave + 1);
    }
}

bool IsWaveActive() {
    return g_WaveActive;
}

bool IsWaveComplete() {
    if (g_CurrentWave < 0) return false;
    return !g_WaveActive && g_NextSpawnIndex >= (int)g_Waves[g_CurrentWave].spawns.size();
}

int GetCurrentWaveNumber() {
    return g_CurrentWave;
}
