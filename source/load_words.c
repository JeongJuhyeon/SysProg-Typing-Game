#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mainlib.h"

int load_words(char * file_name, char ** word_list, int list_size)
{
	FILE* word_file_fd = fopen(file_name, "r");

	if(!word_file_fd)
	{
		perror("no word list");
		exit(1);
	}

	//memory allocating + load words to array
	for(int i = 0;i<list_size;i++)
	{
		word_list[i] = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));
		fscanf(word_file_fd, "%s", word_list[i]);
	}
}

void main(int argc, char* argv[])
{
	char * word_list[WORD_LIST_SIZE];
	load_words(argv[1], word_list, WORD_LIST_SIZE);

	for(int i;i<WORD_LIST_SIZE;i++)//for test
	{
		printf("%d : %s\n",i+1,  word_list[i]);
	}
	

}
