# PG2 - Šimon, Kohout

## 🎮 Funkce

- ✅ **OpenGL 4.6 Core** + GLSL 460
- ✅ **Vykreslování 3D modelů** (.obj) s vlastní strukturou `Model`, `Mesh`, `Vertex`
- ✅ **Kamera** s volným pohybem (`WASD`, myš), FPS styl
- ✅ **Podpora průhlednosti** (alpha blending)
- ✅ **Kolizní detekce (AABB)** pro stěny, cílovou kostku a animovaný objekt
- ✅ **Osvětlení**
  - ambientní světlo
  - směrové světlo (slunce, s animací)
  - 2 bodová světla (včetně pohybující se kostky)
  - spotlight (baterka)
- ✅ **Animace objektů** (kostka se hýbe, králík rotuje, slunce obíhá scénu)
- ✅ **Maze generátor** s náhodnou mapou a cílovým bodem
- ✅ **Základní HUD pomocí ImGui** (FPS, stavové info)
- ✅ **Konfigurace pomocí `config.json`**:
  - výchozí velikost okna
  - fullscreen
  - vsync
  - antialiasing (zapnuto při spuštění)
  - zapnutí baterky a noclipu

## 🕹️ Ovládání

| Klávesa / Myš     | Akce                                      |
|-------------------|--------------------------------------------|
| `W A S D`         | Pohyb dopředu, dozadu, do stran            |
| Myš               | Otáčení kamery (FPS look)                  |
| `SPACE`           | Skok (v chodeckém režimu)                  |
| `F1`              | Přepínání fullscreen/window                |
| `F2`              | Zobrazit / skrýt HUD                       |
| `F3`              | Přepínání noclip (volné létání vs chůze)   |
| `F4`              | Přepínání VSync                            |
| `C`               | Přepnutí režimu kurzoru (zamčený / volný)  |
| `Esc`             | Zavřít aplikaci                            |
| Kolečko myši      | Zoom (změna FOV kamery mezi 30° a 90°)     |
| Levé tlačítko myši| Zapnutí / vypnutí baterky (spotlight)      |

## 🔧 Závislosti

- `GLFW`
- `GLEW`
- `GLM`
- `OpenCV`
- `ImGui`
- `nlohmann/json`

Vše spravováno přes `vcpkg`.

## 🗂️ Struktura

- `main.cpp` – vstupní bod
- `App.*` – hlavní třída aplikace
- `Model`, `Mesh`, `ShaderProgram` – základní 3D renderer
- `camera.hpp` – pohyb kamery
- `assets.hpp` – generování scény
- `resources/` – modely, textury, konfigurační soubor

## ▶️ Spuštění

1. Otevřít projekt ve Visual Studio 2022
2. Ujistit se, že všechny závislosti jsou nainstalovány přes `vcpkg install`
3. Spustit projekt
4. (Volitelně) upravit `resources/config.json` pro změnu nastavení

## 📁 Příklad `config.json`

```json
{
  "antialiasing": true,
  "vsync": true,
  "fullscreen": false,
  "noclip": false,
  "flashlight": false,
  "window_width": 1280,
  "window_height": 720
}
```
