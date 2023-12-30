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

    if (strcmp(option, "-b") == 0) {
        joinFiles(argc - 2, argv + 2, outputName);
    } else if (strcmp(option, "-a") == 0) {
        splitFiles(outputName, "folder");
    } else {
        printf("Please enter a valid argument.\n");
        return 1;
    }

    return 0;
}

// Function to extract files from a merged file
void splitFiles(char arch[], char dir[]) {
    FILE *input = fopen("output.sau", "r");

    // Check if the file was opened successfully
    if (!input) {
        perror("Cannot open output file");
        return;
    }

    fseek(input, 0, SEEK_END);
    long fSize = ftell(input);
    fseek(input, 0, SEEK_SET);

    char *data = (char *)malloc(fSize + 1);

    if (!data) {
        perror("Memory error");
        fclose(input);
        return;
    }

    fread(data, 1, fSize, input);
    data[fSize] = '\0';

    const char delim[] = "|,";

    char *tok = strtok(data, delim);

    FileDetail detail;
    FileDetail *detailsArray = NULL;
    int detailCount = 0;
    int totalSize = 0;

    // Parse the merged file's content
    while (tok != NULL) {
        if (strstr(tok, ".txt") != NULL) {
            strcpy(detail.fileName, tok);
            tok = strtok(NULL, delim);
            strcpy(detail.filePermissions, tok);
            tok = strtok(NULL, delim);
            if (tok != NULL && atoi(tok) != 0) {
                detail.fileSize = atoi(tok);
                totalSize += detail.fileSize;
                char *content = (char *)malloc(detail.fileSize);
                if (!content) {
                    perror("Memory error");
                    fclose(input);
                    free(data);
                    return;
                }
                fread(content, 1, detail.fileSize, input);
                free(content);
            }
            detailsArray = realloc(detailsArray, (detailCount + 1) * sizeof(FileDetail));
            detailsArray[detailCount++] = detail;
        }
        tok = strtok(NULL, delim);
    }

    int pos = fSize;
    for (int i = detailCount - 1; i >= 0; i--) {
        printf("File Name: %s\n", detailsArray[i].fileName);
        printf("Permissions: %s\n", detailsArray[i].filePermissions);
        printf("Size: %ld bytes\n", detailsArray[i].fileSize);
        char buffer[detailsArray[i].fileSize];
        FILE *outFile = fopen(detailsArray[i].fileName, "wb");
        int offset = pos - detailsArray[i].fileSize;
        for(int j = offset; j < pos; j++){
            fseek(input, j, SEEK_SET);
            fputc(fgetc(input), outFile);
        }
        pos = offset;
        fclose(outFile);
    }

    free(detailsArray);
    free(data);
    fclose(input);
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
        
        // Print merged file content
        printf("%s:\n", allFiles[i]);
        fwrite(data, 1, size, stdout);
        printf("\n");
        
        fclose(inFile);
        free(data);
    }
    fclose(output);
}

