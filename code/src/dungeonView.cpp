#include <iostream>
#define RLIGHTS_IMPLEMENTATION
#include "dungeonView.h"
#include "dungeonStat.h"
#include "model.h"

inline const char* to_string(viewMode mode){
    switch(mode){
    case DUNGEON:
        return "Dungeon";
    case DENSITY_MAP:
        return "Density Map";
    case PATH_MAP:
        return "Path Map";
    default:
        return "Unknown";
    }
}


DungeonView::DungeonView(std::string name,DungeonStat * stats){
    this->name = name;
    this->stats = stats;
}

void DungeonView::Setup() 
{
    viewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    // Lights
    lightingShader = LoadShader(
        TextFormat("resources/shaders/glsl%i/lighting.vs", GLSL_VERSION),
        TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");

    int ambientLoc = GetShaderLocation(lightingShader, "ambient");
    float f[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    SetShaderValue(lightingShader, ambientLoc,f, SHADER_UNIFORM_VEC4);

    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ 5, 7, 10 }, {0,0,0}, WHITE, lightingShader);
    lights->enabled = true;

    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void DungeonView::ShowAsWindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    if (ImGui::Begin(std::string(name + " 3D View").c_str(), &open, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoBackground)){
        Show();
    }
    ImGui::End();
    
    ImGui::PopStyleVar();
}

void DungeonView::ShowAsChild()
{        
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // custom button spacing
    ImGui::Checkbox("Hide view",&hidden);
    if(!hidden){
        if(ImGui::Button("Restart view")) hasToRestartView = true;
        ImGui::SameLine();
        if(ImGui::Button("Focus start")) hasToFocusStart = true;
        ImGui::SameLine();
        if(ImGui::Button("Focus goal")) hasToFocusGoal = true;

        const char* viewModes[] = {"Dungeon","Density map","Path map","Start map","Goal map"};
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::Combo("View mode",&mode,viewModes,IM_ARRAYSIZE(viewModes));
        ImGui::PopItemWidth();
        ImGui::BeginDisabled(stats==nullptr||!stats->IsPathGenerated()||mode!=DUNGEON);
        ImGui::SameLine();
        ImGui::Checkbox("Show path",&showPath);
        ImGui::EndDisabled();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0)); // force l¡lower line on the view
        if (ImGui::BeginChild(std::string(name + " 3D View").c_str(),ImVec2(0,-(float)GetScreenHeight()/2), ImGuiChildFlags_ResizeY,ImGuiWindowFlags_None)){
            Show();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    
    ImGui::PopStyleVar();
}

void DungeonView::Show(){
    focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

    ImVec2 content = ImGui::GetContentRegionAvail();
    ImVec2 mouse = ImGui::GetMousePos();
    ImVec2 windowP = ImGui::GetWindowPos();
    ImVec2 windowS = ImGui::GetWindowSize();
    mousePos.x = mouse.x - windowP.x - windowS.x + content.x;
    mousePos.y = mouse.y - windowP.y - windowS.y + content.y;
    
    int oldScreenWidth = screenWidth;
    int oldScreenHeight = screenHeight;
    // screenWidth = ImGui::GetWindowWidth();
    // screenHeight = ImGui::GetWindowHeight();
    screenWidth = content.x;
    screenHeight = content.y;
    
    resized = oldScreenWidth != screenWidth || oldScreenHeight != screenHeight;

    if(hidden) return;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(windowP,ImVec2(windowP.x+windowS.x,windowP.y+windowS.y),IM_COL32_BLACK); //Hides the artifacting generated when resizing the texture
    // draw the view
    if(IsRenderTextureReady(viewTexture)) rlImGuiImageRenderTextureFit(&viewTexture, true);
}

void DungeonView::Update(){
    double currentTime = GetTime();
    double deltaTime = currentTime - previousTime;
    previousTime = currentTime;
    if (!open||hidden)
        return;

    if (resized){
        UnloadRenderTexture(viewTexture);
        viewTexture = LoadRenderTexture(screenWidth, screenHeight);
    }

    if(focused){
        InteractionModeUpdate(targetPos);
    }
    InteractionModePasiveUpdate();

    //Generate model
    if(stats!=nullptr&&stats->hasToUpdateView){
        stats->hasToUpdateView = false;
        if(stats->currentDungeon!=nullptr){
            if(modelLoaded) {
                // UnloadModel(dungeonModel);
                for(auto model : dungeonModels) UnloadModel(model);
            }
            dungeonModels = modelVectorFromDungeon(stats->currentDungeon,{8,8,8});
            // dungeonModel = LoadModelFromMesh(meshFromDungeon(stats->currentDungeon));

            dungeonPos.x = - (float)stats->currentDungeon->GetSize().x/2;
            // dungeonPos.y = - (float)currentDungeon->size_y/2;
            dungeonPos.y = 0;
            dungeonPos.z = - (float)stats->currentDungeon->GetSize().z/2;
            for(auto model : dungeonModels) model.materials[0].shader = lightingShader;
            // dungeonModel.materials[0].shader = lightingShader; //!

            // Setting camera
            SetCameraDefault();

            modelLoaded = true;
        }
    }

    // Render update
    float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    // Update camera position and rotation
    camera.target = targetPos;
    camera.position = targetPos;
    camera.position.x += cos(cameraAngle.x) * cos(cameraAngle.y) * cameraDistance;
    camera.position.z += sin(cameraAngle.x) * cos(cameraAngle.y) * cameraDistance;
    camera.position.y = sin(cameraAngle.y) * cameraDistance;

    BeginTextureMode(viewTexture);
    ClearBackground(BLACK);
    BeginMode3D(camera);

    if(modelLoaded&&mode == DUNGEON)
    {
        Vector3 offset = Vector3Sum(dungeonPos,{0.5f,0.5f,0.5f});
        // DrawModel(dungeonModel,dungeonPos,1,GRAY);
        bool alt = false;
        int index = 0;
        for(auto model : dungeonModels){
            alt = !alt;
            DrawModel(model,dungeonPos,1,alt ? GRAY : DARKGRAY);
            index++;
        }

        DrawCube(Vector3Sum(stats->GetStart(),offset),0.5f,0.5f,0.5f,GREEN);
        DrawCube(Vector3Sum(stats->GetEnd(),offset),0.5f,0.5f,0.5f,RED);

        float pathSphereSize = cameraDistance/100 > 0.4 ? 0.4 : cameraDistance/100;
        stats->DrawPath(offset,ORANGE,pathSphereSize,false);
    }

    if(mode==DENSITY_MAP){
        densityMapAnim = densityMapAnim >= 1 ? 1 : densityMapAnim + deltaTime * densityMapAnimSpeed;
        stats->DrawDensityMap(dungeonPos,{1,densityMapAnim,1},BLUE);
    }else if(mode==PATH_MAP){
        densityMapAnim = densityMapAnim >= 1 ? 1 : densityMapAnim + deltaTime * densityMapAnimSpeed;
        stats->DrawPathDensityMap(dungeonPos,{1,densityMapAnim,1},ORANGE);
    }else if(mode==START_MAP){
        densityMapAnim = densityMapAnim >= 1 ? 1 : densityMapAnim + deltaTime * densityMapAnimSpeed;
        stats->DrawStartDensityMap(dungeonPos,{1,densityMapAnim,1},GREEN);
    }else if(mode==GOAL_MAP){
        densityMapAnim = densityMapAnim >= 1 ? 1 : densityMapAnim + deltaTime * densityMapAnimSpeed;
        stats->DrawGoalDensityMap(dungeonPos,{1,densityMapAnim,1},RED);
    }else{
        densityMapAnim = 0;
    }

    Ray ray = GetViewRay(mousePos,camera,screenWidth,screenHeight); //Raylib 5.1
    RayCollision coll = GetRayCollisionQuad(ray,{-1000,0,-1000},{-1000,0,1000},{1000,0,1000},{1000,0,-1000});
    clickWorld = coll.point;

    DrawSphereWires(ray.position,1000,10,10,{ 55, 41, 100, 255 });

    EndMode3D();

    EndTextureMode();

}

void DungeonView::Shutdown()
{
    if(IsModelReady(dungeonModel)) {
        // UnloadModel(dungeonModel);
        for(auto model : dungeonModels) UnloadModel(model);
    }
    UnloadRenderTexture(viewTexture);
    UnloadShader(lightingShader);
}

void DungeonView::InteractionModePasiveUpdate(){
    if(hasToFocusStart){
        hasToFocusStart = false;
        targetPos = Vector3Sum(dungeonPos,stats->GetStart());
        cameraDistance = 15;
    }
    if(hasToFocusGoal){
        hasToFocusGoal = false;
        targetPos = Vector3Sum(dungeonPos,stats->GetEnd());
        cameraDistance = 15;
    }
    if(hasToRestartView){
        hasToRestartView = false;
        SetCameraDefault();
    }
}

void DungeonView::InteractionModeUpdate(Vector3 & playerPos){
    int playerMatrixPos[3] = {0,0,0};
    Vector3 cameraRotation = {0,0,0};
    Vector3 movement = {0,0,0};
    Vector2 localMovement = {0,0};

    cameraDistance += GetMouseWheelMove();
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Ray ray = GetViewRay(mousePos,camera,screenWidth,screenHeight);
        RayCollision coll = GetRayCollisionQuad(ray,{-1000,0,-1000},{-1000,0,1000},{1000,0,1000},{1000,0,-1000});
        clickWorld = Vector3Subtract(playerPos,coll.point); 
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        Ray ray = GetViewRay(mousePos,camera,screenWidth,screenHeight);
        RayCollision coll = GetRayCollisionQuad(ray,{-1000,0,-1000},{-1000,0,1000},{1000,0,1000},{1000,0,-1000});
        playerPos = Vector3Sum(playerPos,Vector3Subtract(clickWorld,coll.point));
    } else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){            
        cameraAngle = {cameraAngle.x+GetMouseDelta().x*mouseSensitivity,cameraAngle.y+GetMouseDelta().y*mouseSensitivity};
        
        cameraAngle.y = cameraAngle.y > 1.5 ? 1.5 : cameraAngle.y;
        cameraAngle.y = cameraAngle.y < -1.5 ? -1.5 : cameraAngle.y;
    }

    if(IsKeyPressed(KEY_F)){
        hasToFocusStart = false;
        targetPos = Vector3Sum(dungeonPos,stats->GetStart());
        cameraDistance = 15;
    }
}

void DungeonView::SetCameraDefault(){
    if(stats!=nullptr&&stats->currentDungeon!=nullptr)
        cameraDistance = stats->currentDungeon->GetSize().x+stats->currentDungeon->GetSize().z;
    else
        cameraDistance = 10;
    cameraAngle = {DEG2RAD * 45,DEG2RAD * 45};
    targetPos = {0,0,0};
}