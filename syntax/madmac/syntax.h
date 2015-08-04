/* snytax.h  syntax header file for vasm */
/* (c) in 2015 by Frank Wille */

/* macros to recognize identifiers */
#define ISIDSTART(x) ((x)=='.'||(x)=='?'||(x)=='_'||isalpha((unsigned char)(x)))
#define ISIDCHAR(x) ((x)=='$'||(x)=='?'||(x)=='_'||isalnum((unsigned char)(x)))
#define ISEOL(x) (*(x)=='\0' || *(x)==commentchar)

/* result of a boolean operation */
#define BOOLEAN(x) (x)

/* overwrite macro defaults */
#define MAXMACPARAMS 64
