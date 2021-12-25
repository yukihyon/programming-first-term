#include <stdio.h>
#include <stdlib.h>

struct queue{
    int * time;
    int size;
};

int main(int argc, char *argv[]) {
    int window;
    scanf("%d", &window);
    window -= 1;
    char **requests = (char **) malloc (0);
    struct queue logQueue;
    logQueue.time = (int *) malloc(0);
    logQueue.size = 0;
    int maxQue = 0;
    int windowRequestTime[2];
    int countErrors = 0;
    FILE *filePointer;
    filePointer = fopen("access_log_Jul95", "r");
    if (filePointer != NULL) {
        char *logLine = (char *) malloc(1024);
        int days, hours, minutes, seconds;
        int quotes[2];
        int countQotes;
        char errorType;
        while (!feof(filePointer)) {
            while (fgets(logLine, 1024, filePointer)) {
                int dateStart;
                for (int i = 0; i < 1024; i++) {
                    if (logLine[i] == '[') {
                        dateStart = i;
                        break;
                    }
                }
                logQueue.time = (int *) realloc(logQueue.time, (logQueue.size + 1) * sizeof (int));
                days = (logLine[dateStart + 1] - '0') * 10 + (logLine[dateStart + 2] - '0');
                hours = (logLine[dateStart + 13] - '0') * 10 + (logLine[dateStart + 14] - '0');
                minutes = (logLine[dateStart + 16] - '0') * 10 + (logLine[dateStart + 17] - '0');
                seconds = (logLine[dateStart + 19] - '0') * 10 + (logLine[dateStart + 20] - '0');
                logQueue.time[logQueue.size] = (days * 24 * 60 * 60) + (hours * 60 * 60) + (minutes * 60) + seconds;
                logQueue.size += 1;
                if ((logQueue.time[logQueue.size - 1] - logQueue.time[0]) > window) {
                    int newStart = 0;
                    for (int i = 0; i < logQueue.size; i++) {
                        if ((logQueue.time[logQueue.size - 1] - logQueue.time[i]) <= window) {
                            newStart = i;
                            break;
                        }
                    }
                    for (int i = 0; i < logQueue.size - newStart; i++) {
                        logQueue.time[i] = logQueue.time[i + newStart];
                    }
                    for (int i = logQueue.size - 1; i > logQueue.size - 1 - newStart; i--) {
                        logQueue.time[i] = 0;
                    }
                    logQueue.size -= newStart;
                }
                if (logQueue.size > maxQue) {
                    maxQue = logQueue.size;
                    windowRequestTime[0] = logQueue.time[0];
                    windowRequestTime[1] = logQueue.time[maxQue - 1];
                }

                countQotes = 0;
                for (int i = 0; i < 1024; i++) {
                    if (logLine[i] == '"'){
                        quotes[countQotes] = i;
                        countQotes++;
                    }
                    if (countQotes == 2){
                        errorType = logLine[i + 2];
                        break;
                    }
                }
                if (errorType == '5') {
                    int requestSize = quotes[1] - quotes[0] - 1;
                    requests = (char **) realloc(requests, (countErrors + 1) * sizeof(char *));
                    requests[countErrors] = calloc(requestSize, requestSize * sizeof(char));
                    for (int i = 0; i < requestSize; i++) {
                        requests[countErrors][i] = logLine[quotes[0] + 1 + i];
                    }
                    countErrors++;
                }
            }
        }
        printf("Number of 5XX errors: %d\n", countErrors);
        /*
        for (int i = 0; i < countErrors; i++) {
            printf("%s", requests[i]);
            printf("\n");
        }
         */
        printf("Max count of requests in %d seconds: %d\n", window + 1, maxQue);
        for (int i = 0; i < 2; i++) {
            int day = windowRequestTime[i] / (24 * 60 * 60);
            int hour = (windowRequestTime[i] % (24 * 60 * 60)) / (60 * 60);
            int minute = ((windowRequestTime[i] % (24 * 60 * 60)) % (60 * 60)) / 60;
            int second = ((windowRequestTime[i] % (24 * 60 * 60)) % (60 * 60)) % 60;
            printf("Date: %d of July, Time: %d hours %d minutes %d seconds\n", day, hour, minute, second);
        }
    } else {
        printf("File connection error.");
    }
    fclose(filePointer);
}
