#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define THREADS_COUNT 4
#define WORDS_BUFFER 100
#define WORDS_MAX_SIZE 25

pthread_mutex_t mutex;
pthread_t threads[THREADS_COUNT];
char *string;
char words[WORDS_BUFFER][WORDS_MAX_SIZE];
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
    while (fgets(words[i], WORDS_MAX_SIZE, fptr)) {
        words[i][strlen(words[i]) - 1] = '\0';
        i++;
    }
    wordsCount = i;
}

int findFirstOccur(char *wrd) {
    char word[25];
    int n, m, i = 0, j, line = 0;
    int threadNumber;

    strcpy(word, wrd);

    m = strlen(word);

    for (int i = 0; i < THREADS_COUNT; i++)
        if (pthread_self() == threads[i]) {
            threadNumber = i + 1;
            break;
        }

    while (string[i] != '\0') {
        line++;

        while (line < (linesCount / THREADS_COUNT * (threadNumber - 1) + 1) &&
               string[i] != '\0')
            while (string[i] != '\0')
                if (string[i++] == '\n') {
                    line++;
                    break;
                }

        if (line > (linesCount / THREADS_COUNT * threadNumber) &&
            threadNumber < THREADS_COUNT)
            break;

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
    int threadNumber;

    for (int i = 0; i < THREADS_COUNT; i++)
        if (pthread_self() == threads[i]) {
            threadNumber = i + 1;
            break;
        }

    for (int i = 0; i < wordsCount; i++) {
        *myLine = findFirstOccur(words[i]);

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
        if (c == '\n') linesCount = linesCount + 1;
    fclose(f);

    getWords();

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < THREADS_COUNT; i++)
        if (pthread_create(&threads[i], NULL, &routine1, NULL) != 0) return 1;

    for (int i = 0; i < THREADS_COUNT; i++)
        if (pthread_join(threads[i], NULL) != 0) perror("thread fail!");

    fclose(fopen("results.txt", "w"));
    f = fopen("results.txt", "a");
    fprintf(f, "Mutex method with %d threads\n", THREADS_COUNT);
    for (int i = 0; i < wordsCount; i++)
        if (results[i].line != __INT_MAX__)
            fprintf(f,
                    "\"%s\" found on line %d in thread %d in %lf seconds and "
                    "written "
                    "in file in %lf seconds\n",
                    words[i], results[i].line, results[i].thread,
                    (double)(results[i].time - startTime) / CLOCKS_PER_SEC,
                    (double)(clock() - startTime) / CLOCKS_PER_SEC);
    fclose(f);

    pthread_mutex_destroy(&mutex);
    return 0;
}