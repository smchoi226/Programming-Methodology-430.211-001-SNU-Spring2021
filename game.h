#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

const sf::Color ALIVE_COLOR = sf::Color::Black;
const sf::Color OLD_COLOR = sf::Color(100, 100, 100);
const sf::Color RED = sf::Color::Red;
const sf::Color GREEN = sf::Color::Green;
const sf::Color BLUE = sf::Color::Blue;
const sf::Color YELLOW = sf::Color::Yellow;
const sf::Color CYAN = sf::Color::Cyan;
const sf::Color MAGENTA = sf::Color::Magenta;
const sf::Color BLACK = sf::Color::Black;

/*
 * Enum of three possible cell states.
 */
enum class CellState {
    DEAD,               // 0
    ALIVE,              // 1
    OLD,                // 2
    R = 3,              // 3
    G = 4,              // 4
    B = 5,              // 5
    R_AND_G = 12,       // 12
    G_AND_B = 20,       // 20
    B_AND_R = 15,       // 15
    R_AND_G_AND_B = 60  // 60
    // You can add more states if needed for custom cell
};

/*
 * Struct that holds three values of cell's row number, column number, and its corresponding state
 * This is used for representing initial cell configurations as the location and the desired state of a cell.
 */
struct CellCoord {
    int row;
    int col;
    CellState state;
};

/*
 * Class representing a single cell in the grid of Game of Life.
 *
 * It holds information of a cell, including its location in the window, current state, color, future state, and its neighbors.
 * By using the information about its neighbors, the cell class also implements how to decide the next state of a cell.
 * This class also acts as the base cell for variant cells for different game modes.
 */
class Cell {
public:
    Cell(float xVal, float yVal) : x(xVal), y(yVal) {}

    float getX() const { return x; }
    float getY() const { return y; }
    CellState getState() const { return state; }
    sf::Color getColor() const { return color; }
    virtual bool isAlive() const; /* TODO */

    void update() { state = nextState; }
    virtual void setState(CellState newState) { state = newState; }
    virtual void computeNextState(); /* TODO */
    void setNeighbor(const Cell* cell); /* TODO */

protected:
    sf::Color color = ALIVE_COLOR;
    CellState state = CellState::DEAD;
    CellState nextState = CellState::DEAD;
    std::vector<const Cell*> neighbors;
private:
    float x;
    float y;
};

// Return true if cell is not dead
bool Cell::isAlive() const {
    if (state == CellState::ALIVE) return true;
    else return false;
}

// compute next state through its neighbors
void Cell::computeNextState() {
    int live_cell = 0, dead_cell = 0;
    for (const Cell* a_cell : neighbors) {
        if (a_cell->isAlive()) live_cell++;
        else if (!a_cell->isAlive()) dead_cell++;
    }
    if (isAlive() && live_cell < 2) nextState = CellState::DEAD;
    else if (isAlive() && live_cell > 3) nextState = CellState::DEAD;
    else if (!isAlive() && live_cell == 3) nextState = CellState::ALIVE;
    else nextState = getState();
}

// Assign the given cell as a neighbor to itself.
void Cell::setNeighbor(const Cell* cell) {
    neighbors.push_back(cell);
}

/*
 * Class representing an aging cell in the Aging variant of Game of Life.
 */
 /* TODO: AgingCell */
class AgingCell : public Cell {
public:
    AgingCell(float x, float y);
    bool isAlive() const;
    void setState(CellState newState);
    void computeNextState();
private:
    int age;
};

// Constructor of AgingCell
AgingCell::AgingCell(float x, float y) : Cell(x, y), age(0) {}

// return true if ALIVE or OLD
bool AgingCell::isAlive() const{
    if (state == CellState::ALIVE || state == CellState::OLD) return true;
    else return false;
}

// set new State and Color
void AgingCell::setState(CellState newState) {
    state = newState;
    if (state == CellState::ALIVE) color = ALIVE_COLOR;
    if (state == CellState::OLD) color = OLD_COLOR;
}

// compute the next state of cell for AGING mode. All rules are same as BASIC cell, except that OLD state has been added
void AgingCell::computeNextState() {
    int live_cell = 0, dead_cell = 0;
    for (const Cell* a_cell : neighbors) {
        if (a_cell->isAlive()) live_cell++;
        else if (!a_cell->isAlive()) dead_cell++;
    }
    if (isAlive()) age++;
    if (isAlive() && live_cell < 2) nextState = CellState::DEAD;
    else if (isAlive() && live_cell > 3) nextState = CellState::DEAD;
    else if (!isAlive() && live_cell == 3) nextState = CellState::ALIVE;
    else if (getState() == CellState::OLD) nextState = CellState::DEAD;
    else nextState = getState();
    if (nextState == CellState::DEAD) {
        age = 0;
        color = ALIVE_COLOR;
    }
    if (nextState == CellState::ALIVE && age == 3) {
        nextState = CellState::OLD;
        color = OLD_COLOR;
    }
}


 /*
  * Class representing a rule-based cell that follows the given rule variant of Game of Life.
  */
class RuleBasedCell : public Cell {
public:
    RuleBasedCell(float x, float y, std::string rule);
    void computeNextState();
private:
    std::vector<int> b;
    std::vector<int> s;
};

// Constructor of RuleBasedCell
RuleBasedCell::RuleBasedCell(float x, float y, std::string rule) : Cell(x, y){
    int b_start = rule.find("B") + 1;
    int s_start = rule.find("S") + 1;
    int b_end = rule.find("S") - 2;
    int s_end = rule.size() - 1;
    for (int i = b_start; i <= b_end; i++) b.push_back(rule.at(i) - '0');
    for (int i = s_start; i <= s_end; i++) s.push_back(rule.at(i) - '0');
}

// compute the next state of cell for RULE_BASED mode. Only the numbers of cells for remaining alive or becoming alive are different from BASIC mode
void RuleBasedCell::computeNextState() {
    int live_cell = 0, dead_cell = 0;
    for (const Cell* a_cell : neighbors) {
        if (a_cell->isAlive()) live_cell++;
        else if (!a_cell->isAlive()) dead_cell++;
    }
    if (isAlive() && find(s.begin(), s.end(), live_cell) == s.end()) nextState = CellState::DEAD;
    else if (!isAlive() && find(b.begin(), b.end(), live_cell) != b.end()) nextState = CellState::ALIVE;
    else nextState = getState();
}

  /*
   * Class representing a custom cell of your own variant of Game of Life.
   */
class CustomCell : public Cell {
public:
    CustomCell(float x, float y);
    bool isAlive() const;
    void setState(CellState newState);
    bool isProducable() const;
    void computeNextState();
    int lcm(int m, int n);
    std::pair<int, std::vector<CellState>> maxDict(std::map<CellState, int> dict);
private:
    int age;
    std::map<CellState, sf::Color> StateToColor;
};

// Constructor of CustomCell
CustomCell::CustomCell(float x, float y) : Cell(x, y), age(0) {
    StateToColor[CellState::OLD] = OLD_COLOR;
    StateToColor[CellState::R] = RED;
    StateToColor[CellState::G] = GREEN;
    StateToColor[CellState::B] = BLUE;
    StateToColor[CellState::R_AND_G] = YELLOW;
    StateToColor[CellState::G_AND_B] = CYAN;
    StateToColor[CellState::B_AND_R] = MAGENTA;
    StateToColor[CellState::R_AND_G_AND_B] = BLACK;
}

// return true if Cell is either R, G, B, R&G, G&B, B&R, R&G&B, OLD
bool CustomCell::isAlive() const {
    if (getState() == CellState::R || getState() == CellState::G || getState() == CellState::B || getState() == CellState::R_AND_G || getState() == CellState::G_AND_B || getState() == CellState::B_AND_R || getState() == CellState::R_AND_G_AND_B || getState() == CellState::OLD) return true;
    else return false;
}

// set new State and Color for Cell
void CustomCell::setState(CellState newState) {
    state = newState;
    if (state != CellState::DEAD) color = StateToColor[state];
}

// return true if Cell is either R, G, B, R&G, G&B, B&R, R&G&B
bool CustomCell::isProducable() const {
    if (getState() == CellState::R || getState() == CellState::G || getState() == CellState::B || getState() == CellState::R_AND_G || getState() == CellState::G_AND_B || getState() == CellState::B_AND_R || getState() == CellState::R_AND_G_AND_B) return true;
    else return false;
}

// compute the next state of cell for CUSTOM mode. Specific rules are written in the report
void CustomCell::computeNextState() {
    std::map<CellState, int> dict;
    dict.insert(std::make_pair(CellState::R, 0));
    dict.insert(std::make_pair(CellState::G, 0));
    dict.insert(std::make_pair(CellState::B, 0));
    dict.insert(std::make_pair(CellState::R_AND_G, 0));
    dict.insert(std::make_pair(CellState::G_AND_B, 0));
    dict.insert(std::make_pair(CellState::B_AND_R, 0));
    dict.insert(std::make_pair(CellState::R_AND_G_AND_B, 0));
    for (const Cell* a_cell : neighbors) {
        if (a_cell->isAlive() && a_cell->getState() != CellState::OLD) dict[a_cell->getState()]++;
    }
    int max_amount = maxDict(dict).first;
    std::vector<CellState> maxStates = maxDict(dict).second;
    if (isAlive()) {
        if (isProducable()) {
            age++;
            if (max_amount <= 1) nextState = CellState::DEAD; 
            else if (max_amount > 1 && max_amount <= 4){
                if (find(maxStates.begin(), maxStates.end(), getState()) != maxStates.end()) nextState = getState();
                else nextState = CellState::DEAD;
            }
            else if (max_amount > 4) nextState = CellState::DEAD;
            if (nextState != CellState::DEAD) {
                if (age == 3) nextState = CellState::OLD;
                color = StateToColor[nextState];
            }
        }
        else nextState = CellState::DEAD;
    }
    else {
        // new cell becomes born only when there are more than 3 same state of cells
        if (max_amount == 2 || max_amount == 3) {
            if (maxStates.size() == 1) nextState = maxStates[0];
            if (maxStates.size() == 2) nextState = static_cast<CellState>(lcm(static_cast<int>(maxStates[0]), static_cast<int>(maxStates[1])));
            color = StateToColor[nextState];
        }
        else nextState = getState();
    }
    if (nextState == CellState::DEAD) age = 0;
}

// return a pair that has information about CellStates that has the max amount in dictionary
std::pair<int, std::vector<CellState>> CustomCell::maxDict(std::map<CellState, int> dict) {
    int max_amount = 0;
    std::vector<CellState> states;
    for (auto it = dict.begin(); it != dict.end(); it++) if (it->second > max_amount) max_amount = it->second;
    for (auto it = dict.begin(); it != dict.end(); it++) if (it->second == max_amount) states.push_back(it->first);
    return std::make_pair(max_amount, states);
}

// calculate least common multiple for 2 integers
int CustomCell::lcm(int m, int n) {
    int gcd, a = m, b = n;
    while (b != 0) {
        if (a < b) {
            int temp = a;
            a = b;
            b = temp;
        }
        if (b != 0)a %= b;
    }
    gcd = a;
    return m * n / gcd;
}

   /*
* Enum of four available game modes.
*/
enum class GameMode {
    BASIC = 1,
    AGING,
    RULE_BASED,
    CUSTOM
};

/*
 * Struct containing various configuration values for the game ranging from program window sizes to game mode.
 *
 * You should keep the default given configuration values for basic/aging/rule_based game modes.
 * You are allowed to use custom configuration for your custom game mode.
 * The grid width/height is computed using window width/height - 2 * marginSize
 */
struct GameConfig {
    int windowWidth = 600;
    int windowHeight = 600;
    float marginSize = 60;
    int stepSpeedInMilliseconds = 80;
    sf::Color backgroundColor = sf::Color::White;
    sf::Color fontColor = sf::Color::Black;
    std::string fontPath = "Arial.ttf";
    int fontSize = 13;
    std::string windowTitle = "Conway's Game of Life";
    int numRows = 10;
    int numCols = 10;
    GameMode gameMode = GameMode::BASIC;
    std::string gameRule = "";
    sf::Color gridLineColor = sf::Color(200, 200, 200);
    float gridLineThickness = 1.0;
};

/*
 * Class that holds and manages all the cells in the grid of Game of Life.
 *
 * Its main use is to apply batch operations on cells such as initializing, resetting, drawing and updating.
 */
class Grid {
public:
    Grid(const GameConfig& cfg); /* TODO */
    ~Grid(); /* TODO */
    void initializeCells(const std::vector<CellCoord>& coords); /* TODO */
    void updateCells(); /* TODO */
    void resetCells(); /* TODO */
    const Cell* getCell(int rowIdx, int colIdx) const { return cells.at(rowIdx).at(colIdx); }

    // Draw the cells on given window
    void drawOn(sf::RenderWindow& window) const;
private:
    GameConfig config;
    std::vector<std::vector<Cell*>> cells;
};

// Helper functions to compute grid-related dimensions from game configuration
float getGridWidth(const GameConfig& config);
float getGridHeight(const GameConfig& config);
float getCellWidth(const GameConfig& config);
float getCellHeight(const GameConfig& config);

// Construct a Grid class and dynamically allocate cells.
Grid::Grid(const GameConfig& cfg): config(cfg) {
    std::vector<std::vector<std::pair<int, int>*>> neighbors;
    for (int i = 0; i < cfg.numRows; i++) {
        std::vector<Cell*> col;
        std::vector<std::pair<int, int>*> col_idx;
        cells.push_back(col);
        neighbors.push_back(col_idx);
        for (int j = 0; j < cfg.numCols; j++) {
            float x = config.marginSize + j * getCellWidth(config);
            float y = config.marginSize + i * getCellHeight(config);
            Cell* c;
            if (config.gameMode == GameMode::BASIC) c = new Cell(x, y);
            else if (config.gameMode == GameMode::AGING) c = new AgingCell(x, y);
            else if (config.gameMode == GameMode::RULE_BASED) c = new RuleBasedCell(x, y, config.gameRule);
            else if (config.gameMode == GameMode::CUSTOM) c = new CustomCell(x, y);
            cells.at(i).push_back(c);
            std::pair<int,int>* neighbor_index = new std::pair<int,int>[8];
            int left = j - 1;
            int right = j + 1;
            int up = i - 1;
            int down = i + 1;
            if (left == -1) left = cfg.numCols - 1;
            if (right == cfg.numCols) right = 0;
            if (up == -1) up = cfg.numRows - 1;
            if (down == cfg.numRows) down = 0;
            neighbor_index[0] = std::make_pair(up, left);
            neighbor_index[1] = std::make_pair(i, left);
            neighbor_index[2] = std::make_pair(down, left);
            neighbor_index[3] = std::make_pair(up, j);
            neighbor_index[4] = std::make_pair(down, j);
            neighbor_index[5] = std::make_pair(up, right);
            neighbor_index[6] = std::make_pair(i, right);
            neighbor_index[7] = std::make_pair(down, right);
            neighbors[i].push_back(neighbor_index);
        }
    }
    for (int i = 0; i < cfg.numRows; i++) {
        for (int j = 0; j < cfg.numCols; j++) {
            for (int k = 0; k < 8; k++) {
                int y = neighbors.at(i).at(j)[k].first;
                int x = neighbors.at(i).at(j)[k].second;
                cells.at(i).at(j)->setNeighbor(cells.at(y).at(x));
            }
        }
    }
    for (int i = 0; i < cfg.numRows; i++) {
        for (int j = 0; j < cfg.numCols; j++) {
            delete[] neighbors.at(i).at(j);
            neighbors.at(i).at(j) = NULL;
        }
    }
}

// Destruct all the dynamically allocated cells
Grid::~Grid() {
    for (std::vector<Cell*> a_row : cells) {
        for (Cell* a_cell : a_row) {
            delete a_cell;
            a_cell = NULL;
        }
    }
}

// Initialize starting cell states using given initial cell configuration
void Grid::initializeCells(const std::vector<CellCoord>& coords) {
    for (CellCoord a_cell : coords) cells.at(a_cell.row).at(a_cell.col)->setState(a_cell.state);
    
}

// Compute the future state after a single step, and update into the computed future state for all cells.
void Grid::updateCells() {
    for (std::vector<Cell*> a_row : cells) for (Cell* a_cell : a_row) a_cell->computeNextState();
    for (std::vector<Cell*> a_row : cells) for (Cell* a_cell : a_row) a_cell->update();
}

// Reset the state to dead state on all cells
void Grid::resetCells() {
    for (std::vector<Cell*> a_row : cells) {
        for (Cell* a_cell : a_row) a_cell->setState(CellState::DEAD);
    }
}

float getGridWidth(const GameConfig& config) {
    return static_cast<float>(config.windowWidth) - 2 * config.marginSize;
}

float getGridHeight(const GameConfig& config) {
    return static_cast<float>(config.windowHeight) - 2 * config.marginSize;
}

float getCellWidth(const GameConfig& config) {
    return getGridWidth(config) / static_cast<float>(config.numCols);
}

float getCellHeight(const GameConfig& config) {
    return getGridHeight(config) / static_cast<float>(config.numRows);
}

void Grid::drawOn(sf::RenderWindow& window) const {
    // Draw cell if alive
    if (!cells.empty()) {
        for (int i = 0; i < config.numRows; i++) {
            for (int j = 0; j < config.numCols; j++) {
                if (cells[i][j]->isAlive()) {
                    sf::RectangleShape rect(sf::Vector2f(getCellWidth(config), getCellHeight(config)));
                    rect.setPosition(sf::Vector2f(cells[i][j]->getX(), cells[i][j]->getY()));
                    rect.setFillColor(cells[i][j]->getColor());
                    window.draw(rect);
                }
            }
        }
    }

    // Draw grid lines
    for (int i = 0; i <= config.numRows; i++) {  // horizontal lines
        sf::RectangleShape line(sf::Vector2f(getGridWidth(config), config.gridLineThickness));
        line.setPosition(sf::Vector2f(config.marginSize, config.marginSize + static_cast<float>(i) * getCellHeight(config)));
        line.setFillColor(config.gridLineColor);
        window.draw(line);
    }
    for (int i = 0; i <= config.numCols; i++) {  // vertical lines
        sf::RectangleShape line(sf::Vector2f(config.gridLineThickness, getGridHeight(config)));
        line.setPosition(sf::Vector2f(config.marginSize + static_cast<float>(i) * getCellWidth(config), config.marginSize));
        line.setFillColor(config.gridLineColor);
        window.draw(line);
    }
}

/*
 * Enum of two possible game states.
 */
enum class GameState {
    PLAYING,
    PAUSED
};

/*
 * Class that manages the entire game. Holds the grid object and the main rendering loop of the program.
 */
class GameManager {
public:
    // Construct a GameManager class with given configuration
    GameManager(const GameConfig& cfg);

    // Save the given initial cell coordinates in order to set and reset the grid during the game.
    void setInitialCoords(const std::vector<CellCoord>& coords) { initialCoords = coords; }

    // Main rendering loop that checks for keyboard events, updates program graphics, and calls grid updates.
    void run();
private:
    GameConfig config;
    sf::Font textFont;
    std::vector<CellCoord> initialCoords;
    sf::RenderWindow window;
    Grid grid;
    GameState state = GameState::PAUSED;
    int num_steps = 0;

    // Internal helper function for drawing information text in the window
    void drawInterface();
};

GameManager::GameManager(const GameConfig& cfg) : grid(cfg), config(cfg) {
    // load font file
    textFont.loadFromFile(config.fontPath);
    std::cout << "GameManager Initialized!" << std::endl;
}

void GameManager::run() {
    std::cout << "Starting game..." << std::endl;
    // Create program window and initialize grid cells
    window.create(sf::VideoMode(config.windowWidth, config.windowHeight), config.windowTitle);
    grid.initializeCells(initialCoords);
    window.clear(config.backgroundColor);
    
    // Run main program rendering/update loop
    sf::Clock clock;
    while (window.isOpen()) {
        // Check for program events (close window, keyboard press)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    // toggle play/pause program when Space key is pressed
                case sf::Keyboard::Space:
                    state = state == GameState::PLAYING ? GameState::PAUSED : GameState::PLAYING;
                    clock.restart();
                    break;
                    // reset program when R key is pressed
                case sf::Keyboard::R:
                    state = GameState::PAUSED;
                    grid.resetCells();
                    grid.initializeCells(initialCoords);
                    num_steps = 0;
                    clock.restart();
                    break;
                    // apply single grid update and pause when N key is pressed
                case sf::Keyboard::N:
                    state = GameState::PAUSED;
                    num_steps++;
                    grid.updateCells();
                    break;
                }
            }
        }

        if (state == GameState::PLAYING) {
            // update grid cells after every fixed amount of time is elapsed
            if (clock.getElapsedTime().asMilliseconds() >= config.stepSpeedInMilliseconds) {
                num_steps++;
                grid.updateCells();
                clock.restart();
            }
        }

        // redraw interface and grid
        window.clear(config.backgroundColor);
        drawInterface();
        grid.drawOn(window);
        window.display();
    }
    
}

void GameManager::drawInterface() {
    float topY = config.marginSize / 2 - 5;
    float bottomY = static_cast<float>(config.windowWidth) - config.marginSize / 2;
    float centerX = static_cast<float>(config.windowWidth) / 2;

    // create text objects

    // top
    sf::Text topText("Press space to play/pause, R to reset, N to update once", textFont, config.fontSize);
    topText.setFillColor(config.fontColor);
    auto topBounds = topText.getLocalBounds();
    topText.setPosition(centerX - topBounds.width / 2, topY - topBounds.height / 2);

    // bottom
    sf::Text bottomText((state == GameState::PLAYING ? "PLAYING" : "PAUSED"), textFont, config.fontSize);
    bottomText.setFillColor(config.fontColor);
    auto bottomBounds = bottomText.getLocalBounds();
    bottomText.setPosition(centerX - bottomBounds.width / 2, bottomY - bottomBounds.height / 2);

    // bottom-left
    sf::Text leftBottomText("t=" + std::to_string(num_steps), textFont, config.fontSize);
    leftBottomText.setFillColor(config.fontColor);
    auto lbBounds = leftBottomText.getLocalBounds();
    leftBottomText.setPosition(config.marginSize, bottomY - lbBounds.height / 2);

    // right-bottom
    std::string gameModeName;
    switch (config.gameMode) {
    case GameMode::BASIC:
        gameModeName = "BASIC";
        break;
    case GameMode::AGING:
        gameModeName = "AGING";
        break;
    case GameMode::RULE_BASED:
        gameModeName = "RULE(" + config.gameRule + ")";
        break;
    case GameMode::CUSTOM:
        gameModeName = "CUSTOM";
        break;
    }
    sf::Text rightBottomText("MODE: " + gameModeName, textFont, config.fontSize);
    auto rbBounds = rightBottomText.getLocalBounds();
    rightBottomText.setFillColor(sf::Color::Black);
    rightBottomText.setPosition(static_cast<float>(config.windowWidth) - config.marginSize - rbBounds.width, bottomY - rbBounds.height / 2);

    // draw created text objects
    window.draw(topText);
    window.draw(bottomText);
    window.draw(leftBottomText);
    window.draw(rightBottomText);
}

#endif
