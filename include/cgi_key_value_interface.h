#ifndef _cgi_keyvalue_interface_h_
#define _cgi_keyvalue_interface_h_

#ifdef __cplusplus
extern "C" {
#endif

char* cgi_key_value_decoding(char *path);
int cgi_key_value_encoding(char *path, int overrite);
int cgi_key_value_cleanup();
int cgi_key_value_tree_print();

/*
used format:
key:value
*/
char* cgi_key_value_get(char *key);
int cgi_key_value_set(char *key, char *value);
int cgi_key_value_del(char *key);

/*
used format: to be deleted
key:child_value_key;child_value|child_value_key;child_value|......
*/
int cgi_key_value_child_set(char *key, int child_param_num, char *child_value);
char* cgi_key_value_child_get(char *key, int child_param_num);

/*
used format:
key:child_value_key;child_value;child_value;...|child_value_key;child_value;child_value;...|......
*/
int cgi_key_value_child_value_set(char *key, char* child_value_key, int child_param_num, char *value_param);
char* cgi_key_value_child_value_get(char *key, char* child_value_key, int child_param_num);
int cgi_key_value_child_value_del(char *key, char *child_value_key, int pos, char *value_param);

int cgi_key_value_tree_print();

#ifdef __cplusplus
}
#endif
	
#endif
