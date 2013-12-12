#ifndef _cgi_keyvalue_h_
#define _cgi_keyvalue_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

enum ERROR_TYPE{
    CGI_ERROR_NONE = 0,
    CGI_INVALID_PARAM,
    CGI_OPERATION_FAILED,
    CGI_PERMISSION_DENIED,
    CGI_OPERATION_NOT_SUPPORTED,
    CGI_OPERATION_INTERPUT,
    CGI_CONFIG_NOT_SUPPORTED,
    CGI_HARDWARE_ERROR,
    CGI_INTERNAL_ERROR,
    CGI_NETWORK_ERROR,
    CGI_HELP = 10,
    CGI_CONFIG_VALUE_NULL = 128
};

#define DEBUG_SET 0
#if DEBUG_SET
#define CGI_TRACE(format, ...) do{ \
    printf("%s %d:\n", __FUNCTION__, __LINE__);  \
    cgi_trace(format, ##__VA_ARGS__ ); \
}while(0)

#define CGI_FUNC_BEGIN do{ \
    printf("%s begin %d++++++++++++++++++++++\n", __FUNCTION__, __LINE__); \
}while(0)

#define CGI_FUNC_END do{ \
    printf("%s end   %d++++++++++++++++++++++\n", __FUNCTION__, __LINE__); \
}while(0)

#define CGI_LOCATION do{ \
    printf("%s %d\n", __FUNCTION__, __LINE__); \
}while(0)

#else
#define CGI_TRACE (void)
#define CGI_FUNC_BEGIN
#define CGI_FUNC_END
#define CGI_LOCATION

#endif

#define CGI_STDOUT printf

#define CGI_LTRIM cgi_ltrim
#define CGI_RTRIM cgi_rtrim
#define CGI_ARGV_PRINT cgi_input_print

#define CGI_MULTILINE_VALUE_SUPPORT

#define CGI_VALUE_CHECK(VALUE, RET) do{ \
    if(VALUE){ \
        return (RET); \
    } \
}while(0)

#define MAX_READ_BUFFER 4096
#define LIST_NODE_ARRAY_SIZE  20
#define LIST_HASH_USED 0

#define CGI_VALUE_SEPRATOR "|"
#define CGI_VALUE_SEPRATOR_CH '|'
#define CGI_PARAM_SEPRATOR ";"
#define CGI_PARAM_SEPRATOR_CH ';'
#define CGI_KEY_VALUE_SEPRATOR '='

#define CGI_VALUE_PROPERTY_SINGLE 1
#define CGI_VALUE_PROPERTY_MULTIPLE 1<<1
#define CGI_VALUE_PROPERTY_OVERWRITE 1<<2

#define CGI_VALUE_FREE(A)  do{ \
    if(NULL != (A)) \
    { \
        free(A); \
        (A) = NULL; \
    } \
} while (0)

#define CGI_VALUE_MALLOC(VALUE, SIZE, STRUCT_TYPE) do{ \
    if(NULL == (VALUE)) \
    { \
        (VALUE) = (STRUCT_TYPE*)malloc((SIZE)*sizeof(STRUCT_TYPE)); \
        if((VALUE) == NULL) \
        { \
            return CGI_OPERATION_FAILED; \
        } \
    } \
    memset(VALUE, 0, (SIZE)*sizeof(STRUCT_TYPE)); \
}while(0)

#define CGI_LIST_NODE_ADD(POS, NODE) do{ \
    if((POS) == NULL || (NODE) == NULL) \
    { \
        return CGI_OPERATION_FAILED; \
    } \
    if((POS)->next == NULL) \
    { \
        (POS)->next = (NODE); \
        (NODE)->next = NULL; \
    } \
    else{ \
        (NODE)->next = (POS)->next; \
        (POS)->next = (NODE); \
    } \
}while(0)

typedef struct cgi_child_value_param_t
{
    char *param_value;
    struct cgi_child_value_param_t *next;
} CGI_CHILD_VALUE_PARAM_T;

typedef struct cgi_child_value_t
{
    char *child_value_key;
    struct cgi_child_value_param_t *param_first;
    struct cgi_child_value_param_t *param_last;
    struct cgi_child_value_t *next;
} CGI_CHILD_VALUE_T;

typedef struct cgi_key_str
{
    char *key_name;
} CGI_KEY_STR;

typedef struct cgi_value_str
{
    char *value_string;
    CGI_CHILD_VALUE_T *first_child;
    CGI_CHILD_VALUE_T *last_child;
} CGI_VALUE_STR;

typedef struct cgi_node_list
{
    char *key;
    CGI_VALUE_STR *value;
    struct cgi_node_list *next;
} CGI_NODE_LIST;

typedef struct cgi_kv_table_t
{
    CGI_NODE_LIST *first_node;
    CGI_NODE_LIST *last_node;
#ifdef CGI_MULTILINE_VALUE_SUPPORT
    char *cur_node_key;
    char *cur_node_value;
#endif
#ifdef CGI_GET_MAIN_BUFFER
    char *main_buffer;
#endif
} CGI_KV_TABLE;

#ifdef __cplusplus
}
#endif

#endif
