#include "hud.h"
#include "game_attributes.h"
#include "enemy_system.h"
#include <GLFW/glfw3.h>
#include <sstream>
#include <iomanip>

// Declaração externa da função de text rendering (definida em main.cpp)
extern void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale);
extern int g_PlayerLives;

int g_PlayerMoney;

// Variáveis globais de mensagens
std::deque<std::string> g_ConsoleMessages;

// ==================== ECONOMIA ====================

void InitializeEconomy() {
    g_PlayerMoney = PLAYER_MONEY_START;
}

bool SpendMoney(int amount) {
    if (g_PlayerMoney >= amount) {
        g_PlayerMoney -= amount;
        return true;
    }
    return false;
}

void AddMoney(int amount) {
    g_PlayerMoney += amount;
}

// ==================== MENSAGENS ====================

void AddConsoleMessage(const std::string& message) {
    g_ConsoleMessages.push_back(message);
    
    // Manter apenas as últimas MAX_CONSOLE_MESSAGES mensagens
    while (g_ConsoleMessages.size() > MAX_CONSOLE_MESSAGES) {
        g_ConsoleMessages.pop_front();
    }
}

void ClearConsoleMessages() {
    g_ConsoleMessages.clear();
}

// ==================== HUD ====================

void InitializeHUD() {
    InitializeEconomy();
    ClearConsoleMessages();
    AddConsoleMessage("Bem-vindo ao Ovocidio!");
}

void RenderHUD(GLFWwindow* window, int screenWidth, int screenHeight) {
    std::stringstream moneyText;
    moneyText << "Dinheiro: $" << g_PlayerMoney;
    
    float moneyX = -0.95f;
    float moneyY = 0.9f;
    float moneyScale = 1.5f;
    
    TextRendering_PrintString(window, moneyText.str(), moneyX, moneyY, moneyScale);

   std::stringstream livesText;
    livesText << "Vidas: " << g_PlayerLives << "/" << PLAYER_STARTING_LIVES;
    

    TextRendering_PrintString(window, livesText.str(), -0.95f, 0.8f, 1.3f);
    
    // ===== WAVE INFO =====
    int currentWave = GetCurrentWaveNumber();
    bool waveActive = IsWaveActive();
    
    std::stringstream waveText;
    if (currentWave >= 0) {
        if (waveActive) {
            waveText << "Wave " << (currentWave + 1) << " - EM ANDAMENTO";
        } else {
            waveText << "Wave " << (currentWave + 1) << " COMPLETA!";
        }
    } else {
        waveText << "Pressione ENTER para iniciar Wave 1";
    }
    TextRendering_PrintString(window, waveText.str(), -0.95f, 0.7f, 1.2f);
    
    // ===== GAME OVER =====
    if (g_PlayerLives <= 0) {
        std::stringstream gameOverText;
        gameOverText << "=== GAME OVER ===";
        TextRendering_PrintString(window, gameOverText.str(), -0.3f, 0.0f, 2.0f);
                
    }

    // ===== RENDERIZAR MENSAGENS DO CONSOLE (CANTO INFERIOR ESQUERDO) =====
    float messageX = -0.95f;
    float messageY = -0.7f;
    float messageScale = 1.0f;
    float lineSpacing = 0.08f;
    
    // Título do console
    TextRendering_PrintString(window, "Console:", messageX, messageY, messageScale);
    
    // Mensagens (da mais antiga para a mais recente, de cima para baixo)
    float currentY = messageY - lineSpacing;
    for (size_t i = 0; i < g_ConsoleMessages.size(); i++) {
        TextRendering_PrintString(window, g_ConsoleMessages[i], messageX, currentY, messageScale);
        currentY -= lineSpacing;
    }
    
    // ===== INSTRUÇÕES (CANTO SUPERIOR DIREITO) =====
    float instrX = 0.4f;
    float instrY = 0.9f;
    float instrScale = 0.8f;
    
    TextRendering_PrintString(window, "Duplo-clique: Abrir menu", instrX, instrY, instrScale);
    TextRendering_PrintString(window, "ENTER: Iniciar proxima wave", instrX, instrY - 0.09f, instrScale);
    TextRendering_PrintString(window, "1/2: Comprar torre", instrX, instrY - 0.12f, instrScale);
    TextRendering_PrintString(window, "1: Torre Galinha ($100)", instrX, instrY - 0.15f, instrScale);
    TextRendering_PrintString(window, "2: Torre Beagle ($200)", instrX, instrY - 0.18f, instrScale);
}
