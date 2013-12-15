#include<stdlibed.h>
#include<stdio.h>
#include<string.h>
#include <sys/stat.h>

#include "libed_key_value.h"
#include "libed_key_value_interface.h"

extern LIBED_KV_TABLE kv_table;

void libed_key_value_init()
{
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
#ifdef LIBED_MULTILINE_VALUE_SUPPORT
    kv_table.cur_node_key = NULL;
    kv_table.cur_node_value = NULL;
#endif
#ifdef LIBED_GET_MAIN_BUFFER
    kv_table.main_buffer = NULL;
#endif
}

static int libed_value_set(LIBED_NODE_LIST *node, char *string)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == string, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == node->value, LIBED_OPERATION_FAILLIBED);
    
    LIBED_VALUE_FREE(node->value->value_string);
    int len = strlen(string);
    LIBED_VALUE_MALLOC(node->value->value_string, len+1, char);
    snprintf(node->value->value_string, len+1, "%s", string);
    LIBED_TRACE("value = %s\n", node->value->value_string);
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

static int libed_key_set(LIBED_NODE_LIST *node, char *string)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == string, LIBED_OPERATION_FAILLIBED);

    LIBED_VALUE_FREE(node->key);
    int len = strlen(string);
    LIBED_VALUE_MALLOC(node->key, len+1, char);
    snprintf(node->key, len+1, "%s", string);
    LIBED_TRACE("key = %s\n", node->key);
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

LIBED_NODE_LIST* libed_kv_list_node_add(char *key)
{
    LIBED_FUNC_BEGIN;
    
    LIBED_NODE_LIST *node_list = NULL;
    if(NULL == kv_table.first_node)
    {
        LIBED_TRACE("first node add\n");
        kv_table.first_node = (LIBED_NODE_LIST*)malloc(sizeof(LIBED_NODE_LIST));
        memset(kv_table.first_node, 0, sizeof(LIBED_NODE_LIST));
        kv_table.last_node = kv_table.first_node;
        node_list = kv_table.first_node;
    }
    else if(kv_table.last_node && (NULL == kv_table.last_node->next))
    {
        LIBED_NODE_LIST *tmp_node = (LIBED_NODE_LIST*)libed_kv_list_node_get(key);
        if(NULL != tmp_node)
        {
            //reset value
            LIBED_TRACE("reset node value in list\n");
            LIBED_FUNC_END;
            return tmp_node;
        }
        LIBED_TRACE("add node in list\n");
        kv_table.last_node->next = (LIBED_NODE_LIST*)malloc(sizeof(LIBED_NODE_LIST));
        memset(kv_table.last_node->next, 0, sizeof(LIBED_NODE_LIST));
        kv_table.last_node = kv_table.last_node->next;
        node_list = kv_table.last_node;
    }
    else
    {
        node_list = NULL;
    }

    if(NULL != node_list)
    {
        if(NULL == node_list->value)
        {
            node_list->value = (LIBED_VALUE_STR*)malloc(sizeof(LIBED_VALUE_STR));
            memset(node_list->value, 0, sizeof(LIBED_VALUE_STR));
        }
    }
    LIBED_FUNC_END;
    return node_list;
}

LIBED_CHILD_VALUE_T* libed_key_value_child_value_add(LIBED_NODE_LIST *node, char *child_value_key)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, NULL);
    LIBED_VALUE_CHECK(NULL == child_value_key, NULL);
    LIBED_CHILD_VALUE_T *cv = NULL;

    if(NULL == node->value->first_child)
    {
        LIBED_TRACE("first child value add!\n");
        node->value->first_child = (LIBED_CHILD_VALUE_T*)malloc(sizeof(LIBED_CHILD_VALUE_T));
        memset(node->value->first_child, 0, sizeof(LIBED_CHILD_VALUE_T));
        node->value->last_child = node->value->first_child;
        cv = node->value->first_child;
    }
    else if(NULL != node->value->last_child && NULL == node->value->last_child->next)
    {
        cv = (LIBED_CHILD_VALUE_T*)libed_key_value_child_value_struct_get(node, child_value_key);
        if(NULL != cv)
        {
            LIBED_TRACE("reset child value in list\n");
            LIBED_FUNC_END;
            return cv;
        }
        LIBED_TRACE("add child value in list!\n");
        node->value->last_child->next = (LIBED_CHILD_VALUE_T*)malloc(sizeof(LIBED_CHILD_VALUE_T));
        memset(node->value->last_child->next, 0, sizeof(LIBED_CHILD_VALUE_T));
        node->value->last_child = node->value->last_child->next;
        cv = node->value->last_child;
    }
    else
    {
        cv = NULL;
    }

    LIBED_FUNC_END;
    return cv;
}

int libed_key_value_child_param_add(LIBED_CHILD_VALUE_T *child_value, int child_param_num, char *value_param)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == child_value, LIBED_INVALID_PARAM);
    LIBED_VALUE_CHECK(NULL == value_param, LIBED_INVALID_PARAM);

    LIBED_CHILD_VALUE_PARAM_T *vp = NULL;
    if(NULL == child_value->param_first)
    {
        LIBED_TRACE("first value param add!\n");
        child_value->param_first = (LIBED_CHILD_VALUE_PARAM_T*)malloc(sizeof(LIBED_CHILD_VALUE_PARAM_T));
        memset(child_value->param_first, 0, sizeof(LIBED_CHILD_VALUE_PARAM_T));
        child_value->param_first->param_value = NULL;
        child_value->param_last = child_value->param_first;
        vp = child_value->param_first;
    }
    else if(NULL != child_value->param_last && NULL == child_value->param_last->next)
    {
        vp = (LIBED_CHILD_VALUE_PARAM_T*)libed_key_value_child_param_struct_get(child_value, child_param_num);
        if(NULL != vp)
        {
            LIBED_TRACE("reset value param in list\n");
            goto OUT;
        }
        LIBED_TRACE("add value param in list!\n");
        child_value->param_last->next = (LIBED_CHILD_VALUE_PARAM_T*)malloc(sizeof(LIBED_CHILD_VALUE_PARAM_T));
        memset(child_value->param_last->next, 0, sizeof(LIBED_CHILD_VALUE_PARAM_T));
        child_value->param_last = child_value->param_last->next;
        child_value->param_last->next = NULL;
        vp = child_value->param_last;
    }
    else
    {
        vp = NULL;
    }

OUT:
    if(NULL != vp)
    {
        LIBED_VALUE_FREE(vp->param_value);
        int len = strlen(value_param);
        LIBED_VALUE_MALLOC(vp->param_value, len+1, char);
        snprintf(vp->param_value, len+1, "%s", value_param);
        LIBED_TRACE("vp->param_value = %s, %p\n", vp->param_value, vp->param_value);
    }
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

static int libed_key_value_encoding_line(LIBED_NODE_LIST *node, FILE *fd)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == fd, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == node->key, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(NULL == node->value, LIBED_OPERATION_FAILLIBED);

    LIBED_NODE_LIST *tmp_node = node;
    char *string_encoding = NULL;
    int string_len = 0;
    char tmp_value[MAX_READ_BUFFER] = {0};
    
    //get value
    LIBED_VALUE_FREE(tmp_node->value->value_string);
    int child_index = 0;
    LIBED_CHILD_VALUE_T *child_value = tmp_node->value->first_child;
    for(; NULL != child_value; child_value = child_value->next)
    {
        //add value seprator
        LIBED_TRACE("add value seprator\n");
        if(child_index != 0 && NULL != tmp_node->value->value_string)
        {
            string_len += 1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string,
                LIBED_VALUE_SEPRATOR);
        }
        
        //add child value key
        LIBED_TRACE("add child value key\n");
        if(NULL != child_value->child_value_key)
        {
            encode_special_char(child_value->child_value_key, tmp_value);
            string_len += strlen(tmp_value);
        }
        else
        {
            break;
        }
        LIBED_TRACE("child_value->child_value_key = %s\n", tmp_value);
        if(NULL == tmp_node->value->value_string)
        {
            LIBED_TRACE("value_string malloc\n");
            LIBED_VALUE_MALLOC(tmp_node->value->value_string, string_len+1, char);
            snprintf(tmp_node->value->value_string, string_len+1, "%s", tmp_value);
            LIBED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        else
        {
            LIBED_TRACE("value_string realloc\n");
            string_len += strlen(tmp_value);
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string, tmp_value);
            tmp_node->value->value_string[string_len] = '\0';
            LIBED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        //add child value param
        LIBED_TRACE("add child value param\n");
        LIBED_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
        for(; (NULL != value_param) && (NULL != value_param->param_value); value_param = value_param->next)
        {
            encode_special_char(value_param->param_value, tmp_value);
            string_len += strlen(tmp_value)+1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            //add param seprator
            sprintf(tmp_node->value->value_string, "%s%s%s", tmp_node->value->value_string,
                LIBED_PARAM_SEPRATOR, tmp_value);
        }
        ++child_index;
        LIBED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
    }

    if(NULL != tmp_node->value->value_string)
    {
        if(string_encoding == NULL)
        {
            //number+2--- '=' , '\n'
            string_len = strlen(tmp_node->key)+strlen(tmp_node->value->value_string)+4;
            LIBED_VALUE_MALLOC(string_encoding, string_len+1, char);
            snprintf(string_encoding, string_len+1, "%s %c %s\n", 
                tmp_node->key, LIBED_KEY_VALUE_SEPRATOR, tmp_node->value->value_string);
            LIBED_TRACE("string_encoding = %s\n", string_encoding);
            if(fputs(string_encoding, fd) == -1)
            {
                LIBED_TRACE("fputs error\n");
                LIBED_VALUE_FREE(tmp_node->value->value_string);
                LIBED_VALUE_FREE(string_encoding);
                return LIBED_OPERATION_FAILLIBED;
            }
            LIBED_VALUE_FREE(tmp_node->value->value_string);
            LIBED_VALUE_FREE(string_encoding);
        }
    }

    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

int libed_key_value_set(char *key, char *value)
{
    LIBED_FUNC_BEGIN;
    if((NULL == key) || (NULL == value))
    {
        return LIBED_OPERATION_FAILLIBED;
    }
    int ret = LIBED_ERROR_NONE;
#if LIST_HASH_USLIBED
    int index = (int)(*key)%LIST_NODE_ARRAY_SIZE;
    LIBED_TRACE("index = %d\n", index);
    LIBED_VALUE_MALLOC(kv_table.first_node, LIST_NODE_ARRAY_SIZE, LIBED_NODE_LIST);
    LIBED_NODE_LIST *tmp_node = &(kv_table.first_node[index]);
    while(1)
    {
        if(tmp_node)
        {
            if(NULL == tmp_node->key)
            {
                LIBED_TRACE("add first node\n");
                libed_key_set(tmp_node, key);
                libed_value_set(tmp_node, value);
                break;
            }
            else if(0 == strcmp(tmp_node->key, key))
            {
                LIBED_TRACE("set value in list which have the same key\n");
                libed_value_set(tmp_node, value);
                break;
            }

            if(NULL == tmp_node->next)
            {
                LIBED_TRACE("add node in list tail\n");
                LIBED_VALUE_MALLOC(tmp_node->next, 1, LIBED_NODE_LIST);
                libed_key_set(tmp_node->next, key);
                libed_value_set(tmp_node->next, value);
                break;
            }
            tmp_node = tmp_node->next;
        }
    }
#else
    libed_key_value_del(key);
    if(NULL != strstr(value, ";"))
    {
        char *child_value = NULL;
        int index = 0;
        child_value = strtok(value, ";");
        if(NULL != child_value)
        {
            ret = libed_key_value_child_value_set(key, value, 0, NULL);
            ++index;
        }
        while(NULL != (child_value = strtok(NULL, ";")))
        {
            ret = libed_key_value_child_value_set(key, NULL, index, child_value);
            ++index;
        }
    }
    else
    {
        ret = libed_key_value_child_value_set(key, value, 0, NULL);
    }
#endif    
    LIBED_FUNC_END;
    return ret;
}

int libed_key_value_child_value_set(char *key, char *child_key, int child_param_num, char *value_param)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == key, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(child_param_num < 0, LIBED_OPERATION_FAILLIBED);
    LIBED_TRACE("key = %s, child key = %s, child param num = %d, value param = %s\n",
        key, child_key, child_param_num, value_param);
    
    int ret = LIBED_ERROR_NONE;
    int len = 0;
    LIBED_CHILD_VALUE_T *cv = NULL;
    LIBED_CHILD_VALUE_PARAM_T *cv_param = NULL;
    LIBED_CHILD_VALUE_PARAM_T *next_param = NULL;

    /* add node */
    LIBED_NODE_LIST *node = (LIBED_NODE_LIST*)libed_kv_list_node_get(key);
    if(NULL == node)
    {
        node = libed_kv_list_node_add(key);
        ret = libed_key_set(node, key);
    }
    if(NULL == child_key)
    {
        cv = node->value->first_child;
    }
    else
    {
        /* add child value */
        if(NULL == value_param)
        {
            cv = node->value->first_child;
        }
        else
        {
            cv = (LIBED_CHILD_VALUE_T*)libed_key_value_child_value_struct_get(node, child_key);
        }
        if(NULL == cv)
        {
            cv = libed_key_value_child_value_add(node, child_key);
        }
        
        if(cv->child_value_key != child_key)
        {
            LIBED_VALUE_FREE(cv->child_value_key);
            len = strlen(child_key);
            LIBED_VALUE_MALLOC(cv->child_value_key, len+1, char);
            LIBED_TRACE("len = %d, child_value_key = %s\n", len, child_key);
            snprintf(cv->child_value_key, len+1, "%s", child_key);
            LIBED_TRACE("child key = %s\n", cv->child_value_key);
        }
    }

    if(child_param_num == 0)
    {
        LIBED_FUNC_END;
        return ret;
    }
    else
    {
        /* add value param */
        ret = libed_key_value_child_param_add(cv, child_param_num, value_param);
    }
    LIBED_FUNC_END;

    return ret;
}

int libed_key_value_child_set(char *key, int child_param_num, char *child_value)
{
    LIBED_FUNC_BEGIN;
    return libed_key_value_child_value_set(key, NULL, child_param_num, child_value);
}

int libed_key_value_param_del(LIBED_CHILD_VALUE_T *child_value)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == child_value, LIBED_INVALID_PARAM);

    LIBED_VALUE_FREE(child_value->child_value_key);
    LIBED_CHILD_VALUE_PARAM_T *next_param = NULL;

    for(; NULL != child_value->param_first; child_value->param_first = next_param)
    {
        if(NULL != child_value->param_first)
        {
            next_param = child_value->param_first->next;
        }
        LIBED_VALUE_FREE(child_value->param_first->param_value);
        LIBED_VALUE_FREE(child_value->param_first);
    }
    child_value->param_last = NULL;
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

int libed_key_value_child_del(LIBED_NODE_LIST *node)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, LIBED_INVALID_PARAM);
    LIBED_VALUE_CHECK(NULL == node->value, LIBED_INVALID_PARAM);
    
    LIBED_VALUE_FREE(node->key);
    LIBED_VALUE_FREE(node->value->value_string);

    LIBED_CHILD_VALUE_T *next_child = NULL;
    for(; node->value->first_child; node->value->first_child = next_child)
    {
        if(NULL != node->value->first_child)
        {
            next_child = node->value->first_child->next;
        }
        libed_key_value_param_del(node->value->first_child);
    }
    node->value->last_child = NULL;
    LIBED_VALUE_FREE(node->value);
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

int libed_key_value_del(char *key)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK((NULL == key || NULL == kv_table.first_node), LIBED_OPERATION_FAILLIBED);

    LIBED_NODE_LIST *tmp_node = kv_table.first_node;
    LIBED_NODE_LIST *next_node = NULL;
    if(NULL != kv_table.first_node->next)
    {
        next_node = kv_table.first_node->next;
    }
    if((NULL != kv_table.first_node)
        && (NULL != kv_table.first_node->key)
        && (0 == strcmp(kv_table.first_node->key, key)))
    {
        LIBED_TRACE("first node delete\n");
        libed_key_value_child_del(kv_table.first_node);
        LIBED_VALUE_FREE(kv_table.first_node);
        kv_table.first_node = next_node;
        if(next_node == kv_table.last_node)
        {
            kv_table.last_node = kv_table.first_node;
        }
        
        return LIBED_ERROR_NONE;
    }

    if(NULL != tmp_node && NULL != tmp_node->next)
    {
        next_node = tmp_node->next;
        while(tmp_node->next)
        {
            if(NULL != tmp_node->next->key
                && 0 == strcmp(tmp_node->next->key, key))
            {
                LIBED_TRACE("node delete\n");
                libed_key_value_child_del(tmp_node->next);
                LIBED_VALUE_FREE(tmp_node->next);
                tmp_node->next = next_node->next;
                if(next_node == kv_table.last_node)
                {
                    kv_table.last_node = tmp_node;
                }
                break;
            }
            else
            {
                tmp_node = tmp_node->next;
                next_node = next_node->next;
            }
        }
    }
    
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

int libed_key_value_child_value_del(char *key, char *child_value_key, int pos, char *value_param)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == key, LIBED_OPERATION_FAILLIBED);
    LIBED_VALUE_CHECK(pos < 0, LIBED_OPERATION_FAILLIBED);
    //LIBED_VALUE_CHECK(NULL == value_param, LIBED_OPERATION_FAILLIBED);
    LIBED_TRACE("key = %s, child_value_key = %s, pos = %d, value_param = %s\n",
        key, child_value_key, pos, value_param);
    int ret = LIBED_ERROR_NONE;
    LIBED_NODE_LIST *node = (LIBED_NODE_LIST*)libed_kv_list_node_get(key);
    
    if(NULL != node)
    {
        LIBED_CHILD_VALUE_T *child_value = NULL;
        if(child_value_key == NULL)
        {
            child_value = node->value->first_child;
        }
        else
        {
            child_value = (LIBED_CHILD_VALUE_T*)libed_key_value_child_value_struct_get(node, child_value_key);
        }

        LIBED_VALUE_CHECK(NULL == child_value, LIBED_OPERATION_FAILLIBED);
        //delete one child
        LIBED_VALUE_CHECK(NULL == value_param, libed_key_value_param_del(child_value));

        //delete child's child
        if(NULL != child_value->param_first)
        {
            LIBED_CHILD_VALUE_PARAM_T *prev_param = child_value->param_first;
            LIBED_CHILD_VALUE_PARAM_T *next_param = prev_param->next;
            LIBED_VALUE_CHECK(NULL == child_value->param_first->param_value, LIBED_OPERATION_FAILLIBED);
            
            if(pos == 0 && 0 == strcmp(child_value->child_value_key, value_param))
            {
                LIBED_VALUE_FREE(child_value->child_value_key);
                int len = strlen(child_value->param_first->param_value);
                LIBED_VALUE_MALLOC(child_value->child_value_key, len+1, char);
                snprintf(child_value->child_value_key, len+1, "%s", child_value->param_first->param_value);
                LIBED_VALUE_FREE(child_value->param_first->param_value);
                LIBED_VALUE_FREE(child_value->param_first);
                child_value->param_first = next_param;
            }
            else if(pos < 2 && 0 == strcmp(child_value->param_first->param_value, value_param))
            {
                LIBED_VALUE_FREE(child_value->param_first->param_value);
                LIBED_VALUE_FREE(child_value->param_first);
                child_value->param_first = next_param;
            }
            else
            {
                int index = 0;
                for(index = 1; index < pos-1; index++)
                {
                    prev_param = prev_param->next;
                }
                for(; NULL != prev_param && NULL != prev_param->next; prev_param = prev_param->next)
                {
                    LIBED_TRACE("param_value = %s\n", prev_param->param_value);
                    next_param = prev_param->next;
                    LIBED_TRACE("next_param = %s\n", next_param->param_value);
                    if(NULL == next_param->param_value)
                    {
                        return LIBED_OPERATION_FAILLIBED;
                    }
                    else if(0 == strcmp(prev_param->next->param_value, value_param))
                    {
                        if(prev_param->next == child_value->param_last)
                        {
                            child_value->param_last = prev_param;
                        }
                        LIBED_VALUE_FREE(prev_param->next->param_value);
                        LIBED_VALUE_FREE(prev_param->next);
                        prev_param->next = next_param->next;
                        ret = LIBED_ERROR_NONE;
                        break;
                    }
                }
            }
        }
        else
        {
            if(0 == strcmp(child_value->child_value_key, value_param))
            {
                LIBED_VALUE_FREE(child_value->child_value_key);
                ret = libed_key_value_del(key);
            }
        }
    }
    return ret;
}

int libed_key_value_encoding(char *path, int overwrite)
{
    LIBED_FUNC_BEGIN;
    if(NULL == path)
    {
        return LIBED_OPERATION_FAILLIBED;
    }
    LIBED_TRACE("\nencoding=========================================\n");
    FILE *fd = NULL;
    if(overwrite)
    {
        fd = fopen(path, "w+");
    }
    else
    {
        fd = fopen(path, "a+");
    }
    
    if(fd == NULL)
    {
        return LIBED_OPERATION_FAILLIBED;
    }
    chmod(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if(kv_table.first_node != NULL)
    {
#if LIST_HASH_USLIBED
        int index = -1;
        for(index = 0; index < LIST_NODE_ARRAY_SIZE; index++)
        {
            libed_key_value_encoding_line(&(kv_table.first_node[index]),fd);
        }
#else
        LIBED_NODE_LIST *node = NULL;
        for(node = kv_table.first_node; NULL != node; node = node->next)
        {
            libed_key_value_encoding_line(node, fd);
        }
#endif
        libed_key_value_cleanup();
        fclose(fd);
    }
    else
    {
        fclose(fd);
    }
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

int libed_key_value_cleanup()
{
    LIBED_FUNC_BEGIN;
    if(NULL != kv_table.first_node)
    {
#if LIST_HASH_USLIBED
        int i = 0;
        for(; i<LIST_NODE_ARRAY_SIZE; i++)
        {
            LIBED_NODE_LIST *tmp_node = &(kv_table.first_node[i]);
            while(tmp_node)
            {
                LIBED_VALUE_FREE(tmp_node->key);
                LIBED_VALUE_FREE(tmp_node->value->value_string);
                tmp_node = tmp_node->next;
            }
        }
        LIBED_VALUE_FREE(kv_table.first_node);
#else
        LIBED_NODE_LIST *tmp_node = kv_table.first_node;
        LIBED_NODE_LIST *next_node = kv_table.first_node->next;
        while(NULL != tmp_node)
        {
            libed_key_value_del(tmp_node->key);
            tmp_node = next_node;
            if(NULL != next_node)
            {
                next_node = next_node->next;
            }
        }

#endif
    }
#ifdef LIBED_GET_MAIN_BUFFER
    LIBED_VALUE_FREE(kv_table.main_buffer);
#endif
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

