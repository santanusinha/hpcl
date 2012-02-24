AC_INIT([hpcl], [0.01-alpha], [santanu.sinha@gmail.com], [hpcl])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([./source/common/include/print.h])
AC_CONFIG_AUX_DIR([./config])

AC_PROG_CC
AC_PROG_CXX(g++)
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([1.9])
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE(debug,
		[  --enable-debug          enable debug build (default=no)], ,
		enable_debug=no
)

AC_CONFIG_FILES([
Makefile
source/Makefile
source/common/Makefile
source/common/include/Makefile
source/common/src/Makefile
source/communication/Makefile
source/communication/include/Makefile
source/communication/src/Makefile
])

AC_OUTPUT
