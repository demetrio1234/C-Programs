#ifdef _WIN32
#include "dirent.h" // Include the custom dirent.h for Windows
#else
#include <dirent.h> // Use the standard dirent.h for Unix-based systems
#endif
#include <string.h> // For strcat

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
