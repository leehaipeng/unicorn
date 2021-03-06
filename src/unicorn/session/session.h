#ifndef UNICORN_SESSION_H
#define UNICORN_SESSION_H


#include <sys/time.h>

#include "unicorn/types.h"


struct UnicornSessionOutputState
{
    int index;
    int length;
    char string[50];
};

struct UnicornTestSession
{
    UnicornCollection *results;
    UnicornCollection *failures;
    struct timeval start_time;
    struct timeval end_time;
    UnicornSessionOutputState output_state;
};

UnicornTestSession *unicorn_new_test_session();

void unicorn_test_session_start(UnicornTestSession *test_session);
void unicorn_test_session_end(UnicornTestSession *test_session);

void unicorn_test_session_run_test(UnicornTestSession *test_session, UnicornTest *test);
void unicorn_test_session_run_parameterized_test(UnicornTestSession *test_session, UnicornTest *test, UnicornCollectionItem *param_item);
void unicorn_test_session_run_test_group(UnicornTestSession *test_session, UnicornTestGroup *test_group);

void unicorn_free_test_session(UnicornTestSession *test_session);


#endif
