#ifndef ALGORITHMLIST_H
#define ALGORITHMLIST_H

#include "ui.h"
#include <vector>
#include <filesystem>
#include <iostream>
class DungeonTab;
class AlgorithmConfig;

class AlgorithmList : public DocumentWindow{
    std::string dir = "./algorithm";
    std::string outdir = "./temp";
    std::vector <std::string> algorithms;
    std::string selected;

    bool hasToUpdate = false;

    void Reload();
protected:
    void Show() override;
public:
    void Setup() override;
    void ShowAsWindow() override;
    void ShowAsChild() override;
    // void ShowAsTabItem() override;
    void Update() override;
    void Shutdown() override;

    void UpdateAlgorithms(std::vector<AlgorithmConfig *> *);
    void UpdateDungeonTabs(std::vector<DungeonTab *> *);
};

#endif