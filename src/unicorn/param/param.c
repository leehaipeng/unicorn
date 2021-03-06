#include <stdlib.h>
#include <string.h>

#include "unicorn/collection/collection.h"
#include "unicorn/param/param.h"


/*
 * Currently accessible params
 */

UnicornCollection *_unicorn_current_params = NULL;


/*
 * Test param initialization
 */

static void initialize_test_param(UnicornTestParam *test_param, char *name, void *values, size_t count, UnicornTest *test)
{
    test_param->name = name;
    test_param->index = 0;
    test_param->count = count;
    test_param->values = values;
    test_param->test = test;
}

UnicornTestParam *unicorn_new_test_param(char *name, void *values, size_t count, UnicornTest *test)
{
    UnicornTestParam *test_param = malloc(sizeof (UnicornTestParam));
    initialize_test_param(test_param, name, values, count, test);

    return test_param;
}


/*
 * Get param from param collection
 */

UnicornTestParam *unicorn_get_test_param(UnicornCollection *params, char *param_name)
{
    UnicornTestParam *test_param;
    UNICORN_EACH(test_param, params)
    {
        if (strcmp(test_param->name, param_name) == 0)
        {
            return test_param;
        }
    }
    return NULL;
}


/*
 * Cleanup
 */

void unicorn_free_test_param(UnicornTestParam *test_param)
{
    free(test_param);
}
