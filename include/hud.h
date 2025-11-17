#ifndef HUD_H
#define HUD_H

#include <string>
#include <deque>
#include <glm/glm.hpp>

// Forward declaration
struct GLFWwindow;

// Sistema de economia
extern int g_PlayerMoney;

// Sistema de mensagens
const int MAX_CONSOLE_MESSAGES = 5;
extern std::deque<std::string> g_ConsoleMessages;

// Funções de economia
void InitializeEconomy();
bool SpendMoney(int amount);
void AddMoney(int amount);

// Funções de mensagens
void AddConsoleMessage(const std::string& message);
void ClearConsoleMessages();

// Funções de renderização
void InitializeHUD();
void RenderHUD(GLFWwindow* window, int screenWidth, int screenHeight);

#endif // HUD_H
