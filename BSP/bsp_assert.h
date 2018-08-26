
#undef assert

#ifdef    __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
/*
 * If not debugging, assert does nothing.
 */
#define assert(x)    ((void)0)

#else /* debugging enabled */

/*
 * CRTDLL nicely supplies a function which does the actual output and
 * call to abort.
 */
extern void _assert(const char *mesg, const char *file, int line);

/*
 * Definition of the assert macro.
 */
#define assert(e)       ((e) ? (void)0 : _assert(#e, __FILE__, __LINE__))

#endif    /* NDEBUG */

#ifdef    __cplusplus
}
#endif
