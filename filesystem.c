#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 100
#define MAX_FILE_NAME_LENGTH 50
#define BLOCK_SIZE 512
#define NUM_BLOCKS 1000

typedef struct {
    char name[MAX_FILE_NAME_LENGTH];
    int start_block;
    int size;
} File;

typedef struct {
    int used_blocks[NUM_BLOCKS];
    File files[MAX_FILES];
    int num_files;
} FileSystem;

void initializeFileSystem(FileSystem *fs) {
    memset(fs->used_blocks, 0, sizeof(fs->used_blocks));
    fs->num_files = 0;
}

int allocateBlock(FileSystem *fs) {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (!fs->used_blocks[i]) {
            fs->used_blocks[i] = 1;
            return i;
        }
    }
    return -1; // No available blocks
}

void createFile(FileSystem *fs, const char *name, int size) {
    if (fs->num_files >= MAX_FILES) {
        printf("Maximum file limit reached.\n");
        return;
    }

    if (size <= 0 || size > NUM_BLOCKS) {
        printf("Invalid file size.\n");
        return;
    }

    int start_block = allocateBlock(fs);
    if (start_block == -1) {
        printf("No available space for file.\n");
        return;
    }

    File new_file;
    strcpy(new_file.name, name);
    new_file.start_block = start_block;
    new_file.size = size;

    // Create the file inside the project's directory
    FILE *file = fopen(name, "wb");
    if (file) {
        // Write dummy data to the file
        char data[BLOCK_SIZE] = {0};
        for (int i = 0; i < size; ++i) {
            fwrite(data, BLOCK_SIZE, 1, file);
        }
        fclose(file);

        fs->files[fs->num_files++] = new_file;
        printf("File '%s' created.\n", name);
    } else {
        printf("Error creating file '%s'.\n", name);
    }
}

void deleteFile(FileSystem *fs, const char *name) {
    for (int i = 0; i < fs->num_files; ++i) {
        if (strcmp(fs->files[i].name, name) == 0) {
            fs->used_blocks[fs->files[i].start_block] = 0;

            // Remove the file from the array by overwriting with the last file
            fs->files[i] = fs->files[fs->num_files - 1];
            fs->num_files--;

            // Delete the file from the project's directory
            if (remove(name) == 0) {
                printf("File '%s' deleted.\n", name);
            } else {
                printf("Error deleting file '%s'.\n", name);
            }
            return;
        }
    }
    printf("File '%s' not found.\n", name);
}

int main() {
    FileSystem fs;
    initializeFileSystem(&fs);

    while (1) {
        int choice;
        printf("Enter 1 to create a file or 2 to delete a file (0 to quit): ");
        scanf("%d", &choice);

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            char name[MAX_FILE_NAME_LENGTH];
            int size;

            printf("Enter the file name: ");
            scanf("%s", name);
            printf("Enter the file size: ");
            scanf("%d", &size);

            createFile(&fs, name, size);
        } else if (choice == 2) {
            char name[MAX_FILE_NAME_LENGTH];

            printf("Enter the file name to delete: ");
            scanf("%s", name);

            deleteFile(&fs, name);
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
