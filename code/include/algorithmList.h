#ifndef ALGORITHMLIST_H
#define ALGORITHMLIST_H

#include "ui.h"
#include <vector>
#include <filesystem>
#include <iostream>
#include "algorithmConfig.h"

class AlgorithmList : public DocumentWindow{
    std::string dir = "./algorithm";
    std::string outdir = "./temp";
    std::vector <std::string> algorithms;
    std::string selected;

    bool hasToUpdate = false;

    void Reload();
public:
    void Setup() override;
    void Show() override;
    void Update() override;
    void Shutdown() override;

    void UpdateAlgorithms(std::vector<AlgorithmConfig *> *);
};

#endif