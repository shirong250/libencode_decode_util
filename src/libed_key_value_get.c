#include<stdlibed.h>
#include<stdio.h>
#include<string.h>

#include "libed_key_value.h"
#include "libed_key_value_interface.h"

LIBED_KV_TABLE kv_table;

LIBED_NODE_LIST* libed_kv_list_node_get(char *key)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == key, NULL);
    LIBED_NODE_LIST *node_list = kv_table.first_node;
    for(; NULL != node_list; node_list = node_list->next)
    {
        if((NULL != node_list->key) && (0 == strcmp(node_list->key, key)))
        {
            LIBED_FUNC_END;
            return node_list;
        }
    }
    LIBED_FUNC_END;
    return NULL;
}

LIBED_CHILD_VALUE_T* libed_key_value_child_value_struct_get(LIBED_NODE_LIST *node, char *child_value_key)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == node, NULL);
    LIBED_VALUE_CHECK(NULL == child_value_key, NULL);
    LIBED_VALUE_CHECK(NULL == node->value, NULL);
    LIBED_CHILD_VALUE_T *child_value = node->value->first_child;
    for(; NULL != child_value; child_value = child_value->next)
    {
        if((NULL != child_value->child_value_key)
            && (0 == strcmp(child_value->child_value_key, child_value_key)))
        {
            LIBED_FUNC_END;
            return child_value;
        }
    }
    LIBED_FUNC_END;
    return NULL;
}

LIBED_CHILD_VALUE_PARAM_T* libed_key_value_child_param_struct_get(LIBED_CHILD_VALUE_T *value_child, int vp_num)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == value_child, NULL);
    LIBED_CHILD_VALUE_PARAM_T *value_param = value_child->param_first;
    int index = 1;
    for(; NULL != value_param; value_param = value_param->next)
    {
        if(index == vp_num)
        {
            LIBED_FUNC_END;
            return value_param;
        }
        index++;
    }
    LIBED_FUNC_END;
    return NULL;
}

static int libed_file_read_line(char *string)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == string, LIBED_OPERATION_FAILED);
    LIBED_TRACE("string = %s\n", string);
    char *tmp_string = string;
    char *current_pos = string;
    char *key = NULL;
    char *child_value_key = NULL;
    char *value = NULL;
    int string_len = 0;
    int index = 0;
    int ret = LIBED_ERROR_NONE;

    while(1)
    {
        if(*current_pos == '\0')
        {
            break;
        }
        switch(*current_pos)
        {
            /* get key name */
            case LIBED_KEY_VALUE_SEPRATOR:
                if(NULL == key)
                {
                    string_len = current_pos - tmp_string;
                    LIBED_TRACE("key string_len = %d\n", string_len);
                    LIBED_VALUE_MALLOC(key, string_len+1, char);
                    strncpy(key, tmp_string, string_len);
                    LIBED_TRACE("get key = %s\n", key);
                    key[string_len] = '\0';
                    string_len = libed_ltrim(key);
                    string_len = libed_rtrim(key);
                    decode_special_char(key, key);
                    LIBED_TRACE("key string_len = %d\n", string_len);
                    LIBED_TRACE("get key = %s\n", key);
#ifdef LIBED_MULTILINE_VALUE_SUPPORT
                    LIBED_VALUE_FREE(kv_table.cur_node_key);
                    LIBED_VALUE_MALLOC(kv_table.cur_node_key, string_len+1, char);
                    strncpy(kv_table.cur_node_key, key, string_len);
                    kv_table.cur_node_key[string_len] = '\0';
#endif
                    tmp_string = current_pos;
                    ++tmp_string;
                }
                break;
            case LIBED_PARAM_SEPRATOR_CH:
                if(NULL != tmp_string && tmp_string != current_pos)
                {
                    string_len = current_pos - tmp_string;
                    if(NULL == child_value_key)
                    {
                        LIBED_VALUE_MALLOC(child_value_key, string_len+1, char);
                        snprintf(child_value_key, string_len+1, "%s", tmp_string);
                        libed_ltrim(child_value_key);
                        libed_rtrim(child_value_key);
                        decode_special_char(child_value_key, child_value_key);
                        LIBED_TRACE("child_value_key = %s\n", child_value_key);
                    }
                    else
                    {
                        char *child_value = NULL;
                        LIBED_VALUE_MALLOC(child_value, string_len+1, char);
                        snprintf(child_value, string_len+1, "%s", tmp_string);
                        libed_ltrim(child_value);
                        libed_rtrim(child_value);
                        decode_special_char(child_value, child_value);
                        LIBED_TRACE("child_value = %s\n", child_value);
                        ret = libed_key_value_child_value_set(key, child_value_key, index, child_value);
                        if(ret != LIBED_ERROR_NONE)
                        {
                            break;
                        }
                        LIBED_TRACE("child_value = %s\n", child_value);
                        LIBED_VALUE_FREE(child_value);
                    }
                    ++index;
                    tmp_string = current_pos;
                    ++tmp_string;
                }
                break;
            case LIBED_VALUE_SEPRATOR_CH:
                if(NULL != tmp_string && tmp_string != current_pos)
                {
                    char *child_value = NULL;
                    string_len = current_pos - tmp_string;
                    LIBED_VALUE_MALLOC(child_value, string_len+1, char);
                    snprintf(child_value, string_len+1, "%s", tmp_string);
                    libed_ltrim(child_value);
                    libed_rtrim(child_value);
                    decode_special_char(child_value, child_value);
                    LIBED_TRACE("key = %s, child_value_key = %s, child value = %s, index = %d\n",
                        key, child_value_key, child_value, index);
                    if(NULL == child_value_key)
                    {
                        ret = libed_key_value_child_value_set(key, child_value, index, NULL);
                    }
                    else
                    {
                        ret = libed_key_value_child_value_set(key, child_value_key, index, child_value);
                    }
                    LIBED_VALUE_FREE(child_value);
                    LIBED_VALUE_FREE(child_value_key);
                    if(ret != LIBED_ERROR_NONE)
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
                    if(*(tmp_string-1) == LIBED_PARAM_SEPRATOR_CH
                        || *(tmp_string-1) == LIBED_VALUE_SEPRATOR_CH)
                    {
                        char *child_value = NULL;
                        string_len = current_pos - tmp_string;
                        LIBED_VALUE_MALLOC(child_value, string_len+1, char);
                        snprintf(child_value, string_len+1, "%s", tmp_string);
                        libed_ltrim(child_value);
                        libed_rtrim(child_value);
                        decode_special_char(child_value, child_value);
                        if(NULL == child_value_key)
                        {
                            ret = libed_key_value_child_value_set(key, child_value, index, NULL);
                        }
                        else
                        {
                            ret = libed_key_value_child_value_set(key, child_value_key, index, child_value);
                        }
                        LIBED_VALUE_FREE(child_value);
                        LIBED_VALUE_FREE(child_value_key);
                        LIBED_VALUE_FREE(key);
                        if(ret != LIBED_ERROR_NONE)
                        {
                            break;
                        }                        
                    }
                    else if(*(tmp_string-1) == LIBED_KEY_VALUE_SEPRATOR)
                    {
                        /*  value not in the same line as key */
                        if(string_len == 0)
                        {
                            LIBED_TRACE("value not in the same line as key\n");
                            string_len = strlen(string);
                            break;
                        }
                        else
                        {
                            string_len = current_pos - tmp_string;
                        }
                        LIBED_TRACE("value string_len = %d\n", string_len);
                        LIBED_VALUE_MALLOC(value, string_len+1, char);
                        snprintf(value, string_len+1, "%s", tmp_string);
                        string_len = libed_ltrim(value);
                        string_len = libed_rtrim(value);
                        decode_special_char(value, value);
                        LIBED_TRACE("value = %s\n", value);
                        libed_key_value_child_value_set(key, value, 0, NULL);
                        LIBED_VALUE_FREE(key);
                        LIBED_VALUE_FREE(value);
                    }
                    tmp_string = current_pos;
                    ++tmp_string;
                    index = 0;
                    return LIBED_ERROR_NONE;
                }
#ifdef LIBED_MULTILINE_VALUE_SUPPORT
                else if(key == NULL && value == NULL)
                {
                    LIBED_TRACE("kv_table.cur_node_key = %s\n", kv_table.cur_node_key);
                    char *tmp_value = libed_key_value_get(kv_table.cur_node_key);
                    LIBED_TRACE("tmp_value = %s\n", tmp_value);
                    if(tmp_value)
                    {
                        string_len = strlen(tmp_value)+strlen(tmp_string);
                        LIBED_TRACE("string_len = %d, %d\n", strlen(tmp_value), strlen(tmp_string));
                        LIBED_VALUE_MALLOC(value, string_len+1, char);
                        snprintf(value, string_len+1, "%s\n%s", tmp_value, tmp_string);
                        decode_special_char(value, value);
                        LIBED_TRACE("value = %s\n", value);
                        libed_key_value_set(kv_table.cur_node_key, value);
                        LIBED_VALUE_FREE(value);
                    }
                }
#endif
                break;
            default:
                break;
        }
        ++current_pos;
    }
    LIBED_VALUE_FREE(child_value_key);
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

char* libed_key_value_decoding(char *path)
{
    LIBED_FUNC_BEGIN;
    if(NULL == path)
    {
        return NULL;
    }
    libed_key_value_cleanup();
    libed_key_value_init();
    LIBED_TRACE("\ndecoding begin:%s\n", path);
    int first_in = 0;
    int string_len = 0;
    int len = 0;
    int last_pos = 0;
    int cur_pos = 0;
    FILE *fd = fopen(path,"r");
    if(NULL == fd)
    {
        LIBED_TRACE("LIBED_FILE_NO_EXIST\n");
        return NULL;
    }

    last_pos = cur_pos = ftell(fd);
    while(1)
    {
        char buffer[MAX_READ_BUFFER] = {0};
        char *tmp_string = fgets(buffer, MAX_READ_BUFFER, fd); 
        LIBED_TRACE("tmp_sting = %s\n", tmp_string);
        cur_pos = ftell(fd);
        len = cur_pos-last_pos;
        LIBED_TRACE("len = %d\n", len);
        last_pos = cur_pos;
        if(NULL == tmp_string || 0 == len)
        {
            fclose(fd);
            LIBED_FUNC_END;
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
#ifdef LIBED_GET_MAIN_BUFFER
        if(NULL == kv_table.main_buffer)
        {
            len = strlen(tmp_string);
            kv_table.main_buffer = (char*)malloc(len+1);
            if(kv_table.main_buffer == NULL)
            {
                fclose(fd);
                LIBED_FUNC_END;
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
                LIBED_FUNC_END;
                return NULL;
            }
            strcat(kv_table.main_buffer, tmp_string);
        }
#endif        
        libed_file_read_line(buffer);
    }
    fclose(fd);
    LIBED_FUNC_END;
#ifdef LIBED_GET_MAIN_BUFFER
    return kv_table.main_buffer;
#else
    return "finish!";
#endif
}

char* libed_key_value_get(char *key)
{
    LIBED_FUNC_BEGIN;
    if(NULL == key)
    {
        return NULL;
    }


    if(kv_table.first_node != NULL)
    {
#if LIST_HASH_USED
        int index = (int)(*key)%LIST_NODE_ARRAY_SIZE;
        LIBED_NODE_LIST *tmp_node = &(kv_table.first_node[index]);
        for(; NULL != tmp_node; tmp_node = tmp_node->next)
        {
            if(tmp_node->key.key_name && (0 == strcmp(tmp_node->key.key_name, key)))
            {
                LIBED_TRACE("tmp_node->key.key_name is %s\n", tmp_node->key.key_name);
                return tmp_node->value->value_string;
            }
        }
    
#else
        //return libed_key_value_child_value_get(key, NULL, 0);

        LIBED_NODE_LIST *node = libed_kv_list_node_get(key);
        if(NULL != node)
        {
            LIBED_VALUE_FREE(node->value->value_string);
            node->value->value_string = (char*)malloc(MAX_READ_BUFFER);
            LIBED_CHILD_VALUE_T *child_value = node->value->first_child;
            for(; NULL != child_value; child_value = child_value->next)
            {
                LIBED_TRACE("child_value->child_value_key = %s\n", child_value->child_value_key);
                if(child_value != node->value->first_child)
                {
                    sprintf(node->value->value_string, "%s%s%s",
                        node->value->value_string, LIBED_VALUE_SEPRATOR, child_value->child_value_key);
                }
                else
                {
                    snprintf(node->value->value_string, MAX_READ_BUFFER, "%s", child_value->child_value_key);
                }
                LIBED_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
                for(; NULL != value_param; value_param = value_param->next)
                {
                    LIBED_TRACE("value_param->param_value = %s\n", value_param->param_value);
                    sprintf(node->value->value_string, "%s%s%s",
                        node->value->value_string, LIBED_PARAM_SEPRATOR, value_param->param_value);
                }
            }
            return node->value->value_string;
        }
        
#endif
    }
    LIBED_FUNC_END;
    return NULL;
}

char* libed_key_value_child_get(char *key, int child_param_num)
{
    LIBED_FUNC_BEGIN;
    LIBED_FUNC_END;
    return libed_key_value_child_value_get(key, NULL, child_param_num);
}

char* libed_key_value_child_value_get(char *key, char* child_value_key, int child_param_num)
{
    LIBED_FUNC_BEGIN;
    LIBED_VALUE_CHECK(NULL == key, NULL);

    LIBED_NODE_LIST *node_list = libed_kv_list_node_get(key);
    LIBED_VALUE_CHECK(NULL == node_list, NULL);

    LIBED_CHILD_VALUE_T *child_value = NULL;
    if(NULL == child_value_key)
    {
        child_value = node_list->value->first_child;
    }
    else
    {
        child_value = libed_key_value_child_value_struct_get(node_list, child_value_key);
    }
    LIBED_VALUE_CHECK(NULL == child_value, NULL);

    if(child_param_num == 0)
    {
        LIBED_FUNC_END;
        return child_value->child_value_key;
    }
    else
    {
        LIBED_CHILD_VALUE_PARAM_T *child_param = libed_key_value_child_param_struct_get(child_value, child_param_num);
        LIBED_VALUE_CHECK(NULL == child_param, NULL);
        LIBED_FUNC_END;
        return child_param->param_value;
    }
}

int libed_key_value_tree_print()
{
    LIBED_FUNC_BEGIN;
    LIBED_NODE_LIST *node_list = kv_table.first_node;
    for(; node_list != NULL; node_list = node_list->next)
    {
        LIBED_STDOUT("key = %s\n", node_list->key);
        LIBED_CHILD_VALUE_T *child_value = node_list->value->first_child;
        for(; child_value != NULL; child_value = child_value->next)
        {
            LIBED_STDOUT("child value key = %s\n", child_value->child_value_key);
            LIBED_CHILD_VALUE_PARAM_T *value_param = child_value->param_first;
            LIBED_STDOUT("child value param = %p", value_param);
            for(; value_param != NULL; value_param = value_param->next)
            {
                LIBED_STDOUT(" %s", value_param->param_value);
            }
            LIBED_STDOUT("\n");
        }
    }
    LIBED_FUNC_END;
    return LIBED_ERROR_NONE;
}

