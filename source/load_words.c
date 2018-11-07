#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mainlib.h"

#define WORD_LIST_SIZE 5000

int load_words(FILE * word_file_fd, char ** word_list)
{
	//memory allocating + load words to array
	for(int i = 0;i<WORD_LIST_SIZE;i++)
	{
		word_list[i] = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));
		fscanf(word_file_fd, "%s", word_list[i]);
	}
}

void main()
{
	FILE* word_file_fd = fopen("../words/words_5000", "r");
	char * word_list[WORD_LIST_SIZE];
	load_words(word_file_fd, word_list);

	for(int i;i<WORD_LIST_SIZE;i++)//for test
	{
		printf("%d : %s\n",i+1,  word_list[i]);
	}
	

}
