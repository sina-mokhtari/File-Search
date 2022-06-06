#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define WORDS_BUFFER 100

/*
int occur() {
    char wrd[256], buffer[256];
    int n, m, i, j, line;

    FILE *fp;
    fp = fopen("text.txt", "r");  // open file

    printf("Enter the word you want to search in the file: ");
    gets(wrd);

    m = strlen(wrd);  // length of input word

    printf("All positions of word \"%s\" in the file\n", wrd);

    line = 0;

    while (fgets(buffer, 256, fp) != NULL) {
        line++;

        if (line <) i = 0;
        n = strlen(buffer);
        // the followinf loop find position of the input word in the current
        // line and print the position of the word on the screen the loop
        // basically reads each word of the file and compare it with the input
        // word
        while (i < n) {
            // comparing current word with input word
            j = 0;
            while (i < n && j < m && buffer[i] == wrd[j]) {
                ++i, ++j;
            }

            // the following condition implies that the current word of buffer
            // is equal to input word
            if ((i == n || buffer[i] == ' ' || buffer[i] == '\n') && j == m) {
                printf("Line: %d ", line);
                printf("Column: %d\n", i - m);
            }

            // moving to next word
            while (i < n && buffer[i] != ' ') {
                ++i;
            }
            ++i;
        }
    }

    fclose(fp);
}
*/
int mails = 0;
pthread_mutex_t mutex;
pthread_t threads[4];
char *string;
char words[WORDS_BUFFER][25];
int length;
int wordsCount = 0;
int linesCount = 1;
clock_t startTime;

typedef struct {
    int line;
    int thread;
    clock_t time;
} result;

result results[WORDS_BUFFER] = {[0 ... WORDS_BUFFER - 1] = {__INT_MAX__, 0, 0}};

void getWords() {
    FILE *fptr = NULL;
    int i = 0;

    fptr = fopen("words.txt", "r");
    while (fgets(words[i], 25, fptr)) {
        words[i][strlen(words[i]) - 1] = '\0';
        i++;
    }
    wordsCount = i;
}

int findFirstOccur(char *str, char *wrd) {
    char word[25];  // string[131072];
    int n, m, i = 0, j, line = 0;

    // strcpy(string, str);
    strcpy(word, wrd);

    m = strlen(word);  // length of input word

    int threadNumber;
    if (pthread_self() == threads[0]) threadNumber = 1;
    if (pthread_self() == threads[1]) threadNumber = 2;
    if (pthread_self() == threads[2]) threadNumber = 3;
    if (pthread_self() == threads[3]) threadNumber = 4;

    while (string[i] != '\0') {
        line++;

        while (line < (linesCount / 4 * (threadNumber - 1) + 1) &&
               string[i] != '\0')
            while (string[i] != '\0')
                if (string[i++] == '\n') {
                    line++;
                    break;
                }

        if (line > (linesCount / 4 * threadNumber) && threadNumber < 4) break;

        while (string[i] != '\n' && string[i] != '\0') {
            j = 0;
            while (string[i] == word[j] && j < m && string[i] != '\n' &&
                   string[i] != '\0') {
                i++, j++;
            }

            if (j == m &&
                (string[i] == ' ' || string[i] == '\n' || string[i] == '\0')) {
                return (line);
            }

            while (string[i] != ' ' && string[i] != '\n' && string[i] != '\0') {
                i++;
            }
            if (string[i] == ' ') i++;
        }
        if (string[i] == '\n') i++;
    }
    return 0;
}

void *routine1(void *args) {
    int *myLine = malloc(sizeof(int));
    char quarterStr[1000];

    int threadNumber;
    if (pthread_self() == threads[0]) threadNumber = 1;
    if (pthread_self() == threads[1]) threadNumber = 2;
    if (pthread_self() == threads[2]) threadNumber = 3;
    if (pthread_self() == threads[3]) threadNumber = 4;
    /*
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
        }*/

    for (int i = 0; i < wordsCount; i++) {
        *myLine = findFirstOccur(quarterStr, words[i]);

        if (*myLine != 0) {
            pthread_mutex_lock(&mutex);
            if (results[i].line > *myLine) {
                results[i].line = *myLine;
                results[i].thread = threadNumber;
                results[i].time = clock();
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    // return (void *)line;
}

int main(int argc, char *argv[]) {
    startTime = clock();
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

    f = fopen("text.txt", "r");
    char c;
    for (c = getc(f); c != EOF; c = getc(f))
        if (c == '\n')  // Increment count if this character is newline
            linesCount = linesCount + 1;

    fclose(f);

    getWords();
    // int quarterSize = length / 4;

    int *returnValue;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 4; i++)
        if (pthread_create(&threads[i], NULL, &routine1, NULL) != 0) return 1;

    // for (int i = 0; i < 4; i++) printf("thread:%ld\n", threads[i]);

    // pthread_t p1, p2, p3, p4;

    for (int i = 0; i < 4; i++)
        if (pthread_join(threads[i], NULL) != 0) perror("thread fail!");

    fclose(fopen("results.txt", "w"));
    f = fopen("results.txt", "a");
    for (int i = 0; i < wordsCount; i++)
        if (results[i].line != __INT_MAX__) /* Write content to file */
            fprintf(f,
                    "%s found on line %d in thread %d on time %lf and written "
                    "in file on time %lf\n",
                    words[i], results[i].line, results[i].thread,
                    (double)(results[i].time - startTime) / CLOCKS_PER_SEC,
                    (double)(clock() - startTime) / CLOCKS_PER_SEC);
    fclose(f);

    pthread_mutex_destroy(&mutex);
    //  printf("Number of mails: %d\n", mails);
    return 0;
}