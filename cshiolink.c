#include "cshiolink.h"

bool cshiolink_load(const char* dirpath, bool (*load)(const char*)){
	return cshiori_load(dirpath, load);
}

char* cshiolink_request(FILE *in, struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*)){
	char** lines = NULL;
	char** lines_new;
	size_t lines_index = 0;
	char* str;
	size_t i;
	while(true){
		char* line;
		if(0 == (lines_index % CSHIOLINK_REQUEST_LINES_BUFFER_STEP)){
			lines_new = (char**)realloc(lines, sizeof(char*) * (lines_index + CSHIOLINK_REQUEST_LINES_BUFFER_STEP));
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
		line = getline(in);
		chomp(line);
		*(lines + lines_index) = line;
		lines_index ++;
		if(0 == strlen(line)) break;
	}
	str = cshiori_request(lines, lines_index, request);
	for(i = 0; i < lines_index; ++i){
		free(*(lines + i));
	}
	free(lines);
	return str;
}

bool cshiolink_unload(bool (*unload)(void)){
	return cshiori_unload(unload);
}

void cshiolink_mainloop(FILE *in, FILE *out, bool (*load)(const char*), struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*), bool (*unload)(void)){
	while(true){
		char* line = getline(in);
		char* str;
#if defined(WIN32)||defined(_WIN32)||defined(_Windows)||defined(__CYGWIN__)
		char* str_crlf;
#endif
		if(line == NULL) exit(EXIT_FAILURE);
		if((*line != '*') || (*(line + 2) != ':')) continue;
		switch(*(line + 1)){
			case 'L':
				chomp(line);
				str = (char*)malloc(sizeof(char) * (strlen(line) - 2));
				cshiolink_load(strcpy(str, line + 3), load);
				free(line);
				break;
			case 'U':
				free(line);
				cshiolink_unload(unload);
				return;
			case 'S':
				fputs(line, out);
				fflush(out);
				free(line);
				str = cshiolink_request(in, request);
				if(str == NULL) str = cshiori_shiori_response_build_internal_server_error();
#if defined(WIN32)||defined(_WIN32)||defined(_Windows)||defined(__CYGWIN__)
				str_crlf = str;
				str = crlftolf(str_crlf);
				free(str_crlf);
#endif
				fputs(str, out);
				fflush(out);
				free(str);
				break;
		}
	}
}
