#ifndef DUNGEONTAB_H
#define DUNGEONTAB_H

#include "ui.h"
#include <vector>
#include <filesystem>
#include <iostream>

class AlgorithmConfig;
class DungeonStat;
class DungeonView;

class DungeonTab : public DocumentWindow{
    void Reload();
    AlgorithmConfig * config = nullptr;
    DungeonStat * stats = nullptr;
    DungeonView * view = nullptr;
protected:
    void Show() override;
public:
    DungeonTab();
    void SetConfig(std::string name,std::string path,std::string out);
    void SetDungeon(std::string path,std::string file);
    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    void Update() override;
    void Shutdown() override;
    std::string GetAlgorithmName();
    std::string GetDungeonName();
    std::string GetName();
};

#endif