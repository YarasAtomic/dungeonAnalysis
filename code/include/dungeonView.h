#ifndef VIEW_H
#define VIEW_H

#include "ui.h"
#include "rlights.h"
#include "ezdungeon.h"

class DungeonStat;

class DungeonView : public DocumentWindow{

    enum Mode {player,menu};
    Mode mode = menu;

    Vector2 cameraAngle = {0,0};
    Vector3 targetPos = {0,0,0};
    float cameraDistance = 50;
    Vector3 clickWorld = {0,0,0};
    Vector2 mousePos = {0,0};
    int screenWidth = 800;
    int screenHeight = 600;
    Shader lightingShader;

    const float playerSpeed = 0.1;
    const float mouseSensitivity = 0.01;
    
    // Mesh dungeonMesh;
    int style = 0;
    bool modelLoaded = false;
    Model dungeonModel;
    std::vector<Model> dungeonModels;
    Vector3 dungeonPos;

    bool isDungeon2d = false;

    DungeonStat * stats = nullptr;
    
    
    void enterPlayMode();

    void playerModeUpdate(dungeonMatrix * dungeon,Vector3 dungeonPos,Vector3 & playerPos);

    void InteractionModeUpdate(Vector3 & playerPos);
protected:
    void Show() override;
public:
    std::string name;

    DungeonView(std::string name,DungeonStat * stats);

    Camera3D camera = { 0 };
    // Texture2D gridTexture = { 0 };
    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    // void ShowAsTabItem() override;
    void Update() override;
    void Shutdown() override;
};

#endif