/*
 Copyright 2014 Smartsheet Inc.
 Copyright 2019 SmJNI Contributors
 
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

#ifndef HEADER_HAVA_RUNTIME_H_INCLUDED
#define HEADER_HAVA_RUNTIME_H_INCLUDED

#include <smjni/java_class.h>
#include <smjni/java_method.h>
#include <smjni/java_field.h>
#include <smjni/java_exception.h>

namespace smjni
{
    class java_runtime final
    {
    private:
        template<typename T>
        class core_class : public java_class<T>
        {
        public:
            core_class(JNIEnv * env):
                java_class<T>(env, java_runtime::get_core_class<T>)
            {}       
        };
    public:
        template<typename T>
        class simple_java_class : public java_class<T>
        {
        public:
            simple_java_class(JNIEnv * env):
                java_class<T>(env, [] (JNIEnv * env) { return java_runtime::get_class<T>(env); })
            {}   
        };

        class object_class final : public core_class<jobject>
        {
        public:
            object_class(JNIEnv * env):
                core_class(env),
                toString(env, *this, "toString")
            {}   
                
            const java_method<jstring, jobject> toString;
        };
        class throwable_class final : public core_class<jthrowable>
        {
        public:
            throwable_class(JNIEnv * env):
                core_class(env),
                ctor(env, *this) 
            {}   
                
            const smjni::java_constructor<jthrowable, jstring> ctor; 
        };
    public:
        static void init(JNIEnv * env);
        static void term();
        
        static const object_class & object()
        {
           return s_instance->m_object; 
        }
        static const throwable_class & throwable()
        {
           return s_instance->m_throwable; 
        }
        
        template<typename T> 
        static local_java_ref<jclass> get_class(JNIEnv * env)
        {
            auto ret = do_find<T>(env);
            if (!ret)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("failed to locate %s", java_type_traits<T>::class_name());
            }
            return ret;
        }

        template<typename T> 
        static local_java_ref<jclass> find_class(JNIEnv * env)
        {
            auto ret = do_find<T>(env);
            if (!ret)
                env->ExceptionClear();
            return ret;
        }
        
        
    private:
        java_runtime(JNIEnv * jenv);
            
        template<typename T> 
        static local_java_ref<jclass> get_core_class(JNIEnv * env)
        {
            auto ret = do_find<T>(env);
            if (!ret)
            {
                env->ExceptionClear();
                THROW_JAVA_PROBLEM("failed to locate %s", java_type_traits<T>::class_name());
            }
            return ret;
        }  

        template<typename T> 
        static local_java_ref<jclass> do_find(JNIEnv * jenv)
        {
            std::string name = java_type_traits<T>::class_name();
            for(char & c: name) 
                if (c == '.') c ='/';
            return jattach(jenv, jenv->FindClass(name.c_str()));
        }
    private:
        const object_class m_object;
        const throwable_class m_throwable;
        
        static java_runtime * s_instance;
    };
    
    
}

#endif //HEADER_HAVA_RUNTIME_H_INCLUDED
