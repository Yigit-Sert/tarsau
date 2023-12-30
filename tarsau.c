#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_FILE_SIZE 256

typedef long int fileSize;

// Data structure to store file details
typedef struct {
    char fileName[MAX_FILE_SIZE];
    char filePermissions[11]; 
    fileSize fileSize;
} FileDetail;

int main(int argc, char *argv[]) {
    // Check if filenames are provided
    if (argc < 3) {
        printf("Usage: %s [option] file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    char* option = argv[1];
    char outputName[] = "output.sau"; 

    if (strcmp(option, "-a") == 0) {
        joinFiles(argc - 2, argv + 2, outputName);
    } else if (strcmp(option, "-b") == 0) {
        splitFiles(outputName, "folder");
    } else {
        printf("Please enter a valid argument.\n");
        return 1;
    }

    return 0;
}

// Function to extract files from a merged file
void splitFiles(char archName[], char dirName[]) {
    FILE *sau = fopen("output.sau", "r");

    if (!sau) {
        perror("output.sau Can not open file.");
        return;
    }

    fseek(sau, 0, SEEK_END);
    long fileSize = ftell(sau);
    fseek(sau, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize + 1);

    if (!buffer) {
        perror("Memory allocation error");
        fclose(sau);
        return;
    }

    fread(buffer, 1, fileSize, sau);
    buffer[fileSize] = '\0';

    const char delimiters[] = "|,";

    char *token = strtok(buffer, delimiters);

    FileDetail detail;
    FileDetail *detailArray = NULL;
    int detailCount = 0;

    while (token != NULL) {
        if (strstr(token, ".txt") != NULL) {
            strcpy(detail.fileName, token);

            token = strtok(NULL, delimiters);
            strcpy(detail.filePermissions, token);

            token = strtok(NULL, delimiters);
            if (token != NULL && atoi(token) != 0) {
                detail.fileSize = atoi(token);

                char *fileContent = (char *)malloc(detail.fileSize);
                if (!fileContent) {
                    perror("Memory allocation error");
                    fclose(sau);
                    free(buffer);
                    return;
                }

                fread(fileContent, 1, detail.fileSize, sau);

                free(fileContent);
            }

            detailArray = realloc(detailArray, (detailCount + 1) * sizeof(FileDetail));
            detailArray[detailCount++] = detail;
        }

        token = strtok(NULL, delimiters);
    }

    printf("%ld", fileSize);
    printf(" \n");
    
    for (int i = detailCount - 1; i >= 0; i--) {
        printf("File Name---: %s\n", detailArray[i].fileName);
        printf("Permissions: %s\n", detailArray[i].filePermissions);
        printf("File Size: %ld bytes\n", detailArray[i].fileSize);

        char chunkBuffer[detailArray[i].fileSize];
        FILE *destFile = fopen(detailArray[i].fileName, "wb");
        
        for (long j = fileSize - detailArray[i].fileSize; j < fileSize; j++) {
            fseek(sau, j, SEEK_SET);
            fputc(fgetc(sau), destFile);
        }
        
        fclose(destFile);
    }

    // Free memory
    free(detailArray);
    free(buffer);
    fclose(sau);
}


// Function to retrieve file details
void fetchFileDetails(const char *name, FileDetail *fileD) {
    struct stat fileStats;

    // Fetch the file's statistics
    if (stat(name, &fileStats) == -1) {
        perror("Could not retrieve file details");
        exit(1);
    }

    strcpy(fileD->fileName, name);

    // Convert permissions to a readable format
    snprintf(fileD->filePermissions, sizeof(fileD->filePermissions), "%o", fileStats.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

    fileD->fileSize = fileStats.st_size;
}

// Function to merge multiple files into one
void joinFiles(int count, char *allFiles[], char outName[]) {
    fileSize total = 0;
    FILE *output = fopen(outName, "wb"); // Open in binary mode
    
    // Calculate total size of all files
    for (int i = 0; i < count; i++) { 
        FileDetail detail;
        fetchFileDetails(allFiles[i], &detail);
        total += detail.fileSize;
    }

    if (!output) {
        perror("Failed to open output file");
        return;
    }
	
    // Write the total size to the output file
    fprintf(output, "%010ld", total);
    
    // Append file details to the output file
    for (int i = 0; i < count; i++) {
        FileDetail detail;
        fetchFileDetails(allFiles[i], &detail);
        fprintf(output, "|%s, %s, %ld", detail.fileName, detail.filePermissions, detail.fileSize);
    }
    fprintf(output, "|\n"); // End of details

    // Copy file contents to the output file and print them
    for (int i = 0; i < count; i++) { 
        FileDetail detail;
        FILE *inFile = fopen(allFiles[i], "rb"); // Open in binary mode
        fetchFileDetails(allFiles[i], &detail);
        int size = detail.fileSize;
        char* data = (char *)malloc(size);
        fread(data, 1, size, inFile);
        fwrite(data, 1, size, output);
        
      
        fclose(inFile);
        free(data);
    }
      // Print merged file content
        printf("The files have been merged.\n");
    fclose(output);
}

