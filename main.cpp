/*
 * 2021 Programming Methodology Project 3
 */
#include <iostream>
#include <fstream>
#include "Game.h"


void readConfigFile(const std::string& fileName, GameConfig& config, std::vector<CellCoord>& initialCoords) {
    std::ifstream infile(fileName);

    if (infile.is_open()) {
        infile >> config.numRows >> config.numCols;
        std::cout << "Grid size:                    " << config.numRows << "," << config.numCols << std::endl;

        std::string mode;
        infile >> mode;
        if (mode == "BASIC")
            config.gameMode = GameMode::BASIC;
        else if (mode == "AGING")
            config.gameMode = GameMode::AGING;
        else if (mode == "RULE_BASED")
            config.gameMode = GameMode::RULE_BASED;
        else if (mode == "CUSTOM")
            config.gameMode = GameMode::CUSTOM;
        else {
            std::cout << "Unknown game mode: " << mode << std::endl;
            exit(1);
        }
        std::cout << "Game mode:                    " << mode << std::endl;

        if (config.gameMode == GameMode::RULE_BASED) {
            infile >> config.gameRule;
            if (!(config.gameRule[0] == 'B' && config.gameRule.find("/S") != std::string::npos)) {
                std::cout << "Expected game rule in format 'BXX/SYYY', found: " << config.gameRule << std::endl;
                exit(1);
            }
        }
        else if (config.gameMode == GameMode::CUSTOM) {
            // TODO: Tweak game config for custom mode (if you want to)
        }

        int row, col, stateNum;
        while (infile >> row >> col >> stateNum) {
            CellCoord c = { row - 1, col - 1, static_cast<CellState>(stateNum) };
            initialCoords.push_back(c);
        }
        std::cout << "Number of initialized cells:  " << initialCoords.size() << std::endl;
    }
    else {
        std::cout << "Error opening file!" << std::endl;
        exit(1);
    }
    infile.close();
}


int main() {
    // Read configuration file ---------------------------------------------------
    std::cout << "Enter configuration file name: " << std::endl << ">> ";
    std::string fileName;
    std::cin >> fileName;

    GameConfig config;
    std::vector<CellCoord> initialCoords;
    readConfigFile(fileName, config, initialCoords);

    // Start Game ----------------------------------------------------------------
    GameManager gm(config);
    gm.setInitialCoords(initialCoords);
    gm.run();

    return 0;
}
