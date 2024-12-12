#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_NAME1 "fifo1" 
#define FIFO_NAME2 "fifo2"

int main() {
    srand(time(NULL));

    if (mkfifo(FIFO_NAME1, 0666) == -1) {
        perror("Ошибка создания канала fifo1");
        exit(1);
    }
    if (mkfifo(FIFO_NAME2, 0666) == -1) {
        perror("Ошибка создания канала fifo2");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {

        int low = 1, high = 100, guess, attempts = 0;
        int correct = 0;
        while (1) {
            guess = (low + high) / 2;
            attempts++;
            printf("Попытка угадывания %d: %d\n", attempts, guess);

            int fd = open(FIFO_NAME1, O_WRONLY);
            write(fd, &guess, sizeof(guess));
            close(fd);

            fd = open(FIFO_NAME2, O_RDONLY);
            read(fd, &correct, sizeof(correct));
            close(fd);

            if (correct == 0) {
                printf("Число меньше!\n");
                high = guess - 1;
            } else if (correct == 1) {
                printf("Число больше!\n");
                low = guess + 1;
            } else if (correct == 2) {
                printf("Число было отгаданно через %d попыток\n", attempts);
                break;
            }
        }
    } else {
        int number_to_guess, guess;
        while (1) {
            number_to_guess = rand() % 100 + 1;
            int attempts = 0;
            printf("Предполагаемое число: %d\n", number_to_guess);

            int fd = open(FIFO_NAME1, O_RDONLY);
            read(fd, &guess, sizeof(guess));
            close(fd);

            attempts++;
            printf("Попыток %d: Загаданное число %d\n", attempts, guess);

            if (guess == number_to_guess) {
                int correct = 2;
                fd = open(FIFO_NAME2, O_WRONLY);
                write(fd, &correct, sizeof(correct));
                close(fd);
                printf("Число угадано верно.\n");
                break;
            } else if (guess < number_to_guess) {
                int correct = 1;
                fd = open(FIFO_NAME2, O_WRONLY);
                write(fd, &correct, sizeof(correct));
                close(fd);
            } else if (guess > number_to_guess) {
                int correct = 0;
                fd = open(FIFO_NAME2, O_WRONLY);
                write(fd, &correct, sizeof(correct));
                close(fd);
            }
        }
    }

    unlink(FIFO_NAME1);
    unlink(FIFO_NAME2);

    return 0;
}
