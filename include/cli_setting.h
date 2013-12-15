#ifndef _CGI_SETTING_H
#define _CGI_SETTING_H

#define LIBED_Y 1
#define LIBED_N 2

#define LIBED_TEST_SINGLE_CMD

typedef struct libed_setting_t
{
    int y_or_n;
    int quite;
} LIBED_SETTING;

int libed_setting_auto_reply_set(struct libed_setting_t *setting);

void libed_setting_usage_print();

#endif
