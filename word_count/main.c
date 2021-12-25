#include <stdio.h>
#include <string.h>
#define WORD 1 /* Состояние если указатель находится внутри слова*/
#define SPACE 0 /* Состояние если указатель не находится внутри слова*/

int main(int argc, char *argv[]) {
    int character, wordsCount = 0, linesCount = 0, bytesCount = 0, state = SPACE;
    FILE *filePointer;
    filePointer = fopen(argv[2], "r");
    if(filePointer != NULL) {
        if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--bytes") == 0)) {
            while ((character = getc(filePointer)) != EOF) {
                if (character == '\n') {
                    bytesCount += 2;
                } else {
                    bytesCount += 1;
                }
            }
            printf("%d", bytesCount);
        } else if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--lines") == 0)) {
            int previous;
            int blank = 1;
            while ((character = getc(filePointer)) != EOF) {
                blank = 0;
                if (character == '\n') {
                    if (previous != '\n') {
                        linesCount++;
                    }
                }
                previous = character;
            }
            if ((previous != '\n')&&(blank != 1)) {
                linesCount++;
            }
            printf("%d", linesCount);
        } else if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--words") == 0)) {
            while ((character = getc(filePointer)) != EOF) {
                if ((character != ' ') && (character != '\n') && (character != '\t')) {
                    state = WORD;
                }
                else if (state == WORD) {
                    state = SPACE;
                    wordsCount++;
                }
            }
            if (state == WORD) {
                wordsCount++;
            }
            printf("%d", wordsCount);
        }
    } else {
        printf("File connection error.");
    }
    fclose(filePointer);
}
