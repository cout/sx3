#ifndef CTL__TYPES_H
#define CTL__TYPES_H

typedef char CTL_BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define CTL_COMPARISON_FUNCTION(type, name) CTL_BOOL (*name)(type, type)

#endif
