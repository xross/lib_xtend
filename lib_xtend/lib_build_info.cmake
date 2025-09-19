set(LIB_NAME lib_xtend)
set(LIB_VERSION 0.0.1)
set(LIB_INCLUDES api)

set(LIB_DEPENDENT_MODULES "")

set(LIB_OPTIONAL_HEADERS xtend_conf.h)

set(LIB_COMPILER_FLAGS -O3
                       -g
                       )

XMOS_REGISTER_MODULE()
