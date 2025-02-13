#include "tests.h"

int main(int argc, char **argv) {
    #ifdef MS_RUN_TESTS
    ms_run_tests(argc, argv);
    #endif
    return 0;
}
