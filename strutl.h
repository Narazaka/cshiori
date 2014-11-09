/**
 * @file
 * @brief 文字列ユーティリティ
 * @author Narazaka: http://narazaka.net/
 *
 * (C) 2014 Narazaka : Licensed under The MIT License - http://narazaka.net/license/MIT?2014
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef STRUTL_H
#define STRUTL_H

/**
 * @brief getline()の1回の読み込み(fgets())バッファサイズ。用途に応じて調整してください。
 **/
#define GETLINE_BUF_SIZE 100

/**
 * @brief 与えられたファイルポインタから、改行までの任意長の文字列を読み込み返します。(size_t上限まで)
 * 
 * fgetsがNULLを返せばそこまでの文字列を返します。メモリ確保に失敗すればNULLを返します。
 * @param fp ファイルポインタ
 * @return 改行を含む1行の文字列
 **/
char* getline(const FILE *fp);

/**
 * @brief 与えられた文字列に末尾の改行があれば削除します。
 * 
 * システムの改行コードにかかわらず\\rと\\nどちらも削除します。
 * @param line 1行の文字列
 **/
void chomp(char* const line);

/**
 * @brief 与えられた文字列の"\r\n"を"\n"に変換した文字列を返します。
 * 
 * @param str 文字列
 * @return 変換後文字列
 **/
char* crlftolf(char* const str);

#endif
