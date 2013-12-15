#ifndef _libed_keyvalue_interface_h_
#define _libed_keyvalue_interface_h_

#ifdef __cplusplus
extern "C" {
#endif

char* libed_key_value_decoding(char *path);
int libed_key_value_encoding(char *path, int overrite);
int libed_key_value_cleanup();
int libed_key_value_tree_print();

/*
used format:
key:value
*/
char* libed_key_value_get(char *key);
int libed_key_value_set(char *key, char *value);
int libed_key_value_del(char *key);

/*
used format: to be deleted
key:child_value_key;child_value|child_value_key;child_value|......
*/
int libed_key_value_child_set(char *key, int child_param_num, char *child_value);
char* libed_key_value_child_get(char *key, int child_param_num);

/*
used format:
key:child_value_key;child_value;child_value;...|child_value_key;child_value;child_value;...|......
*/
int libed_key_value_child_value_set(char *key, char* child_value_key, int child_param_num, char *value_param);
char* libed_key_value_child_value_get(char *key, char* child_value_key, int child_param_num);
int libed_key_value_child_value_del(char *key, char *child_value_key, int pos, char *value_param);

int libed_key_value_tree_print();

#ifdef __cplusplus
}
#endif
	
#endif
