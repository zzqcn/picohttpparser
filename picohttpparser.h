/*
 * Copyright (c) 2009-2014 Kazuho Oku, Tokuhiro Matsuno, Daisuke Murase,
 *                         Shigeo Mitsunari
 *
 * The software is licensed under either the MIT License (below) or the Perl
 * license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef picohttpparser_h
#define picohttpparser_h

#include <sys/types.h>

#ifdef _MSC_VER
#define ssize_t intptr_t
#endif

/* $Id$ */

#ifdef __cplusplus
extern "C" {
#endif

/* contains name and value of a header (name == NULL if is a continuing line
 * of a multiline header */

/** HTTP首部字段 */
struct phr_header {
    const char *name;   /**< 首部字段名, 如Host. 为NULL表示多行首部的连续行 */
    size_t name_len;    /**< 首部字段名长度 */
    const char *value;  /**< 首部字段值, 如baidu.com */
    size_t value_len;   /**< 首部字段值长度 */
};

/* returns number of bytes consumed if successful, -2 if request is partial,
 * -1 if failed */

/**
 * @brief 解析HTTP请求
 * 
 * @param buf [IN] 数据
 * @param len [IN] 数据长度
 * @param method [OUT] HTTP请求
 * @param method_len [OUT] HTTP请求长度
 * @param path [OUT] 请求URL
 * @param path_len [OUT] 请求URL长度
 * @param minor_version  [OUT] HTTP协议子版本号, 如HTTP/1.1中最后一个数字
 * @param headers [OUT] HTTP首部字段数组
 * @param num_headers [IN/OUT] HTTP首部字段数组元素个数. 调用时传入的值表示最多解析多少个字段
 * @param last_len [IN] ???
 * 
 * @return int 成功时返回已处理的字节数, 否则返回错误码
 * @retval -1 失败, 比如遇到格式错误
 * @retval -2 请求首部不完整
 */
int phr_parse_request(const char *buf, size_t len, const char **method, size_t *method_len,
                      const char **path, size_t *path_len, int *minor_version,
                      struct phr_header *headers, size_t *num_headers, size_t last_len);

/**
 * @brief 解析HTTP响应
 * 
 * @param buf [IN] 数据
 * @param len [IN] 数据长度
 * @param minor_version  [OUT] HTTP协议子版本号, 如HTTP/1.1中最后一个数字
 * @param status [OUT] 状态码, 如200或404
 * @param msg [OUT] 响应消息, 如OK或Not Found
 * @param msg_len [OUT] 响应消息长度
 * @param headers [OUT] HTTP首部字段数组
 * @param num_headers [IN/OUT] HTTP首部字段数组元素个数. 调用时传入的值表示最多解析多少个字段
 * @param last_len [IN] ???
 * 
 * @return int 成功时返回已处理的字节数, 否则返回错误码
 * @retval -1 失败, 比如遇到格式错误
 * @retval -2 请求首部不完整
 */
int phr_parse_response(const char *_buf, size_t len, int *minor_version, int *status,
                       const char **msg, size_t *msg_len, struct phr_header *headers,
                       size_t *num_headers, size_t last_len);

/**
 * @brief 解析HTTP首部字段
 * 
 * @param buf [IN] 数据
 * @param len [IN] 数据长度
 * @param headers [OUT] HTTP首部字段数组
 * @param num_headers [IN/OUT] HTTP首部字段数组元素个数. 调用时传入的值表示最多解析多少个字段
 * @param last_len [IN] ???
 * 
 * @return int 成功时返回已处理的字节数, 否则返回错误码
 * @retval -1 失败, 比如遇到格式错误
 * @retval -2 请求首部不完整
 */
int phr_parse_headers(const char *buf, size_t len, struct phr_header *headers, size_t *num_headers,
                      size_t last_len);

/* should be zero-filled before start */
struct phr_chunked_decoder {
    size_t bytes_left_in_chunk; /* number of bytes left in current chunk */
    char consume_trailer;       /* if trailing headers should be consumed */
    char _hex_count;
    char _state;
};

/* the function rewrites the buffer given as (buf, bufsz) removing the chunked-
 * encoding headers.  When the function returns without an error, bufsz is
 * updated to the length of the decoded data available.  Applications should
 * repeatedly call the function while it returns -2 (incomplete) every time
 * supplying newly arrived data.  If the end of the chunked-encoded data is
 * found, the function returns a non-negative number indicating the number of
 * octets left undecoded at the tail of the supplied buffer.  Returns -1 on
 * error.
 */
ssize_t phr_decode_chunked(struct phr_chunked_decoder *decoder, char *buf, size_t *bufsz);

/* returns if the chunked decoder is in middle of chunked data */
int phr_decode_chunked_is_in_data(struct phr_chunked_decoder *decoder);

#ifdef __cplusplus
}
#endif

#endif
