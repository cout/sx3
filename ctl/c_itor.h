#ifndef CTL__C_ITOR_H
#define CTL__C_ITOR_H

// CTL Iterators
// A CTL iterator is a set of macros or functions for operating on type xxx,
// with one or more of the following operations:
//   1) The increment operation.  This is equivalent to operator++ in the STL.
//      We use xxx_INC(it) to represent this, where xxx is our type, and it
//      is our iterator.  An iterator may also define an xxx_DEC(it) operation
//      to represent a decrement.
//   2) The comparison operation.  This is equivalent to operator< in the STL.
//      We use xxx_LT(it1, it2); this returns true if it1 is less than it2,
//      and false otherwise.
//   3) The equality operation.  This is equivalent to operator== in the STL.
//      We use xxx_EQ(it1, it2); this returns true if it1 is equal to it2,
//      and false otherwise.  Inequality can be tested using this operation.
//   4) The dereference operation.  This is equivalent to operator* in the STL.
//      We use xxx_GET(it) to represent this.
//   5) The assignment operation.  This is equivalent to operator= in the STL.
//      We use the same operator in CTL.  This means that struct pointers
//      do NOT make good iterators!
// The problem here is that we need to know the type of our data if we are to
// perform any operations on an iterator.  This is a limitation of the CTL.
// Also, an iterator need not define all of the above operations, but if an
// operation is not defined, then this will limit which algorithms can be used
// with that data type.

// ARRAY_IT is an iterator that iterates through an array.
#define ARRAY_IT(type) type*

// CONST_ARRAY_IT is a const iterator that iterates through an array.
#define CONST_ARRAY_IT(type) const type*

// ARRAY_BEGIN returns an iterator pointing to the start of the array.
#define ARRAY_BEGIN(arr) (arr)

// ARRAY_END returns an interator pointing to the end of the array.  Because
// the array has no size information internally, ARRAY_END takes size as an
// argument
#define ARRAY_END(arr,size) ((arr)+(size))

// Lastly, the operations on the array iterators:
#define ARRAY_IT_INC(it)        (it++)
#define ARRAY_IT_LT(it1, it2)   ((it1) < (it2))
#define ARRAY_IT_EQ(it1, it2)   ((it1) == (it2))
#define ARRAY_IT_GET(it)        (*it)

// the STACK_array iterators are special cases of the array iterators, in
// that they will only work with arrays that are created on the stack.
#define STACK_ARRAY_IT(type)       type*
#define CONST_STACK_ARRAY_IT(type) const type*
#define STACK_ARRAY_BEGIN(arr)     (arr)
#define STACK_ARRAY_END(arr)       ((arr)+(sizeof(arr)/sizeof(*arr)))

// And the operations on the stack array iterators
#define STACK_ARRAY_IT_INC(it)        (it++)
#define STACK_ARRAY_IT_LT(it1, it2)   ((it1) < (it2))
#define STACK_ARRAY_IT_EQ(it1, it2)   ((it1) == (it2))
#define STACK_ARRAY_GET(it)           (*it)

#endif // CTL__C_ITOR_H
