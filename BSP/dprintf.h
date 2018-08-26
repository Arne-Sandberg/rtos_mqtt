#ifndef __DPRINTF_H__
#define __DPRINTF_H__

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _DEBUG
#include <stdarg.h>
#include <stdio.h>
extern const char *getCurrentFileName(const char* strFileName);
#define dprintf(fmt,...) printf("%s,line:%d,"fmt,getCurrentFileName(__FILE__),__LINE__,##__VA_ARGS__)

#else
#define dprintf(fmt,...)
#endif


#ifdef __cplusplus
}
#endif

#endif
