#ifndef VIEW_H
#define VIEW_H

#include "ui.h"
#include "rlights.h"
#include "ezdungeon.h"

class DungeonStat;

enum viewMode{ DUNGEON = 0, DENSITY_MAP = 1 , PATH_MAP = 2 , START_MAP = 3, GOAL_MAP = 4};

class DungeonView : public DocumentWindow{

    // Time
    double previousTime = 0;

    // Modes
    int mode = DUNGEON;

    // Screen data
    Vector2 cameraAngle = {0,0};
    Vector3 targetPos = {0,0,0};
    float cameraDistance = 50;
    Vector3 clickWorld = {0,0,0};
    Vector2 mousePos = {0,0};
    int screenWidth = 800;
    int screenHeight = 600;
    Shader lightingShader;

    // Input parameters
    const float playerSpeed = 0.1;
    const float mouseSensitivity = 0.01;
    
    // Model data
    int style = 0;
    bool modelLoaded = false;
    Model dungeonModel;
    std::vector<Model> dungeonModels;
    Vector3 dungeonPos;

    // Animation data
    float densityMapAnim = 0;
    float densityMapAnimSpeed = 4;

    // Dungeon data
    bool isDungeon2d = false;

    DungeonStat * stats = nullptr;
    
    void enterPlayMode();

    void playerModeUpdate(DungeonMatrix * dungeon,Vector3 dungeonPos,Vector3 & playerPos);

    void InteractionModeUpdate(Vector3 & playerPos);
    void InteractionModePasiveUpdate();
protected:
    void Show() override;
public:
    std::string name;
    bool hidden = true;

    bool hasToFocusGoal = false;
    bool hasToFocusStart = false;
    bool hasToRestartView = false;

    bool showPath = true;
    bool showDensityMap = false;

    DungeonView(std::string name,DungeonStat * stats);

    Camera3D camera = { 0 };
    // Texture2D gridTexture = { 0 };
    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    // void ShowAsTabItem() override;
    void Update() override;
    void Shutdown() override;

    void SetCameraDefault();
};

#endif