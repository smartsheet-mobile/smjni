#ifndef HEADER_TEST_UTIL_H
#define HEADER_TEST_UTIL_H

#include "generated/all_classes.h"

#define CONCAT_TOKEN(foo, bar) CONCAT_TOKEN_IMPL(foo, bar)
#define CONCAT_TOKEN_IMPL(foo, bar) foo ## bar
#define STRINGIFY(name) STRINGIFY_IMPL(name)
#define STRINGIFY_IMPL(name) #name

#define THROW_ASSERTION_FAILURE(message) throw java_exception(java_classes::get<AssertionError>().ctor(env, java_string_create(env, message " at " __FILE__ ":" STRINGIFY(__LINE__))))

#define ASSERT_TRUE(x) if (!(x)) \
    THROW_ASSERTION_FAILURE(STRINGIFY(x) " is false, expected true")
#define ASSERT_FALSE(x) if (x) \
    THROW_ASSERTION_FAILURE(STRINGIFY(x) " is true, expected false")
#define ASSERT_EQUAL(expected, actual) if ((expected) != (actual)) \
    THROW_ASSERTION_FAILURE(STRINGIFY(expected) " != " STRINGIFY(actual));

#define NATIVE_PROLOG  try {
#define NATIVE_EPILOG  } \
                       catch(java_exception & ex) \
                       { \
                           ex.raise(env);\
                       }\
                       catch(std::exception & ex)\
                       {\
                           java_exception::translate(env, ex);\
                       }

class AssertionError : public smjni::java_runtime::simple_java_class<jAssertionError>
{
public:
    AssertionError(JNIEnv * env):
        simple_java_class(env),
        ctor(env, *this)
    {}

    smjni::java_constructor<jAssertionError, jstring> ctor;
};

typedef smjni::java_class_table<AssertionError, JNIGEN_ALL_GENERATED_CLASSES> java_classes;


#endif