#include <string.h>

bool shiori_load(const char* dirpath){
	return true;
}

bool shiori_unload(void){
	return true;
}

struct cshiori_response_message* shiori_request(struct cshiori_request_message* req, struct cshiori_response_message* res){
	res->version = "3.0";
	res->sender = "cshiori";
	res->charset = "Shift_JIS";
	if(req->method == NOTIFY){
		res->status_code = 204;
		return res;
	}
	if(req->id == NULL){
		return NULL;
	}
	if(0 == strcmp(req->id, "version")){
		res->status_code = 200;
		res->value = "0.0.1";
	}else if(0 == strcmp(req->id, "name")){
		res->status_code = 200;
		res->value = "cshiori";
	}else if(0 == strcmp(req->id, "craftman")){
		res->status_code = 200;
		res->value = "Narazaka";
	}else if(0 == strcmp(req->id, "craftmanw")){
		res->status_code = 200;
		res->value = "奈良阪某";
	}else if(0 == strcmp(req->id, "OnBoot")){
		res->status_code = 200;
		res->value = "\\h\\s[0]起動。\\e";
	}else if(0 == strcmp(req->id, "OnClose")){
		res->status_code = 200;
		res->value = "\\h\\s[0]終了。\\w9\\-";
	}else{
		return NULL;
	}
	return res;
}
