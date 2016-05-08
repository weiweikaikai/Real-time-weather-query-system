AC_DEFUN([CONFIG_EXTRA], [

AH_TOP([
#pragma once

/* _SYS_FEATURE_TESTS_H is Solaris, _FEATURES_H is GCC */
#if defined( _SYS_FEATURE_TESTS_H) || defined(_FEATURES_H)
#error "You should include config.h as your first include file"
#endif

])

AH_BOTTOM([

#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
 
#if defined(__cplusplus) 
#  include CINTTYPES_H 
#else 
#  include <inttypes.h> 
#endif

#if !defined(HAVE_ULONG) && !defined(__USE_MISC)
# define HAVE_ULONG 1
typedef unsigned long int ulong;
#endif 

#define RPACKAGE "memcached" 
 
#define RVERSION "1.4.13"

])

])dnl CONFIG_EXTRA
