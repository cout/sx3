#ifndef CTL__C_HASH_H
#define CTL__C_HASH_H

#define HASH_FUNCTION(type,name) size_t (*name)(type *)

// The "type" parameter with hash macros is always HASH_ELEMENT
#define HASH_ELEMENT(key_type, value_type) \
    struct { \
        key_type key; \
        value_type value; \
    }

#define HASH_BUCKET(key_type, value_type) \
    struct { \
        size_t count; \
        size_t max; \
        HASH_ELEMENT(key_type, value_type) *data; \
    }

#define HASH(key_type, value_type) \
    struct { \
        HASH_BUCKET(key_type, value_type) *data; \
        size_t size; \
        size_t key_size; \
        size_t key_start; \
        size_t value_size; \
        size_t value_start; \
        size_t element_size; \
        HASH_FUNCTION(key_type, hash); \
    }

#define HASH_PARTIAL_TYPE_CHECK(h) \
    CTL_ASSERT_EXISTS(h.data), \
    CTL_ASSERT_EXISTS(h.size), \
    CTL_ASSERT_EXISTS(h.key_size), \
    CTL_ASSERT_EXISTS(h.key_start), \
    CTL_ASSERT_EXISTS(h.value_size), \
    CTL_ASSERT_EXISTS(h.value_start), \
    CTL_ASSERT_EXISTS(h.element_size), \
    CTL_ASSERT_EXISTS(h.hash)

#define HASH_TYPE_CHECK(h) { \
    HASH_PARTIAL_TYPE_CHECK(h); \
    CTL_ASSERT_IS_TYPE_OF(h.size, size_t); \
    CTL_ASSERT_IS_TYPE_OF(h.key_size, size_t); \
    CTL_ASSERT_IS_TYPE_OF(h.value_size, size_t); \
    CTL_ASSERT_IS_TYPE_OF(h.value_start, size_t); \
    CTL_ASSERT_IS_TYPE_OF(h.element_size, size_t); \
}

#define HASH_DATA_TYPE_CHECK(h, key_type, value_type) { \
    CTL_ASSERT_IS_TYPE_OF(h.data, HASH_BUCKET(key_type, value_type)); \
    CTL_ASSERT_IS_TYPE_OF(h.hash, HASH_FUNCTION(key_type, hash)); \
}

#define HASH_IT(type)               /* TODO */
#define HASH_CONST_IT(type)         /* TODO */
#define HASH_BEGIN(h)               /* TODO */
#define HASH_END(h)                 /* TODO */
#define HASH_INIT(h, size, f) \
    do { \
        HASH_TYPE_CHECK(h); \
        ctl__hash_init( \
            (CTL__Hash *)&h, \
            sizeof(*h.data->data), \
            size, \
            (void *)f); \
        CTL__HASH_POST_INIT( \
            h, \
            sizeof(h.data->data->key), \
            (char *)&h.data->data->key - (char *)&h.data->data[0], \
            sizeof(h.data->data->value), \
            (char *)&h.data->data->value - (char *)&h.data->data[0]); \
    } while(0)

#define HASH_FREE(h) { \
    HASH_TYPE_CHECK(h); \
    ctl__hash_free((CTL__Hash *)&h); \
}
#define HASH_INSERT(h, k, v) { \
    HASH_TYPE_CHECK(h); \
    CTL_ASSERT_SAME_TYPE(h.data->data->key, k); \
    CTL_ASSERT_SAME_TYPE(h.data->data->value, v); \
    ctl__hash_insert((CTL__Hash *)&h, &k, &v); \
}
#define HASH_FIND(h, key, comp, result) ( \
    /* TODO */ \
    HASH_PARTIAL_TYPE_CHECK(h), \
    ctl__hash_find((CTL__Hash *)&h, &key, (void *)comp, &result) \
)

#define HASH_IT_LT(it1, it2)        /* TODO */
#define HASH_IT_EQ(it1, it2)        /* TODO */
#define HASH_IT_INC(it)             /* TODO */
#define HASH_IT_GET(it)             /* TODO */

#include "c_hash.i"

#endif
