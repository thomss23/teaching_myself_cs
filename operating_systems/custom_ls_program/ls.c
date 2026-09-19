#include <sys/types.h>
#include <dirent.h>
#include<stdio.h>
#include<unistd.h>

int main(int argc, char** argv) {
    
    DIR* dir = opendir(argv[1]);

    if (dir == NULL) {
        printf("Error opening directory\n");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        printf("name: %s\n", entry->d_name);
        printf("inode: %ld\n", entry->d_ino);
        printf("type: %d\n", entry->d_type);
        printf("size: %d\n", entry->d_reclen);
        printf("======\n");
    }


    closedir(dir);
    return 0;
}