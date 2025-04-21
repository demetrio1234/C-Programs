#ifdef _WIN32
#include "dirent.h" // Include the custom dirent.h for Windows
#include <windows.h>
#else
#include <dirent.h> // Use the standard dirent.h for Unix-based systems
#endif
#include <string.h>   // For strcat
#include <sys/stat.h> // For struct stat
#include <stdbool.h>  // For bool type

int printFile(char filename[])
{
    FILE *file;
    char ch;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    printf("File content of %s:\n", filename);

    // Read and print each character until EOF
    while ((ch = fgetc(file)) != EOF)
    {
        putchar(ch);
    }

    // Close the file
    fclose(file);
    return 0;
}

char **listFilesInFolder(char folderName[], int *count)
{
    struct dirent *entry;
    DIR *folder = opendir(folderName); // Open the directory
    char **fileList = NULL;
    *count = 0;

    if (folder == NULL)
    {
        perror("Unable to open directory");
        return NULL;
    }

    printf("Files in the directory:\n");

    while ((entry = readdir(folder)) != NULL)
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Allocate memory for the new file name
        char **temp = realloc(fileList, (*count + 1) * sizeof(char *));
        if (temp == NULL)
        {
            perror("Memory allocation failed");
            free(fileList); // Free previously allocated memory
            closedir(folder);
            return NULL;
        }
        fileList = temp;

        fileList[*count] = malloc(strlen(entry->d_name) + 1);
        if (fileList[*count] == NULL)
        {
            perror("Memory allocation failed");
            for (int i = 0; i < *count; i++)
            {
                free(fileList[i]);
            }
            free(fileList);
            closedir(folder);
            return NULL;
        }

        strcpy(fileList[*count], entry->d_name);
        (*count)++;
    }

    closedir(folder); // Close the directory
    return fileList;
}

char **listFilesInFolderWithProperties(char folderName[], int *count, bool withProperties)
{
    struct dirent *entry;
    DIR *folder = opendir(folderName); // Open the directory
    char **fileList = NULL;
    char **errorList = NULL; // List to store names of files/folders with errors
    int errorCount = 0;      // Count of errors
    *count = 0;

    if (folder == NULL)
    {
        perror("Unable to open directory");
        return NULL;
    }

    printf("Files in the directory:\n");

    while ((entry = readdir(folder)) != NULL)
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Allocate memory for the new file name
        char **temp = realloc(fileList, (*count + 1) * sizeof(char *));
        if (temp == NULL)
        {
            perror("Memory allocation failed");
            free(fileList); // Free previously allocated memory
            closedir(folder);
            return NULL;
        }
        fileList = temp;

        if (withProperties)
        {
            // Get file properties
            struct stat fileStat;
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", folderName, entry->d_name);
#if _POSIX
            if (stat(filePath, &fileStat) == -1)
            {
                // Add the file/folder name to the error list
                char **errorTemp = realloc(errorList, (errorCount + 1) * sizeof(char *));
                if (errorTemp == NULL)
                {
                    perror("Memory allocation failed for error list");
                    continue;
                }
                errorList = errorTemp;

                errorList[errorCount] = malloc(strlen(entry->d_name) + 1);
                if (errorList[errorCount] == NULL)
                {
                    perror("Memory allocation failed for error name");
                    continue;
                }

                strcpy(errorList[errorCount], entry->d_name);
                errorCount++;
                continue;
            }
#elif _WIN32
            if (!GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileStat))
            {
                // Add the file/folder name to the error list
                char **errorTemp = realloc(errorList, (errorCount + 1) * sizeof(char *));
                if (errorTemp == NULL)
                {
                    perror("Memory allocation failed for error list");
                    continue;
                }
                errorList = errorTemp;

                errorList[errorCount] = malloc(strlen(entry->d_name) + 1);
                if (errorList[errorCount] == NULL)
                {
                    perror("Memory allocation failed for error name");
                    continue;
                }

                strcpy(errorList[errorCount], entry->d_name);
                errorCount++;
                continue;
            }
#endif
            char properties[1024];
#if _WIN32
            if (GetFileAttributesEx(entry->d_name, GetFileExInfoStandard, &fileStat))
            {
                // assign to the pointer *prefix the memory address of the read-only memory address of "<DIR>" or "\t"
                char *prefix = S_ISDIR(fileStat.st_mode) ? "<DIR>" : "\t";

                // Format properties like "dir" command output on Windows
                snprintf(properties,
                         sizeof(properties),
                         "%10lld %s %s",
                         (long long)fileStat.st_size,
                         prefix,
                         entry->d_name);
                printf("%s\n", properties); // Ensure the formatted string is printed
            }
            else
            {
                perror("stat failed");
            }
#else
            // Format properties like "ls -lai"
            char properties[1024];
            snprintf(properties, sizeof(properties), "%ld %ld %ld %ld %s",
                     (long)fileStat.st_ino, (long)fileStat.st_mode, (long)fileStat.st_nlink,
                     (long)fileStat.st_size, entry->d_name);
#endif

            fileList[*count] = malloc(strlen(properties) + 1);
            if (fileList[*count] == NULL)
            {
                perror("Memory allocation failed");
                for (int i = 0; i < *count; i++)
                {
                    free(fileList[i]);
                }
                free(fileList);
                closedir(folder);
                return NULL;
            }

            strcpy(fileList[*count], properties);
        }
        else
        {
            fileList[*count] = malloc(strlen(entry->d_name) + 1);
            if (fileList[*count] == NULL)
            {
                perror("Memory allocation failed");
                for (int i = 0; i < *count; i++)
                {
                    free(fileList[i]);
                }
                free(fileList);
                closedir(folder);
                return NULL;
            }

            strcpy(fileList[*count], entry->d_name);
        }

        (*count)++;
    }

    closedir(folder); // Close the directory

    // Print the error list
    if (errorCount > 0)
    {
        printf("Files/Folders with property errors:\n");
        for (int i = 0; i < errorCount; i++)
        {
            printf("%s\n", errorList[i]);
            free(errorList[i]); // Free memory for each error name
        }
        free(errorList); // Free the error list
    }

    return fileList;
}

void getFileAttributes(const char *filePath)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo))
    {
        // Check if it's a directory
        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s is a directory\n", filePath);
        }
        else
        {
            printf("%s is a file\n", filePath);
        }

        // File size (combine high and low parts)
        LARGE_INTEGER fileSize;
        fileSize.HighPart = fileInfo.nFileSizeHigh;
        fileSize.LowPart = fileInfo.nFileSizeLow;
        printf("File size: %lld bytes\n", fileSize.QuadPart);
    }
    else
    {
        printf("Failed to get attributes for %s. Error: %lu\n", filePath, GetLastError());
    }
}