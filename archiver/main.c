#include <stdio.h>
#include <string.h>
#include <direct.h>

#pragma pack(push, 1)

typedef struct archiveHeader {
    unsigned int filesCount;  //количество файлов в архиве
};

typedef struct archiveFileHeader {
    unsigned int fileNameSize; //размер имени файла
    unsigned int fileSize; //размер файла
};

#pragma pack(pop)

void createArchive(int argc, char *argv[]) {
    FILE *fileOutput = fopen(argv[2], "wb");
    struct archiveHeader myArchiveHeader;
    myArchiveHeader.filesCount = argc - 4;
    fwrite(&myArchiveHeader, sizeof(struct archiveHeader), 1, fileOutput);
    for (int i = 4; i < argc; i++) {
        struct archiveFileHeader myArchiveFileHeader;
        FILE *fileInput = fopen(argv[i], "rb");
        myArchiveFileHeader.fileNameSize = strlen(argv[i]);
        fseek(fileInput, 0L, SEEK_END);
        myArchiveFileHeader.fileSize = ftell(fileInput);
        fseek(fileInput, 0L, SEEK_SET);
        fwrite(&myArchiveFileHeader, sizeof(struct archiveFileHeader), 1, fileOutput);
        fwrite(argv[i], myArchiveFileHeader.fileNameSize, 1, fileOutput);
        int byte;
        while ((byte = getc(fileInput)) != EOF) {
            fputc(byte, fileOutput);
        }
        fclose(fileInput);
        remove(argv[i]);
    }
    fclose(fileOutput);
}

void extractArchive(int argc, char *argv[]) {
    FILE *fileInput = fopen(argv[2], "rb");
    struct archiveHeader myArchiveHeader;
    fread(&myArchiveHeader, sizeof(struct archiveHeader), 1, fileInput);
    char dirName[1024];
    for (int i = 0; i < strlen(argv[2]) - 4; i++) {
        dirName[i] = argv[2][i];
    }
    mkdir(dirName);
    for (int i = 0; i < myArchiveHeader.filesCount; i++) {
        struct archiveFileHeader myArchiveFileHeader;
        fread(&myArchiveFileHeader, sizeof(struct archiveFileHeader), 1, fileInput);
        char filename[myArchiveFileHeader.fileNameSize + 1];
        for (int i = 0; i < myArchiveFileHeader.fileNameSize; i++) {
            filename[i] = getc(fileInput);
        }
        filename[myArchiveFileHeader.fileNameSize] = '\0';
        char path[1024];
        snprintf(path, 1024, "%s%s%s", dirName, "//", filename);
        FILE *fileOutput = fopen(path, "wb");
        int byte;
        for (int i = 0; i < myArchiveFileHeader.fileSize; i++) {
            byte = getc(fileInput);
            fputc(byte, fileOutput);
        }
        fclose(fileOutput);
    }
    fclose(fileInput);
    remove(argv[2]);
}

void listOfFilesInArchive(int argc, char *argv[]) {
    printf("List of files in archive:\n");
    FILE *fileInput = fopen(argv[2], "rb");
    struct archiveHeader myArchiveHeader;
    fread(&myArchiveHeader, sizeof(struct archiveHeader), 1, fileInput);
    for (int i = 0; i < myArchiveHeader.filesCount; i++) {
        struct archiveFileHeader myArchiveFileHeader;
        fread(&myArchiveFileHeader, sizeof(struct archiveFileHeader), 1, fileInput);
        char filename[myArchiveFileHeader.fileNameSize + 1];
        for (int i = 0; i < myArchiveFileHeader.fileNameSize; i++) {
            filename[i] = getc(fileInput);
        }
        filename[myArchiveFileHeader.fileNameSize] = '\0';
        printf("%d) %-20s size %d bytes\n", i + 1, filename, myArchiveFileHeader.fileSize);
        fseek(fileInput, myArchiveFileHeader.fileSize, SEEK_CUR);
    }
}

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "--file") == 0) {
        if (strcmp(argv[3], "--create") == 0) {
            createArchive(argc, argv);
        }
        else if (strcmp(argv[3], "--extract") == 0) {
            extractArchive(argc, argv);
        }
        else if (strcmp(argv[3], "--list") == 0) {
            listOfFilesInArchive(argc, argv);
        } else {
            printf("Invalid command");
        }
    } else {
        printf("No archive file given");
    }
    return 0;
}
