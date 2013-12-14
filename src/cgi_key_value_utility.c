#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pwd.h>

#include "cgi_key_value.h"

#define CGI_PASSWROD_DATE_LEN 128

void cgi_trace(char *format, ...)
{
    char trace_buffer[MAX_READ_BUFFER] = {0}; 
    va_list arg_ptr; 
    va_start(arg_ptr, format);
    vsprintf(trace_buffer, format, arg_ptr); 
    va_end(arg_ptr); 
    printf("%s\n", trace_buffer); 
}

/* remove space in the front */
int cgi_ltrim(char *string)
{
    if(NULL == string || (!strlen(string)))
    {
        return 0;
    }
   
    int index = 0;
    int space_num=0;
    while(string[space_num] == ' ' || string[space_num] == '\t')
    {
        space_num++;
    }
    while(string[space_num] != '\0' && string[space_num] != CGI_KEY_VALUE_SEPRATOR)
    {
        string[index++] = string[space_num++];
    }
    string[index] = '\0';
    return index;
}

/* remove space tail */
int cgi_rtrim(char *string)
{
    if(NULL == string || (!strlen(string)))
    {
        return 0;
    }

    int space_num = 0;
    int len = strlen(string);
    while(string[len-space_num-1] == ' ' || string[len-space_num-1] == '\t')
    {
        string[len-space_num-1] = '\0';
        len--;
    }
    return len;
}

void cgi_strupr(char *string, int len)
{
    int index = 0;
    for(; index < len; ++index)
    {
        string[index] = toupper(string[index]);
    }
}

int cgi_blockhead(char *string)
{
    if(NULL == string || (!strlen(string)))
    {
        return 0;
    }
    string[0] -= 32;
}

void cgi_input_print(int argc, char **argv)
{
    int index = 0;
    for(; index < argc; ++index)
    {
        CGI_STDOUT("%s ", argv[index]);
    }
    CGI_STDOUT("\n");
}

char *cgi_get_current_user()
{
    char *username = NULL;
    username = getenv("BUI_USER");
    if(NULL == username)
    {
        struct passwd *pwd;
        pwd = getpwuid(getuid());
        username = pwd->pw_name;
    }
    return username;
}

int cgi_utility_check_date(char *date)
{
    CGI_VALUE_CHECK(NULL == date, CGI_INVALID_PARAM);
    int ret = 0;
    char year[CGI_PASSWROD_DATE_LEN] = {0};
    char month[CGI_PASSWROD_DATE_LEN] = {0};
    char day[CGI_PASSWROD_DATE_LEN] = {0};

    if(0 == strcmp(date, "Never"))
    {
        return CGI_ERROR_NONE;
    }
    ret = sscanf(date, "%2s/%2s/%2s", month, day, year);
    if(month[0] != '\0' && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }
    
    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%4s-%2s-%2s", year, month, day);
    if(month[0] != '\0' && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }
    
    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%2s-%2s-%2s", year, month, day);
    if(month[0] != '\0' && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }
    
    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%2s-%3s-%2s", day, month, year);
    if(month[0] != '\0' && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }

    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%2s-%3s-%4s", day, month, year);
    if(month[0] != '\0' && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }

    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%2s %3s %2s", day, month, year);
    if(strlen(month) == 3 && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }

    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%2s %3s %4s", day, month, year);
    if(strlen(month) == 3 && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }
    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%3s %2s, %4s", month, day, year);
    if(strlen(month) == 3 && day[0] != '\0' && strlen(year) == 4)
    {
        return CGI_ERROR_NONE;
    }
    
    memset(year, 0, CGI_PASSWROD_DATE_LEN);
    memset(month, 0, CGI_PASSWROD_DATE_LEN);
    memset(day, 0, CGI_PASSWROD_DATE_LEN);
    ret = sscanf(date, "%3s %2s, %2s", month, day, year);
    if(strlen(month) == 3 && day[0] != '\0' && year[0] != '\0')
    {
        return CGI_ERROR_NONE;
    }

    return CGI_INVALID_PARAM;
}


