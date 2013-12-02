#ifndef V8_TEST_H
#define V8_TEST_H

#include <stdio.h>

#define assert_that(S, X) printf(S " -> %s\n", (X) ? "pass" : "fail")

#endif
