#include <stdio.h>
#include <stdlib.h>
#include <conio.h>     // For getch(), kbhit()
#include <windows.h>   // For Sleep()
#include <time.h>

#define HEIGHT 20
#define WIDTH 60

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STOP
};

enum Direction dir;
int score;
int fruit_x, fruit_y;
int head_x, head_y;
int tail_length;
int tail_x[100];
int tail_y[100];

void setup();
void draw();
void input();
void game_play();
void clear_screen();

int main() {
    srand(time(NULL));
    setup();
    while (1) {
        draw();
        input();
        game_play();
        Sleep(100);  // Sleep in milliseconds
    }
}

void input() {
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
            case 'a':
            case 'A':
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
            case 'D':
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'w':
            case 'W':
                if (dir != DOWN) dir = UP;
                break;
            case 's':
            case 'S':
                if (dir != UP) dir = DOWN;
                break;
            case 'x':
            case 'X':
                printf("\nExiting game.\n");
                exit(0);
        }
    }
}

void game_play() {
    for (int i = tail_length - 1; i > 0; i--) {
        tail_x[i] = tail_x[i - 1];
        tail_y[i] = tail_y[i - 1];
    }
    if (tail_length > 0) {
        tail_x[0] = head_x;
        tail_y[0] = head_y;
    }

    switch (dir) {
        case UP:
            head_y--;
            break;
        case DOWN:
            head_y++;
            break;
        case LEFT:
            head_x--;
            break;
        case RIGHT:
            head_x++;
            break;
        case STOP:
            return;
    }

    // Wall wrap
    if (head_x < 0) head_x = WIDTH - 1;
    else if (head_x >= WIDTH) head_x = 0;

    if (head_y < 0) head_y = HEIGHT - 1;
    else if (head_y >= HEIGHT) head_y = 0;

    // Check self-collision
    for (int i = 0; i < tail_length; i++) {
        if (tail_x[i] == head_x && tail_y[i] == head_y) {
            printf("\nYou hit your tail! GAME OVER.\n");
            exit(0);
        }
    }

    // Fruit eaten
    if (head_x == fruit_x && head_y == fruit_y) {
        score += 10;
        tail_length++;
        fruit_x = rand() % WIDTH;
        fruit_y = rand() % HEIGHT;
    }
}

void setup() {
    head_x = WIDTH / 2;
    head_y = HEIGHT / 2;
    fruit_x = rand() % WIDTH;
    fruit_y = rand() % HEIGHT;
    dir = STOP;
    score = 0;
    tail_length = 0;
}

void draw() {
    clear_screen();
    printf("\t\tWelcome to The Snake Game!!!\n");

    for (int i = 0; i < WIDTH + 2; i++) printf("#");

    for (int i = 0; i < HEIGHT; i++) {
        printf("\n#");
        for (int j = 0; j < WIDTH; j++) {
            if (i == head_y && j == head_x)
                printf("O");
            else if (i == fruit_y && j == fruit_x)
                printf("F");
            else {
                int printed = 0;
                for (int k = 0; k < tail_length; k++) {
                    if (tail_x[k] == j && tail_y[k] == i) {
                        printf("o");
                        printed = 1;
                        break;
                    }
                }
                if (!printed) printf(" ");
            }
        }
        printf("#");
    }

    printf("\n");
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\nScore: %d\n", score);
}

void clear_screen() {
    system("cls");
}
