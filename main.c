#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./includes/file_utils.h"
#include "./includes/dirent.h"

int main()
{
    bool print = false;
    char folderName[1024];
    printf("Enter the folder to scan: ");
    scanf("%1023s", folderName); // Read user input, limiting to 1023 characters

    struct dirent *entry;
    DIR *folder = opendir(folderName); // Open the directory

    if (folder == NULL)
    {
        perror("Unable to open directory");
        return 1;
    }

    int fileCount = 0;
    char **fileNames = listFilesInFolderWithProperties(folderName, &fileCount, true);

    printf("Number of items in the folder: %d\n", fileCount);

    while ((entry = readdir(folder)) != NULL)
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        printf("%s\n", entry->d_name); // Print the name of each file

        // Construct the full path
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "./%s", entry->d_name);

        if (print == true)
            printFile(fullPath);
    }

    closedir(folder); // Close the directory
    return 0;
}