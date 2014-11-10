/**
 * @mainpage
 * @li cshioriはCによるSHIORI/3.xプロトコルインターフェース・パーサ・ビルダ・コンテナの実装です。 cshiori.h
 * @li cshiolinkはCによるSHIOLINKインターフェースの実装です。 cshiolink.h
 * @li strutlは文字列のユーティリティです。 strutl.h
 **/

/**
 * @file
 * @brief SHIORI/3.xインターフェース・パーサ・ビルダ・コンテナ
 * @author Narazaka: http://narazaka.net/
 *
 * (C) 2014 Narazaka : Licensed under The MIT License - http://narazaka.net/license/MIT?2014
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bool.h"

#ifndef CSHIORI_H
#define CSHIORI_H

#define LF "\r\n"
#define LF_LEN 2

/**
 * @brief SHIORI/3.x Request METHOD
 **/
enum cshiori_request_method {
	NONE, /**< 無効 */
	GET, /**< GET */
	NOTIFY /**< NOTIFY */
};

/**
 * @brief cshiori_request_message で配列参照可能なreferenceのサイズ
 **/
#define CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE 100

/**
 * @brief SHIORI/3.x Request
 * 
 * common headersにはショートカットがあります。
 * 全てのヘッダにアクセスするには cshiori_request_message_get(), cshiori_request_message_set() を使ってください。
 **/
struct cshiori_request_message{
	enum cshiori_request_method method; /**< METHOD */
	char *version; /**< SHIORI version */
	/* common headers */
	char *id; /**< ID (common header) */
	char *sender; /**< Sender (common header) */
	char *charset; /**< Charset (common header) */
	char *reference[CSHIORI_REQUEST_MESSAGE_REFERENCE_SIZE]; /**< Reference* (common header) */
	char *security_level; /**< SecurityLevel (common header) */
	char *status; /**< Status (common header) */
	char *base_id; /**< BaseID (common header) */
	/* all headers */
	char **names; /**< all header names */
	char **values; /**< all header values */
	size_t header_size; /**< all header size */
};

/**
 * @brief cshiori_response_message で配列参照可能なreferenceのサイズ
 **/
#define CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE 100

/**
 * @brief SHIORI/3.x Response
 * 
 * common headersのみ扱えます。
 **/
struct cshiori_response_message{
	short status_code; /**< STATUS CODE */
	char *version; /**< SHIORI version */
	/* common headers */
	char *value; /**< Value (common header) */
	char *sender; /**< Sender (common header) */
	char *charset; /**< Charset (common header) */
	char *reference[CSHIORI_RESPONSE_MESSAGE_REFERENCE_SIZE]; /**< Reference* (common header) */
//	char **names;
//	char **values;
//	size_t header_size;
};

/**
 * @brief SHIORI/3.x load
 * 
 * load()を実行します。
 * 
 * @param dirpath SHIORIのディレクトリパス
 * @param load load()関数へのポインタ
 * 
 * load()関数は
 * @code
 * bool load(const char* dirpath);
 * @endcode
 * です。
 * 
 * @return 成功/失敗
 **/
bool cshiori_load(const char* dirpath, bool (*load)(const char*));

/**
 * @brief SHIORI/3.x request
 * 
 * SHIORI/3.x Requestをパースし、 request() 関数に渡して、戻ったSHIORI/3.x Responseを文字列にして返します。
 * 
 * @param lines 行単位で分割され改行文字を削除したSHIORI/3.x Request文字列
 * @param lines_length SHIORI/3.x Requestの行数
 * @param request request()関数へのポインタ
 * 
 * request()関数は
 * @code
 * struct cshiori_response_message* request(struct cshiori_request_message* req, struct cshiori_response_message* res);
 * @endcode
 * です。
 * 
 * request()にはすでに初期化されたreq, resが渡されます。
 * reqを参照しながらresを適切に設定してそのまま返せば大丈夫です。
 * 
 * 入力をSHIORI/3.xとしてパースできなかった場合と、request()がNULLを返した場合は、400 Bad Requestの文字列を返します。
 * 
 * @return SHIORI/3.x Response文字列。失敗した場合NULLを返します。
 **/
char* cshiori_request(char** const lines, const size_t lines_length, struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*));

/**
 * @brief SHIORI/3.x unload
 * 
 * unload()を実行します。
 * 
 * @param unload unload()関数へのポインタ
 * 
 * unload()関数は
 * @code
 * bool unload(void);
 * @endcode
 * です。
 * 
 * @return 成功/失敗
 **/
bool cshiori_unload(bool (*unload)(void));

/**
 * @brief SHIORI/3.x Requestをmallocします。
 * 
 * methodは0、common headersはNULL、header_sizeは0で初期化されます。
 * @return SHIORI/3.x Request
 **/
struct cshiori_request_message* cshiori_request_message_new(void);

/**
 * @brief SHIORI/3.x Requestをfreeします。
 * 
 * @param req SHIORI/3.x Request
 **/
void cshiori_request_message_delete(struct cshiori_request_message* req);

/**
 * @brief SHIORI/3.x Requestのヘッダを取得します。
 * 
 * @param req SHIORI/3.x Request
 * @param name ヘッダ名
 * @return ヘッダの値。存在しない場合NULLを返します。
 **/
char* cshiori_request_message_get(struct cshiori_request_message* req, const char* const name);

/**
 * @brief SHIORI/3.x Requestのヘッダを設定します。
 * 
 * これで設定したヘッダはcommon headersのショートカットに反映されません。common headersに含まれないヘッダを扱う場合に使用してください。
 * 
 * 同名のヘッダがある場合上書きします。
 * 
 * @param req SHIORI/3.x Request
 * @param name ヘッダ名
 * @param value ヘッダ値。NULLを指定した場合、ヘッダは削除されます。
 * @return 成功時true / 失敗時false
 **/
bool cshiori_request_message_set(struct cshiori_request_message* req, char* const name, char* const value);

/**
 * @brief SHIORI/3.x Responseをmallocします。
 * 
 * status_codeは0、common headersはNULLで初期化されます。
 * @return SHIORI/3.x Response
 **/
struct cshiori_response_message* cshiori_response_message_new(void);

/**
 * @brief SHIORI/3.x Responseをfreeします。
 * 
 * @param res SHIORI/3.x Response
 **/
void cshiori_response_message_delete(struct cshiori_response_message* res);

/*
void cshiori_response_message_get(struct cshiori_response_message* res, const char* const name);

void cshiori_response_message_set(struct cshiori_response_message* res, const char* const name, const char* const value);
*/

/**
 * @brief cshiori_shiori_request_parse() で一回に確保するヘッダサイズ
 **/
#define CSHIORI_SHIORI_REQUEST_PARSE_HEADER_UNIT_SIZE 10

/**
 * @brief SHIORI/3.x Requestをパースします。
 * 
 * @param lines 行単位で分割され改行文字を削除したSHIORI/3.x Request文字列
 * @param lines_length SHIORI/3.x Requestの行数
 * @return SHIORI/3.x Request。失敗した場合NULLを返します。
 **/
struct cshiori_request_message* cshiori_shiori_request_parse(char** const lines, const size_t lines_length);

/**
 * @brief SHIORI/3.x Requestをビルドします。
 * 
 * @param req SHIORI/3.x Request
 * @return SHIORI/3.x Request文字列。失敗した場合NULLを返します。
 **/
char* cshiori_shiori_request_build(const struct cshiori_request_message* const req);

/**
 * @brief SHIORI/3.x Responseをビルドします。
 * 
 * @param res SHIORI/3.x Response
 * @return SHIORI/3.x Response文字列。失敗した場合NULLを返します。
 **/
char* cshiori_shiori_response_build(const struct cshiori_response_message* const res);

/**
 * @brief SHIORI/3.x Response 400 Bad Requestをビルドします。
 * 
 * @return SHIORI/3.x Response文字列。失敗した場合NULLを返します。
 **/
char* cshiori_shiori_response_build_bad_request();

/**
 * @brief SHIORI/3.x Response 500 Internal Server Errorをビルドします。
 * 
 * @return SHIORI/3.x Response文字列。失敗した場合NULLを返します。
 **/
char* cshiori_shiori_response_build_internal_server_error();

#endif
