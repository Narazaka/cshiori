/**
 * @file
 * @brief SHIORI/3.x DLLインターフェース
 * @author Narazaka: http://narazaka.net/
 *
 * (C) 2014 Narazaka : Licensed under The MIT License - http://narazaka.net/license/MIT?2014
 **/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "shiori.h"

#include "cshiori.c"
#include "shiori_events.c"

SHIORI_EXPORT BOOL SHIORI_CALL load(const MEMORY_HANDLE h,long len){
	char* str = (char*)malloc(sizeof(char) * (len + 1));
	strncpy(str, (const char*)h, len);
	str[len] = '\0';
	SHIORI_FREE(h);
	return cshiori_load(str, shiori_load);
}

#define SHIORI_LINES_BUFFER_STEP 10
SHIORI_EXPORT MEMORY_HANDLE SHIORI_CALL request(const MEMORY_HANDLE h,long *len){
	char* str = (char*)malloc(sizeof(char) * (*len + 1));
	strncpy(str, (const char*)h, *len);
	str[*len] = '\0';
	SHIORI_FREE(h);
	char** lines = NULL;
	char** lines_new;
	size_t lines_index = 0;
	char* str_p = str;
	char* str_p_new;
	while(true){
		if(0 == (lines_index % SHIORI_LINES_BUFFER_STEP)){
			lines_new = (char**)realloc(lines, sizeof(char*) * (lines_index + SHIORI_LINES_BUFFER_STEP));
			if(lines_new == NULL){
				size_t i;
				for(i = 0; i < lines_index; ++i){
					free(*(lines + i));
				}
				free(lines);
				return NULL;
			}
			lines = lines_new;
		}
		str_p_new = strstr(str_p, "\r\n");
		if(str_p_new == NULL) break;
		size_t line_length = str_p_new - str_p;
		char* line = (char*)malloc(sizeof(char) * (line_length + 1));
		strncpy(line, str_p, line_length);
		line[line_length] = '\0';
		*(lines + lines_index) = line;
		str_p = str_p_new + 2;
		lines_index ++;
	}
	char* resstr = cshiori_request(lines, lines_index, shiori_request);
	*len = strlen(resstr);
	MEMORY_HANDLE reth=(MEMORY_HANDLE)SHIORI_MALLOC(*len);
	memcpy(reth, resstr, *len);
	return reth;
}

SHIORI_EXPORT BOOL SHIORI_CALL unload(void){
	return cshiori_unload(shiori_unload);
}
