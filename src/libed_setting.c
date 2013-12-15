#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libed_key_value.h"
#include "cli_setting.h"
#include "libed_key_value_interface.h"

#define Y_OR_N "[y|n]"

#define CLI_TEST_COUNT_LEN 16
#define CLI_TEST_CONF_PATH "/scf/test/test_cmds.conf"

#ifdef CLI_TEST_SINGLE_CMD
#define CLI_TEST_CONF_OUTPUT "/scf/test/OUTPUT"
#endif

char *cli_error_reference[] = {
    "",
    "Invalid parameter.\n",
    "Operation failed.\n",
    "Permission denied.\n",
    "Operation not supported on this system.\n",
    "Operation interrupted.\n",
    "The current configuration does not support this operation.\n",
    "A hardware error occurred. Please check the error log for details.\n",
    "An internal error has occurred. Please contact your system administrator.\n",
    "Cannot communicate with XXX. Please check XXX's state.\n",
    ""
};

char *cli_setting_reference[] = {
    "Continue?"
};

static char *cli_setting_test_string[] = {
    "count",
    "return",
    "output"
};

enum CLI_REFERENE_TYPE{CONTINUE};
enum CLI_TEST_PARAM_VALUE{TEST_PARAM_COUNT, TEST_PARAM_RETURN, TEST_PARAM_OUTPUT};

static char* cli_setting_read_output(char *path)
{
    LIBED_VALUE_CHECK(path == NULL, NULL);
    char *buffer = NULL;
    char tmp_buffer[MAX_READ_BUFFER] = {0};
    char *tmp_string = NULL;
    int len = 0;

    FILE *fp = fopen(path, "r");
    if(fp)
    {
        len = 0;
        buffer = (char*)malloc(len+1);
        buffer[0] = '\0';
        while(NULL != (tmp_string = fgets(tmp_buffer, MAX_READ_BUFFER, fp)))
        {
            len += strlen(tmp_string);
            buffer = realloc(buffer, len+1);
            sprintf(buffer, "%s%s", buffer, tmp_string);
        }
        len = strlen(buffer);
    }
    return buffer;
}

int cli_setting_pre_check(char *name)
{
    int ret = LIBED_ERROR_NONE;
    char *output = NULL;
    char *temp_string = NULL;
    char *temp_name = NULL;
    char *key = NULL;
    int len = 0;
    unsigned int count = 0;
    char value[CLI_TEST_COUNT_LEN];
    memset(value, 0, CLI_TEST_COUNT_LEN);

    if(NULL == libed_key_value_decoding(CLI_TEST_CONF_PATH))
    {
        return LIBED_ERROR_NONE;
    }

    //strrstr no exist
    len = strlen(name);
    while(name[len] != '/' && len >= 0)
    {
        --len;
    }
    temp_name = &name[len+1];
    
    len = strlen(temp_name)+strlen(cli_setting_test_string[TEST_PARAM_COUNT])+1;
    LIBED_VALUE_MALLOC(key, len+1, char);
    snprintf(key, len+1, "%s.%s", temp_name, cli_setting_test_string[TEST_PARAM_COUNT]);
    if(temp_string = libed_key_value_get(key))
    {
        count = atoi(temp_string);
        if(count > 1)
        {
            sprintf(value, "%d", --count);
            libed_key_value_set(key, value);
        }
        else if(count == 1)
        {
#ifdef CLI_TEST_SINGLE_CMD
            output = CLI_TEST_CONF_OUTPUT;
#else
            len = strlen(temp_name)+strlen(cli_setting_test_string[TEST_PARAM_OUTPUT])+1;
            key = realloc(key, len+1);
            snprintf(key, len+1, "%s.%s", temp_name, cli_setting_test_string[TEST_PARAM_OUTPUT]);
            output = libed_key_value_get(key);
#endif
            output = cli_setting_read_output(output);
            if(NULL != output)
            {
                LIBED_STDOUT("%s", output);
            }

            len = strlen(temp_name)+strlen(cli_setting_test_string[TEST_PARAM_RETURN])+1;
            key = realloc(key, len+1);
            snprintf(key, len+1, "%s.%s", temp_name, cli_setting_test_string[TEST_PARAM_RETURN]);
            if(temp_string = libed_key_value_get(key))
            {
                ret = atoi(temp_string);
            }

            if(ret != 0 || output)
            {
#ifndef CLI_TEST_SINGLE_CMD
                LIBED_VALUE_FREE(output);
#endif
                exit(ret);
            }
        }
        else
        {
            ret = LIBED_OPERATION_INTERPUT;
        }
    }
    libed_key_value_encoding(CLI_TEST_CONF_PATH, 1);
    return ret;
}

int cli_setting_auto_reply_set(struct cli_setting_t *setting)
{
    char ch = 0;
    int ret = LIBED_ERROR_NONE;
    LIBED_VALUE_CHECK(NULL == setting, LIBED_OPERATION_FAILED);

    if(!setting->quite)
    {
        LIBED_STDOUT("%s%s%s", cli_setting_reference[CONTINUE], Y_OR_N, " : ");
        if(setting->y_or_n == CLI_Y)
        {
            LIBED_STDOUT("y\n");
            ch = 'y';
        }
        else if(setting->y_or_n == CLI_N)
        {
            LIBED_STDOUT("n\n");
            ch = 'n';
        }
        else
        {
            while((ch = getchar()) != '\n')
            {
                break;
            }
        }
    }
    else if(setting->y_or_n == CLI_N)
    {
        ch = 'n';
    }
    else if(setting->y_or_n == CLI_Y)
    {
        ch = 'y';
    }
    else 
    {
        ret = LIBED_INVALID_PARAM;
    }

    switch(ch)
    {
        case 'y':
        case 'Y':
            ret = LIBED_ERROR_NONE;
            break;
        case 'n':
        case 'N':
            ret = LIBED_OPERATION_FAILED;
            break;
        default:
            ret = LIBED_ERROR_NONE;
            break;
    }

    return ret;
}


