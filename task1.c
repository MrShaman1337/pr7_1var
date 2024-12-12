#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

volatile int guessed = 0;
int N;


void guess_handler(int sig) {
    static int attempts = 0;
    attempts++;
    int guess = rand() % N + 1;

    printf("Попытка %d: Предполагаемое число %d\n", attempts, guess);

    if (guess == N) {
        printf("Число было угадано через %d попыток.\n", attempts);
        guessed = 1;
        kill(getppid(), SIGUSR1);
    } else {
        kill(getppid(), SIGUSR2);
    }
}


void start_new_game(int sig) {
    N = rand() % 100 + 1; 
    guessed = 0;
    printf("\nНачало новой игры, загаданное число: %d\n", N);
}

int main() {
    srand(time(NULL));

    signal(SIGUSR1, guess_handler);
    signal(SIGUSR2, guess_handler);
    signal(SIGINT, start_new_game);

    pid_t pid = fork(); 

    if (pid == 0) {
        while (!guessed) {
            kill(getppid(), SIGUSR1);
            pause();
        }
    } else {
        N = rand() % 100 + 1;
        printf("Предполагаемое число: %d\n", N);

        while (!guessed) {
            pause();
        }
    }

    return 0;
}
