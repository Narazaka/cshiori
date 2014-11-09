#include "strutl.h"

char* getline(const FILE *fp){
	char *buf;
	char *line;
	char *line_new;
	size_t line_len;
	size_t line_len_old;
	buf = (char*)malloc(sizeof(char) * GETLINE_BUF_SIZE);
	line = (char*)malloc(sizeof(char));
	*line = '\0';
	line_len = 0;
	while(true){
		if(fgets(buf, GETLINE_BUF_SIZE, stdin) == NULL){
			return line;
		};
		line_len_old = line_len;
		line_len += strlen(buf);
		line_new = (char*)realloc(line, sizeof(char) * (line_len + 1));
		if(line_new == NULL){
			free(line);
			return NULL;
		}
		line = line_new;
		strcpy(line + line_len_old, buf); /* strcat(line, buf) */
		if(strchr(buf, '\n') != NULL){
			break;
		}
	}
	return line;
}

void chomp(char* const line){
	char *index;
	if((index = strrchr(line, (int)'\n')) != NULL && *(index + 1) == '\0') *index = '\0';
	if((index = strrchr(line, (int)'\r')) != NULL && *(index + 1) == '\0') *index = '\0';
}

char* crlftolf(char* const str){
	char* str_lf = (char*)malloc(sizeof(char) * (strlen(str) + 1));
	char* str_lf_p = str_lf;
	char* str_p = str;
	char* str_p_new;
	while(true){
		str_p_new = strstr(str_p, "\r\n");
		if(str_p_new == NULL){
			strcpy(str_lf_p, str_p);
			break;
		}else{
			size_t len = str_p_new - str_p;
			strncpy(str_lf_p, str_p, len);
			*(str_lf_p + len) = '\n';
			*(str_lf_p + len + 1) = '\0';
			str_p += len + 2;
			str_lf_p += len + 1;
		}
	}
	return str_lf;
}
