#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Define color codes for CMD (Windows 10+)
#define RESET       "\x1b[0m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define BOLD        "\x1b[1m"

#define MAX_QUES_LEN 300
#define MAX_OPTION_LEN 100

typedef struct {
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPTION_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Question;

void play_game(Question* questions, int no_of_questions);
int read_questions(char* file_name, Question** questions);
void print_question(Question question);
int use_lifeline(Question* question, int* lifeline);

// Sound functions
void play_correct_sound() { Beep(1000, 200); }
void play_wrong_sound()   { Beep(300, 500); }
void play_timeout_sound() { Beep(400, 800); }
void play_victory_sound() {
    int melody[] = { 659, 698, 784, 880 };
    for (int i = 0; i < 4; i++) {
        Beep(melody[i], 200);
        Sleep(100);
    }
}

// Timer input
int wait_for_input(char* input, int timeout_seconds) {
    time_t start = time(NULL);
    while (difftime(time(NULL), start) < timeout_seconds) {
        if (_kbhit()) {
            *input = toupper(_getch());
            return 1;
        }
        Sleep(100);
    }
    return 0;
}

int main() {
    system("cls");
    printf(BOLD CYAN "\n\t==============================\n");
    printf("\t  WHO WANTS TO BE A MILLIONAIRE?\n");
    printf("\t==============================\n" RESET);

    Question* questions;
    int no_of_questions = read_questions("questions.txt", &questions);
    play_game(questions, no_of_questions);
    free(questions);

    return 0;
}

void play_game(Question* questions, int no_of_questions) {
    int money_won = 0;
    int lifeline[] = {1, 1};

    for (int i = 0; i < no_of_questions; i++) {
        print_question(questions[i]);
        char ch;
        int answered = wait_for_input(&ch, questions[i].timeout);

        if (!answered) {
            play_timeout_sound();
            printf(RED "\nTime's up! You took too long." RESET "\n");
            _getch();
            break;
        }

        printf("%c\n", ch);

        if (ch == 'L') {
            int used = use_lifeline(&questions[i], lifeline);
            if (used != 2) {
                i--;
            }
            continue;
        }

        if (ch == questions[i].correct_option) {
            play_correct_sound();
            printf(GREEN "\nCorrect! Well done." RESET);
            money_won = questions[i].prize_money;
            printf(BLUE "\nYou've earned: $%d\n" RESET, money_won);
        } else {
            play_wrong_sound();
            printf(RED "\nWrong answer! The correct answer was %c.\n" RESET, questions[i].correct_option);
            break;
        }
    }

    printf(BOLD CYAN "\nGame Over! Your total winnings: $%d\n" RESET, money_won);
    if (money_won > 0) {
        play_victory_sound();
    }
}

void print_question(Question question) {
    printf(BOLD YELLOW "\nQuestion:\n%s" RESET, question.text);
    for (int i = 0; i < 4; i++) {
        if (question.options[i][0] != '\0') {
            printf(CYAN "%c. %s\n" RESET, ('A' + i), question.options[i]);
        }
    }
    printf(MAGENTA "\nYou have %d seconds to answer." RESET, question.timeout);
    printf(GREEN "\nEnter your answer (A-D) or L for lifeline: " RESET);
}

int use_lifeline(Question* question, int* lifeline) {
    printf(BOLD YELLOW "\nAvailable Lifelines:\n" RESET);
    if (lifeline[0]) printf(CYAN "1. Fifty-Fifty (50/50)\n" RESET);
    if (lifeline[1]) printf(CYAN "2. Skip the Question\n" RESET);
    printf(GREEN "Choose lifeline (1/2) or press any other key to return: " RESET);

    char ch = toupper(_getch());
    printf("%c\n", ch);

    switch (ch) {
        case '1':
            if (lifeline[0]) {
                lifeline[0] = 0;
                int removed = 0;
                while (removed < 2) {
                    int rand_opt = rand() % 4;
                    if ((rand_opt + 'A') != question->correct_option && question->options[rand_opt][0] != '\0') {
                        question->options[rand_opt][0] = '\0';
                        removed++;
                    }
                }
                printf(YELLOW "\nTwo incorrect options removed.\n" RESET);
                return 1;
            }
            break;
        case '2':
            if (lifeline[1]) {
                lifeline[1] = 0;
                printf(YELLOW "\nQuestion skipped.\n" RESET);
                return 2;
            }
            break;
        default:
            printf(BLUE "\nReturning to the question...\n" RESET);
            break;
    }
    return 0;
}

int read_questions(char* file_name, Question** questions) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf(RED "Error: Cannot open questions file.\n" RESET);
        exit(1);
    }

    char str[MAX_QUES_LEN];
    int no_of_lines = 0;
    while (fgets(str, MAX_QUES_LEN, file)) {
        no_of_lines++;
    }

    int no_of_questions = no_of_lines / 8;
    *questions = (Question*)malloc(no_of_questions * sizeof(Question));
    rewind(file);

    for (int i = 0; i < no_of_questions; i++) {
        fgets((*questions)[i].text, MAX_QUES_LEN, file);
        for (int j = 0; j < 4; j++) {
            fgets((*questions)[i].options[j], MAX_OPTION_LEN, file);
            (*questions)[i].options[j][strcspn((*questions)[i].options[j], "\n")] = 0;
        }
        char line[10];
        fgets(line, 10, file);
        (*questions)[i].correct_option = toupper(line[0]);

        fgets(line, 10, file);
        (*questions)[i].timeout = atoi(line);

        fgets(line, 10, file);
        (*questions)[i].prize_money = atoi(line);
    }

    fclose(file);
    return no_of_questions;
}
