# ovo-cidio

**Projeto de Fundamentos de Computação Gráfica - UFRGS**

Tower Defense do Galinheiro

## Setup

### Dependências

- **GLFW3** - Gerenciamento de janelas
- **GLAD** - Carregador de funções OpenGL
- **CMake** - Sistema de build
- **OpenGL 3.3+** - API gráfica

### Preparação do Ambiente

RedHat:
```bash
sudo dnf install cmake g++ glfw-devel
```

Debian:
```bash
sudo apt install cmake g++ libglfw3-dev libxi-dev
```

Windows (Mingw):
```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

### Configurar GLAD
1. Acesse https://glad.dav1d.de/
2. Configure:
   - Language: C/C++
   - Specification: OpenGL
   - API gl: Version 3.3 (ou superior)
   - Profile: Core
3. Clique em "Generate"
4. Baixe o arquivo ZIP
5. Extraia e coloque o conteúdo em `external/glad/`

### Compilar e Executar

Linux:
```bash
mkdir build
cd build
cmake ..
make
```

```bash
./CarpaDiem
```

Windows:
- Clicar no botão "Play"
