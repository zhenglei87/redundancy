
#define LOG(frm, arg...) printf(frm, ##arg)

#define BLOCK_SIZE	4
#define SPLIT_FILE_NUM	5
#define FILE_NAME_MAX_SIZE	50

/* define file head version and algorithm version for spliting */
#define HEAD_VERSION 1
#define ALGORITHM_VERSION 1

extern const char *recover_filename_suffix;
extern const char *split_filename_suffix[SPLIT_FILE_NUM];


typedef enum
{
	Block_0 = 0,
	Block_1,
	Block_2,
	Block_3,
	Block_4,
	Block_MAX
}E_BLOCK_Loc;

typedef struct
{
    char version;//V1
}T_FILE_Split_Head;

typedef struct
{
    char block_loc;
    char last_block_cnt;
}T_FILE_Split_HeadV1;

typedef struct
{
    char block_loc;
    char last_block_cnt;
    unsigned char algorithm_version;
}T_FILE_Split_HeadV2;

typedef struct
{
    const char *filename;
    unsigned char algorithm_version;
}T_FILE_Split_Input;

typedef struct
{
    const char *filename;
}T_FILE_Recover_Input;



size_t get_file_size(FILE *fp);

void print_help();


