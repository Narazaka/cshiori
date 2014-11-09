#include "cshiori.h"

bool cshiori_load(const char* dirpath, bool (*load)(const char*)){
	return load(dirpath);
}

char* cshiori_request(char** const lines, const size_t lines_length, struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*)){
	struct cshiori_request_message* req = cshiori_shiori_request_parse(lines, lines_length);
	if(req == NULL) return cshiori_shiori_response_build_bad_request();
	struct cshiori_response_message* res = cshiori_response_message_new();
	res = request(req, res);
	cshiori_request_message_delete(req);
	if(res == NULL) return cshiori_shiori_response_build_bad_request();
	char* str = cshiori_shiori_response_build(res);
	cshiori_response_message_delete(res);
	return str;
}

bool cshiori_unload(bool (*unload)(void)){
	return unload();
}

struct cshiori_request_message* cshiori_request_message_new(void){
	struct cshiori_request_message* req = (struct cshiori_request_message*)malloc(sizeof(struct cshiori_request_message));
	req->method = NONE;
	req->version = NULL;
	req->id = NULL;
	req->sender = NULL;
	req->charset = NULL;
	size_t i;
	for(i = 0; i < CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE; ++i) req->reference[i] = NULL;
	req->security_level = NULL;
	req->status = NULL;
	req->base_id = NULL;
	req->names = NULL;
	req->values = NULL;
	req->header_size = 0;
	return req;
}

void cshiori_request_message_delete(struct cshiori_request_message* req){
/*	free(req->id);
	free(req->sender);
	free(req->charset);
	for(i = 0; i < CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE; ++i) free(req->reference[i]);
	free(req->security_level);
	free(req->status);
	free(req->base_id);*/
	size_t i;
	for(i = 0; i < req->header_size; ++i){
		free(*(req->names + i));
		free(*(req->values + i));
	}
	free(req->names);
	free(req->values);
	free(req);
}

char* cshiori_request_message_get(struct cshiori_request_message* req, const char* const name){
	size_t i;
	for(i = 0; i < req->header_size; ++i){
		char* h_name = *(req->names + i);
		if(h_name != NULL && (0 == strcmp(h_name, name))){
			return *(req->values + i);
		}
	}
	return NULL;
}

bool cshiori_request_message_set(struct cshiori_request_message* req, char* const name, char* const value){
	char** names_new;
	char** values_new;
	size_t i;
	for(i = 0; i < req->header_size; ++i){
		char* h_name = *(req->names + i);
		if(h_name != NULL && (0 == strcmp(h_name, name))){
			free(*(req->values + i));
			*(req->values + i) = value;
			return true;
		}
	}
	names_new = (char**)realloc(req->names, sizeof(char*) * req->header_size + 1);
	if(names_new == NULL){
		return false;
	}
	req->names = names_new;
	values_new = (char**)realloc(req->values, sizeof(char*) * req->header_size + 1);
	if(values_new == NULL){
		return false;
	}
	req->values = values_new;
	*(req->names + req->header_size) = name;
	*(req->values + req->header_size) = value;
	req->header_size ++;
	return true;
}

struct cshiori_response_message* cshiori_response_message_new(void){
	struct cshiori_response_message* res = (struct cshiori_response_message*)malloc(sizeof(struct cshiori_response_message));
	res->status_code = 0;
	res->version = NULL;
	res->value = NULL;
	res->sender = NULL;
	res->charset = NULL;
	size_t i;
	for(i = 0; i < CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE; ++i) res->reference[i] = NULL;
	return res;
}

void cshiori_response_message_delete(struct cshiori_response_message* res){
	free(res->value);
	free(res->sender);
	free(res->charset);
	size_t i;
	for(i = 0; i < CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE; ++i) free(res->reference[i]);
	free(res);
}

#define CSHIORI_SHIORI_REQUEST_PARSE_HEADER_UNIT_SIZE 10
struct cshiori_request_message* cshiori_shiori_request_parse(char** const lines, const size_t lines_length){
	struct cshiori_request_message* req = cshiori_request_message_new();
	char** names_new;
	char** values_new;
	size_t lines_index;
	char* line;
	char* header_separator;
	/* status line */
	static const char* const shiori3_get_pre = "GET SHIORI/3.";
	static const char* const shiori3_notify_pre = "NOTIFY SHIORI/3.";
	static const char* const reference_pre = "Reference";
	const size_t shiori3_get_pre_length = strlen(shiori3_get_pre);
	const size_t shiori3_notify_pre_length = strlen(shiori3_notify_pre);
	const size_t reference_pre_length = strlen(reference_pre);
	line = *lines;
	if(0 == strncmp(line, shiori3_get_pre, shiori3_get_pre_length)){
		req->method = GET;
		req->version = (char*)malloc(sizeof(char) * (strlen(line) - shiori3_get_pre_length + 3));
		strcpy(req->version, line + shiori3_get_pre_length - 2);
	}else if(0 == strncmp(line, shiori3_notify_pre, strlen(shiori3_notify_pre))){
		req->method = NOTIFY;
		req->version = (char*)malloc(sizeof(char) * (strlen(line) - shiori3_notify_pre_length + 3));
		strcpy(req->version, line + shiori3_notify_pre_length - 2);
	}else{
		return NULL;
	}
	/* headers */
	for(lines_index = 1; lines_index < lines_length - 1; ++lines_index){
		line = *(lines + lines_index);
		header_separator = strstr(line, ": ");
		if(header_separator == NULL) return NULL;
		size_t name_length = header_separator - line;
		char* name = (char*)malloc(sizeof(char) * (name_length + 1));
		strncpy(name, line, name_length);
		name[name_length] = '\0';
		size_t value_length = strlen(header_separator) - 2;
		char* value = (char*)malloc(sizeof(char) * (value_length + 1));
		strncpy(value, header_separator + 2, value_length);
		value[value_length] = '\0';
		if(0 == strncmp(name, reference_pre, reference_pre_length)){
			char* number = name + reference_pre_length;
			char nc;
			size_t i;
			bool is_number = true;
			for(i = 0; i < name_length - reference_pre_length; ++i){
				nc = *(number + i);
				if(!('0' <= nc && nc <= '9')){
					is_number = false;
					break;
				}
			}
			if(is_number){
				size_t index = atoi(number);
				if(index < CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE) req->reference[index] = value;
			}
		}
		else if(0 == strcmp(name, "ID")){ req->id = value; }
		else if(0 == strcmp(name, "Charset")){ req->charset = value; }
		else if(0 == strcmp(name, "Sender")){ req->sender = value; }
		else if(0 == strcmp(name, "SecurityLevel")){ req->security_level = value; }
		else if(0 == strcmp(name, "Status")){ req->status = value; }
		else if(0 == strcmp(name, "BaseID")){ req->base_id = value; }
		if(0 == (req->header_size % CSHIORI_SHIORI_REQUEST_PARSE_HEADER_UNIT_SIZE)){
			names_new = (char**)realloc(req->names, sizeof(char*) * (req->header_size + CSHIORI_SHIORI_REQUEST_PARSE_HEADER_UNIT_SIZE));
			if(names_new == NULL){
				size_t i;
				for(i = 0; i < req->header_size; ++i){
					free(*(req->names + i));
				}
				free(req->names);
				return NULL;
			}
			req->names = names_new;
			values_new = (char**)realloc(req->values, sizeof(char*) * (req->header_size + CSHIORI_SHIORI_REQUEST_PARSE_HEADER_UNIT_SIZE));
			if(values_new == NULL){
				size_t i;
				for(i = 0; i < req->header_size; ++i){
					free(*(req->values + i));
				}
				free(req->values);
				return NULL;
			}
			req->values = values_new;
		}
		*(req->names + req->header_size) = name;
		*(req->values + req->header_size) = value;
		req->header_size ++;
	}
	return req;
}

char* cshiori_shiori_request_build(const struct cshiori_request_message* const req){
	char* str = NULL;
	char* str_p;
	size_t i;
	/* length */
	size_t content_length = 0;
	size_t request_line_length;
	size_t* header_length;
	size_t line_length = 1;
	char* request_line;
	switch(req->method){
		case GET:
			sprintf(request_line, "GET SHIORI/%s", req->version);
			break;
		case NOTIFY:
			sprintf(request_line, "NOTIFY SHIORI/%s", req->version);
			break;
		default: return NULL;
	}
	request_line_length = strlen(request_line);
	line_length ++;
	content_length += request_line_length;
	header_length = (size_t*)malloc(sizeof(size_t) * req->header_size);
	for(i = 0; i < req->header_size; ++i){
		char* name = *(req->names + i);
		char* value = *(req->values + i);
		if(name != NULL && value != NULL){
			*(header_length + i) = strlen(name) + 2 + strlen(value);
			line_length ++;
			content_length += *(header_length + i);
		}
	}
	content_length += line_length * LF_LEN; /* LF */
	/* copy str */
	str = (char*)malloc(sizeof(char) * (content_length + 1));
	str_p = str;
	strcpy(str_p, request_line);
	str_p += request_line_length;
	strcpy(str_p, LF);
	str_p += LF_LEN;
	for(i = 0; i < req->header_size; ++i){
		char* name = *(req->names + i);
		char* value = *(req->values + i);
		if(name != NULL && value != NULL){
			sprintf(str_p, "%s: %s", name, value);
			str_p += *(header_length + i);
			strcpy(str_p, LF);
			str_p += LF_LEN;
		}
	}
	strcpy(str_p, LF);
	free(header_length);
	return str;
}

char* cshiori_shiori_response_build(const struct cshiori_response_message* const res){
	char* str = NULL;
	char* str_p;
	size_t i;
	/* length */
	static const char* const shiori_pre = "SHIORI/";
	const size_t shiori_pre_length = strlen(shiori_pre);
	size_t content_length = 0;
	size_t status_length = 0, value_length = 0, sender_length = 0, charset_length = 0;
	size_t reference_length[CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE];
	size_t line_length = 1;
	if(res->version == NULL) return NULL;
	char* status_message;
	switch(res->status_code){
		case 200: status_message = "OK"; break;
		case 204: status_message = "No Content"; break;
		case 311: status_message = "Not Enough"; break;
		case 312: status_message = "Advice"; break;
		case 400: status_message = "Bad Request"; break;
		case 500: status_message = "Internal Server Error"; break;
		default: return NULL;
	}
	status_length = shiori_pre_length + strlen(res->version) + 5 + strlen(status_message); line_length ++;
	if(res->value != NULL){ value_length = 5 + 2 + strlen(res->value); line_length ++; }
	if(res->sender != NULL){ sender_length = 6 + 2 + strlen(res->sender); line_length ++; }
	if(res->charset != NULL){ charset_length = 7 + 2 + strlen(res->charset); line_length ++; }
	content_length += status_length + value_length + sender_length + charset_length;
	for(i = 0; i < CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE; ++i){
		if(res->reference[i] != NULL){
			reference_length[i] = 9 + (1 + ((size_t) log10(i))) + 2 + strlen(res->reference[i]);
			line_length ++;
		}else{
			reference_length[i] = 0;
		}
		content_length += reference_length[i];
	}
	content_length += line_length * LF_LEN; /* LF */
	/* copy str */
	str = (char*)malloc(sizeof(char) * (content_length + 1));
	str_p = str;
	sprintf(str, "%s%s %03d %s", shiori_pre, res->version, res->status_code, status_message);
	str_p += status_length;
	strcpy(str_p, LF);
	str_p += LF_LEN;
	if(res->value != NULL){ sprintf(str_p, "Value: %s", res->value); str_p += value_length; strcpy(str_p, LF); str_p += LF_LEN; }
	if(res->sender != NULL){ sprintf(str_p, "Sender: %s", res->sender); str_p += sender_length; strcpy(str_p, LF); str_p += LF_LEN; }
	if(res->charset != NULL){ sprintf(str_p, "Charset: %s", res->charset); str_p += charset_length; strcpy(str_p, LF); str_p += LF_LEN; }
	for(i = 0; i < CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE; ++i){
		if(res->reference[i] != NULL){
			sprintf(str_p, "Reference%d: %s", i, res->reference[i]);
			str_p += reference_length[i];
			strcpy(str_p, LF);
			str_p += LF_LEN;
		}
	}
	strcpy(str_p, LF);
	return str;
}

char* cshiori_shiori_response_build_bad_request(){
	struct cshiori_response_message* res = cshiori_response_message_new();
	res->version = "3.0";
	res->status_code = 400;
	res->charset = "Shift_JIS";
	char* str = cshiori_shiori_response_build(res);
	cshiori_response_message_delete(res);
	return str;
}

char* cshiori_shiori_response_build_internal_server_error(){
	struct cshiori_response_message* res = cshiori_response_message_new();
	res->version = "3.0";
	res->status_code = 500;
	res->charset = "Shift_JIS";
	char* str = cshiori_shiori_response_build(res);
	cshiori_response_message_delete(res);
	return str;
}
