# 📁 Estrutura de Modelos 3D

Esta pasta contém todos os modelos 3D do jogo Tower Defense.

## 📂 Estrutura

```
models/
├── towers/        ← Modelos de torres (defesas)
├── enemies/       ← Modelos de inimigos
├── projectiles/   ← Modelos de projéteis
└── environment/   ← Modelos de cenário (árvores, cerca, base)
```

## 📥 Modelos Necessários

### Torres (4 modelos)
- [ ] `chicken_tower.obj` - Torre de galinha
- [ ] `rooster_tower.obj` - Torre de galo
- [ ] `scarecrow_tower.obj` - Espantalho
- [ ] `egg_cannon.obj` - Canhão de ovos

### Inimigos (4 modelos)
- [ ] `fox.obj` - Raposa
- [ ] `rat.obj` - Rato
- [ ] `weasel.obj` - Doninha
- [ ] `hawk.obj` - Gavião

### Projéteis (3 modelos)
- [ ] `egg.obj` - Ovo
- [ ] `corn.obj` - Milho
- [ ] `feather.obj` - Pena

### Ambiente (4 modelos)
- [ ] `tree.obj` - Árvore
- [ ] `fence.obj` - Cerca
- [ ] `henhouse.obj` - Galinheiro (base)
- [ ] `grass_tuft.obj` - Tufo de grama

## 🌐 Onde Baixar Modelos Gratuitos

### Recomendado: Kenney Assets
- **URL**: https://kenney.nl/assets
- **Vantagens**: 
  - ✅ 100% Gratuito (CC0 License)
  - ✅ Estilo low-poly consistente
  - ✅ Formato .OBJ direto
  - ✅ Pacotes temáticos completos

**Pacotes úteis:**
- Tower Defense Kit: https://kenney.nl/assets/tower-defense-kit
- Farm Kit: https://kenney.nl/assets/farm-kit
- Animal Pack: https://kenney.nl/assets/animal-pack

### Outros Sites
- **Sketchfab**: https://sketchfab.com/feed (filtrar "Downloadable")
- **Free3D**: https://free3d.com/
- **Poly Haven**: https://polyhaven.com/models
- **Quaternius**: https://quaternius.com/packs.html

## 📝 Como Adicionar um Modelo

1. **Baixar** o arquivo `.obj` e texturas
2. **Renomear** para o nome esperado (ex: `fox.obj`)
3. **Copiar** para a pasta correta
4. **Compilar** - o modelo será carregado automaticamente!

## ⚠️ Requisitos dos Modelos

- **Formato**: `.OBJ` (Wavefront)
- **Poligonagem**: < 5000 triângulos (low-poly)
- **Escala**: Ajustável no código (não precisa ser exata)
- **Orientação**: Ajustável no código

## 🧪 Testando sem Modelos

Se os arquivos `.obj` não existirem, o jogo:
- ✅ Ainda compila e roda
- ✅ Mostra aviso no console
- ✅ Usa placeholders (esfera/coelho)

## 🎨 Exemplo de Uso no Código

```cpp
// Desenhar uma torre de galinha
glm::mat4 model = Matrix_Translate(2.0f, 0.0f, 3.0f)
                * Matrix_Scale(0.3f, 0.3f, 0.3f);
glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
glUniform1i(g_object_id_uniform, MODEL_CHICKEN_TOWER);
DrawVirtualObject("chicken_tower");
```

---

**Última atualização**: Outubro 2025
