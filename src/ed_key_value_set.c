#include<stded.h>
#include<stdio.h>
#include<string.h>
#include <sys/stat.h>

#include "ed_key_value.h"
#include "ed_key_value_interface.h"

extern ED_KV_TABLE kv_table;

void ed_key_value_init()
{
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
#ifdef ED_MULTILINE_VALUE_SUPPORT
    kv_table.cur_node_key = NULL;
    kv_table.cur_node_value = NULL;
#endif
#ifdef ED_GET_MAIN_BUFFER
    kv_table.main_buffer = NULL;
#endif
}

static int ed_value_set(ED_NODE_LIST *node, char *string)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == node, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == string, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == node->value, ED_OPERATION_FAILED);
    
    ED_VALUE_FREE(node->value->value_string);
    int len = strlen(string);
    ED_VALUE_MALLOC(node->value->value_string, len+1, char);
    snprintf(node->value->value_string, len+1, "%s", string);
    ED_TRACE("value = %s\n", node->value->value_string);
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

static int ed_key_set(ED_NODE_LIST *node, char *string)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == node, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == string, ED_OPERATION_FAILED);

    ED_VALUE_FREE(node->key);
    int len = strlen(string);
    ED_VALUE_MALLOC(node->key, len+1, char);
    snprintf(node->key, len+1, "%s", string);
    ED_TRACE("key = %s\n", node->key);
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

ED_NODE_LIST* ed_kv_list_node_add(char *key)
{
    ED_FUNC_BEGIN;
    
    ED_NODE_LIST *node_list = NULL;
    if(NULL == kv_table.first_node)
    {
        ED_TRACE("first node add\n");
        kv_table.first_node = (ED_NODE_LIST*)malloc(sizeof(ED_NODE_LIST));
        memset(kv_table.first_node, 0, sizeof(ED_NODE_LIST));
        kv_table.last_node = kv_table.first_node;
        node_list = kv_table.first_node;
    }
    else if(kv_table.last_node && (NULL == kv_table.last_node->next))
    {
        ED_NODE_LIST *tmp_node = (ED_NODE_LIST*)ed_kv_list_node_get(key);
        if(NULL != tmp_node)
        {
            //reset value
            ED_TRACE("reset node value in list\n");
            ED_FUNC_END;
            return tmp_node;
        }
        ED_TRACE("add node in list\n");
        kv_table.last_node->next = (ED_NODE_LIST*)malloc(sizeof(ED_NODE_LIST));
        memset(kv_table.last_node->next, 0, sizeof(ED_NODE_LIST));
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
            node_list->value = (ED_VALUE_STR*)malloc(sizeof(ED_VALUE_STR));
            memset(node_list->value, 0, sizeof(ED_VALUE_STR));
        }
    }
    ED_FUNC_END;
    return node_list;
}

ED_CHILD_VALUE_T* ed_key_value_child_value_add(ED_NODE_LIST *node, char *child_value_key)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == node, NULL);
    ED_VALUE_CHECK(NULL == child_value_key, NULL);
    ED_CHILD_VALUE_T *cv = NULL;

    if(NULL == node->value->first_child)
    {
        ED_TRACE("first child value add!\n");
        node->value->first_child = (ED_CHILD_VALUE_T*)malloc(sizeof(ED_CHILD_VALUE_T));
        memset(node->value->first_child, 0, sizeof(ED_CHILD_VALUE_T));
        node->value->last_child = node->value->first_child;
        cv = node->value->first_child;
    }
    else if(NULL != node->value->last_child && NULL == node->value->last_child->next)
    {
        cv = (ED_CHILD_VALUE_T*)ed_key_value_child_value_struct_get(node, child_value_key);
        if(NULL != cv)
        {
            ED_TRACE("reset child value in list\n");
            ED_FUNC_END;
            return cv;
        }
        ED_TRACE("add child value in list!\n");
        node->value->last_child->next = (ED_CHILD_VALUE_T*)malloc(sizeof(ED_CHILD_VALUE_T));
        memset(node->value->last_child->next, 0, sizeof(ED_CHILD_VALUE_T));
        node->value->last_child = node->value->last_child->next;
        cv = node->value->last_child;
    }
    else
    {
        cv = NULL;
    }

    ED_FUNC_END;
    return cv;
}

int ed_key_value_child_param_add(ED_CHILD_VALUE_T *child_value, int child_param_num, char *value_param)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == child_value, ED_INVALID_PARAM);
    ED_VALUE_CHECK(NULL == value_param, ED_INVALID_PARAM);

    ED_CHILD_VALUE_PARAM_T *vp = NULL;
    if(NULL == child_value->param_first)
    {
        ED_TRACE("first value param add!\n");
        child_value->param_first = (ED_CHILD_VALUE_PARAM_T*)malloc(sizeof(ED_CHILD_VALUE_PARAM_T));
        memset(child_value->param_first, 0, sizeof(ED_CHILD_VALUE_PARAM_T));
        child_value->param_first->param_value = NULL;
        child_value->param_last = child_value->param_first;
        vp = child_value->param_first;
    }
    else if(NULL != child_value->param_last && NULL == child_value->param_last->next)
    {
        vp = (ED_CHILD_VALUE_PARAM_T*)ed_key_value_child_param_struct_get(child_value, child_param_num);
        if(NULL != vp)
        {
            ED_TRACE("reset value param in list\n");
            goto OUT;
        }
        ED_TRACE("add value param in list!\n");
        child_value->param_last->next = (ED_CHILD_VALUE_PARAM_T*)malloc(sizeof(ED_CHILD_VALUE_PARAM_T));
        memset(child_value->param_last->next, 0, sizeof(ED_CHILD_VALUE_PARAM_T));
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
        ED_VALUE_FREE(vp->param_value);
        int len = strlen(value_param);
        ED_VALUE_MALLOC(vp->param_value, len+1, char);
        snprintf(vp->param_value, len+1, "%s", value_param);
        ED_TRACE("vp->param_value = %s, %p\n", vp->param_value, vp->param_value);
    }
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

static int ed_key_value_encoding_line(ED_NODE_LIST *node, FILE *fd)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == node, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == fd, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == node->key, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(NULL == node->value, ED_OPERATION_FAILED);

    ED_NODE_LIST *tmp_node = node;
    char *string_encoding = NULL;
    int string_len = 0;
    char tmp_value[MAX_READ_BUFFER] = {0};
    
    //get value
    ED_VALUE_FREE(tmp_node->value->value_string);
    int child_index = 0;
    ED_CHILD_VALUE_T *child_value = tmp_node->value->first_child;
    for(; NULL != child_value; child_value = child_value->next)
    {
        //add value seprator
        ED_TRACE("add value seprator\n");
        if(child_index != 0 && NULL != tmp_node->value->value_string)
        {
            string_len += 1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string,
                ED_VALUE_SEPRATOR);
        }
        
        //add child value key
        ED_TRACE("add child value key\n");
        if(NULL != child_value->child_value_key)
        {
            encode_special_char(child_value->child_value_key, tmp_value);
            string_len += strlen(tmp_value);
        }
        else
        {
            break;
        }
        ED_TRACE("child_value->child_value_key = %s\n", tmp_value);
        if(NULL == tmp_node->value->value_string)
        {
            ED_TRACE("value_string malloc\n");
            ED_VALUE_MALLOC(tmp_node->value->value_string, string_len+1, char);
            snprintf(tmp_node->value->value_string, string_len+1, "%s", tmp_value);
            ED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        else
        {
            ED_TRACE("value_string realloc\n");
            string_len += strlen(tmp_value);
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            sprintf(tmp_node->value->value_string, "%s%s", tmp_node->value->value_string, tmp_value);
            tmp_node->value->value_string[string_len] = '\0';
            ED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
        }
        //add child value param
        ED_TRACE("add child value param\n");
        ED_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
        for(; (NULL != value_param) && (NULL != value_param->param_value); value_param = value_param->next)
        {
            encode_special_char(value_param->param_value, tmp_value);
            string_len += strlen(tmp_value)+1;
            tmp_node->value->value_string = realloc(tmp_node->value->value_string, string_len+1);
            //add param seprator
            sprintf(tmp_node->value->value_string, "%s%s%s", tmp_node->value->value_string,
                ED_PARAM_SEPRATOR, tmp_value);
        }
        ++child_index;
        ED_TRACE("tmp_node->value->value_string = %s\n", tmp_node->value->value_string);
    }

    if(NULL != tmp_node->value->value_string)
    {
        if(string_encoding == NULL)
        {
            //number+2--- '=' , '\n'
            string_len = strlen(tmp_node->key)+strlen(tmp_node->value->value_string)+4;
            ED_VALUE_MALLOC(string_encoding, string_len+1, char);
            snprintf(string_encoding, string_len+1, "%s %c %s\n", 
                tmp_node->key, ED_KEY_VALUE_SEPRATOR, tmp_node->value->value_string);
            ED_TRACE("string_encoding = %s\n", string_encoding);
            if(fputs(string_encoding, fd) == -1)
            {
                ED_TRACE("fputs error\n");
                ED_VALUE_FREE(tmp_node->value->value_string);
                ED_VALUE_FREE(string_encoding);
                return ED_OPERATION_FAILED;
            }
            ED_VALUE_FREE(tmp_node->value->value_string);
            ED_VALUE_FREE(string_encoding);
        }
    }

    ED_FUNC_END;
    return ED_ERROR_NONE;
}

int ed_key_value_set(char *key, char *value)
{
    ED_FUNC_BEGIN;
    if((NULL == key) || (NULL == value))
    {
        return ED_OPERATION_FAILED;
    }
    int ret = ED_ERROR_NONE;
#if LIST_HASH_USED
    int index = (int)(*key)%LIST_NODE_ARRAY_SIZE;
    ED_TRACE("index = %d\n", index);
    ED_VALUE_MALLOC(kv_table.first_node, LIST_NODE_ARRAY_SIZE, ED_NODE_LIST);
    ED_NODE_LIST *tmp_node = &(kv_table.first_node[index]);
    while(1)
    {
        if(tmp_node)
        {
            if(NULL == tmp_node->key)
            {
                ED_TRACE("add first node\n");
                ed_key_set(tmp_node, key);
                ed_value_set(tmp_node, value);
                break;
            }
            else if(0 == strcmp(tmp_node->key, key))
            {
                ED_TRACE("set value in list which have the same key\n");
                ed_value_set(tmp_node, value);
                break;
            }

            if(NULL == tmp_node->next)
            {
                ED_TRACE("add node in list tail\n");
                ED_VALUE_MALLOC(tmp_node->next, 1, ED_NODE_LIST);
                ed_key_set(tmp_node->next, key);
                ed_value_set(tmp_node->next, value);
                break;
            }
            tmp_node = tmp_node->next;
        }
    }
#else
    ed_key_value_del(key);
    if(NULL != strstr(value, ";"))
    {
        char *child_value = NULL;
        int index = 0;
        child_value = strtok(value, ";");
        if(NULL != child_value)
        {
            ret = ed_key_value_child_value_set(key, value, 0, NULL);
            ++index;
        }
        while(NULL != (child_value = strtok(NULL, ";")))
        {
            ret = ed_key_value_child_value_set(key, NULL, index, child_value);
            ++index;
        }
    }
    else
    {
        ret = ed_key_value_child_value_set(key, value, 0, NULL);
    }
#endif    
    ED_FUNC_END;
    return ret;
}

int ed_key_value_child_value_set(char *key, char *child_key, int child_param_num, char *value_param)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == key, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(child_param_num < 0, ED_OPERATION_FAILED);
    ED_TRACE("key = %s, child key = %s, child param num = %d, value param = %s\n",
        key, child_key, child_param_num, value_param);
    
    int ret = ED_ERROR_NONE;
    int len = 0;
    ED_CHILD_VALUE_T *cv = NULL;
    ED_CHILD_VALUE_PARAM_T *cv_param = NULL;
    ED_CHILD_VALUE_PARAM_T *next_param = NULL;

    /* add node */
    ED_NODE_LIST *node = (ED_NODE_LIST*)ed_kv_list_node_get(key);
    if(NULL == node)
    {
        node = ed_kv_list_node_add(key);
        ret = ed_key_set(node, key);
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
            cv = (ED_CHILD_VALUE_T*)ed_key_value_child_value_struct_get(node, child_key);
        }
        if(NULL == cv)
        {
            cv = ed_key_value_child_value_add(node, child_key);
        }
        
        if(cv->child_value_key != child_key)
        {
            ED_VALUE_FREE(cv->child_value_key);
            len = strlen(child_key);
            ED_VALUE_MALLOC(cv->child_value_key, len+1, char);
            ED_TRACE("len = %d, child_value_key = %s\n", len, child_key);
            snprintf(cv->child_value_key, len+1, "%s", child_key);
            ED_TRACE("child key = %s\n", cv->child_value_key);
        }
    }

    if(child_param_num == 0)
    {
        ED_FUNC_END;
        return ret;
    }
    else
    {
        /* add value param */
        ret = ed_key_value_child_param_add(cv, child_param_num, value_param);
    }
    ED_FUNC_END;

    return ret;
}

int ed_key_value_child_set(char *key, int child_param_num, char *child_value)
{
    ED_FUNC_BEGIN;
    return ed_key_value_child_value_set(key, NULL, child_param_num, child_value);
}

int ed_key_value_param_del(ED_CHILD_VALUE_T *child_value)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == child_value, ED_INVALID_PARAM);

    ED_VALUE_FREE(child_value->child_value_key);
    ED_CHILD_VALUE_PARAM_T *next_param = NULL;

    for(; NULL != child_value->param_first; child_value->param_first = next_param)
    {
        if(NULL != child_value->param_first)
        {
            next_param = child_value->param_first->next;
        }
        ED_VALUE_FREE(child_value->param_first->param_value);
        ED_VALUE_FREE(child_value->param_first);
    }
    child_value->param_last = NULL;
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

int ed_key_value_child_del(ED_NODE_LIST *node)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == node, ED_INVALID_PARAM);
    ED_VALUE_CHECK(NULL == node->value, ED_INVALID_PARAM);
    
    ED_VALUE_FREE(node->key);
    ED_VALUE_FREE(node->value->value_string);

    ED_CHILD_VALUE_T *next_child = NULL;
    for(; node->value->first_child; node->value->first_child = next_child)
    {
        if(NULL != node->value->first_child)
        {
            next_child = node->value->first_child->next;
        }
        ed_key_value_param_del(node->value->first_child);
    }
    node->value->last_child = NULL;
    ED_VALUE_FREE(node->value);
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

int ed_key_value_del(char *key)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK((NULL == key || NULL == kv_table.first_node), ED_OPERATION_FAILED);

    ED_NODE_LIST *tmp_node = kv_table.first_node;
    ED_NODE_LIST *next_node = NULL;
    if(NULL != kv_table.first_node->next)
    {
        next_node = kv_table.first_node->next;
    }
    if((NULL != kv_table.first_node)
        && (NULL != kv_table.first_node->key)
        && (0 == strcmp(kv_table.first_node->key, key)))
    {
        ED_TRACE("first node delete\n");
        ed_key_value_child_del(kv_table.first_node);
        ED_VALUE_FREE(kv_table.first_node);
        kv_table.first_node = next_node;
        if(next_node == kv_table.last_node)
        {
            kv_table.last_node = kv_table.first_node;
        }
        
        return ED_ERROR_NONE;
    }

    if(NULL != tmp_node && NULL != tmp_node->next)
    {
        next_node = tmp_node->next;
        while(tmp_node->next)
        {
            if(NULL != tmp_node->next->key
                && 0 == strcmp(tmp_node->next->key, key))
            {
                ED_TRACE("node delete\n");
                ed_key_value_child_del(tmp_node->next);
                ED_VALUE_FREE(tmp_node->next);
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
    
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

int ed_key_value_child_value_del(char *key, char *child_value_key, int pos, char *value_param)
{
    ED_FUNC_BEGIN;
    ED_VALUE_CHECK(NULL == key, ED_OPERATION_FAILED);
    ED_VALUE_CHECK(pos < 0, ED_OPERATION_FAILED);
    //ED_VALUE_CHECK(NULL == value_param, ED_OPERATION_FAILED);
    ED_TRACE("key = %s, child_value_key = %s, pos = %d, value_param = %s\n",
        key, child_value_key, pos, value_param);
    int ret = ED_ERROR_NONE;
    ED_NODE_LIST *node = (ED_NODE_LIST*)ed_kv_list_node_get(key);
    
    if(NULL != node)
    {
        ED_CHILD_VALUE_T *child_value = NULL;
        if(child_value_key == NULL)
        {
            child_value = node->value->first_child;
        }
        else
        {
            child_value = (ED_CHILD_VALUE_T*)ed_key_value_child_value_struct_get(node, child_value_key);
        }

        ED_VALUE_CHECK(NULL == child_value, ED_OPERATION_FAILED);
        //delete one child
        ED_VALUE_CHECK(NULL == value_param, ed_key_value_param_del(child_value));

        //delete child's child
        if(NULL != child_value->param_first)
        {
            ED_CHILD_VALUE_PARAM_T *prev_param = child_value->param_first;
            ED_CHILD_VALUE_PARAM_T *next_param = prev_param->next;
            ED_VALUE_CHECK(NULL == child_value->param_first->param_value, ED_OPERATION_FAILED);
            
            if(pos == 0 && 0 == strcmp(child_value->child_value_key, value_param))
            {
                ED_VALUE_FREE(child_value->child_value_key);
                int len = strlen(child_value->param_first->param_value);
                ED_VALUE_MALLOC(child_value->child_value_key, len+1, char);
                snprintf(child_value->child_value_key, len+1, "%s", child_value->param_first->param_value);
                ED_VALUE_FREE(child_value->param_first->param_value);
                ED_VALUE_FREE(child_value->param_first);
                child_value->param_first = next_param;
            }
            else if(pos < 2 && 0 == strcmp(child_value->param_first->param_value, value_param))
            {
                ED_VALUE_FREE(child_value->param_first->param_value);
                ED_VALUE_FREE(child_value->param_first);
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
                    ED_TRACE("param_value = %s\n", prev_param->param_value);
                    next_param = prev_param->next;
                    ED_TRACE("next_param = %s\n", next_param->param_value);
                    if(NULL == next_param->param_value)
                    {
                        return ED_OPERATION_FAILED;
                    }
                    else if(0 == strcmp(prev_param->next->param_value, value_param))
                    {
                        if(prev_param->next == child_value->param_last)
                        {
                            child_value->param_last = prev_param;
                        }
                        ED_VALUE_FREE(prev_param->next->param_value);
                        ED_VALUE_FREE(prev_param->next);
                        prev_param->next = next_param->next;
                        ret = ED_ERROR_NONE;
                        break;
                    }
                }
            }
        }
        else
        {
            if(0 == strcmp(child_value->child_value_key, value_param))
            {
                ED_VALUE_FREE(child_value->child_value_key);
                ret = ed_key_value_del(key);
            }
        }
    }
    return ret;
}

int ed_key_value_encoding(char *path, int overwrite)
{
    ED_FUNC_BEGIN;
    if(NULL == path)
    {
        return ED_OPERATION_FAILED;
    }
    ED_TRACE("\nencoding=========================================\n");
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
        return ED_OPERATION_FAILED;
    }
    chmod(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if(kv_table.first_node != NULL)
    {
#if LIST_HASH_USED
        int index = -1;
        for(index = 0; index < LIST_NODE_ARRAY_SIZE; index++)
        {
            ed_key_value_encoding_line(&(kv_table.first_node[index]),fd);
        }
#else
        ED_NODE_LIST *node = NULL;
        for(node = kv_table.first_node; NULL != node; node = node->next)
        {
            ed_key_value_encoding_line(node, fd);
        }
#endif
        ed_key_value_cleanup();
        fclose(fd);
    }
    else
    {
        fclose(fd);
    }
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

int ed_key_value_cleanup()
{
    ED_FUNC_BEGIN;
    if(NULL != kv_table.first_node)
    {
#if LIST_HASH_USED
        int i = 0;
        for(; i<LIST_NODE_ARRAY_SIZE; i++)
        {
            ED_NODE_LIST *tmp_node = &(kv_table.first_node[i]);
            while(tmp_node)
            {
                ED_VALUE_FREE(tmp_node->key);
                ED_VALUE_FREE(tmp_node->value->value_string);
                tmp_node = tmp_node->next;
            }
        }
        ED_VALUE_FREE(kv_table.first_node);
#else
        ED_NODE_LIST *tmp_node = kv_table.first_node;
        ED_NODE_LIST *next_node = kv_table.first_node->next;
        while(NULL != tmp_node)
        {
            ed_key_value_del(tmp_node->key);
            tmp_node = next_node;
            if(NULL != next_node)
            {
                next_node = next_node->next;
            }
        }

#endif
    }
#ifdef ED_GET_MAIN_BUFFER
    ED_VALUE_FREE(kv_table.main_buffer);
#endif
    kv_table.first_node = NULL;
    kv_table.last_node = NULL;
    ED_FUNC_END;
    return ED_ERROR_NONE;
}

