
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "cgi_key_value.h"
#include "cgi_key_value_interface.h"

CGI_KV_TABLE kv_table;

CGI_NODE_LIST* cgi_kv_list_node_get(char *key)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == key, NULL);
    CGI_NODE_LIST *node_list = kv_table.first_node;
    for(; NULL != node_list; node_list = node_list->next)
    {
        if((NULL != node_list->key) && (0 == strcmp(node_list->key, key)))
        {
            CGI_FUNC_END;
            return node_list;
        }
    }
    CGI_FUNC_END;
    return NULL;
}

CGI_CHILD_VALUE_T* cgi_key_value_child_value_struct_get(CGI_NODE_LIST *node, char *child_value_key)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == node, NULL);
    CGI_VALUE_CHECK(NULL == child_value_key, NULL);
    CGI_VALUE_CHECK(NULL == node->value, NULL);
    CGI_CHILD_VALUE_T *child_value = node->value->first_child;
    for(; NULL != child_value; child_value = child_value->next)
    {
        if((NULL != child_value->child_value_key)
            && (0 == strcmp(child_value->child_value_key, child_value_key)))
        {
            CGI_FUNC_END;
            return child_value;
        }
    }
    CGI_FUNC_END;
    return NULL;
}

CGI_CHILD_VALUE_PARAM_T* cgi_key_value_child_param_struct_get(CGI_CHILD_VALUE_T *value_child, int vp_num)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == value_child, NULL);
    CGI_CHILD_VALUE_PARAM_T *value_param = value_child->param_first;
    int index = 1;
    for(; NULL != value_param; value_param = value_param->next)
    {
        if(index == vp_num)
        {
            CGI_FUNC_END;
            return value_param;
        }
        index++;
    }
    CGI_FUNC_END;
    return NULL;
}

static int cgi_file_read_line(char *string)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == string, CGI_OPERATION_FAILED);
    CGI_TRACE("string = %s\n", string);
    char *tmp_string = string;
    char *current_pos = string;
    char *key = NULL;
    char *child_value_key = NULL;
    char *value = NULL;
    int string_len = 0;
    int index = 0;
    int ret = CGI_ERROR_NONE;

    while(1)
    {
        if(*current_pos == '\0')
        {
            break;
        }
        switch(*current_pos)
        {
            /* get key name */
            case CGI_KEY_VALUE_SEPRATOR:
                if(NULL == key)
                {
                    string_len = current_pos - tmp_string;
                    CGI_TRACE("key string_len = %d\n", string_len);
                    CGI_VALUE_MALLOC(key, string_len+1, char);
                    strncpy(key, tmp_string, string_len);
                    CGI_TRACE("get key = %s\n", key);
                    key[string_len] = '\0';
                    string_len = cgi_ltrim(key);
                    string_len = cgi_rtrim(key);
                    decode_special_char(key, key);
                    CGI_TRACE("key string_len = %d\n", string_len);
                    CGI_TRACE("get key = %s\n", key);
#ifdef CGI_MULTILINE_VALUE_SUPPORT
                    CGI_VALUE_FREE(kv_table.cur_node_key);
                    CGI_VALUE_MALLOC(kv_table.cur_node_key, string_len+1, char);
                    strncpy(kv_table.cur_node_key, key, string_len);
                    kv_table.cur_node_key[string_len] = '\0';
#endif
                    tmp_string = current_pos;
                    ++tmp_string;
                }
                break;
            case CGI_PARAM_SEPRATOR_CH:
                if(NULL != tmp_string && tmp_string != current_pos)
                {
                    string_len = current_pos - tmp_string;
                    if(NULL == child_value_key)
                    {
                        CGI_VALUE_MALLOC(child_value_key, string_len+1, char);
                        snprintf(child_value_key, string_len+1, "%s", tmp_string);
                        cgi_ltrim(child_value_key);
                        cgi_rtrim(child_value_key);
                        decode_special_char(child_value_key, child_value_key);
                        CGI_TRACE("child_value_key = %s\n", child_value_key);
                    }
                    else
                    {
                        char *child_value = NULL;
                        CGI_VALUE_MALLOC(child_value, string_len+1, char);
                        snprintf(child_value, string_len+1, "%s", tmp_string);
                        cgi_ltrim(child_value);
                        cgi_rtrim(child_value);
                        decode_special_char(child_value, child_value);
                        CGI_TRACE("child_value = %s\n", child_value);
                        ret = cgi_key_value_child_value_set(key, child_value_key, index, child_value);
                        if(ret != CGI_ERROR_NONE)
                        {
                            break;
                        }
                        CGI_TRACE("child_value = %s\n", child_value);
                        CGI_VALUE_FREE(child_value);
                    }
                    ++index;
                    tmp_string = current_pos;
                    ++tmp_string;
                }
                break;
            case CGI_VALUE_SEPRATOR_CH:
                if(NULL != tmp_string && tmp_string != current_pos)
                {
                    char *child_value = NULL;
                    string_len = current_pos - tmp_string;
                    CGI_VALUE_MALLOC(child_value, string_len+1, char);
                    snprintf(child_value, string_len+1, "%s", tmp_string);
                    cgi_ltrim(child_value);
                    cgi_rtrim(child_value);
                    decode_special_char(child_value, child_value);
                    CGI_TRACE("key = %s, child_value_key = %s, child value = %s, index = %d\n",
                        key, child_value_key, child_value, index);
                    if(NULL == child_value_key)
                    {
                        ret = cgi_key_value_child_value_set(key, child_value, index, NULL);
                    }
                    else
                    {
                        ret = cgi_key_value_child_value_set(key, child_value_key, index, child_value);
                    }
                    CGI_VALUE_FREE(child_value);
                    CGI_VALUE_FREE(child_value_key);
                    if(ret != CGI_ERROR_NONE)
                    {
                        break;
                    }
                    index = 0;
                    tmp_string = current_pos;
                    ++tmp_string;
                }
                break;
            /* get value string */
            case '\n':
                if(NULL != key)
                {
                    if(*(tmp_string-1) == CGI_PARAM_SEPRATOR_CH
                        || *(tmp_string-1) == CGI_VALUE_SEPRATOR_CH)
                    {
                        char *child_value = NULL;
                        string_len = current_pos - tmp_string;
                        CGI_VALUE_MALLOC(child_value, string_len+1, char);
                        snprintf(child_value, string_len+1, "%s", tmp_string);
                        cgi_ltrim(child_value);
                        cgi_rtrim(child_value);
                        decode_special_char(child_value, child_value);
                        if(NULL == child_value_key)
                        {
                            ret = cgi_key_value_child_value_set(key, child_value, index, NULL);
                        }
                        else
                        {
                            ret = cgi_key_value_child_value_set(key, child_value_key, index, child_value);
                        }
                        CGI_VALUE_FREE(child_value);
                        CGI_VALUE_FREE(child_value_key);
                        CGI_VALUE_FREE(key);
                        if(ret != CGI_ERROR_NONE)
                        {
                            break;
                        }                        
                    }
                    else if(*(tmp_string-1) == CGI_KEY_VALUE_SEPRATOR)
                    {
                        /*  value not in the same line as key */
                        if(string_len == 0)
                        {
                            CGI_TRACE("value not in the same line as key\n");
                            string_len = strlen(string);
                            break;
                        }
                        else
                        {
                            string_len = current_pos - tmp_string;
                        }
                        CGI_TRACE("value string_len = %d\n", string_len);
                        CGI_VALUE_MALLOC(value, string_len+1, char);
                        snprintf(value, string_len+1, "%s", tmp_string);
                        string_len = cgi_ltrim(value);
                        string_len = cgi_rtrim(value);
                        decode_special_char(value, value);
                        CGI_TRACE("value = %s\n", value);
                        cgi_key_value_child_value_set(key, value, 0, NULL);
                        CGI_VALUE_FREE(key);
                        CGI_VALUE_FREE(value);
                    }
                    tmp_string = current_pos;
                    ++tmp_string;
                    index = 0;
                    return CGI_ERROR_NONE;
                }
#ifdef CGI_MULTILINE_VALUE_SUPPORT
                else if(key == NULL && value == NULL)
                {
                    CGI_TRACE("kv_table.cur_node_key = %s\n", kv_table.cur_node_key);
                    char *tmp_value = cgi_key_value_get(kv_table.cur_node_key);
                    CGI_TRACE("tmp_value = %s\n", tmp_value);
                    if(tmp_value)
                    {
                        string_len = strlen(tmp_value)+strlen(tmp_string);
                        CGI_TRACE("string_len = %d, %d\n", strlen(tmp_value), strlen(tmp_string));
                        CGI_VALUE_MALLOC(value, string_len+1, char);
                        snprintf(value, string_len+1, "%s\n%s", tmp_value, tmp_string);
                        decode_special_char(value, value);
                        CGI_TRACE("value = %s\n", value);
                        cgi_key_value_set(kv_table.cur_node_key, value);
                        CGI_VALUE_FREE(value);
                    }
                }
#endif
                break;
            default:
                break;
        }
        ++current_pos;
    }
    CGI_VALUE_FREE(child_value_key);
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}

char* cgi_key_value_decoding(char *path)
{
    CGI_FUNC_BEGIN;
    if(NULL == path)
    {
        return NULL;
    }
    cgi_key_value_cleanup();
    cgi_key_value_init();
    CGI_TRACE("\ndecoding begin:%s\n", path);
    int first_in = 0;
    int string_len = 0;
    int len = 0;
    int last_pos = 0;
    int cur_pos = 0;
    FILE *fd = fopen(path,"r");
    if(NULL == fd)
    {
        CGI_TRACE("CGI_FILE_NO_EXIST\n");
        return NULL;
    }

    last_pos = cur_pos = ftell(fd);
    while(1)
    {
        char buffer[MAX_READ_BUFFER] = {0};
        char *tmp_string = fgets(buffer, MAX_READ_BUFFER, fd); 
        CGI_TRACE("tmp_sting = %s\n", tmp_string);
        cur_pos = ftell(fd);
        len = cur_pos-last_pos;
        CGI_TRACE("len = %d\n", len);
        last_pos = cur_pos;
        if(NULL == tmp_string || 0 == len)
        {
            fclose(fd);
            CGI_FUNC_END;
            if(first_in == 0)
            {
                return NULL;
            }
            else
            {
                return "finish!";
            }
        }
        ++first_in;
#ifdef CGI_GET_MAIN_BUFFER
        if(NULL == kv_table.main_buffer)
        {
            len = strlen(tmp_string);
            kv_table.main_buffer = (char*)malloc(len+1);
            if(kv_table.main_buffer == NULL)
            {
                fclose(fd);
                CGI_FUNC_END;
                return NULL;
            }
            memset(kv_table.main_buffer, 0, len+1);
            strncpy(kv_table.main_buffer, tmp_string, len+1);
        }
        else
        {
            len = strlen(kv_table.main_buffer);
            len += strlen(tmp_string);
            kv_table.main_buffer = realloc(kv_table.main_buffer, len+1);
            if(kv_table.main_buffer == NULL)
            {
                fclose(fd);
                CGI_FUNC_END;
                return NULL;
            }
            strcat(kv_table.main_buffer, tmp_string);
        }
#endif        
        cgi_file_read_line(buffer);
    }
    fclose(fd);
    CGI_FUNC_END;
#ifdef CGI_GET_MAIN_BUFFER
    return kv_table.main_buffer;
#else
    return "finish!";
#endif
}

char* cgi_key_value_get(char *key)
{
    CGI_FUNC_BEGIN;
    if(NULL == key)
    {
        return NULL;
    }


    if(kv_table.first_node != NULL)
    {
#if LIST_HASH_USED
        int index = (int)(*key)%LIST_NODE_ARRAY_SIZE;
        CGI_NODE_LIST *tmp_node = &(kv_table.first_node[index]);
        for(; NULL != tmp_node; tmp_node = tmp_node->next)
        {
            if(tmp_node->key.key_name && (0 == strcmp(tmp_node->key.key_name, key)))
            {
                CGI_TRACE("tmp_node->key.key_name is %s\n", tmp_node->key.key_name);
                return tmp_node->value->value_string;
            }
        }
    
#else
        //return cgi_key_value_child_value_get(key, NULL, 0);

        CGI_NODE_LIST *node = cgi_kv_list_node_get(key);
        if(NULL != node)
        {
            CGI_VALUE_FREE(node->value->value_string);
            node->value->value_string = (char*)malloc(MAX_READ_BUFFER);
            CGI_CHILD_VALUE_T *child_value = node->value->first_child;
            for(; NULL != child_value; child_value = child_value->next)
            {
                CGI_TRACE("child_value->child_value_key = %s\n", child_value->child_value_key);
                if(child_value != node->value->first_child)
                {
                    sprintf(node->value->value_string, "%s%s%s",
                        node->value->value_string, CGI_VALUE_SEPRATOR, child_value->child_value_key);
                }
                else
                {
                    snprintf(node->value->value_string, MAX_READ_BUFFER, "%s", child_value->child_value_key);
                }
                CGI_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
                for(; NULL != value_param; value_param = value_param->next)
                {
                    CGI_TRACE("value_param->param_value = %s\n", value_param->param_value);
                    sprintf(node->value->value_string, "%s%s%s",
                        node->value->value_string, CGI_PARAM_SEPRATOR, value_param->param_value);
                }
            }
            return node->value->value_string;
        }
        
#endif
    }
    CGI_FUNC_END;
    return NULL;
}

char* cgi_key_value_child_get(char *key, int child_param_num)
{
    CGI_FUNC_BEGIN;
    CGI_FUNC_END;
    return cgi_key_value_child_value_get(key, NULL, child_param_num);
}

char* cgi_key_value_child_value_get(char *key, char* child_value_key, int child_param_num)
{
    CGI_FUNC_BEGIN;
    CGI_VALUE_CHECK(NULL == key, NULL);

    CGI_NODE_LIST *node_list = cgi_kv_list_node_get(key);
    CGI_VALUE_CHECK(NULL == node_list, NULL);

    CGI_CHILD_VALUE_T *child_value = NULL;
    if(NULL == child_value_key)
    {
        child_value = node_list->value->first_child;
    }
    else
    {
        child_value = cgi_key_value_child_value_struct_get(node_list, child_value_key);
    }
    CGI_VALUE_CHECK(NULL == child_value, NULL);

    if(child_param_num == 0)
    {
        CGI_FUNC_END;
        return child_value->child_value_key;
    }
    else
    {
        CGI_CHILD_VALUE_PARAM_T *child_param = cgi_key_value_child_param_struct_get(child_value, child_param_num);
        CGI_VALUE_CHECK(NULL == child_param, NULL);
        CGI_FUNC_END;
        return child_param->param_value;
    }
}

int cgi_key_value_tree_print()
{
    CGI_FUNC_BEGIN;
    CGI_NODE_LIST *node_list = kv_table.first_node;
    for(; node_list != NULL; node_list = node_list->next)
    {
        CGI_STDOUT("key = %s\n", node_list->key);
        CGI_CHILD_VALUE_T *child_value = node_list->value->first_child;
        for(; child_value != NULL; child_value = child_value->next)
        {
            CGI_STDOUT("child value key = %s\n", child_value->child_value_key);
            CGI_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
            CGI_STDOUT("child value param = %p", value_param);
            for(; value_param != NULL; value_param = value_param->next)
            {
                CGI_STDOUT(" %s", value_param->param_value);
            }
            CGI_STDOUT("\n");
        }
    }
    CGI_FUNC_END;
    return CGI_ERROR_NONE;
}
