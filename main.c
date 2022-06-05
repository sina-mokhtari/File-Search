#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int findFirstOccur(char *str, char *wrd) {
    char word[25], string[131072];
    int n, m, i = 0, j, line;

    strcpy(string, str);
    strcpy(word, wrd);

    m = strlen(word);  // length of input word

    line = 1;

    while (string[i] != '\0') {
        while (string[i] != '\n' && string[i] != '\0') {
            j = 0;
            while (string[i] == word[j] && j < m && string[i] != '\n' &&
                   string[i] != '\0') {
                i++, j++;
            }

            if (j == m &&
                (string[i] == ' ' || string[i] == '\n' || string[i] == '\0')) {
                return (line);
                // printf("Column: %d\n", i - m);
            }

            while (string[i] != ' ' &&
                   (string[i] == '\n' || string[i] == '\0')) {
                i++;
            }
            i++;
        }
        if (string[i] == '\n') i++;
        line++;
    }
    return 0;
}

int mails = 0;
pthread_mutex_t mutex;
pthread_t threads[4];
char *string;
int length;

void *routine1(void *args) {
    // printf("shit");
    char *word = "abc";
    int *line = malloc(sizeof(int));
    char quarterStr[1000];

    int threadNumber = 5;
    if (pthread_self() == threads[0]) threadNumber = 1;
    if (pthread_self() == threads[1]) threadNumber = 2;
    if (pthread_self() == threads[2]) threadNumber = 3;
    if (pthread_self() == threads[3]) threadNumber = 4;

    int quarterSize = length / 4;
    switch (threadNumber) {
        case 1:
            strncpy(quarterStr, string, quarterSize);
            quarterStr[quarterSize] = '\0';
            break;
        case 2:
            strncpy(quarterStr, string + quarterSize, quarterSize);
            quarterStr[quarterSize] = '\0';
            break;
        case 3:
            strncpy(quarterStr, string + quarterSize * 2, quarterSize);
            quarterStr[quarterSize] = '\0';
            break;
        case 4:
            strncpy(quarterStr, string + quarterSize * 3,
                    length - quarterSize * 3);
            quarterStr[length - quarterSize * 3] = '\0';
            break;
        default:
            printf("shit");
            return NULL;
            break;
    }
    *line = findFirstOccur(quarterStr, word);
    if (*line != 0) {
        printf("%s word found on line %d in thread %d\n", word, *line,
               threadNumber);
    }
    // return (void *)line;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("text.txt", "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        string = malloc(length);
        if (string) {
            fread(string, 1, length, f);
        }
        fclose(f);
    }
    // int quarterSize = length / 4;

    int *returnValue;

    for (int i = 0; i < 4; i++)
        if (pthread_create(&threads[i], NULL, &routine1, NULL) != 0) return 1;

    // for (int i = 0; i < 4; i++) printf("thread:%ld\n", threads[i]);

    // pthread_t p1, p2, p3, p4;
    // pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 4; i++)
        if (pthread_join(threads[i], NULL) != 0) perror("thread fail!");

    // printf("%d\n", *returnValue);
    // printf("thread id: %ld\n", syscall(__NR_gettid));
    //  if (pthread_join(p4, NULL) != 0) {
    //      return 8;
    //  }
    //  pthread_mutex_destroy(&mutex);
    //  printf("Number of mails: %d\n", mails);
    return 0;
}