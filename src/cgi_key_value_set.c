#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/stat.h>

#include "cgi_key_value.h"
#include "cgi_key_value_interface.h"

extern CGI_KV_TABLE kv_table;

void cgi_key_value_init()
{
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
#ifdef CGI_MULTILINE_VALUE_SUPPORT
    kv_table.cur_node_key = NULL;
    kv_table.cur_node_value = NULL;
#endif
#ifdef CGI_GET_MAIN_BUFFER
    kv_table.main_buffer = NULL;
#endif
}

static int cgi_value_set(CGI_NODE_LIST *node, char *string)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == string, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == node->value, CGI_OPERATION_FAILED);
    
    CGI_VALUE_FREE(node->value->value_string);
    int len = strlen(string);
    CGI_VALUE_MALLOC(node->value->value_string, len+1, char);
    snprintf(node->value->value_string, len+1, "%s", string);
    CGI_TRACE("value = %s\n", node->value->value_string);
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

static int cgi_key_set(CGI_NODE_LIST *node, char *string)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == string, CGI_OPERATION_FAILED);

    CGI_VALUE_FREE(node->key);
    int len = strlen(string);
    CGI_VALUE_MALLOC(node->key, len+1, char);
    snprintf(node->key, len+1, "%s", string);
    CGI_TRACE("key = %s\n", node->key);
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

CGI_NODE_LIST* cgi_kv_list_node_add(char *key)
{
    CGI_FUNC_BEGIN;
    
    CGI_NODE_LIST *node_list = NULL;
    if(NULL == kv_table.first_node)
    {
        CGI_TRACE("first node add\n");
        kv_table.first_node = (CGI_NODE_LIST*)malloc(sizeof(CGI_NODE_LIST));
        memset(kv_table.first_node, 0, sizeof(CGI_NODE_LIST));
        kv_table.last_node = kv_table.first_node;
        node_list = kv_table.first_node;
    }
    else if(kv_table.last_node && (NULL == kv_table.last_node->next))
    {
        CGI_NODE_LIST *tmp_node = (CGI_NODE_LIST*)cgi_kv_list_node_get(key);
        if(NULL != tmp_node)
        {
            //reset value
            CGI_TRACE("reset node value in list\n");
            CGI_FUNC_END;
            return tmp_node;
        }
        CGI_TRACE("add node in list\n");
        kv_table.last_node->next = (CGI_NODE_LIST*)malloc(sizeof(CGI_NODE_LIST));
        memset(kv_table.last_node->next, 0, sizeof(CGI_NODE_LIST));
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
            node_list->value = (CGI_VALUE_STR*)malloc(sizeof(CGI_VALUE_STR));
            memset(node_list->value, 0, sizeof(CGI_VALUE_STR));
        }
    }
    CGI_FUNC_END;
    return node_list;
}

CGI_CHILD_VALUE_T* cgi_key_value_child_value_add(CGI_NODE_LIST *node, char *child_value_key)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, NULL);
    CGI_VALUE_CHECK(NULL == child_value_key, NULL);
    CGI_CHILD_VALUE_T *cv = NULL;

    if(NULL == node->value->first_child)
    {
        CGI_TRACE("first child value add!\n");
        node->value->first_child = (CGI_CHILD_VALUE_T*)malloc(sizeof(CGI_CHILD_VALUE_T));
        memset(node->value->first_child, 0, sizeof(CGI_CHILD_VALUE_T));
        node->value->last_child = node->value->first_child;
        cv = node->value->first_child;
    }
    else if(NULL != node->value->last_child && NULL == node->value->last_child->next)
    {
        cv = (CGI_CHILD_VALUE_T*)cgi_key_value_child_value_struct_get(node, child_value_key);
        if(NULL != cv)
        {
            CGI_TRACE("reset child value in list\n");
            CGI_FUNC_END;
            return cv;
        }
        CGI_TRACE("add child value in list!\n");
        node->value->last_child->next = (CGI_CHILD_VALUE_T*)malloc(sizeof(CGI_CHILD_VALUE_T));
        memset(node->value->last_child->next, 0, sizeof(CGI_CHILD_VALUE_T));
        node->value->last_child = node->value->last_child->next;
        cv = node->value->last_child;
    }
    else
    {
        cv = NULL;
    }

    CGI_FUNC_END;
    return cv;
}

int cgi_key_value_child_param_add(CGI_CHILD_VALUE_T *child_value, int child_param_num, char *value_param)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == child_value, CGI_INVALID_PARAM);
    CGI_VALUE_CHECK(NULL == value_param, CGI_INVALID_PARAM);

    CGI_CHILD_VALUE_PARAM_T *vp = NULL;
    if(NULL == child_value->param_first)
    {
        CGI_TRACE("first value param add!\n");
        child_value->param_first = (CGI_CHILD_VALUE_PARAM_T*)malloc(sizeof(CGI_CHILD_VALUE_PARAM_T));
        memset(child_value->param_first, 0, sizeof(CGI_CHILD_VALUE_PARAM_T));
        child_value->param_first->param_value = NULL;
        child_value->param_last = child_value->param_first;
        vp = child_value->param_first;
    }
    else if(NULL != child_value->param_last && NULL == child_value->param_last->next)
    {
        vp = (CGI_CHILD_VALUE_PARAM_T*)cgi_key_value_child_param_struct_get(child_value, child_param_num);
        if(NULL != vp)
        {
            CGI_TRACE("reset value param in list\n");
            goto OUT;
        }
        CGI_TRACE("add value param in list!\n");
        child_value->param_last->next = (CGI_CHILD_VALUE_PARAM_T*)malloc(sizeof(CGI_CHILD_VALUE_PARAM_T));
        memset(child_value->param_last->next, 0, sizeof(CGI_CHILD_VALUE_PARAM_T));
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
        CGI_VALUE_FREE(vp->param_value);
        int len = strlen(value_param);
        CGI_VALUE_MALLOC(vp->param_value, len+1, char);
        snprintf(vp->param_value, len+1, "%s", value_param);
        CGI_TRACE("vp->param_value = %s, %p\n", vp->param_value, vp->param_value);
    }
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

static int cgi_key_value_encoding_line(CGI_NODE_LIST *node, FILE *fd)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == fd, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == node->key, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(NULL == node->value, CGI_OPERATION_FAILED);

    CGI_NODE_LIST *tmp_node = node;
    char *string_encoding = NULL;
    int string_len = 0;
    char tmp_value[MAX_READ_BUFFER] = {0};
    
    //get value
    CGI_VALUE_FREE(tmp_node->value->value_string);
    int child_index = 0;
    CGI_CHILD_VALUE_T *child_value = tmp_node->value->first_child;
    for(; NULL != child_value; child_value = child_value->next)
    {
        //add value seprator
        CGI_TRACE("add value seprator\n");
        if(child_index != 0 && NULL != tmp_node->value->value_string)
        {
            string_len += 1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string,
                CGI_VALUE_SEPRATOR);
        }
        
        //add child value key
        CGI_TRACE("add child value key\n");
        if(NULL != child_value->child_value_key)
        {
            encode_special_char(child_value->child_value_key, tmp_value);
            string_len += strlen(tmp_value);
        }
        else
        {
            break;
        }
        CGI_TRACE("child_value->child_value_key = %s\n", tmp_value);
        if(NULL == tmp_node->value->value_string)
        {
            CGI_TRACE("value_string malloc\n");
            CGI_VALUE_MALLOC(tmp_node->value->value_string, string_len+1, char);
            snprintf(tmp_node->value->value_string, string_len+1, "%s", tmp_value);
            CGI_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        else
        {
            CGI_TRACE("value_string realloc\n");
            string_len += strlen(tmp_value);
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string, tmp_value);
            tmp_node->value->value_string[string_len] = '\0';
            CGI_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        //add child value param
        CGI_TRACE("add child value param\n");
        CGI_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
        for(; (NULL != value_param) && (NULL != value_param->param_value); value_param = value_param->next)
        {
            encode_special_char(value_param->param_value, tmp_value);
            string_len += strlen(tmp_value)+1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            //add param seprator
            sprintf(tmp_node->value->value_string, "%s%s%s", tmp_node->value->value_string,
                CGI_PARAM_SEPRATOR, tmp_value);
        }
        ++child_index;
        CGI_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
    }

    if(NULL != tmp_node->value->value_string)
    {
        if(string_encoding == NULL)
        {
            //number+2--- '=' , '\n'
            string_len = strlen(tmp_node->key)+strlen(tmp_node->value->value_string)+4;
            CGI_VALUE_MALLOC(string_encoding, string_len+1, char);
            snprintf(string_encoding, string_len+1, "%s %c %s\n", 
                tmp_node->key, CGI_KEY_VALUE_SEPRATOR, tmp_node->value->value_string);
            CGI_TRACE("string_encoding = %s\n", string_encoding);
            if(fputs(string_encoding, fd) == -1)
            {
                CGI_TRACE("fputs error\n");
                CGI_VALUE_FREE(tmp_node->value->value_string);
                CGI_VALUE_FREE(string_encoding);
                return CGI_OPERATION_FAILED;
            }
            CGI_VALUE_FREE(tmp_node->value->value_string);
            CGI_VALUE_FREE(string_encoding);
        }
    }

    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

int cgi_key_value_set(char *key, char *value)
{
    CGI_FUNC_BEGIN;
    if((NULL == key) || (NULL == value))
    {
        return CGI_OPERATION_FAILED;
    }
    int ret = CGI_ERROR_NONE;
#if LIST_HASH_USED
    int index = (int)(*key)%LIST_NODE_ARRAY_SIZE;
    CGI_TRACE("index = %d\n", index);
    CGI_VALUE_MALLOC(kv_table.first_node, LIST_NODE_ARRAY_SIZE, CGI_NODE_LIST);
    CGI_NODE_LIST *tmp_node = &(kv_table.first_node[index]);
    while(1)
    {
        if(tmp_node)
        {
            if(NULL == tmp_node->key)
            {
                CGI_TRACE("add first node\n");
                cgi_key_set(tmp_node, key);
                cgi_value_set(tmp_node, value);
                break;
            }
            else if(0 == strcmp(tmp_node->key, key))
            {
                CGI_TRACE("set value in list which have the same key\n");
                cgi_value_set(tmp_node, value);
                break;
            }

            if(NULL == tmp_node->next)
            {
                CGI_TRACE("add node in list tail\n");
                CGI_VALUE_MALLOC(tmp_node->next, 1, CGI_NODE_LIST);
                cgi_key_set(tmp_node->next, key);
                cgi_value_set(tmp_node->next, value);
                break;
            }
            tmp_node = tmp_node->next;
        }
    }
#else
    cgi_key_value_del(key);
    if(NULL != strstr(value, ";"))
    {
        char *child_value = NULL;
        int index = 0;
        child_value = strtok(value, ";");
        if(NULL != child_value)
        {
            ret = cgi_key_value_child_value_set(key, value, 0, NULL);
            ++index;
        }
        while(NULL != (child_value = strtok(NULL, ";")))
        {
            ret = cgi_key_value_child_value_set(key, NULL, index, child_value);
            ++index;
        }
    }
    else
    {
        ret = cgi_key_value_child_value_set(key, value, 0, NULL);
    }
#endif    
    CGI_FUNC_END;
    return ret;
}

int cgi_key_value_child_value_set(char *key, char *child_key, int child_param_num, char *value_param)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == key, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(child_param_num < 0, CGI_OPERATION_FAILED);
    CGI_TRACE("key = %s, child key = %s, child param num = %d, value param = %s\n",
        key, child_key, child_param_num, value_param);
    
    int ret = CGI_ERROR_NONE;
    int len = 0;
    CGI_CHILD_VALUE_T *cv = NULL;
    CGI_CHILD_VALUE_PARAM_T *cv_param = NULL;
    CGI_CHILD_VALUE_PARAM_T *next_param = NULL;

    /* add node */
    CGI_NODE_LIST *node = (CGI_NODE_LIST*)cgi_kv_list_node_get(key);
    if(NULL == node)
    {
        node = cgi_kv_list_node_add(key);
        ret = cgi_key_set(node, key);
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
            cv = (CGI_CHILD_VALUE_T*)cgi_key_value_child_value_struct_get(node, child_key);
        }
        if(NULL == cv)
        {
            cv = cgi_key_value_child_value_add(node, child_key);
        }
        
        if(cv->child_value_key != child_key)
        {
            CGI_VALUE_FREE(cv->child_value_key);
            len = strlen(child_key);
            CGI_VALUE_MALLOC(cv->child_value_key, len+1, char);
            CGI_TRACE("len = %d, child_value_key = %s\n", len, child_key);
            snprintf(cv->child_value_key, len+1, "%s", child_key);
            CGI_TRACE("child key = %s\n", cv->child_value_key);
        }
    }

    if(child_param_num == 0)
    {
        CGI_FUNC_END;
        return ret;
    }
    else
    {
        /* add value param */
        ret = cgi_key_value_child_param_add(cv, child_param_num, value_param);
    }
    CGI_FUNC_END;

    return ret;
}

int cgi_key_value_child_set(char *key, int child_param_num, char *child_value)
{
    CGI_FUNC_BEGIN;
    return cgi_key_value_child_value_set(key, NULL, child_param_num, child_value);
}

int cgi_key_value_param_del(CGI_CHILD_VALUE_T *child_value)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == child_value, CGI_INVALID_PARAM);

    CGI_VALUE_FREE(child_value->child_value_key);
    CGI_CHILD_VALUE_PARAM_T *next_param = NULL;

    for(; NULL != child_value->param_first; child_value->param_first = next_param)
    {
        if(NULL != child_value->param_first)
        {
            next_param = child_value->param_first->next;
        }
        CGI_VALUE_FREE(child_value->param_first->param_value);
        CGI_VALUE_FREE(child_value->param_first);
    }
    child_value->param_last = NULL;
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

int cgi_key_value_child_del(CGI_NODE_LIST *node)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, CGI_INVALID_PARAM);
    CGI_VALUE_CHECK(NULL == node->value, CGI_INVALID_PARAM);
    
    CGI_VALUE_FREE(node->key);
    CGI_VALUE_FREE(node->value->value_string);

    CGI_CHILD_VALUE_T *next_child = NULL;
    for(; node->value->first_child; node->value->first_child = next_child)
    {
        if(NULL != node->value->first_child)
        {
            next_child = node->value->first_child->next;
        }
        cgi_key_value_param_del(node->value->first_child);
    }
    node->value->last_child = NULL;
    CGI_VALUE_FREE(node->value);
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

int cgi_key_value_del(char *key)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK((NULL == key || NULL == kv_table.first_node), CGI_OPERATION_FAILED);

    CGI_NODE_LIST *tmp_node = kv_table.first_node;
    CGI_NODE_LIST *next_node = NULL;
    if(NULL != kv_table.first_node->next)
    {
        next_node = kv_table.first_node->next;
    }
    if((NULL != kv_table.first_node)
        && (NULL != kv_table.first_node->key)
        && (0 == strcmp(kv_table.first_node->key, key)))
    {
        CGI_TRACE("first node delete\n");
        cgi_key_value_child_del(kv_table.first_node);
        CGI_VALUE_FREE(kv_table.first_node);
        kv_table.first_node = next_node;
        if(next_node == kv_table.last_node)
        {
            kv_table.last_node = kv_table.first_node;
        }
        
        return CGI_ERROR_NONE;
    }

    if(NULL != tmp_node && NULL != tmp_node->next)
    {
        next_node = tmp_node->next;
        while(tmp_node->next)
        {
            if(NULL != tmp_node->next->key
                && 0 == strcmp(tmp_node->next->key, key))
            {
                CGI_TRACE("node delete\n");
                cgi_key_value_child_del(tmp_node->next);
                CGI_VALUE_FREE(tmp_node->next);
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
    
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

int cgi_key_value_child_value_del(char *key, char *child_value_key, int pos, char *value_param)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == key, CGI_OPERATION_FAILED);
    CGI_VALUE_CHECK(pos < 0, CGI_OPERATION_FAILED);
    //CGI_VALUE_CHECK(NULL == value_param, CGI_OPERATION_FAILED);
    CGI_TRACE("key = %s, child_value_key = %s, pos = %d, value_param = %s\n",
        key, child_value_key, pos, value_param);
    int ret = CGI_ERROR_NONE;
    CGI_NODE_LIST *node = (CGI_NODE_LIST*)cgi_kv_list_node_get(key);
    
    if(NULL != node)
    {
        CGI_CHILD_VALUE_T *child_value = NULL;
        if(child_value_key == NULL)
        {
            child_value = node->value->first_child;
        }
        else
        {
            child_value = (CGI_CHILD_VALUE_T*)cgi_key_value_child_value_struct_get(node, child_value_key);
        }

        CGI_VALUE_CHECK(NULL == child_value, CGI_OPERATION_FAILED);
        //delete one child
        CGI_VALUE_CHECK(NULL == value_param, cgi_key_value_param_del(child_value));

        //delete child's child
        if(NULL != child_value->param_first)
        {
            CGI_CHILD_VALUE_PARAM_T *prev_param = child_value->param_first;
            CGI_CHILD_VALUE_PARAM_T *next_param = prev_param->next;
            CGI_VALUE_CHECK(NULL == child_value->param_first->param_value, CGI_OPERATION_FAILED);
            
            if(pos == 0 && 0 == strcmp(child_value->child_value_key, value_param))
            {
                CGI_VALUE_FREE(child_value->child_value_key);
                int len = strlen(child_value->param_first->param_value);
                CGI_VALUE_MALLOC(child_value->child_value_key, len+1, char);
                snprintf(child_value->child_value_key, len+1, "%s", child_value->param_first->param_value);
                CGI_VALUE_FREE(child_value->param_first->param_value);
                CGI_VALUE_FREE(child_value->param_first);
                child_value->param_first = next_param;
            }
            else if(pos < 2 && 0 == strcmp(child_value->param_first->param_value, value_param))
            {
                CGI_VALUE_FREE(child_value->param_first->param_value);
                CGI_VALUE_FREE(child_value->param_first);
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
                    CGI_TRACE("param_value = %s\n", prev_param->param_value);
                    next_param = prev_param->next;
                    CGI_TRACE("next_param = %s\n", next_param->param_value);
                    if(NULL == next_param->param_value)
                    {
                        return CGI_OPERATION_FAILED;
                    }
                    else if(0 == strcmp(prev_param->next->param_value, value_param))
                    {
                        if(prev_param->next == child_value->param_last)
                        {
                            child_value->param_last = prev_param;
                        }
                        CGI_VALUE_FREE(prev_param->next->param_value);
                        CGI_VALUE_FREE(prev_param->next);
                        prev_param->next = next_param->next;
                        ret = CGI_ERROR_NONE;
                        break;
                    }
                }
            }
        }
        else
        {
            if(0 == strcmp(child_value->child_value_key, value_param))
            {
                CGI_VALUE_FREE(child_value->child_value_key);
                ret = cgi_key_value_del(key);
            }
        }
    }
    return ret;
}

int cgi_key_value_encoding(char *path, int overwrite)
{
    CGI_FUNC_BEGIN;
    if(NULL == path)
    {
        return CGI_OPERATION_FAILED;
    }
    CGI_TRACE("\nencoding=========================================\n");
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
        return CGI_OPERATION_FAILED;
    }
    chmod(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if(kv_table.first_node != NULL)
    {
#if LIST_HASH_USED
        int index = -1;
        for(index = 0; index < LIST_NODE_ARRAY_SIZE; index++)
        {
            cgi_key_value_encoding_line(&(kv_table.first_node[index]),fd);
        }
#else
        CGI_NODE_LIST *node = NULL;
        for(node = kv_table.first_node; NULL != node; node = node->next)
        {
            cgi_key_value_encoding_line(node, fd);
        }
#endif
        cgi_key_value_cleanup();
        fclose(fd);
    }
    else
    {
        fclose(fd);
    }
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

int cgi_key_value_cleanup()
{
    CGI_FUNC_BEGIN;
    if(NULL != kv_table.first_node)
    {
#if LIST_HASH_USED
        int i = 0;
        for(; i<LIST_NODE_ARRAY_SIZE; i++)
        {
            CGI_NODE_LIST *tmp_node = &(kv_table.first_node[i]);
            while(tmp_node)
            {
                CGI_VALUE_FREE(tmp_node->key);
                CGI_VALUE_FREE(tmp_node->value->value_string);
                tmp_node = tmp_node->next;
            }
        }
        CGI_VALUE_FREE(kv_table.first_node);
#else
        CGI_NODE_LIST *tmp_node = kv_table.first_node;
        CGI_NODE_LIST *next_node = kv_table.first_node->next;
        while(NULL != tmp_node)
        {
            cgi_key_value_del(tmp_node->key);
            tmp_node = next_node;
            if(NULL != next_node)
            {
                next_node = next_node->next;
            }
        }

#endif
    }
#ifdef CGI_GET_MAIN_BUFFER
    CGI_VALUE_FREE(kv_table.main_buffer);
#endif
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

