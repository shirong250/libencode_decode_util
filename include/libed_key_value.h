#ifndef _libed_keyvalue_h_
#define _libed_keyvalue_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

enum ERROR_TYPE{
    LIBED_ERROR_NONE = 0,
    LIBED_INVALID_PARAM,
    LIBED_OPERATION_FAILED,
    LIBED_PERMISSION_DENIED,
    LIBED_OPERATION_NOT_SUPPORTED,
    LIBED_OPERATION_INTERPUT,
    LIBED_CONFIG_NOT_SUPPORTED,
    LIBED_HARDWARE_ERROR,
    LIBED_INTERNAL_ERROR,
    LIBED_NETWORK_ERROR,
    LIBED_HELP = 10,
    LIBED_CONFIG_VALUE_NULL = 128
};

#define DEBUG_SET 0
#if DEBUG_SET
#define LIBED_TRACE(format, ...) do{ \
    printf("%s %d:\n", __FUNCTION__, __LINE__);  \
    libed_trace(format, ##__VA_ARGS__ ); \
}while(0)

#define LIBED_FUNC_BEGIN do{ \
    printf("%s begin %d++++++++++++++++++++++\n", __FUNCTION__, __LINE__); \
}while(0)

#define LIBED_FUNC_END do{ \
    printf("%s end   %d++++++++++++++++++++++\n", __FUNCTION__, __LINE__); \
}while(0)

#define LIBED_LOCATION do{ \
    printf("%s %d\n", __FUNCTION__, __LINE__); \
}while(0)

#else
#define LIBED_TRACE (void)
#define LIBED_FUNC_BEGIN
#define LIBED_FUNC_END
#define LIBED_LOCATION

#endif

#define LIBED_STDOUT printf

#define LIBED_LTRIM libed_ltrim
#define LIBED_RTRIM libed_rtrim
#define LIBED_ARGV_PRINT libed_input_print

#define LIBED_MULTILINE_VALUE_SUPPORT

#define LIBED_VALUE_CHECK(VALUE, RET) do{ \
    if(VALUE){ \
        return (RET); \
    } \
}while(0)

#define MAX_READ_BUFFER 4096
#define LIST_NODE_ARRAY_SIZE  20
#define LIST_HASH_USED 0

#define LIBED_VALUE_SEPRATOR "|"
#define LIBED_VALUE_SEPRATOR_CH '|'
#define LIBED_PARAM_SEPRATOR ";"
#define LIBED_PARAM_SEPRATOR_CH ';'
#define LIBED_KEY_VALUE_SEPRATOR '='

#define LIBED_VALUE_PROPERTY_SINGLE 1
#define LIBED_VALUE_PROPERTY_MULTIPLE 1<<1
#define LIBED_VALUE_PROPERTY_OVERWRITE 1<<2

#define LIBED_VALUE_FREE(A)  do{ \
    if(NULL != (A)) \
    { \
        free(A); \
        (A) = NULL; \
    } \
} while (0)

#define LIBED_VALUE_MALLOC(VALUE, SIZE, STRUCT_TYPE) do{ \
    if(NULL == (VALUE)) \
    { \
        (VALUE) = (STRUCT_TYPE*)malloc((SIZE)*sizeof(STRUCT_TYPE)); \
        if((VALUE) == NULL) \
        { \
            return LIBED_OPERATION_FAILED; \
        } \
    } \
    memset(VALUE, 0, (SIZE)*sizeof(STRUCT_TYPE)); \
}while(0)

#define LIBED_LIST_NODE_ADD(POS, NODE) do{ \
    if((POS) == NULL || (NODE) == NULL) \
    { \
        return LIBED_OPERATION_FAILED; \
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

typedef struct libed_child_value_param_t
{
    char *param_value;
    struct libed_child_value_param_t *next;
} LIBED_CHILD_VALUE_PARAM_T;

typedef struct libed_child_value_t
{
    char *child_value_key;
    struct libed_child_value_param_t *param_first;
    struct libed_child_value_param_t *param_last;
    struct libed_child_value_t *next;
} LIBED_CHILD_VALUE_T;

typedef struct libed_key_str
{
    char *key_name;
} LIBED_KEY_STR;

typedef struct libed_value_str
{
    char *value_string;
    LIBED_CHILD_VALUE_T *first_child;
    LIBED_CHILD_VALUE_T *last_child;
} LIBED_VALUE_STR;

typedef struct libed_node_list
{
    char *key;
    LIBED_VALUE_STR *value;
    struct libed_node_list *next;
} LIBED_NODE_LIST;

typedef struct libed_kv_table_t
{
    LIBED_NODE_LIST *first_node;
    LIBED_NODE_LIST *last_node;
#ifdef LIBED_MULTILINE_VALUE_SUPPORT
    char *cur_node_key;
    char *cur_node_value;
#endif
#ifdef LIBED_GET_MAIN_BUFFER
    char *main_buffer;
#endif
} LIBED_KV_TABLE;

#ifdef __cplusplus
}
#endif

#endif
