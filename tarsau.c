#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

typedef long int FileSize;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    char access[11]; 
    FileSize size;
} FileDetails;

// Function to create a directory
void makeDir(const char *dirPath){
    int status = mkdir(dirPath, S_IRWXU | S_IRWXG | S_IRWXO);
    if(status != 0) {
        perror("Directory creation failed");
        exit(1);
    }
}

// Function to fetch details of a file
void fetchFileInfo(const char *filePath, FileDetails *file) {
    struct stat fileStats;

    if (stat(filePath, &fileStats) == -1) {
        perror("Failed to retrieve file details");
        exit(1);
    }

    strcpy(file->name, filePath);
    snprintf(file->access, sizeof(file->access), "%o", fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    file->size = fileStats.st_size;
}

// Function to combine multiple files into one
void combineFiles(int fileCount, char *filePaths[], char *mergedName){
    FileSize combinedSize = 0;
    FILE *mergedFile = fopen(mergedName, "w");

    if (!mergedFile) {
        perror("Failed to open output file");
        exit(1);
    }

    // Calculate combined size of all files
    for (int i = 2; i < fileCount - 2; i++) {
        FileDetails file;
        fetchFileInfo(filePaths[i], &file);
        combinedSize += file.size;
    }

    // Write combined size to the merged file
    fprintf(mergedFile, "%010ld", combinedSize);

    // Write file details to the merged file
    for (int i = 2; i < fileCount - 2; i++) {
        FileDetails file;
        fetchFileInfo(filePaths[i], &file);
        fprintf(mergedFile, "|%s, %s, %ld", file.name, file.access, file.size);
    }

    // Append file contents to the merged file
    for (int i = 2; i < fileCount - 2; i++) { 
        FileDetails file;
        FILE *inputFile = fopen(filePaths[i], "r");
        fetchFileInfo(filePaths[i], &file);
        char* contentBuffer = (char *)malloc(file.size + 1);
        fread(contentBuffer, 1, file.size, inputFile);
        strcat(contentBuffer, "\n");
        fwrite(contentBuffer, 1, file.size, mergedFile);
        fclose(inputFile);
        free(contentBuffer);
    }

    fclose(mergedFile);
}

// Function to unpack files from an archive
void unpackFiles(char *archiveName, char *targetDir) {
    makeDir(targetDir);
    FILE *archiveFile = fopen(archiveName, "r");
    if (!archiveFile) {
        perror("Failed to open archive");
        return;
    }

    // Determine archive file size
    fseek(archiveFile, 0, SEEK_END);
    long fileSize = ftell(archiveFile);
    fseek(archiveFile, 0, SEEK_SET);

    // Allocate memory for the archive content
    char *buffer = (char *)malloc(fileSize + 1);
    fread(buffer, 1, fileSize, archiveFile);
    buffer[fileSize] = '\0';

    const char delimiters[] = "|,";
    char *token = strtok(buffer, delimiters);

    FileDetails fileInfo;
    FileDetails *fileInfoArray = NULL;
    int fileCount = 0;
    int totalSize = 0;

    // Parse archive content and extract file details
    while (token != NULL) {
        if (strstr(token, ".txt") != NULL) {
            strcpy(fileInfo.name, token);
            token = strtok(NULL, delimiters);
            strcpy(fileInfo.access, token);
            token = strtok(NULL, delimiters);
            if (token != NULL && atoi(token) != 0) {
                fileInfo.size = atoi(token);
                totalSize += fileInfo.size;
                char *content = (char *)malloc(fileInfo.size);
                fread(content, 1, fileInfo.size, archiveFile);
                free(content);
            }
            fileInfoArray = realloc(fileInfoArray, (fileCount + 1) * sizeof(FileDetails));
            fileInfoArray[fileCount++] = fileInfo;
        }
        token = strtok(NULL, delimiters);
    }

    int cumulativeSize = fileSize;
    for(int i = fileCount - 1; i >= 0; --i){
        cumulativeSize += fileInfoArray[i].size;
    }

    // Extract files from the archive
    int nextPosition = fileSize;
    for (int i = fileCount - 1; i >= 0; i--) {
        char *destinationPath = (char *)malloc(strlen(fileInfoArray[i].name) + strlen(targetDir) + strlen(archiveName) + 1);
        sprintf(destinationPath, "%s/%s", targetDir, fileInfoArray[i].name);
        FILE *destinationFile = fopen(destinationPath, "wb");
        int offset = nextPosition - fileInfoArray[i].size;
        for(int j = offset; j < nextPosition; j++){
            fseek(archiveFile, j, SEEK_SET);
            fputc(fgetc(archiveFile), destinationFile);
        }
        nextPosition = offset;
        fclose(destinationFile);
        free(destinationPath);
    }

    // Display extraction summary
    for(int i = fileCount - 1; i >= 0; --i){
        printf("%s, ", fileInfoArray[i].name);
    }
    printf("were extracted to the %s directory.\n", targetDir);

    free(fileInfoArray);
    free(buffer);
    fclose(archiveFile);
}

int main(int argc, char *argv[]) {
    // Check if correct number of arguments are provided
    if (argc < 2) {
        printf("Usage: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    // Check the operation mode
    if(strcmp(argv[1], "-b") == 0) {
        char *output = argv[argc-1]; 
        combineFiles(argc, argv, output);
    }
    else if(strcmp(argv[1], "-a") == 0){
        char *archive = argv[argc-2]; 
        char *directory = argv[argc-1];
        unpackFiles(archive, directory);
    }
    return 0;
}
