#pragma once
#include <stdbool.h>

struct file_header{
	char name[200];
	char isFolder[50];
	unsigned int data_length;

	char folderName[200];
};

char* header_write(struct file_header* header);

struct file_header* header_read(char *data);
