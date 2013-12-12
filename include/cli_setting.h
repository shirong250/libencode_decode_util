#ifndef _CGI_SETTING_H
#define _CGI_SETTING_H

#define CLI_Y 1
#define CLI_N 2

#define CLI_TEST_SINGLE_CMD

typedef struct cli_setting_t
{
    int y_or_n;
    int quite;
} CLI_SETTING;

int cli_setting_auto_reply_set(struct cli_setting_t *setting);

void cli_setting_usage_print();

#endif
