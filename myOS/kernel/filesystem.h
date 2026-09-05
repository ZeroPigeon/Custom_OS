#ifndef FILESYSTEM_H
#define FILESYSTEM_H

typedef struct
{
        char name[32];
        char data[256];
        int size;
} File;

typedef struct Directory
{
        char name[64];

        struct Directory* parent;

        File* files;
        struct Directory* directories;

        int file_count;
	int file_capacity;

        int dir_count;
	int dir_capacity;

} Directory;

extern Directory* current_directory;

void list_files(void);
void list_dirs(void);

void create_file(char* name);
void create_dir(char* name);

int search_for_file(char* dirname);
int search_for_dir(char* filename);
void write_to_file(int fileindex, char* data);
void append_to_file(int fileindex, char* data);
void read_file(int fileindex);
void change_directory(char* name);

#endif
