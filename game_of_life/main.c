#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct cell {
    unsigned char dead;
    unsigned char neighboursAlive;
};

#pragma pack(push, 1)

typedef struct bitmapFileHeader {
    unsigned short type;  //тип файла
    unsigned int size;  //размер файла
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offsetBits;  //отступ от хедера до картинки
};

typedef struct bitmapInfoHeader {
    unsigned int size;  //размер header
    unsigned int width;  //ширина картинки в пикселях
    unsigned int height;  //высотра картинки в пикселях
    unsigned short planes;  //число цыетовых плоскостей
    unsigned short bitPerPixel;  //бит на пиксель
    unsigned int compression;  //степень сжатия
    unsigned int sizeImage;  //размер картинки
    unsigned int xPixelsPerMeter;  //number of pixels per meter in x axis
    unsigned int yPixelsPerMeter;  //number of pixels per meter in y axis
    unsigned int colorUsed;  //количество цветов
    unsigned int colorImportant;  //количество важных цветов
};

typedef struct colorTable{
    unsigned int black;  //кодировка черного цвета
    unsigned int white;  //кодировка белого цвета
};

#pragma pack(pop)

void printBitmapInfo(struct bitmapFileHeader *myBitmapFileHeader, struct bitmapInfoHeader *myBitmapInfoHeader){
    printf("---BitmapFileHeader---\n");
    printf("type: %c%c\n", myBitmapFileHeader->type, (myBitmapFileHeader->type >> 8));
    printf("size: %d \n", myBitmapFileHeader->size);
    printf("reserved 1: %d\n", myBitmapFileHeader->reserved1);
    printf("reserved 1: %d\n", myBitmapFileHeader->reserved2);
    printf("offset: %d \n", myBitmapFileHeader->offsetBits);
    printf("---BitmapInfoHeader---\n");
    printf("size: %d bytes\n", myBitmapInfoHeader->size);
    printf("width: %d pixels\n", myBitmapInfoHeader->width);
    printf("height: %d pixels\n", myBitmapInfoHeader->height);
    printf("planes: %d \n", myBitmapInfoHeader->planes);
    printf("bits per pixel: %d \n", myBitmapInfoHeader->bitPerPixel);
    printf("compression: %d \n", myBitmapInfoHeader->compression);
    printf("size of image: %d \n", myBitmapInfoHeader->sizeImage);
    printf("pixels per meter in x: %d \n", myBitmapInfoHeader->xPixelsPerMeter);
    printf("pixels per meter in y: %d \n", myBitmapInfoHeader->yPixelsPerMeter);
    printf("number of colors: %d \n", myBitmapInfoHeader->colorUsed);
    printf("number of important colors: %d \n", myBitmapInfoHeader->colorImportant);
}

void newNeighborsCount(struct cell *bitmap, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            (bitmap + y * width + x)->neighboursAlive = 0;
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((bitmap + y * width + x)->dead == 0) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) continue;
                        int neighbor = (y + i) * width + (x + j);
                        if ((x + j >= 0) && (y + i >= 0) && (x + j < width) && (y + i < height)) {
                            (bitmap + neighbor)->neighboursAlive++;
                        }
                    }
                }
            }
        }
    }

}

struct cell *LoadMyBitmap(char *filename,
                        struct bitmapFileHeader *myBitmapFileHeader,
                        struct bitmapInfoHeader *myBitmapInfoHeader,
                        struct colorTable *myColorTable) {

    FILE *fileInput = fopen(filename,"rb");
    if (fileInput == NULL) {
        printf("File connection error");
        return NULL;
    }
    fread(myBitmapFileHeader, sizeof(struct bitmapFileHeader), 1, fileInput);
    fread(myBitmapInfoHeader, sizeof(struct bitmapInfoHeader), 1, fileInput);
    fread(myColorTable, sizeof(struct colorTable), 1, fileInput);
    //printBitmapInfo(myBitmapFileHeader, myBitmapInfoHeader);
    unsigned int pixelCount = myBitmapInfoHeader->width * myBitmapInfoHeader->height;
    struct cell *bitmap = (struct cell *) malloc (pixelCount * sizeof(struct cell));
    fseek(fileInput, myBitmapFileHeader->offsetBits, SEEK_SET);

    unsigned int bits;
    unsigned int notSwappedBits;
    struct cell *cellPointer;
    for (unsigned int y = 0; y < myBitmapInfoHeader->height; y++) {
        int x;
        int lastX = 0;
        for (int bit = 0; bit < myBitmapInfoHeader->width; bit += 32) {
            fread(&notSwappedBits, sizeof(notSwappedBits), 1, fileInput);
            bits = ((notSwappedBits >> 24) & 0xff) |
                    ((notSwappedBits << 8) & 0xff0000) |
                    ((notSwappedBits >> 8) & 0xff00) |
                    ((notSwappedBits << 24) & 0xff000000);
            for (x = 0; x < 32; x++) {
                if ((x + lastX) >= myBitmapInfoHeader->width) break;
                cellPointer = (bitmap + y * myBitmapInfoHeader->width + x + lastX);
                cellPointer->dead = (bits >> (31 - x)) & 1u;
            }
            lastX = lastX + x;
        }
    }
    newNeighborsCount(bitmap, myBitmapInfoHeader->width, myBitmapInfoHeader->height);
    fclose(fileInput);
    return bitmap;
}

void step(struct cell *bitmap, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            struct cell *cellPointer = (bitmap + y * width + x);
            if (cellPointer->dead == 0) {
                if ((cellPointer->neighboursAlive < 2) || (cellPointer->neighboursAlive > 3)) {
                    cellPointer->dead = 1;
                }
            } else {
                if (cellPointer->neighboursAlive == 3) {
                    cellPointer->dead = 0;
                }
            }
        }
    }
    newNeighborsCount(bitmap, width, height);
}

void printBitmap(struct cell *bitmap, int width, int height){
    for (int y = 0; y < height; y++) {
        printf("[ ");
        for (int x = 0; x < width; x++) {
            printf("%d ", (bitmap + y * width + x)->dead);
        }
        printf("]\n");
    }
}

SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
SDL_Surface *bmp = NULL;

int main(int argc, char *argv[]) {
    if (argc == 0) {
        printf("No arguments");
        return 0;
    }
    char startFilename[1024];
    if (strcmp(argv[1], "--input") == 0) snprintf(startFilename, 1024, "%s", argv[2]);
    if (strcmp(argv[3], "--input") == 0) snprintf(startFilename, 1024, "%s", argv[4]);
    char outputDir[1024];
    if (strcmp(argv[1], "--output") == 0) snprintf(outputDir, 1024, "%s", argv[2]);
    if (strcmp(argv[3], "--output") == 0) snprintf(outputDir, 1024, "%s", argv[4]);

    long long maxGeneration = INFINITE;
    unsigned int frequency = 1;
    unsigned int speed = 100;
    for (int i = 5; i <= 9; i+=2) {
        if (argc >= i + 2) {
            if (strcmp(argv[i], "--max_iter") == 0) maxGeneration = atoi(argv[i + 1]);
            if (strcmp(argv[i], "--dump_freq") == 0) frequency = atoi(argv[i + 1]);
            if (strcmp(argv[i], "--speed") == 0) speed = atoi(argv[i + 1]);
        }
    }

    struct bitmapFileHeader myBitmapFileHeader;
    struct bitmapInfoHeader myBitmapInfoHeader;
    struct colorTable myColorTable;
    struct cell *bitmap;
    bitmap = LoadMyBitmap(startFilename, &myBitmapFileHeader, &myBitmapInfoHeader, &myColorTable);
    window = SDL_CreateWindow("Game of life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, myBitmapInfoHeader.width, myBitmapInfoHeader.height, SDL_WINDOW_SHOWN);
    screenSurface = SDL_GetWindowSurface(window);
    bmp = SDL_LoadBMP(startFilename);
    SDL_BlitSurface(bmp, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
    Sleep(speed);

    SDL_Event event;
    bool run = true;
    long long iteration = 1;
    while(run) {
        for (; iteration <= maxGeneration; iteration++) {
            while(SDL_PollEvent( &event ) != 0) {
                if(event.type == SDL_QUIT) {
                    run = false;
                    iteration = maxGeneration;
                }
            }
            step(bitmap, myBitmapInfoHeader.width, myBitmapInfoHeader.height);
            if (iteration % frequency != 0) continue;

            char filename[1024];
            snprintf(filename, 1024, "%s%s%d%s%s", outputDir, "/", iteration, "-iter-of-", startFilename);
            FILE *fileOutput = fopen(filename, "wb");
            fwrite(&myBitmapFileHeader, sizeof(struct bitmapFileHeader), 1, fileOutput);
            fwrite(&myBitmapInfoHeader, sizeof(struct bitmapInfoHeader), 1, fileOutput);
            fwrite(&myColorTable, sizeof(struct colorTable), 1, fileOutput);

            for (int y = 0; y < myBitmapInfoHeader.height; y++) {
                int x;
                int lastX = 0;
                for (int bit = 0; bit < myBitmapInfoHeader.width; bit += 32) {
                    unsigned int notSwappedBits;
                    unsigned int bits;
                    for (x = 0; x < 32; x++) {
                        if ((x + lastX) >= myBitmapInfoHeader.width) {
                            notSwappedBits = notSwappedBits << 1;
                        } else {
                            notSwappedBits = notSwappedBits << 1;
                            notSwappedBits += (bitmap + y * myBitmapInfoHeader.width + x + lastX)->dead;
                        }
                    }
                    lastX = lastX + x;

                    bits = ((notSwappedBits >> 24) & 0xff) |
                           ((notSwappedBits << 8) & 0xff0000) |
                           ((notSwappedBits >> 8) & 0xff00) |
                           ((notSwappedBits << 24) & 0xff000000);
                    fwrite(&bits, sizeof(unsigned int), 1, fileOutput);
                }
            }
            fclose(fileOutput);
            SDL_FreeSurface(bmp);
            bmp = SDL_LoadBMP(filename);
            SDL_BlitSurface(bmp, NULL, screenSurface, NULL);
            SDL_UpdateWindowSurface(window);
            Sleep(speed);
        }
        while(SDL_PollEvent( &event ) != 0) {
            if(event.type == SDL_QUIT) {
                run = false;
            }
        }
    }
    SDL_FreeSurface(bmp);
    bmp = NULL;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}