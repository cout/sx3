#include <stdio.h>
#include "ini.h"

const char* get_value(INI_Context *ini, const char *a, const char *b) {
    const char *tmp = ini_get_value(ini, a, b);
    if(tmp) return tmp;
    return "NOT FOUND";
}

int main() {
    INI_Context *ini = ini_new_context();
    ini_load_config_file(ini, "test.ini");
    printf("%s\n", get_value(ini, "testsection", "testvar"));
    printf("%s\n", get_value(ini, "doesnotexist", "doesnotexist"));
    return 0;
}
