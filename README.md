# **Necros**

\_Um remake de uma **game engine** que eu criei lá pelos 15 anos (Template Engine);
_Focada especificamente ao 3D_

> [!WARNING]  
> **Ainda está em fase de desenvolvimento.**  
> Faltam muitas funcionalidades e ainda há bugs para corrigir.

---

## ✨ **Features**

- 🎮 Movimento básico em primeira pessoa
- 🖼️ Interface simples com **ImGui**
- 📦 Importação de modelos via **Assimp**
- ⚡ Renderização otimizada de objetos com **batching** por mesh/textura
- 🌀 Algoritmo básico de geração de labirintos (**Recursive Backtracker**)
- 🛠️ Editor simples:
  - Alteração de propriedades dos objetos (UV, posição, rotação, etc.)
  - Adição e edição de luzes (**Directional, Point e Spot**)
  - Gerador de labirinto com controles de largura, altura, espaçamento, altura das paredes e comprimento do chão

---

## 🎮 **Controles**

| Tecla              | Ação                                         |
| ------------------ | -------------------------------------------- |
| **W**              | Frente                                       |
| **A**              | Esquerda                                     |
| **S**              | Trás                                         |
| **D**              | Direita                                      |
| **Q**              | Abre o editor                                |
| **Esc**            | Pausar o jogo                                |
| **V**              | Ativar voo (noclip)                          |
| **Shift Esquerdo** | Descer (quando voando)                       |
| **Espaço**         | Subir (quando voando) ou Pular (modo normal) |

---

## 🔧 **Compilar**

- **Script facilitado**: `.\b.bat`
- Para rodar **diretamente**: `.\b.bat --direct`
