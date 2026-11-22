#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <ctime>

using namespace std;

class Sub {
private:
    int x;
    int y;
    int oxy;
    int cash;
    int bank;
    char logo;

public:
    Sub(int spawnX, int spawnY)
        : x(spawnX), y(spawnY), oxy(15), cash(0), bank(0), logo('@') {
    }

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


        if (y != 0) {
            oxy--;
        }
    }

    void right() {
        x++;


        if (y != 0) {
            oxy--;
        }
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

    vector<vector<int>> doorIndices;
    vector<vector<int>> leverIndices;

    bool doorsOpen[3] = { false, false, false };

    int doorX[3] = { -1, -1, -1 };
    int doorY[3] = { -1, -1, -1 };

    int rows;
    int cols;
    int entranceX;
    int entranceY;
    bool secondArea;

public:
    Map(int r = 11, int c = 12, bool cave = false)
        : rows(r), cols(c), entranceX(-1), entranceY(-1), secondArea(cave) {

        grid = vector<vector<char>>(rows, vector<char>(cols, '%'));

        oxygenBubbles = vector<vector<bool>>(
            rows, vector<bool>(cols, false)
        );

        doorIndices = vector<vector<int>>(
            rows, vector<int>(cols, 0)
        );

        leverIndices = vector<vector<int>>(
            rows, vector<int>(cols, 0)
        );

        generate();
    }

    void generate() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {

                if (secondArea) {
                    if (i == 0 || i == rows - 1 ||
                        j == 0 || j == cols - 1) {

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

        for (int i = startY; i < startY + 5; i++) {
            for (int j = startX; j < startX + 5; j++) {
                cout << grid[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool boundary(int futureX, int futureY) {
        if (futureY < 0 || futureY >= rows ||
            futureX < 0 || futureX >= cols) {

            return false;
        }

        if (grid[futureY][futureX] == 'D' ||
            grid[futureY][futureX] == '#' ||
            grid[futureY][futureX] == '{' ||
            grid[futureY][futureX] == '}' ||
            grid[futureY][futureX] == '_') {

            if (!isEntrance(futureY, futureX)) {
                cout << "You cannot move through that tile!" << endl;
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

    void placeLeversAndDoors() {

        for (int index = 1; index <= 2; index++) {

            bool leverPlaced = false;

            while (!leverPlaced) {
                int y = rand() % 4 + (rows - 5);
                int x = rand() % (cols - 2) + 1;

                if (grid[y][x] == '%') {
                    grid[y][x] = 'L';
                    leverIndices[y][x] = index;
                    leverPlaced = true;
                }
            }

            bool doorPlaced = false;

            while (!doorPlaced) {
                int y = rand() % 4 + (rows - 5);
                int x = rand() % (cols - 2) + 1;

                if (grid[y][x] == '%') {
                    grid[y][x] = 'D';

                    doorIndices[y][x] = index;

                    doorX[index] = x;
                    doorY[index] = y;

                    doorPlaced = true;
                }
            }
        }
    }

    int getDoorIndexAt(int y, int x) const {
        return doorIndices[y][x];
    }

    bool isDoorOpen(int index) const {
        return doorsOpen[index];
    }

    int getDoorX(int index) const {
        return doorX[index];
    }

    int getDoorY(int index) const {
        return doorY[index];
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

            int aroundX[] = { 0, 0, 0, -1, 1, -1, -1, 1, 1 };
            int aroundY[] = { 0, -1, 1, 0, 0, -1, 1, -1, 1 };

            bool found = false;

            for (int i = 0; i < 9; i++) {

                int checkX = x + aroundX[i];
                int checkY = y + aroundY[i];

                if (checkX < 0 || checkX >= cols ||
                    checkY < 0 || checkY >= rows) {

                    continue;
                }

                if (grid[checkY][checkX] == '$') {

                    cout << "You found nearby treasure!" << endl;

                    grid[checkY][checkX] = '%';

                    int oldCash = player.getCash();
                    player.profit();

                    int treasureFound = player.getCash() - oldCash;

                    cout << "You collected $" << treasureFound
                        << " worth of treasure!" << endl;

                    found = true;
                }

                else if (grid[checkY][checkX] == '!') {

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

                    grid[checkY][checkX] = '%';

                    found = true;
                }

                else if (doorIndices[checkY][checkX] != 0) {

                    int index = doorIndices[checkY][checkX];

                    cout << "You found Door " << index << ". ";

                    if (doorsOpen[index]) {
                        cout << "It is currently open!" << endl;
                    }
                    else {
                        cout << "It is currently closed!" << endl;
                    }

                    found = true;
                }

                else if (leverIndices[checkY][checkX] != 0) {

                    int index = leverIndices[checkY][checkX];

                    doorsOpen[index] = !doorsOpen[index];

                    int dx = doorX[index];
                    int dy = doorY[index];

                    grid[dy][dx] = doorsOpen[index] ? 'd' : 'D';

                    cout << "Lever " << index << " used! "
                        << "Corresponding door has "
                        << (doorsOpen[index] ? "opened!" : "closed!")
                        << endl;

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

                if (grid[i][j] == '@' &&
                    (i != player.getY() || j != player.getX())) {

                    if (leverIndices[i][j] != 0) {
                        grid[i][j] = 'L';
                    }

                    else if (doorIndices[i][j] != 0) {
                        int index = doorIndices[i][j];

                        grid[i][j] = doorsOpen[index] ? 'd' : 'D';
                    }

                    else if (oxygenBubbles[i][j]) {
                        grid[i][j] = 'O';
                    }

                    else if (isEntrance(i, j)) {
                        grid[i][j] = '=';
                    }

                    else if (secondArea &&
                        (i == 0 || i == rows - 1 ||
                            j == 0 || j == cols - 1)) {

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
    cout << "Use WASD to navigate the deep sea!" << endl;
    cout << "Use I to inspect nearby waters." << endl;
    cout << "Levers (L) can be used to open and close doors." << endl;
    cout << "Closed doors are represented by 'D' and opened doors by 'd'." << endl;
    cout << "Press any key to begin" << endl;
}

int main() {

    srand(static_cast<unsigned int>(time(0)));

    bool gameOver = false;
    bool visitedSecondArea = false;

    char input;

    Map area1;
    Map area2(6, 6, true);
    Map area3(6, 6, true);
    Map area4(6, 6, true);

    area1.setEntrance(10, 6);
    area2.setEntrance(0, 3);

    area3.setEntrance(3, 0);
    area4.setEntrance(3, 5);

    area1.placeTreasure();
    area1.placeSpecial();
    area1.placeOxygenBubbles();

    area1.placeLeversAndDoors();

    area2.placeTreasure(5);
    area2.placeSpecial(2);
    area2.placeOxygenBubbles(2);

    area3.placeTreasure(4);
    area3.placeSpecial(2);
    area3.placeOxygenBubbles(2);

    area4.placeTreasure(4);
    area4.placeSpecial(2);
    area4.placeOxygenBubbles(2);

    Map* currentMap = &area1;

    Sub player(rand() % 4 + 4, 0);

    currentMap->setTile(player.getY(), player.getX(), player.getLogo());

    mainMenu();
    cin >> input;

    system("cls");

    area1.printFullMap();

    cout << endl;
    cout << "Press any key to begin exploring!" << endl;

    cin >> input;

    while (gameOver == false) {

        //system("cls");

        cout << endl << endl;

        currentMap->clear(player);
        currentMap->printCam(player);

        cout << "Area: ";

        if (currentMap == &area1)
            cout << "1";
        else if (currentMap == &area2)
            cout << "2";
        else if (currentMap == &area3)
            cout << "3";
        else
            cout << "4";

        cout << endl;

        cout << "Oxygen: " << player.getOxy() << "/15" << endl;
        cout << "Treasure stored sub: $" << player.getCash() << endl;
        cout << "Bank account: $" << player.getBank() << endl;

        cout << "Move (WASD) or Inspect (I): " << endl;

        cin >> input;

        int oldY = player.getY();

        bool moved = false;
        bool returnedToSurface = false;

        int futureX = player.getX();
        int futureY = player.getY();

        if (input == 'w' || input == 'W') {
            futureY--;
        }
        else if (input == 's' || input == 'S') {
            futureY++;
        }
        else if (input == 'a' || input == 'A') {
            futureX--;
        }
        else if (input == 'd' || input == 'D') {
            futureX++;
        }

        if (input == 'w' || input == 'W' ||
            input == 's' || input == 'S' ||
            input == 'a' || input == 'A' ||
            input == 'd' || input == 'D') {

            if (currentMap->boundary(futureX, futureY)) {

                if (currentMap == &area1 &&
                    currentMap->isEntrance(futureY, futureX)) {

                    currentMap->clear(player);

                    if (input == 'w' || input == 'W')
                        player.up();
                    else if (input == 's' || input == 'S')
                        player.down();
                    else if (input == 'a' || input == 'A')
                        player.left();
                    else
                        player.right();

                    currentMap = &area2;
                    player.setPosition(3, 1);

                    visitedSecondArea = true;

                    cout << "You entered Area 2!" << endl;
                    moved = true;
                }

                else if (currentMap == &area2 &&
                    currentMap->isEntrance(futureY, futureX)) {

                    currentMap->clear(player);

                    if (input == 'w' || input == 'W')
                        player.up();
                    else if (input == 's' || input == 'S')
                        player.down();
                    else if (input == 'a' || input == 'A')
                        player.left();
                    else
                        player.right();

                    currentMap = &area1;
                    player.setPosition(6, 9);

                    cout << "You returned to Area 1!" << endl;
                    moved = true;
                }

                else if (currentMap == &area3 &&
                    currentMap->isEntrance(futureY, futureX)) {

                    currentMap->clear(player);
                    player.left();

                    currentMap = &area1;

                    player.setPosition(
                        area1.getDoorX(1),
                        area1.getDoorY(1)
                    );

                    cout << "You returned to Area 1 through Door 1!" << endl;
                    moved = true;
                }

                else if (currentMap == &area4 &&
                    currentMap->isEntrance(futureY, futureX)) {

                    currentMap->clear(player);
                    player.right();

                    currentMap = &area1;

                    player.setPosition(
                        area1.getDoorX(2),
                        area1.getDoorY(2)
                    );

                    cout << "You returned to Area 1 through Door 2!" << endl;
                    moved = true;
                }

                else {

                    if (input == 'w' || input == 'W')
                        player.up();
                    else if (input == 's' || input == 'S')
                        player.down();
                    else if (input == 'a' || input == 'A')
                        player.left();
                    else
                        player.right();

                    moved = true;

                    if (currentMap == &area1) {

                        int index = area1.getDoorIndexAt(
                            player.getY(),
                            player.getX()
                        );

                        if (index != 0 && area1.isDoorOpen(index)) {

                            area1.clear(player);

                            if (index == 1) {
                                currentMap = &area3;
                                player.setPosition(1, 3);

                                cout << "You entered Area 3 through Door 1!" << endl;
                            }
                            else {
                                currentMap = &area4;
                                player.setPosition(4, 3);

                                cout << "You entered Area 4 through Door 2!" << endl;
                            }

                            visitedSecondArea = true;
                        }
                    }
                }
            }
        }

        else if (input == 'i' || input == 'I') {
            currentMap->checkSpot(player, true);
        }

        else {
            cout << "Invalid input. Please use WASD or I to inspect." << endl;
        }

        if (moved && currentMap == &area1 &&
            player.getY() == 0 && oldY != 0) {

            returnedToSurface = true;
        }

        currentMap->checkSpot(player);

        currentMap->setTile(
            player.getY(),
            player.getX(),
            player.getLogo()
        );

        if (returnedToSurface) {

            player.refill();

            cout << "You returned to the surface!" << endl;
            cout << "Oxygen refilled back to 15" << endl;

            cout << "You sold all your treasure and now have $" << player.getBank() << " in your bank account!" << endl;

            //if (visitedSecondArea) {
            //    cout << "You explored the other areas and made it back safely!" << endl;
            //}

            cout << "Press any key to continue." << endl;
            cin >> input;
        }

        if (player.getOxy() <= 0 && gameOver == false) {

            gameOver = true;

            cout << endl;
            cout << "You ran out of oxygen. Game Over. You finished with $"
                << player.getBank() << endl;

            cin >> input;
        }
    }

    return 0;
}
