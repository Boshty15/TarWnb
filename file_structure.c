#include "file_structure.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

char* header_write(struct file_header* header)
{
	unsigned int size = sizeof(struct file_header);
	char *data = (char *) malloc(size);

	memcpy(data, (char *) header, size);
	return data;
}

struct file_header* header_read(char *data)
{
	unsigned int size = sizeof(struct file_header);
	struct file_header *header = (struct file_header*) malloc(size);

	memcpy(header, data, size);
	return header;
}
