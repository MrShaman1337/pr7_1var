#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_NAME1 "fifo1"
#define FIFO_NAME2 "fifo2"

int main() {
    if (mkfifo(FIFO_NAME1, 0666) == -1) {
        perror("Ошибка создания fifo1");
        exit(1);
    }
    if (mkfifo(FIFO_NAME2, 0666) == -1) {
        perror("Ошибка создания fifo2");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {

        int guess, attempts = 0;
        while (1) {
 
            int fd = open(FIFO_NAME2, O_RDONLY);
            read(fd, &guess, sizeof(guess));
            close(fd);

   
            guess = rand() % 100 + 1;
            attempts++;
            printf("Попытка отгадывания %d: %d\n", attempts, guess);


            fd = open(FIFO_NAME1, O_WRONLY);
            write(fd, &guess, sizeof(guess));
            close(fd);

            fd = open(FIFO_NAME2, O_RDONLY);
            int correct;
            read(fd, &correct, sizeof(correct));
            close(fd);

            if (correct) {
                printf("Число было подобрано на %d попытку\n", attempts);
                break; 
            }
        }
    } else {

        int number_to_guess, attempts = 0;
        while (1) {
            number_to_guess = rand() % 100 + 1;
            attempts = 0; 
            printf("Загаданное число: %d\n", number_to_guess);

            int fd = open(FIFO_NAME1, O_RDONLY);
            int guess;
            read(fd, &guess, sizeof(guess));
            close(fd);

            attempts++;
            int correct = (guess == number_to_guess);
            printf("Попытка %d: %d\n", attempts, guess);

            fd = open(FIFO_NAME2, O_WRONLY);
            write(fd, &correct, sizeof(correct));
            close(fd);

            if (correct) {
                printf("Число было подобрано на %d попытку\n", attempts);
                break;
            }
        }
    }


    unlink(FIFO_NAME1);
    unlink(FIFO_NAME2);

    return 0;
}
