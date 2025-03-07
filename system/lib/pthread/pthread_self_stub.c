/*
 * Copyright 2021 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include "pthread_impl.h"
#include <unistd.h>

static struct pthread __main_pthread;

pthread_t __pthread_self(void) {
  return &__main_pthread;
}

// In case the stub syscall is not linked it
static long dummy_getpid() {
  return 42;
}
weak_alias(dummy_getpid, __syscall_getpid);

__attribute__((constructor))
static void init_pthread_self(void) {
  __main_pthread.locale = &libc.global_locale;
  __main_pthread.tid = getpid();
}
