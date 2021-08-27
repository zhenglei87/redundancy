#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <common.h>
#include <recover.h>

typedef struct
{
	char ori_filename[FILE_NAME_MAX_SIZE];
	FILE *fp_recover_file;
	FILE *fp_split_file[SPLIT_FILE_NUM];
	char ori_file_last_block[SPLIT_FILE_NUM];
    unsigned char block_loc[SPLIT_FILE_NUM];
	size_t file_size[SPLIT_FILE_NUM];
    size_t payload_size[SPLIT_FILE_NUM];
	int missed_split_filecnt;
    int file_cnt;
}T_FILE_Recover_Ori;

typedef struct
{
	char ori_filename[FILE_NAME_MAX_SIZE];
	FILE *fp_recover_file;
	FILE *fp_split_file[SPLIT_FILE_NUM];
    E_BLOCK_Loc missblock;
    unsigned char last_block;
    size_t file_size;
    size_t payload_size;
}T_FILE_Recover;


static void recover_file_4byte(char input[5], E_BLOCK_Loc missblock, char ouput[4])
{
	int i = 0;
	char block_miss = 0;
    if (missblock < Block_MAX)
    {
    	input[missblock] = 0;
    	for (i = 0; i < 5; i++)
    	{
    		block_miss = block_miss ^ input[i];
    	}
    	input[missblock] = block_miss;
    }
	for (i = 0; i < 4; i++)
	{
		ouput[i] = input[0] ^ input[i + 1];
	}
	return;
}

static int fopen_for_recover(const char *ori_filename, T_FILE_Recover_Ori *precover_file)
{
	T_FILE_Recover_Ori recover_file;
	char filename[FILE_NAME_MAX_SIZE] = {0};
    int i = 0;
    FILE *fp = NULL;
	
	memset(&recover_file, 0, sizeof(recover_file));

	strcpy(recover_file.ori_filename, ori_filename);

	snprintf(filename, FILE_NAME_MAX_SIZE-1, "%s.%s", ori_filename, recover_filename_suffix);
	recover_file.fp_recover_file = fopen(filename, "wb+");
	assert(recover_file.fp_recover_file != NULL);
	
	for (i = 0; i < SPLIT_FILE_NUM; i++)
	{
		snprintf(filename, FILE_NAME_MAX_SIZE-1, "%s.%s", ori_filename, split_filename_suffix[i]);
		fp = fopen(filename, "rb+");

        if (fp == NULL)
        {
            recover_file.missed_split_filecnt++;
            assert(recover_file.missed_split_filecnt <= 1);
		}
        else
        {
            recover_file.fp_split_file[recover_file.file_cnt] = fp;
            recover_file.file_cnt++;
        }
	}
    memcpy(precover_file, &recover_file, sizeof(recover_file));
	return 0;
}

static int recover_pre_proc(T_FILE_Recover_Ori *fp_recover_ori)
{
    int i = 0;
    for (i = 0; i < SPLIT_FILE_NUM; i++)
    {
        if (fp_recover_ori->fp_split_file[i] != NULL)
        {
            fp_recover_ori->file_size[i] = get_file_size(fp_recover_ori->fp_split_file[i]);
        }
    }
    return 0;
}

static int recover_read_head(T_FILE_Recover_Ori *fp_recover)
{
    int i = 0;
	T_FILE_Split_Head head;
    T_FILE_Split_HeadV1 headv1;
	for (i = 0; i < SPLIT_FILE_NUM; i++)
	{
		if (fp_recover->fp_split_file[i] == NULL) continue;
		fread(&head, sizeof(head), 1, fp_recover->fp_split_file[i]);
        if (head.version == 1)
        {
            fread(&headv1, sizeof(headv1), 1, fp_recover->fp_split_file[i]);
            fp_recover->ori_file_last_block[i] = headv1.last_block_cnt;
            fp_recover->block_loc[i] = headv1.block_loc;
            fp_recover->payload_size[i] = fp_recover->file_size[i] - sizeof(head) - sizeof(headv1);
            assert(fp_recover->ori_file_last_block[i] <= BLOCK_SIZE);
            assert(fp_recover->block_loc[i] < Block_MAX);
        }
        else
        {
            assert(0);
        }
	}
    return 0;
}

static int recover_file_check(T_FILE_Recover_Ori *fp_recover_ori, T_FILE_Recover *fp_recover)
{
    int i = 0;
    for (i = 1; i < fp_recover_ori->file_cnt; i++)
    {
        if (fp_recover_ori->file_size[i] != fp_recover_ori->file_size[0])
        {
            LOG("file size not eque\n");
            assert(0);
        }
        if (fp_recover_ori->ori_file_last_block[i] != fp_recover_ori->ori_file_last_block[0])
        {
            LOG("last block cnt not eque\n");
            assert(0);
        }

        if (fp_recover_ori->payload_size[i] != fp_recover_ori->payload_size[0])
        {
            LOG("payload size not eque\n");
            assert(0);
        }

        if (fp_recover_ori->block_loc[i] == fp_recover_ori->block_loc[0])
        {
            LOG("block_loc eque\n");
            assert(0);
        }
    }

    strcpy(fp_recover->ori_filename, fp_recover_ori->ori_filename);
    fp_recover->fp_recover_file = fp_recover_ori->fp_recover_file;
    fp_recover->file_size = fp_recover_ori->file_size[0];
    fp_recover->payload_size = fp_recover_ori->payload_size[0];
    fp_recover->last_block = fp_recover_ori->ori_file_last_block[0];
    fp_recover->missblock = Block_MAX;
    for (i = 0; i < fp_recover_ori->file_cnt; i++)
    {
        fp_recover->fp_split_file[fp_recover_ori->block_loc[i]] = fp_recover_ori->fp_split_file[i];
    }
    for (i = 0; i < SPLIT_FILE_NUM; i++)
    {
        if (fp_recover->fp_split_file[i] == NULL)
        {
            fp_recover->missblock = i;
        }
    }
    return 0;
}

static int recover_from_file(T_FILE_Recover *fp_recover, char recover_buf[4])
{
    int i = 0;
	char split_buffer[SPLIT_FILE_NUM] = {0};
    char read_buf;
    E_BLOCK_Loc missblock = fp_recover->missblock;
	for (i = 0; i < 5; i++)
	{
		if (fp_recover->fp_split_file[i] == NULL) 
		{
			continue;
		}
		fread(&read_buf, 1, 1, fp_recover->fp_split_file[i]);
        split_buffer[i] = read_buf;
	}
	recover_file_4byte(split_buffer, missblock, recover_buf);
    return 0;
}

static int recover_file_proc(T_FILE_Recover *fp_recover)
{
    size_t i = 0;
	char recover_buf[4] = {0};
    size_t fize_size = fp_recover->payload_size;
	for (i = 0; i < fize_size - 1; i++)
	{
		recover_from_file(fp_recover, recover_buf);
		fwrite(recover_buf, 1, BLOCK_SIZE, fp_recover->fp_recover_file);
	}
	recover_from_file(fp_recover, recover_buf);
	fwrite(recover_buf, 1, fp_recover->last_block, fp_recover->fp_recover_file);
    return 0;
}


static void fclose_for_recover(T_FILE_Recover *fp_recover)
{
    fclose(fp_recover->fp_recover_file);
	fclose(fp_recover->fp_split_file[0]);
	fclose(fp_recover->fp_split_file[1]);
	fclose(fp_recover->fp_split_file[2]);
	fclose(fp_recover->fp_split_file[3]);
	fclose(fp_recover->fp_split_file[4]);
}


int recover_proc(int argc, char *argv[])
{
    const char *filename = argv[2];
	T_FILE_Recover recover_file;
    T_FILE_Recover_Ori recover_file_ori;
    int ret;

    memset(&recover_file, 0, sizeof(recover_file));
    memset(&recover_file_ori, 0, sizeof(recover_file_ori));

    ret = fopen_for_recover(filename, &recover_file_ori);
    if (ret != 0) return ret;
	
    recover_pre_proc(&recover_file_ori);
    
    recover_read_head(&recover_file_ori);

    recover_file_check(&recover_file_ori, &recover_file);
    
	LOG("start recover %s file size = %d last block = %x\n", filename, (int)recover_file.file_size, recover_file.last_block);

    recover_file_proc(&recover_file);

    fclose_for_recover(&recover_file);
	
	LOG("end recover %s file size = %d last block = %d\n", filename, (int)recover_file.file_size, recover_file.last_block);
	
	
	return 0;
}


