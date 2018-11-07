#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mainlib.h"

#define WORD_LIST_SIZE 5000

int load_words(char ** word_list)
{
	FILE* word_file_fd = fopen("../words/words_5000", "r");

	if(!word_file_fd)
	{
		perror("no word list");
		exit(1);
	}

	//memory allocating + load words to array
	for(int i = 0;i<WORD_LIST_SIZE;i++)
	{
		word_list[i] = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));
		fscanf(word_file_fd, "%s", word_list[i]);
	}
}

void main()
{
	char * word_list[WORD_LIST_SIZE];
	load_words(word_list);

	for(int i;i<WORD_LIST_SIZE;i++)//for test
	{
		printf("%d : %s\n",i+1,  word_list[i]);
	}
	

}
