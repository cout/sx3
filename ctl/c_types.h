#ifndef CTL__TYPES_H
#define CTL__TYPES_H

typedef char CTL_BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

// Returns 0 for equal, -1 if lhs is less than rhs, and 1 if
// lhs is greatre than rhs.
#define CTL_COMPARISON_FUNCTION(type, name) int (*name)(type, type)

#define CTL_BOOL_FIELD(field) int field:1

#define IS_TRUE(b)  (b)
#define IS_FALSE(b) (!b)

#endif
