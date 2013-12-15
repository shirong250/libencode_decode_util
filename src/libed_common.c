#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

int libed_check_param(char* argv)
{
    if ((NULL == argv) || (NULL != strstr(argv, "-"))) {
        return ERROR;
    }
    return SUCCESS;
}

char libed_hexval(char c)
{
    if (isdigit(c)) {
        return (c - '0');
    } else {
        return (tolower(c) - 'a' + 10);
    }
}

/*
 * @param   data[input]:string to encode
 * @param   buf[output]:encoded string
 * buf should be 3 times larger than data in order to
 * encode a string only has sepcial char
 *
 * char to encode:
 *  =   :   %3d
 *  |   :   %7c
 *  ;   :   %3b
 *  %   :   %25
 */
void libed_encode_special_char(const char* data, char* buf)
{
    const char* src = data;
    char* dest = buf;
    while (*src != '\0') {
        if (*src == '=' || *src == '|' || *src == ';' || *src == '%') {
            sprintf(dest, "%%%x%x", (*src / 16), (*src % 16));
            dest += 3;
            src++;
        } else {
            *(dest++) = *src;
            src++;
        }
    }
    *dest = '\0';
}

/*
 * @param   data[input]:string to dncode
 * @param   buf[output]:dncoded string
 * buf should be at least same as data
 */
void libed_decode_special_char(const char* data, char* buf)
{
    const char* src = data;
    char* dest = buf;

    int len = strlen(data);
    while (*src != '\0') {
        if (*src == '%' && (src - data + 2) < len) {
            *(dest++) = libed_hexval(src[1]) * 16 + libed_hexval(src[2]);
            src += 3;
        } else {
            *(dest++) = *src;
            src++;
        }
    }
    *dest = '\0';
}

