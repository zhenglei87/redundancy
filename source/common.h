
#define LOG(frm, arg...) printf(frm, ##arg)

#define BLOCK_SIZE	4
#define SPLIT_FILE_NUM	5
#define FILE_NAME_MAX_SIZE	50

extern const char *recover_filename_suffix;
extern const char *split_filename_suffix[SPLIT_FILE_NUM];


typedef enum
{
	Block_ABCD = 0,
	Block_BCD,
	Block_ACD,
	Block_ABD,
	Block_ABC,
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

size_t get_file_size(FILE *fp);

void print_help();


