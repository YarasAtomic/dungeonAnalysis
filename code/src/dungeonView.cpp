#include <iostream>
#define RLIGHTS_IMPLEMENTATION
#include "dungeonView.h"
#include "dungeonStat.h"
#include "model.h"

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
    if (ImGui::BeginChild(std::string(name + " 3D View").c_str(),ImVec2(0,-(float)GetScreenHeight()/2), ImGuiChildFlags_ResizeY,ImGuiWindowFlags_None)){
        Show();
    }
    ImGui::EndChild();
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

    // draw the view
    if(IsRenderTextureReady(viewTexture)) rlImGuiImageRenderTextureFit(&viewTexture, true);
}

void DungeonView::Update()
{
    if (!open)
        return;

    if (resized)
    {
        UnloadRenderTexture(viewTexture);
        viewTexture = LoadRenderTexture(screenWidth, screenHeight);
    }

    if(focused){
        switch(mode){
        case player:
            // playerModeUpdate(currentDungeon,dungeonPos,cameraAngle,targetPos);
            break;
        case menu:
            // menuModeButtons.update(screenWidth,screenHeight);
            InteractionModeUpdate(targetPos);
            break;
        }
    }


    //Generate model
    if(stats!=nullptr&&stats->hasToUpdateView){
        stats->hasToUpdateView = false;

        if(modelLoaded) {
            // UnloadModel(dungeonModel);
            for(auto model : dungeonModels) UnloadModel(model);
        }
        dungeonModels = modelVectorFromDungeon(stats->currentDungeon,{8,8,8});
        // dungeonModel = LoadModelFromMesh(meshFromDungeon(stats->currentDungeon));

        dungeonPos.x = - (float)stats->currentDungeon->size_x/2;
        // dungeonPos.y = - (float)currentDungeon->size_y/2;
        dungeonPos.y = 0;
        dungeonPos.z = - (float)stats->currentDungeon->size_z/2;
        for(auto model : dungeonModels) model.materials[0].shader = lightingShader;
        // dungeonModel.materials[0].shader = lightingShader; //!

        // Setting camera
        cameraDistance = stats->currentDungeon->size_x+stats->currentDungeon->size_z;
        cameraAngle = {DEG2RAD * 45,DEG2RAD * 45};
        targetPos = {0,0,0};

        modelLoaded = true;

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

    if(modelLoaded/*&&IsModelReady(dungeonModel)*/)
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
        
        // DrawLine3D({0,0,0},{0,0,10},WHITE);
        // DrawLine3D({0,0,0},{0,10,0},WHITE);
        // DrawLine3D({0,0,0},{10,0,0},WHITE);

        Ray ray = GetViewRay(mousePos,camera,screenWidth,screenHeight); //Raylib 5.1

        // RayCollision coll = GetRayCollisionQuad(ray,{-INFINITY,0,-INFINITY},{-INFINITY,0,INFINITY},{INFINITY,0,INFINITY},{INFINITY,0,-INFINITY});
        RayCollision coll = GetRayCollisionQuad(ray,{-1000,0,-1000},{-1000,0,1000},{1000,0,1000},{1000,0,-1000});
        clickWorld = coll.point;
        // DrawSphereWires(ray.position,1000,10,10,{ 100, 41, 55, 255 });
        DrawSphereWires(ray.position,1000,10,10,{ 55, 41, 100, 255 });
    }

    EndMode3D();


    if(mode == player){
        DrawText("Press escape to exit Player mode",10,10,20,WHITE);
    }else{
        DrawText("Interact mode",10,10,15,SKYBLUE);
        // menuModeButtons.draw(screenWidth,screenHeight);
    }

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


void DungeonView::playerModeUpdate(dungeonMatrix * dungeon,Vector3 dungeonPos,Vector3 & playerPos){
    int playerMatrixPos[3] = {0,0,0};
    Vector3 cameraRotation = {0,0,0};
    Vector3 movement = {0,0,0};
    Vector2 localMovement = {0,0};
    
    if(IsKeyDown(KEY_W))
    {
        localMovement.y++;
    }

    if(IsKeyDown(KEY_S))
    {
        localMovement.y--;
    }

    if(IsKeyDown(KEY_A))
    {
        localMovement.x--;
    }

    if(IsKeyDown(KEY_D))
    {
        localMovement.x++;
    }

    if(IsKeyDown(KEY_ESCAPE))
    {
        mode = menu;
        ShowCursor();
        return;
    }

    HideCursor();

    cameraAngle = {cameraAngle.x-GetMouseDelta().x*mouseSensitivity,cameraAngle.y-GetMouseDelta().y*mouseSensitivity};
    
    cameraAngle.y = cameraAngle.y > 1.5 ? 1.5 : cameraAngle.y;
    cameraAngle.y = cameraAngle.y < -1.5 ? -1.5 : cameraAngle.y;

    playerMatrixPos[0] = playerPos.x - dungeonPos.x;
    playerMatrixPos[1] = playerPos.y - dungeonPos.y;
    playerMatrixPos[2] = playerPos.z - dungeonPos.z;

    if(
        (playerMatrixPos[0] <0)||(playerMatrixPos[1] <0)||(playerMatrixPos[2] <0)||
        (playerMatrixPos[0] >= dungeon->size_x)||
        (playerMatrixPos[1] >= dungeon->size_y)||
        (playerMatrixPos[2] >= dungeon->size_z)||
        ((dungeon->data[playerMatrixPos[0]][playerMatrixPos[1]][playerMatrixPos[2]] & DUN_PXZ_WALL) == 0))
    {

    }

    movement = {0,0,0};
    if(localMovement.y!=0)
    {
        movement = {localMovement.y*cos(cameraAngle.x),0,localMovement.y*sin(cameraAngle.x)};
    }
    if(localMovement.x!=0)
    {
        movement = {movement.x+localMovement.x*cos(cameraAngle.x+PI/2),0,movement.z+localMovement.x*sin(cameraAngle.x+PI/2)};
    }

    movement = {-movement.x*playerSpeed,-movement.y*playerSpeed,-movement.z*playerSpeed};

    playerPos = Vector3Sum(playerPos,movement);
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
        targetPos = Vector3Sum(dungeonPos,stats->GetStart());
        cameraDistance = 15;
    }
}

void DungeonView::enterPlayMode(){
    mode = player; 
}