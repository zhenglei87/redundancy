#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <common.h>


const char *recover_filename_suffix = "r";

const char *split_filename_suffix[SPLIT_FILE_NUM] = 
		{[Block_ABCD]="ABCD.s", 
		[Block_BCD]="BCD.s", 
		[Block_ACD]="ACD.s", 
		[Block_ABD]="ABD.s", 
		[Block_ABC]="ABC.s"};

extern char _binary_usage_txt_start[];
extern char _binary_usage_txt_end[];
extern char _binary_usage_txt_size[];

void print_help()
{
	int size = (size_t)_binary_usage_txt_size;
	int i;
	printf("size = %d\n", size);
	for (i = 0; i < size; i++)
	{
		printf("%c", _binary_usage_txt_start[i]);
	}
	printf("\n");
}

size_t get_file_size(FILE *fp)
{
    size_t file_size;
    fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);
    return file_size;
}


