
// SubAdventure.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <cstdlib>
#include <conio.h>
#include <string>
#include <ctime>

using namespace std;

class Sub {
private:
    int x;
    // horizontal, x-axis, (columns)
    int y;
    // vertical, y-axis, (rows)
    int oxy;
    int cash;
    int bank;
    char logo;

public:
    Sub(int spawnX, int spawnY) : x(spawnX), y(spawnY), oxy(15), cash(0), bank(0), logo('@') {}

    int getX() const { return x; }
    int getY() const { return y; }
    int getOxy() const { return oxy; }
    int getCash() const { return cash; }
    int getBank() const { return bank; }
    char getLogo() const { return logo; }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void restoreOxygen() {
        oxy = 15;
    }

    void up() {
        y--;
        oxy--;
    }

    void down() {
        y++;
        oxy--;
    }

    void left() {
        x--;
        oxy--;
    }

    void right() {
        x++;
        oxy--;
    }

    void refill() {
        oxy = 15;
        bank += cash;
        cash = 0;
    }

    void profit() {
        cash += rand() % 15 + 10;
    }
};

class Map {
private:
    vector<vector<char>> grid;
    vector<vector<bool>> oxygenBubbles;
    int rows;
    int cols;
    int entranceX;
    int entranceY;
    bool secondArea;

public:
    Map(int r = 11, int c = 12, bool cave = false)
        : rows(r), cols(c), entranceX(-1), entranceY(-1), secondArea(cave) {

        grid = vector<vector<char>>(rows, vector<char>(cols, '%'));
        oxygenBubbles = vector<vector<bool>>(rows, vector<bool>(cols, false));
        generate();
    }

    void generate() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (secondArea) {
                    if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
                        grid[i][j] = '#';
                    }
                    else {
                        grid[i][j] = '%';
                    }
                }
                else {
                    if (i == 0) {
                        grid[i][j] = '~';
                    }
                    else if (i == rows - 1) {
                        grid[i][j] = '_';
                    }

                    if (j == 0 && i != 0) {
                        grid[i][j] = '{';
                    }
                    else if (j == cols - 1 && i != 0) {
                        grid[i][j] = '}';
                    }
                }
            }
        }
    }

    void setEntrance(int y, int x) {
        entranceX = x;
        entranceY = y;
        grid[y][x] = '=';
    }

    bool isEntrance(int y, int x) const {
        return y == entranceY && x == entranceX;
    }

    char getTile(int y, int x) const {
        return grid[y][x];
    }

    void setTile(int y, int x, char tile) {
        grid[y][x] = tile;
    }

    void printFullMap() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printCam(Sub& player) {
        int startX = player.getX() - 2;
        int startY = player.getY() - 2;

        if (startX < 0)
            startX = 0;
        if (startX > cols - 5)
            startX = cols - 5;

        if (startY < 0)
            startY = 0;
        if (startY > rows - 5)
            startY = rows - 5;

        // Display only the 5x5 section.
        for (int i = startY; i < startY + 5; i++) {
            for (int j = startX; j < startX + 5; j++) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool boundary(int futureX, int futureY) {
        if (futureY < 0 || futureY >= rows || futureX < 0 || futureX >= cols) {
            return false;
        }

        if (grid[futureY][futureX] == '#' ||
            grid[futureY][futureX] == '{' ||
            grid[futureY][futureX] == '}' ||
            grid[futureY][futureX] == '_') {

            if (!isEntrance(futureY, futureX)) {
                cout << "You are trying to swim out of bounds! Please try another input." << endl;
                return false;
            }
        }

        return true;
    }

    void placeTreasure(int amount = 10) {
        int num = 0;

        while (num < amount) {
            int y = rand() % (rows - 2) + 1;
            int x = rand() % (cols - 2) + 1;

            if (grid[y][x] == '%') {
                grid[y][x] = '$';
                num++;
            }
        }
    }

    void placeSpecial(int amount = 5) {
        int num = 0;

        while (num < amount) {
            int y = rand() % (rows - 2) + 1;
            int x = rand() % (cols - 2) + 1;

            if (grid[y][x] == '%') {
                grid[y][x] = '!';
                num++;
            }
        }
    }

    void placeOxygenBubbles(int amount = 5) {
        int num = 0;

        while (num < amount) {
            int y = rand() % 4 + (rows - 5);
            int x = rand() % (cols - 2) + 1;

            if (grid[y][x] == '%') {
                grid[y][x] = 'O';
                oxygenBubbles[y][x] = true;
                num++;
            }
        }
    }

    void checkSpot(Sub& player, bool inspect = false) {
        int x = player.getX();
        int y = player.getY();

        if (inspect == false) {
            if (oxygenBubbles[y][x]) {
                if (player.getOxy() < 15) {
                    player.restoreOxygen();
                    cout << "You found an oxygen bubble! Oxygen restored to 15!" << endl;
                }
            }
            else if (grid[y][x] == '$') {
                cout << "You found treasure!" << endl;
                grid[y][x] = '%';

                int oldCash = player.getCash();
                player.profit();
                int found = player.getCash() - oldCash;

                cout << "You picked up $" << found << " worth of treasure!" << endl;
            }
            else if (grid[y][x] == '!') {
                int num = rand() % 5;
                string animal;

                if (num == 0)
                    animal = "great white shark";
                else if (num == 1)
                    animal = "giant squid";
                else if (num == 2)
                    animal = "pack of orcas";
                else if (num == 3)
                    animal = "pack of dolphins";
                else
                    animal = "bunch of old ship debris";

                cout << "You ran into a " << animal << "!" << endl;
                grid[y][x] = '%';
            }
        }

        if (inspect == true) {
            int aroundX[] = { 0, 0, -1, 1, -1, -1, 1, 1 };
            int aroundY[] = { -1, 1, 0, 0, -1, 1, -1, 1 };

            bool found = false;

            for (int i = 0; i < 8; i++) {
                int checkX = x + aroundX[i];
                int checkY = y + aroundY[i];

                if (checkX < 0 || checkX >= cols || checkY < 0 || checkY >= rows) {
                    continue;
                }

                if (grid[checkY][checkX] == '$') {
                    cout << "You found nearby treasure!" << endl;
                    grid[checkY][checkX] = '%';

                    int oldCash = player.getCash();
                    player.profit();
                    int treasureFound = player.getCash() - oldCash;

                    cout << "You collected $" << treasureFound << " worth of treasure!" << endl;
                    found = true;
                }
                else if (grid[checkY][checkX] == '!') {
                    int num = rand() % 5;
                    string animal;

                    if (num == 0) {
                        animal = "great white shark";
                    }
                    else if (num == 1) {
                        animal = "giant squid";
                    }
                    else if (num == 2) {
                        animal = "pack of orcas";
                    }
                    else if (num == 3) {
                        animal = "pack of dolphins";
                    }
                    else {
                        animal = "bunch of old ship debris";
                    }

                    cout << "You ran into a " << animal << "!" << endl;
                    grid[checkY][checkX] = '%';
                    found = true;
                }
            }

            if (found == false) {
                cout << "No events of note found." << endl;
            }
        }
    }

    void clear(Sub& player) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] == '@' && (i != player.getY() || j != player.getX())) {
                    if (oxygenBubbles[i][j]) {
                        grid[i][j] = 'O';
                    }
                    else if (isEntrance(i, j)) {
                        grid[i][j] = '=';
                    }
                    else if (secondArea && (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)) {
                        grid[i][j] = '#';
                    }
                    else if (i == 0) {
                        grid[i][j] = '~';
                    }
                    else if (i == rows - 1) {
                        grid[i][j] = '_';
                    }
                    else if (j == 0) {
                        grid[i][j] = '{';
                    }
                    else if (j == cols - 1) {
                        grid[i][j] = '}';
                    }
                    else {
                        grid[i][j] = '%';
                    }
                }
            }
        }
    }
};

void mainMenu() {
    system("cls");
    cout << "Welcome to Sub Adventure!" << endl;
    cout << "Collect and sell as much treasure as you can without running out of oxygen!" << endl;
    cout << "Use WASD or arrow keys to navigate the deep sea!" << endl;
    cout << "Use I to inspect nearby waters." << endl;
    cout << "Press any key to begin" << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    bool gameOver = false;
    bool visitedSecondArea = false;
    int input;

    Map area1;
    Map area2(6, 6, true);

    area1.setEntrance(10, 6);
    area2.setEntrance(0, 3);

    area1.placeTreasure();
    area1.placeSpecial();
    area1.placeOxygenBubbles();

    area2.placeTreasure(5);
    area2.placeSpecial(2);
    area2.placeOxygenBubbles(2);

    Map* currentMap = &area1;

    Sub player(rand() % 4 + 4, 0);
    currentMap->setTile(player.getY(), player.getX(), player.getLogo());

    mainMenu();
    _getch();

    system("cls");
    area1.printFullMap();

    cout << endl;
    cout << "Press any key to begin exploring!" << endl;
    _getch();

    while (gameOver == false) {
        system("cls");

        cout << endl << endl;
        currentMap->clear(player);
        currentMap->printCam(player);

        cout << "Area: " << (currentMap == &area1 ? "1" : "2") << endl;
        cout << "Oxygen: " << player.getOxy() << "/15" << endl;
        cout << "Treasure stored sub: $" << player.getCash() << endl;
        cout << "Bank account: $" << player.getBank() << endl;
        cout << "Move (WASD) or Inspect (I): " << endl;

        input = _getch();

        int oldX = player.getX();
        int oldY = player.getY();
        bool moved = false;
        bool returnedToSurface = false;

        if (input == 'w' || input == 'W') {
            if (currentMap->boundary(player.getX(), player.getY() - 1)) {
                if (currentMap == &area2 &&
                    currentMap->isEntrance(player.getY() - 1, player.getX())) {

                    currentMap->clear(player);
                    player.up();

                    currentMap = &area1;
                    player.setPosition(6, 10);

                    cout << "You returned to Area 1!" << endl;
                    moved = true;
                }
                else {
                    player.up();
                    moved = true;
                }
            }
        }
        else if (input == 's' || input == 'S') {
            if (currentMap->boundary(player.getX(), player.getY() + 1)) {
                if (currentMap == &area1 &&
                    currentMap->isEntrance(player.getY() + 1, player.getX())) {

                    currentMap->clear(player);
                    player.down();

                    currentMap = &area2;
                    player.setPosition(3, 0);

                    visitedSecondArea = true;

                    cout << "You entered Area 2!" << endl;
                    moved = true;
                }
                else {
                    player.down();
                    moved = true;
                }
            }
        }
        else if (input == 'a' || input == 'A') {
            if (currentMap->boundary(player.getX() - 1, player.getY())) {
                player.left();
                moved = true;
            }
        }
        else if (input == 'd' || input == 'D') {
            if (currentMap->boundary(player.getX() + 1, player.getY())) {
                player.right();
                moved = true;
            }
        }
        else if (input == 'i' || input == 'I') {
            currentMap->checkSpot(player, true);
        }
        else {
            cout << "Invalid input. Please use WASD or I to inspect." << endl;
        }

        if (moved) {
            if (currentMap == &area1 && player.getY() == 0 && oldY != 0) {
                returnedToSurface = true;
            }
        }

        currentMap->checkSpot(player);
        currentMap->setTile(player.getY(), player.getX(), player.getLogo());

        if (returnedToSurface) {
            player.refill();

            cout << "You returned to the surface!" << endl;
            cout << "Oxygen refilled back to 15" << endl;
            cout << "You sold all your treasure and now have $" << player.getBank() << " in your bank account!" << endl;

            if (visitedSecondArea) {
                cout << "You explored the second area and made it back safely! You win!" << endl;
                gameOver = true;
            }

            cout << "Press any key to continue." << endl;
            _getch();
        }

        if (player.getOxy() <= 0 && gameOver == false) {
            gameOver = true;
            cout << endl << "You ran out of oxygen. Game Over. You finished with $" << player.getBank() << endl;
            _getch();
        }
    }

    return 0;
}
