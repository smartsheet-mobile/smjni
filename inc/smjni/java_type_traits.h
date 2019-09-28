/*
 Copyright 2014 Smartsheet.com, Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef HEADER_JAVA_TYPE_TRAITS_H_INCLUDED
#define HEADER_JAVA_TYPE_TRAITS_H_INCLUDED

#include <string>

#include <smjni/java_ref.h>
#include <smjni/java_types.h>

namespace smjni
{

    SMJNI_FORCE_INLINE jsize size_to_java(size_t size)
    {
        if (sizeof(size_t) >= sizeof(jsize))
        {
            if (size > std::numeric_limits<jsize>::max())
                abort();
            
        }
        return jsize(size);
    }

    SMJNI_FORCE_INLINE size_t java_size_to_cpp(jsize size)
    {
        if (size < 0)
            abort();
        if (sizeof(size_t) < sizeof(jsize))
        {
            if (size > std::numeric_limits<size_t>::max())
                abort();
            
        }
        return size_t(size);
    }

    template<typename T> SMJNI_FORCE_INLINE constexpr T argument_to_java(T val) 
        { return val; }
    template<typename T> SMJNI_FORCE_INLINE T argument_to_java(const auto_java_ref<T> & val)
        { return val.c_ptr(); }

    template<typename T> SMJNI_FORCE_INLINE constexpr T return_value_from_java(JNIEnv *, T val)
        { return val; }
    template<typename T> SMJNI_FORCE_INLINE local_java_ref<T *> return_value_from_java(JNIEnv * env, T * val)
        { return jattach(env, val); }


    inline std::string object_signature_from_name(const char * name)
    {
        std::string ret = "L";
        for(const char * p = name; *p; ++p) 
            ret += (*p != '.' ? *p : '/'); 
        ret += ';';
        return ret;
    }

    inline std::string array_signature_from_name(const char * name)
    {
        std::string ret = "[L";
        for(const char * p = name; *p; ++p) 
            ret += (*p != '.' ? *p : '/'); 
        ret += ';';
        return ret;
    }
    

    template<typename T>
    class java_type_traits;

    template<typename T>
    class java_array_type_of;

    template<typename T>
    using java_array_type_of_t = typename java_array_type_of<T>::type;

    template<>
    class java_type_traits<void>
    {
    public:
        typedef void return_type;

        java_type_traits() = delete;
        java_type_traits(const java_type_traits &) = delete;
        java_type_traits & operator=(const java_type_traits &) = delete;

        static std::string signature()
        {
            return "V";
        }

        static void call_method(JNIEnv * jenv, jobject object, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            jenv->CallVoidMethodV(object, method, vl);
            va_end(vl);
        }

        static void call_static_method(JNIEnv * jenv, jclass clazz, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            jenv->CallStaticVoidMethodV(clazz, method, vl);
            va_end(vl);
        }

        static void call_non_virtual_method(JNIEnv * jenv, jobject object, jclass clazz, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            jenv->CallNonvirtualVoidMethodV(object, clazz, method, vl);
            va_end(vl);
        }
    };
}

#define HANDLE_PRIMITIVE_JAVA_TYPE(jtype, name, sig) \
    namespace smjni \
    {\
        template<> \
        class java_type_traits<jtype> \
        { \
        public: \
            typedef jtype return_type; \
            typedef jtype arg_type; \
            \
            java_type_traits() = delete; \
            java_type_traits(const java_type_traits &) = delete; \
            java_type_traits & operator=(const java_type_traits &) = delete; \
            \
            static std::string signature() \
            { \
                return sig; \
            } \
            \
            static jtype call_method(JNIEnv * jenv, jobject object, jmethodID method, ...) \
            { \
                va_list vl; \
                va_start(vl, method); \
                jtype ret = jenv->Call##name##MethodV(object, method, vl); \
                va_end(vl); \
                return ret;\
            }\
            \
            static jtype call_static_method(JNIEnv * jenv, jclass clazz, jmethodID method, ...)\
            {\
                va_list vl;\
                va_start(vl, method);\
                jtype ret = jenv->CallStatic##name##MethodV(clazz, method, vl);\
                va_end(vl);\
                return ret;\
            }\
            \
            static jtype call_non_virtual_method(JNIEnv * jenv, jobject object, jclass clazz, jmethodID method, ...)\
            {\
                va_list vl;\
                va_start(vl, method);\
                jtype ret = jenv->CallNonvirtual##name##MethodV(object, clazz, method, vl);\
                va_end(vl);\
                return ret;\
            }\
            static jtype get_field(JNIEnv * jenv, jobject object, jfieldID field)\
            {\
               jtype ret = jenv->Get##name##Field(object, field);\
               return ret;\
            }\
            static void set_field(JNIEnv * jenv, jobject object, jfieldID field, jtype val)\
            {\
               jenv->Set##name##Field(object, field, val);\
            }\
            static jtype get_static_field(JNIEnv * jenv, jclass clazz, jfieldID field)\
            {\
               jtype ret = jenv->GetStatic##name##Field(clazz, field);\
               return ret;\
            }\
            static void set_static_field(JNIEnv * jenv, jclass clazz, jfieldID field, jtype val)\
            {\
               jenv->SetStatic##name##Field(clazz, field, val);\
            }\
            static jtype##Array new_array(JNIEnv * jenv, jsize size)\
            {\
               return jenv->New##name##Array(size);\
            }\
            static void set_array_region(JNIEnv * jenv, jtype##Array array, jsize start, jsize len, const jtype * buf)\
            {\
                jenv->Set##name##ArrayRegion(array, start, len, buf);\
            }\
            static void get_array_region(JNIEnv * jenv, jtype##Array array, jsize start, jsize len, jtype * buf)\
            {\
                jenv->Get##name##ArrayRegion(array, start, len, buf);\
            }\
        };\
    }

HANDLE_PRIMITIVE_JAVA_TYPE(jboolean,    Boolean,    "Z");
HANDLE_PRIMITIVE_JAVA_TYPE(jbyte,       Byte,       "B");
HANDLE_PRIMITIVE_JAVA_TYPE(jchar,       Char,       "C");
HANDLE_PRIMITIVE_JAVA_TYPE(jshort,      Short,      "S");
HANDLE_PRIMITIVE_JAVA_TYPE(jint,        Int,        "I");
HANDLE_PRIMITIVE_JAVA_TYPE(jlong,       Long,       "J");
HANDLE_PRIMITIVE_JAVA_TYPE(jfloat,      Float,      "F");
HANDLE_PRIMITIVE_JAVA_TYPE(jdouble,     Double,     "D");

#undef HANDLE_PRIMITIVE_JAVA_TYPE

namespace smjni
{
    template<typename T>
    class java_object_type_base
    {
    public:
        typedef local_java_ref<T> return_type;
        typedef const auto_java_ref<T> & arg_type; 

        java_object_type_base() = delete;
        java_object_type_base(const java_object_type_base &) = delete;
        java_object_type_base & operator=(const java_object_type_base &) = delete;

        static T call_method(JNIEnv * jenv, jobject object, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            T ret = static_cast<T>(jenv->CallObjectMethodV(object, method, vl)); 
            va_end(vl);
            return ret;
        }

        static T call_static_method(JNIEnv * jenv, jclass clazz, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            T ret = static_cast<T>(jenv->CallStaticObjectMethodV(clazz, method, vl));
            va_end(vl);
            return ret;
        }

        static T call_non_virtual_method(JNIEnv * jenv, jobject object, jclass clazz, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            T ret = static_cast<T>(jenv->CallNonvirtualObjectMethodV(object, clazz, method, vl));
            va_end(vl);
            return ret;
        }

        static T get_field(JNIEnv * jenv, jobject object, jfieldID field)
        {
            T ret = static_cast<T>(jenv->GetObjectField(object, field));
            return ret;
        }

        static void set_field(JNIEnv * jenv, jobject object, jfieldID field, T val)
        {
            jenv->SetObjectField(object, field, val);
        }

        static T get_static_field(JNIEnv * jenv, jclass clazz, jfieldID field)
        {
            T ret = static_cast<T>(jenv->GetStaticObjectField(clazz, field));
            return ret;
        }

        static void set_static_field(JNIEnv * jenv, jclass clazz, jfieldID field, T val)
        {
            jenv->SetStaticObjectField(clazz, field, val);
        }

        static T new_object(JNIEnv * jenv, jclass clazz, jmethodID method, ...)
        {
            va_list vl;
            va_start(vl, method);
            T ret = static_cast<T>(jenv->NewObjectV(clazz, method, vl));
            va_end(vl);
            return ret;
        }
    };
}

#define HANDLE_OBJECT_JAVA_TYPE(jtype, name) \
    namespace smjni\
    {\
        template<> \
        class java_type_traits<jtype> : public java_object_type_base<jtype> \
        {\
        public:\
            static const std::string & signature()\
            {\
                static const auto the_signature = object_signature_from_name(name);\
                return the_signature;\
            }\
            \
            static const char * class_name() \
            {\
                return name;\
            }\
        };\
    }

HANDLE_OBJECT_JAVA_TYPE(jobject,     "java.lang.Object");
HANDLE_OBJECT_JAVA_TYPE(jstring,     "java.lang.String");
HANDLE_OBJECT_JAVA_TYPE(jthrowable,  "java.lang.Throwable");
HANDLE_OBJECT_JAVA_TYPE(jclass,      "java.lang.Class");

#define HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jtype, name, sig) \
    namespace smjni \
    {\
        template<> \
        class java_type_traits<jtype##Array> : public java_object_type_base<jtype##Array> \
        {\
        public:\
            typedef jtype element_type;\
            \
            static std::string signature()\
            {\
                return sig;\
            }\
            static void release_array_elements(JNIEnv * env, jtype##Array ar, jtype * data, jint flags)\
            {\
                env->Release##name##ArrayElements(ar, data, flags);\
            }\
            static jtype * get_array_elements(JNIEnv * env, jtype##Array ar, jboolean * copied)\
            {\
                return env->Get##name##ArrayElements(ar, copied);\
            }\
        };\
        template<>\
        class java_array_type_of<jtype>\
        {\
            public: typedef jtype##Array type;\
        };\
    }

HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jboolean,    Boolean,    "[Z");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jbyte,       Byte,       "[B");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jchar,       Char,       "[C");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jshort,      Short,      "[S");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jint,        Int,        "[I");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jlong,       Long,       "[J");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jfloat,      Float,      "[F");
HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE(jdouble,     Double,     "[D");

#undef HANDLE_PRIMITIVE_ARRAY_JAVA_TYPE

#define HANDLE_OBJECT_ARRAY_JAVA_TYPE(jtype) \
    namespace smjni \
    {\
        template<> \
        class java_type_traits<jtype##Array> : public java_object_type_base<jtype##Array> \
        {\
        public:\
            typedef jtype element_type;\
            \
            static const std::string & signature()\
            {\
                static const auto the_signature = array_signature_from_name(java_type_traits<jtype>::class_name());\
                return the_signature;\
            }\
        };\
        template<>\
        class java_array_type_of<jtype>\
        {\
            public: typedef jtype##Array type;\
        };\
    }

HANDLE_OBJECT_ARRAY_JAVA_TYPE(jobject);


#define DEFINE_JAVA_TYPE(type, name) \
        class _##type : public _jobject {};\
        typedef _##type * type;\
        HANDLE_OBJECT_JAVA_TYPE(type, name)

#define DEFINE_ARRAY_JAVA_TYPE(type) \
        class _##type##Array : public _jobjectArray {};\
        typedef _##type##Array * type##Array;\
        HANDLE_OBJECT_ARRAY_JAVA_TYPE(type)

namespace smjni
{
    template<typename Dest, typename Source>
    struct java_cast
    {
        Dest operator()(Source src)
        {
            static_assert(std::is_convertible<typename std::remove_pointer<Source>::type, 
                                              typename std::remove_pointer<jobject>::type>::value, 
                          "Not a Java type");
            static_assert(std::is_convertible<typename std::remove_pointer<Source>::type, 
                                              typename std::remove_pointer<Dest>::type>::value, 
                          "Java types are not compatible");
            return src;
        }

    };

    template<typename Dest>
    struct java_cast<Dest, jobject>
    {
        Dest operator()(jobject src)
        {
            static_assert(std::is_convertible<typename std::remove_pointer<Dest>::type, 
                                              typename std::remove_pointer<jobject>::type>::value, 
                          "Not a Java type");
            return static_cast<Dest>(src);
        }
    };

    template<typename Dest, typename Source>
    inline
    Dest jstatic_cast(Source src)
    {
        return java_cast<Dest, Source>()(src);
    }
}

#define DEFINE_JAVA_CONVERSION(T1, T2) \
    namespace smjni\
    {\
        template<> \
        struct java_cast<T1, T2> \
        {\
            T1 operator()(T2 src)\
            {\
                return static_cast<T1>(static_cast<jobject>(src));\
            }\
        };\
        template<> \
        struct java_cast<T2, T1> \
        {\
            T2 operator()(T1 src)\
            {\
                return static_cast<T2>(static_cast<jobject>(src));\
            }\
        };\
    }

#endif //HEADER_JAVA_TYPE_TRAITS_H_INCLUDED
