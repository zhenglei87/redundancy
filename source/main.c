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
	
	if ((argc ==3) && (strcmp(argv[1], "-s") == 0))
	{
		ret = split_proc(argc, argv);
	}
	else if ((argc ==3) && (strcmp(argv[1], "-r") == 0))
	{
		ret = recover_proc(argc, argv);
	}
	else
	{
		print_help();
	}
	
	return ret;
}
