#ifndef CHICKEN_COOP_SYSTEM_H
#define CHICKEN_COOP_SYSTEM_H

#include <glm/glm.hpp>

// Estrutura que representa um galinheiro (ChickenCoop) colocado no mapa
struct ChickenCoop {
    int gridX;
    int gridZ;
    glm::vec3 worldPos;
    float rotation;
    bool active;
};

// Inicializa o sistema (limpa lista interna)
void InitializeChickenCoops();

// Adiciona um galinheiro na posição do grid especificada
void AddChickenCoop(int gridX, int gridZ);

// Desenha todos os galinheiros ativos
void DrawChickenCoops();

#endif // CHICKEN_COOP_SYSTEM_H
