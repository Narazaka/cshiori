/**
 * @file
 * @brief SHIOLINKインターフェース
 * @author Narazaka: http://narazaka.net/
 *
 * (C) 2014 Narazaka : Licensed under The MIT License - http://narazaka.net/license/MIT?2014
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"

#include "cshiori.h"
#include "strutl.h"

#ifndef CSHIOLINK_H
#define CSHIOLINK_H

/**
 * @brief SHIORI/3.x load
 * 
 * load()を実行します。
 * 
 * 通常 cshiolink_mainloop() から呼ばれます。
 * 
 * @param dirpath SHIORIのディレクトリパス
 * @param load load()関数へのポインタ
 * 
 * load()関数は
 * @code
 * bool load(const char* dirpath);
 * @endcode
 * です。
 **/
bool cshiolink_load(const char* dirpath, bool (*load)(const char*));

/**
 * @brief cshiolink_request() で一度に確保する行バッファ
 **/
#define CSHIOLINK_REQUEST_LINES_BUFFER_STEP 10

/**
 * @brief SHIORI/3.x request
 * 
 * 与えられたファイルポインタ in からSHIORI/3.x Requestを読み出し、 request() 関数に渡して、戻ったSHIORI/3.x Responseを文字列にして返します。
 * 
 * 通常 cshiolink_mainloop() から呼ばれます。
 * 
 * @param in SHIORI/3.x Requestを読み出すファイルポインタ
 * @param request request()関数へのポインタ
 * 
 * request()関数は
 * @code
 * struct cshiori_response_message* request(struct cshiori_request_message* req, struct cshiori_response_message* res);
 * @endcode
 * です。
 * 
 * 入力をSHIORI/3.xとしてパースできなかった場合と、request()がNULLを返した場合は、400 Bad Requestの文字列を返します。
 * 
 * @return SHIORI/3.x Response文字列。失敗した場合NULLを返します。
 **/
char* cshiolink_request(FILE *in, struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*));

/**
 * @brief SHIORI/3.x unload
 * 
 * unload()を実行します。
 * 
 * 通常 cshiolink_mainloop() から呼ばれます。
 * 
 * @param unload unload()関数へのポインタ
 * 
 * unload()関数は
 * @code
 * bool unload(void);
 * @endcode
 * です。
 **/
bool cshiolink_unload(bool (*unload)(void));

/**
 * @brief メインループ
 * 
 * 与えられたファイルポインタ in からSHIOLINK Requestを読み出し、対応した処理を実行して out にSHIOLINK Responseを書き出します。
 * 
 * @param in SHIOLINK Requestを読み出すファイルポインタ
 * @param out SHIOLINK Responseを書き出すファイルポインタ
 * @param load load()関数へのポインタ
 * @param request request()関数へのポインタ
 * @param unload unload()関数へのポインタ
 **/
void cshiolink_mainloop(FILE *in, FILE *out, bool (*load)(const char*), struct cshiori_response_message* (*request)(struct cshiori_request_message*, struct cshiori_response_message*), bool (*unload)(void));

#endif
