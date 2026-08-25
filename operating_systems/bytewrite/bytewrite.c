#include<stdio.h>
#include<sys/stat.h>

void main() {
    struct stat info;
    FILE *file = fopen("tmpfile", "w");

    if (file == NULL) {
        perror("Could not open file");
    }

    stat("tmpfile", &info);
    long long oldblocks = 0;
    while(info.st_size < 1000000) {
        fputc('A', file);     
        fflush(file);          

        if (info.st_blocks > oldblocks) {
            printf("File size: %lld bytes\n", (long long) info.st_size);
            printf("Allocated blocks: %lld\n", (long long) info.st_blocks);
            printf("Allocated disk space: %lld bytes\n", (long long) info.st_blocks * 512);
            oldblocks = info.st_blocks;
        }


        stat("tmpfile", &info);
    }

    fclose(file);
}