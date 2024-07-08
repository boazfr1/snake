#include <iostream>
#include <vector>
#include <utility>
#include <ncurses.h>
#include <chrono>
#include <thread>

const int HIGH = 20;
const int WIDTH = 40;
bool gameOver = false;

std::vector<std::pair<int, int>> snake = {{HIGH / 2, WIDTH / 2}};
std::pair<int, int> food = {2, 2};
int direction = KEY_RIGHT; // Start moving right

bool drawSnake(int x, int y) {
    for (const auto& p : snake) {
        if (p.first == y && p.second == x) {
            return true;
        }
    }
    return false;
}

bool drawFood(int x, int y) {
    return (food.first == y && food.second == x);
}

void drawBorder(int y) {
    for (int x = 0; x <= WIDTH + 1; x++) {
        mvaddch(y, x, '*');
    }
}

void moveSnake() {
    std::pair<int, int> newHead = snake.front();

    switch (direction) {
        case KEY_UP:    newHead.first--;  break;
        case KEY_DOWN:  newHead.first++;  break;
        case KEY_LEFT:  newHead.second--; break;
        case KEY_RIGHT: newHead.second++; break;
    }

    // Check for collision with walls or self
    if (newHead.first <= 0 || newHead.first >= HIGH + 1 ||
        newHead.second <= 0 || newHead.second >= WIDTH + 1 ||
        drawSnake(newHead.second, newHead.first)) {
        gameOver = true;
        return;
    }

    snake.insert(snake.begin(), newHead);

    // Check if snake ate the food
    if (newHead.first == food.first && newHead.second == food.second) {
        // Generate new food
        do {
            food.first = rand() % HIGH + 1;
            food.second = rand() % WIDTH + 1;
        } while (drawSnake(food.second, food.first));
    } else {
        snake.pop_back();
    }
}

void drawBoard() {
    clear();

    drawBorder(0);
    drawBorder(HIGH + 1);

    for (int i = 1; i <= HIGH; i++) {
        mvaddch(i, 0, '*');
        mvaddch(i, WIDTH + 1, '*');
        for (int j = 1; j <= WIDTH; j++) {
            if (drawSnake(j, i)) {
                mvaddch(i, j, 'O');
            } else if (drawFood(j, i)) {
                mvaddch(i, j, '$');
            } else {
                mvaddch(i, j, ' ');
            }
        }
    }

    refresh();
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(0);  // Non-blocking input

    while (!gameOver) {
        int ch = getch();
        if (ch != ERR) {
            switch (ch) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    direction = ch;
                    break;
                case 'q':
                case 'Q':
                    gameOver = true;
                    break;
            }
        }

        moveSnake();
        drawBoard();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Control game speed
    }

    endwin();
    std::cout << "Game Over!" << std::endl;
    return 0;
}