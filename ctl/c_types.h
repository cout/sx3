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

#define CTL_BOOL_FIELD(field) int field:1

#define IS_TRUE(b)  (b)
#define IS_FALSE(b) (!b)

#endif
