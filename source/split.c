#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <common.h>
#include <split.h>

typedef struct
{
	char ori_filename[FILE_NAME_MAX_SIZE];
	FILE *fp_ori_file;
	FILE *fp_split_file[SPLIT_FILE_NUM];
	size_t file_size;
    char last_block_cnt;
}T_FILE_Split;


static void split_file_4byte(char input[4], char output[5])
{
	output[0] = input[0] ^ input[1] ^ input[2] ^ input[3];
	output[1] = output[0] ^ input[0];
	output[2] = output[0] ^ input[1];
	output[3] = output[0] ^ input[2];
	output[4] = output[0] ^ input[3];
	return;
}

static int fopen_for_split(const char *ori_filename, T_FILE_Split *psplit_file)
{
	T_FILE_Split split_file;
	char filename[FILE_NAME_MAX_SIZE] = {0};
    int i = 0;
	
	memset(&split_file, 0, sizeof(split_file));

	strcpy(split_file.ori_filename, ori_filename);

	split_file.fp_ori_file = fopen(ori_filename, "rb+");
	assert(split_file.fp_ori_file != NULL);
	
	for (i = 0; i < SPLIT_FILE_NUM; i++)
	{
		snprintf(filename, FILE_NAME_MAX_SIZE-1, "%s.%s", ori_filename, split_filename_suffix[i]);
		split_file.fp_split_file[i] = fopen(filename, "wb+");
		assert(split_file.fp_split_file[i] != NULL);
	}
    memcpy(psplit_file, &split_file, sizeof(split_file));
	return 0;
}

static int split_pre_proc(T_FILE_Split *fp_split)
{
    char last_block_cnt = 0;
    fp_split->file_size = get_file_size(fp_split->fp_ori_file);
    last_block_cnt = fp_split->file_size % BLOCK_SIZE;
	last_block_cnt = (0 == last_block_cnt) ? BLOCK_SIZE : last_block_cnt;
    fp_split->last_block_cnt = last_block_cnt;
    return 0;
}

static void split_buf_to_file(FILE *fp[5], char out[5])
{
	fwrite(&out[0], 1, 1, fp[0]);
	fwrite(&out[1], 1, 1, fp[1]);
	fwrite(&out[2], 1, 1, fp[2]);
	fwrite(&out[3], 1, 1, fp[3]);
	fwrite(&out[4], 1, 1, fp[4]);
	return;
}

static int split_write_head(T_FILE_Split *fp_split)
{
    T_FILE_Split_Head head;
    T_FILE_Split_HeadV1 head_v1;
    int i = 0;

    head.version = 1;
    head_v1.last_block_cnt = fp_split->last_block_cnt;
    for (i = 0; i < SPLIT_FILE_NUM; i++)
    {
        head_v1.block_loc = i;
        fwrite(&head, sizeof(head), 1, fp_split->fp_split_file[i]);
        fwrite(&head_v1, sizeof(head_v1), 1, fp_split->fp_split_file[i]);
    }
    return 0;
}

static void split_to_file(T_FILE_Split *fp_split)
{
    size_t read_size;
    char read_buf[BLOCK_SIZE];
    char split_buf[SPLIT_FILE_NUM];
    memset(read_buf, 0, BLOCK_SIZE);
    while ((read_size = fread(read_buf, 1, 4, fp_split->fp_ori_file)) > 0)
	{
		split_file_4byte(read_buf, split_buf);
		split_buf_to_file(fp_split->fp_split_file, split_buf);
		memset(split_buf, 0, 4);
	}
    return;
}

static void fclose_for_split(T_FILE_Split *fp_split)
{
    fclose(fp_split->fp_ori_file);
	fclose(fp_split->fp_split_file[0]);
	fclose(fp_split->fp_split_file[1]);
	fclose(fp_split->fp_split_file[2]);
	fclose(fp_split->fp_split_file[3]);
	fclose(fp_split->fp_split_file[4]);
}


int split_proc(int argc, char *argv[])
{
	const char *filename = argv[2];
	T_FILE_Split split_file;
    int ret;

    memset(&split_file, 0, sizeof(split_file));

    ret = fopen_for_split(filename, &split_file);
    if (ret != 0) return ret;
	
    split_pre_proc(&split_file);
    
	LOG("start split %s file size = %d last block = %d\n", filename, (int)split_file.file_size, split_file.last_block_cnt);

    split_write_head(&split_file);

    split_to_file(&split_file);

    fclose_for_split(&split_file);
	
	LOG("end split %s file size = %d last block = %d\n", filename, (int)split_file.file_size, split_file.last_block_cnt);
	
	
	return 0;
}


