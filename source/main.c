#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <common.h>
#include <split.h>
#include <recover.h>
int main(int argc, char *argv[])
{
	int ret = 0;
    T_FILE_Split_Input split_input;
    T_FILE_Recover_Input recover_input;
	
	if ((argc ==3) && (strcmp(argv[1], "-s") == 0))
	{
        split_input.filename = argv[2];
        split_input.algorithm_version = ALGORITHM_VERSION;
		ret = split_proc(split_input);
	}
	else if ((argc ==3) && (strcmp(argv[1], "-r") == 0))
	{
        recover_input.filename = argv[2];
		ret = recover_proc(recover_input);
	}
	else
	{
		print_help();
	}
	
	return ret;
}
