#include <stdio.h>
#include <string.h>
#include <malloc.h>

#pragma pack(push, 1)

struct header{
    unsigned char type[3];
    unsigned char version;
    unsigned char subversion;
    unsigned char flags;
    unsigned char size[4];
};

struct frameHeader{
    unsigned char id[4];
    unsigned char size[4];
    unsigned char flags[2];
};

#pragma pack(pop)

int main(int argc, char *argv[]) {
    char *fileName = (char *) malloc(128);
    strncpy(fileName, argv[1] + 11, 128);
    FILE *fileInput = fopen(fileName, "rb");
    if (fileInput != NULL) {
        struct header myHeader;
        fread(&myHeader, sizeof(struct header), 1, fileInput);
        int tagSize = 0;
        int uSynchronisation = 0;
        int extendedHeader = 0;
        if (strncmp(myHeader.type, "ID3", 3) == 0) {
            if (myHeader.version == 3) {
                for (int i = 3; i >= 0; i--) {
                    tagSize += ((int) myHeader.size[i]) << (7 * (3 - i));
                }
                if (myHeader.flags & (1 << 0)) uSynchronisation = 1;
                if (myHeader.flags & (1 << 1)) extendedHeader = 1;
            } else {
                printf("Not 3rd version");
                return 0;
            }
        } else {
            printf("No ID3 tag");
            return 0;
        }
        //printf("type: %s\n", myHeader.type);
        //printf("version: %d.%d\n", myHeader.version, myHeader.subversion);
        //printf("flags: uSynchronisation=%d extendedHeader=%d\n", uSynchronisation, extendedHeader);
        //printf("size: %d\n", tagSize);

        char *command = (char *) malloc(6);
        strncpy(command, argv[2], 6);
        if (strncmp(command, "--show", 6) == 0) {
            for (int bytes = 0; bytes < tagSize; bytes++) {
                struct frameHeader myFrameHeader;
                int frameFieldSize = 0;
                fread(&myFrameHeader, sizeof(struct frameHeader), 1, fileInput);
                bytes += 10;
                if (myFrameHeader.id[0] == 0) break;
                printf("%s: ", myFrameHeader.id);
                for (int i = 3; i >= 0; i--) {
                    frameFieldSize += ((int) myFrameHeader.size[i]) << (7 * (3 - i));
                }
                for (int i = 0; i < frameFieldSize; i++) {
                    printf("%c", getc(fileInput));
                }
                //printf(" | size: %d", frameFieldSize);
                printf("\n");
                bytes += frameFieldSize;
            }
            fclose(fileInput);
        } else if (strncmp(command, "--get", 5) == 0) {
            char *targetFrameId = (char *) malloc(4);
            strncpy(targetFrameId, argv[2] + 6, 4);
            for (int bytes = 0; bytes < tagSize; bytes++) {
                struct frameHeader myFrameHeader;
                int frameFieldSize = 0;
                fread(&myFrameHeader, sizeof(struct frameHeader), 1, fileInput);
                bytes += 10;
                for (int i = 3; i >= 0; i--) {
                    frameFieldSize += ((int) myFrameHeader.size[i]) << (7 * (3 - i));
                }
                if (strcmp(myFrameHeader.id, targetFrameId) == 0) {
                    printf("%s: ", myFrameHeader.id);
                    for (int i = 0; i < frameFieldSize; i++) {
                        printf("%c", getc(fileInput));
                    }
                    //printf(" | size: %d\n", frameFieldSize);
                    return 0;
                } else {
                    for (int i = 0; i < frameFieldSize; i++) {
                        getc(fileInput);
                    }
                }
                bytes += frameFieldSize;
            }
            fclose(fileInput);
        } else if (strncmp(command, "--set", 5) == 0) {
            char *newTag = (char *) malloc(tagSize);
            int index = 0;
            for (int i = 0; i < 3; i++) newTag[index++] = myHeader.type[i];
            newTag[index++] = myHeader.version;
            newTag[index++] = myHeader.subversion;
            newTag[index++] = myHeader.flags;
            for (int i = 0; i < 4; i++) newTag[index++] = myHeader.size[i];

            char *targetFrameId = (char *) malloc(4);
            strncpy(targetFrameId, argv[2] + 6, 4);
            char *value = (char *) malloc(128);
            strncpy(value, argv[3] + 8, 128);

            int found = 0;
            int byte;
            for (int bytes = 0; bytes < tagSize; bytes++) {
                struct frameHeader myFrameHeader;
                int frameFieldSize = 0;
                fread(&myFrameHeader, sizeof(struct frameHeader), 1, fileInput);
                bytes += 10;
                for (int i = 3; i >= 0; i--) {
                    frameFieldSize += ((int) myFrameHeader.size[i]) << (7 * (3 - i));
                }
                if (strcmp(myFrameHeader.id, targetFrameId) == 0) {
                    found = 1;
                    int newFrameFieldSize = strlen(value) + 1;
                    int temporary = newFrameFieldSize;
                    for (int i = 0; i < 4; i++) {
                        myFrameHeader.size[i] = temporary >> (7 * (3 - i));
                        temporary = temporary - (myFrameHeader.size[i] << (7 * (3 - i)));
                    }
                    for (int i = 0; i < 4; i++) newTag[index++] = myFrameHeader.id[i];
                    for (int i = 0; i < 4; i++) newTag[index++] = myFrameHeader.size[i];
                    for (int i = 0; i < 2; i++) newTag[index++] = myFrameHeader.flags[i];
                    newTag[index++] = 0; //кодировка
                    for (int i = 0; i < newFrameFieldSize - 1; i++) {
                        newTag[index++] = value[i];
                    }
                    for (int i = 0; i < frameFieldSize; i++) getc(fileInput);
                    tagSize = tagSize - frameFieldSize + newFrameFieldSize;
                } else {
                    for (int i = 0; i < 4; i++) newTag[index++] = myFrameHeader.id[i];
                    for (int i = 0; i < 4; i++) newTag[index++] = myFrameHeader.size[i];
                    for (int i = 0; i < 2; i++) newTag[index++] = myFrameHeader.flags[i];
                    for (int i = 0; i < frameFieldSize; i++) {
                        byte = getc(fileInput);
                        newTag[index++] = byte;
                    }
                }
                bytes += frameFieldSize;
            }

            int temporary = tagSize;
            for (int i = 0; i < 4; i++) {
                newTag[i + 6] = temporary >> (7 * (3 - i));
                temporary = temporary - (newTag[i + 6] << (7 * (3 - i)));
            }

            FILE *fileOutput = fopen("export.mp3", "wb");
            for (int i = 0; i < index; i++) {
                fputc(newTag[i], fileOutput);
            }

            while ((byte = getc(fileInput)) != EOF) {
                fputc(byte, fileOutput);
            }

            fclose(fileInput);
            fclose(fileOutput);
            remove(fileName);
            rename("export.mp3", fileName);
        }
    } else {
        printf("File connection error.");
    }
    return 0;
}