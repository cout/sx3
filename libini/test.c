#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ini.h"

void assert_ini_value(
        INI_Context *ini,
        const char *section,
        const char *key,
        const char *expected_value)
{
    const char *value = ini_get_value(ini, section, key);
    if (value == 0 || expected_value == 0)
    {
        // Assert that both values are NULL
        assert(value == expected_value);
    }
    else
    {
        // Assert that both values contain the same string
        int value_equals_expected_value =
          !strcmp(value, expected_value);
        assert(value_equals_expected_value);
    }
}

int main()
{
    INI_Context *ini = ini_new_context();
    ini_load_config_file(ini, "test.ini");

    // Assert that the following exist:
    printf("Starting test 1\n");
    assert_ini_value(ini, "testsection", "foo", "test1");
    assert_ini_value(ini, "testsection", "foo2", "test2");
    assert_ini_value(ini, "testsection", "bar", "this is a test");
    assert_ini_value(ini, "foo", "foo", "test2");
    assert_ini_value(ini, "foo", "foo2", "42");

    // Assert that the following do NOT exist:
    printf("Starting test 2\n");
    assert_ini_value(ini, "testsection", "bar2", 0);
    assert_ini_value(ini, "foo", "bar", 0);
    assert_ini_value(ini, "foo2", "foo", 0);
    assert_ini_value(ini, "bar", "testsection", 0);

    return 0;
}
