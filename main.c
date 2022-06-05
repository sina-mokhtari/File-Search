#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void *routine(void *string) {
    //  pthread_mutex_lock(&mutex);
    // mails++;
    //    pthread_mutex_unlock(&mutex);
    int *line = malloc(sizeof(int));
    *line = findFirstOccur((char *)string, "abc");
    return (void *)line;
}

int main(int argc, char *argv[]) {
    FILE *f = fopen("text.txt", "r");
    int length;
    char *string;
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

    pthread_t threads[4];
    int *returnValue;

    for (int i = 0; i < 4; i++)
        if (pthread_create(&threads[i], NULL, &routine, string) != 0) return 1;

    // pthread_t p1, p2, p3, p4;
    // pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 4; i++)
        if (pthread_join(threads[i], (void **)&returnValue) != 0)
            perror("thread fail!");

    printf("%d\n", *returnValue);
    // if (pthread_join(p4, NULL) != 0) {
    //     return 8;
    // }
    // pthread_mutex_destroy(&mutex);
    // printf("Number of mails: %d\n", mails);
    return 0;
}