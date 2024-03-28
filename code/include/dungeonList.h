#ifndef DUNLIST_H
#define DUNLIST_H

#include "ui.h"
#include <vector>
#include <filesystem>
#include <iostream>
#include "dungeonStat.h"

class DungeonList : public DocumentWindow{
    std::string dir = "./dungeon";
    std::string outdir = "./temp";
    std::vector <std::string> dungeons;
    std::string selected;

    bool hasToUpdate = false;

    void Reload();
protected:
    void Show() override;
public:
    void Setup() override;
    void ShowAsWindow() override;
    void Update() override;
    void Shutdown() override;

    void UpdateDungeons(std::vector<DungeonStat *> *);
};

#endif