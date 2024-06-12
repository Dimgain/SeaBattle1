#include <iostream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

const int N = 10;
const int MAX_SHIPS = 10;
const int SHIP_SIZES[MAX_SHIPS] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

struct Ship {
    int size;
    int hits;
    bool destroyed;
};

bool is_valid_position(int field[N][N], int x, int y) {
    if (x < 0 || x >= N || y < 0 || y >= N) {
        return false;
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (x + i >= 0 && x + i < N && y + j >= 0 && y + j < N && field[x + i][y + j] > 0) {
                return false;
            }
        }
    }

    return true;
}

void place_ship(int field[N][N], Ship ships[MAX_SHIPS], int ship_index) {
    int size = ships[ship_index].size;
    bool placed = false;

    while (!placed) {
        int x = rand() % N;
        int y = rand() % N;
        bool horizontal = rand() % 2;

        if (horizontal) {
            if (x + size <= N) {
                bool can_place = true;
                for (int i = 0; i < size; i++) {
                    if (!is_valid_position(field, x + i, y)) {
                        can_place = false;
                        break;
                    }
                }
                if (can_place) {
                    for (int i = 0; i < size; i++) {
                        field[x + i][y] = ship_index + 1;
                    }
                    placed = true;
                }
            }
        }
        else {
            if (y + size <= N) {
                bool can_place = true;
                for (int i = 0; i < size; i++) {
                    if (!is_valid_position(field, x, y + i)) {
                        can_place = false;
                        break;
                    }
                }
                if (can_place) {
                    for (int i = 0; i < size; i++) {
                        field[x][y + i] = ship_index + 1;
                    }
                    placed = true;
                }
            }
        }
    }
}

void initialize_field(int field[N][N], Ship ships[MAX_SHIPS], int& ship_count) {
    for (int i = 0; i < MAX_SHIPS; i++) {
        ships[i].size = SHIP_SIZES[i];
        ships[i].hits = 0;
        ships[i].destroyed = false;
        place_ship(field, ships, i);
    }
    ship_count = MAX_SHIPS;
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_fields(int player_field[N][N], int bot_field[N][N]) {
    clear_screen();
    cout << "Поле Гравця\t\tПоле Бота" << endl;
    cout << "  ";
    for (int i = 0; i < N; i++) {
        cout << i << " ";
    }
    cout << "\t  ";
    for (int i = 0; i < N; i++) {
        cout << i << " ";
    }
    cout << endl;
    cout << "---------------------\t---------------------" << endl;
    for (int i = 0; i < N; i++) {
        cout << i << " ";
        for (int j = 0; j < N; j++) {
            if (player_field[j][i] == 0) {
                cout << '-';
            }
            else if (player_field[j][i] == -1) {
                cout << 'o';
            }
            else if (player_field[j][i] == -2) {
                cout << 'x';
            }
            else {
                cout << '-';
            }
            cout << " ";
        }
        cout << "\t" << i << " ";
        for (int j = 0; j < N; j++) {
            if (bot_field[j][i] == -1) {
                cout << 'o';
            }
            else if (bot_field[j][i] == -2) {
                cout << 'x';
            }
            else if (bot_field[j][i] == 0) {
                cout << '-';
            }
            else {
                cout << '-';
            }
            cout << " ";
        }
        cout << endl;
    }
}

bool check_victory(Ship ships[MAX_SHIPS], int ship_count) {
    for (int i = 0; i < ship_count; i++) {
        if (!ships[i].destroyed) {
            return false;
        }
    }
    return true;
}

void check_ship_status(Ship ships[MAX_SHIPS], int ship_id, string& message) {
    ships[ship_id - 1].hits++;
    if (ships[ship_id - 1].hits == ships[ship_id - 1].size) {
        ships[ship_id - 1].destroyed = true;
        message = "Корабель знищено!";
    }
    else {
        message = "Влучив!";
    }
}

bool player_turn(int bot_field[N][N], Ship bot_ships[MAX_SHIPS], int bot_ship_count, string& message) {
    int x, y;
    bool valid_input = false;

    while (!valid_input) {
        cout << "Введіть координати цілі: " << endl;
        cin >> x >> y;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << "Невірні координати цілі. Спробуйте ще раз." << endl;
            message = "Невірні координати цілі";
        }
        else if (x < 0 || x >= N || y < 0 || y >= N) {
            cout << "Невірні координати цілі. Спробуйте ще раз." << endl;
            message = "Невірні координати цілі";
        }
        else {
            valid_input = true;
        }
    }

    if (bot_field[x][y] > 0) {
        int ship_id = bot_field[x][y];
        bot_field[x][y] = -2;
        check_ship_status(bot_ships, ship_id, message);

        if (check_victory(bot_ships, bot_ship_count)) {
            cout << "Ви перемогли!!!" << endl;
            exit(0);
        }
        return true;
    }
    else if (bot_field[x][y] == 0) {
        message = "Промах!";
        bot_field[x][y] = -1;
        return false;
    }
    return false;
}

bool bot_turn(int player_field[N][N], Ship player_ships[MAX_SHIPS], int player_ship_count, string& message) {
    int x, y;
    while (true) {
        x = rand() % N;
        y = rand() % N;
        if (player_field[x][y] != -1 && player_field[x][y] != -2) {
            break;
        }
    }

    cout << "Бот стріляє по: (" << x << ", " << y << "): ";
    if (player_field[x][y] > 0) {
        int ship_id = player_field[x][y];
        player_field[x][y] = -2;
        check_ship_status(player_ships, ship_id, message);

        if (check_victory(player_ships, player_ship_count)) {
            cout << "Бот переміг!" << endl;
            exit(0);
        }
        return true;
    }
    else {
        message = "Промах";
        player_field[x][y] = -1;
        return false;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    srand(time(NULL));

    int player_field[N][N] = { 0 };
    int bot_field[N][N] = { 0 };
    Ship player_ships[MAX_SHIPS];
    Ship bot_ships[MAX_SHIPS];
    int player_ship_count = 0;
    int bot_ship_count = 0;

    initialize_field(player_field, player_ships, player_ship_count);
    initialize_field(bot_field, bot_ships, bot_ship_count);

    string message;

    while (true) {
        bool player_turn_active = true;
        while (player_turn_active) {
            print_fields(player_field, bot_field);
            cout << message << endl;
            player_turn_active = player_turn(bot_field, bot_ships, bot_ship_count, message);
        }

        bool bot_turn_active = true;
        while (bot_turn_active) {
            print_fields(player_field, bot_field);
            cout << message << endl;
            bot_turn_active = bot_turn(player_field, player_ships, player_ship_count, message);
        }
    }

    return 0;
}
