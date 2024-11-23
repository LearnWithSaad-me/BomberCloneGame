#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <conio.h>

using namespace std;

const int ROWS = 10;
const int COLS = 10;
const int MAX_ENEMIES = 5;

char grid[ROWS][COLS];
int playerX = 0, playerY = 0;
int enemies[MAX_ENEMIES][2]; // Stores enemy positions
int enemyCount = 0;
int score = 0;

// Function prototypes
void initializeGrid();
void printGrid();
void movePlayer(char direction);
void placeBomb();
void enemyMovement();
void handleExplosion(int bombX, int bombY);
bool isGameOver();
void saveHighScore();

int main() {
    srand(time(0));

    initializeGrid();
    printGrid();

    while (!isGameOver()) {
        if (_kbhit()) {
            char input = _getch();
            if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
                movePlayer(input);
            } else if (input == ' ') {
                placeBomb();
            }
        }

        enemyMovement();
        printGrid();
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    cout << "Game Over! Your Score: " << score << endl;
    saveHighScore();
    return 0;
}

void initializeGrid() {
    // Initialize grid with empty spaces
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            grid[i][j] = ' ';
        }
    }

    // Place the player
    grid[playerX][playerY] = 'P';

    // Add indestructible walls
    for (int i = 0; i < ROWS; ++i) {
        grid[i][0] = grid[i][COLS - 1] = '#';
    }
    for (int j = 0; j < COLS; ++j) {
        grid[0][j] = grid[ROWS - 1][j] = '#';
    }

    // Place destructible obstacles
    for (int i = 0; i < 15; ++i) {
        int x = rand() % ROWS;
        int y = rand() % COLS;
        if (grid[x][y] == ' ') {
            grid[x][y] = 'O';
        }
    }

    // Place enemies
    while (enemyCount < MAX_ENEMIES) {
        int x = rand() % ROWS;
        int y = rand() % COLS;
        if (grid[x][y] == ' ') {
            grid[x][y] = 'E';
            enemies[enemyCount][0] = x;
            enemies[enemyCount][1] = y;
            ++enemyCount;
        }
    }
}

void printGrid() {
    system("CLS");
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}

void movePlayer(char direction) {
    int newX = playerX;
    int newY = playerY;

    if (direction == 'w') --newX;
    if (direction == 's') ++newX;
    if (direction == 'a') --newY;
    if (direction == 'd') ++newY;

    if (grid[newX][newY] == ' ' || grid[newX][newY] == '+') {
        if (grid[newX][newY] == '+') {
            score += 20; // Power-up bonus
        }
        grid[playerX][playerY] = ' ';
        playerX = newX;
        playerY = newY;
        grid[playerX][playerY] = 'P';
    }
}

void placeBomb() {
    int bombX = playerX;
    int bombY = playerY;
    grid[bombX][bombY] = 'B';
    printGrid();

    this_thread::sleep_for(chrono::seconds(3));

    handleExplosion(bombX, bombY);
}

void handleExplosion(int bombX, int bombY) {
    grid[bombX][bombY] = 'X';

    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto& dir : directions) {
        int x = bombX;
        int y = bombY;
        for (int i = 0; i < 2; ++i) { // Explosion range of 2
            x += dir[0];
            y += dir[1];
            if (grid[x][y] == '#') break; // Indestructible wall
            if (grid[x][y] == 'O') {
                grid[x][y] = 'X';
                score += 10; // Destroy obstacle
                break;
            }
            if (grid[x][y] == 'E') {
                grid[x][y] = 'X';
                score += 50; // Destroy enemy
                break;
            }
            if (grid[x][y] == ' ') {
                grid[x][y] = 'X';
            }
        }
    }

    printGrid();
    this_thread::sleep_for(chrono::seconds(1));

    for (auto& dir : directions) {
        int x = bombX;
        int y = bombY;
        for (int i = 0; i < 2; ++i) {
            x += dir[0];
            y += dir[1];
            if (grid[x][y] == 'X') grid[x][y] = ' ';
        }
    }
    grid[bombX][bombY] = ' ';
}

void enemyMovement() {
    for (int i = 0; i < enemyCount; ++i) {
        int x = enemies[i][0];
        int y = enemies[i][1];

        grid[x][y] = ' ';
        int direction = rand() % 4;
        int newX = x + (direction == 0 ? -1 : direction == 1 ? 1 : 0);
        int newY = y + (direction == 2 ? -1 : direction == 3 ? 1 : 0);

        if (grid[newX][newY] == ' ') {
            enemies[i][0] = newX;
            enemies[i][1] = newY;
        }
        grid[enemies[i][0]][enemies[i][1]] = 'E';
    }
}

bool isGameOver() {
    for (int i = 0; i < enemyCount; ++i) {
        if (enemies[i][0] == playerX && enemies[i][1] == playerY) {
            return true;
        }
    }
    return false;
}

void saveHighScore() {
    ofstream file("highscores.txt", ios::app);
    file << "Score: " << score << endl;
    file.close();
}
