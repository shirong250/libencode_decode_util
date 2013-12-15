#ifndef LIBED_COMMON_H_
#define LIBED_COMMON_H_

enum FUN_RET{
    NOT_FIND_OUT = 100
};

enum CHECK_RET {
        SUCCESS = 0,
        ERROR
};

enum FLAG {
	FALSE = 0,
	TRUE
};

int libed_check_param(char* argv);
char libed_hexval(char c);
void libed_encode_special_char(const char* data, char* buf);
void libed_decode_special_char(const char* data, char* buf);

#endif /* LIBED_COMMON_H_ */
