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

#ifndef HEADER_JAVA_METHOD_H_INCLUDED
#define	HEADER_JAVA_METHOD_H_INCLUDED

#include <smjni/java_type_traits.h>
#include <smjni/java_class.h>
#include <smjni/java_exception.h>
#include <smjni/ct_string.h>

namespace smjni
{
    class java_method_core
    {
    protected:
        java_method_core() noexcept : m_id(0)
        {
        }
        java_method_core(jmethodID id) noexcept: m_id(id)
        {
        }
        ~java_method_core() noexcept = default;
        java_method_core(const java_method_core &) noexcept = default;
        java_method_core(java_method_core &&) noexcept = default;
        java_method_core & operator=(const java_method_core &) noexcept = default;
        java_method_core & operator=(java_method_core &&) noexcept = default;
        
        static jmethodID get_method_id(JNIEnv * jenv, jclass clazz, const char * name, const char * signature);
        static jmethodID get_static_method_id(JNIEnv * jenv, jclass clazz, const char * name, const char * signature);
        
    protected:
        jmethodID m_id;
    };
    
    template<typename ReturnType, typename... ArgType>
    class java_method_base : public java_method_core
    {
    public:
        static const char * get_signature()
        {
            return internal::java_method_signature<ReturnType, ArgType...>();
        }
    protected:
        java_method_base(jmethodID id):
            java_method_core(id)
        {}
        ~java_method_base() noexcept = default;
        java_method_base() noexcept = default;
        java_method_base(const java_method_base &) noexcept = default;
        java_method_base(java_method_base &&) noexcept = default;
        java_method_base & operator=(const java_method_base &) noexcept = default;
        java_method_base & operator=(java_method_base &&) noexcept = default;
    };
    
    template<typename ReturnType, typename ThisType, typename... ArgType>
    class java_method : public java_method_base<ReturnType, ArgType...>
    {
    private:
        typedef java_method_base<ReturnType, ArgType...> super;
        typedef java_type_traits<ReturnType> traits;
        typedef typename traits::return_type return_type;
    public:
        java_method() = default;
        java_method(JNIEnv * jenv, const java_class<ThisType> & clazz, const char* name):
            super(super::get_method_id(jenv, clazz.c_ptr(), name, super::get_signature()))
        {
        }
        
        return_type operator()(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object, 
                               typename java_type_traits<ArgType>::arg_type... params) const
        {
            ReturnType ret = traits::call_method(jenv, argument_to_java(object), this->m_id, argument_to_java(params)...);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
        
        template<typename ClassType>
        return_type call_non_virtual(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object, const java_class<ClassType> & clazz, 
                                     typename java_type_traits<ArgType>::arg_type... params) const
        {
            ReturnType ret = traits::call_non_virtual_method(jenv, argument_to_java(object), 
                                                             clazz.c_ptr(), this->m_id,  
                                                             argument_to_java(params)...);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
    };
    
    template<typename ThisType, typename... ArgType>
    class java_method<void, ThisType, ArgType...> : public java_method_base<void, ArgType...>
    {
    private:
        typedef java_method_base<void, ArgType...> super;
        typedef java_type_traits<void> traits;
        typedef typename traits::return_type return_type;
    public:
        java_method() = default;
        java_method(JNIEnv * jenv, const java_class<ThisType> & clazz, const char* name):
            super(super::get_method_id(jenv, clazz.c_ptr(), name, super::get_signature()))
        {
        }
        
        void operator()(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object, 
                        typename java_type_traits<ArgType>::arg_type... params) const
        {
            traits::call_method(jenv, argument_to_java(object), this->m_id, argument_to_java(params)...);
            java_exception::check(jenv);
        }
        
        template<typename ClassType>
        void call_non_virtual(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object, const java_class<ClassType> & clazz, 
                              typename java_type_traits<ArgType>::arg_type... params) const
        {
            traits::call_non_virtual_method(jenv, argument_to_java(object), clazz.c_ptr(), this->m_id, argument_to_java(params)...);
            java_exception::check(jenv);
        }
    };
    
    template<typename ReturnType, typename... ArgType>
    class java_static_method : public java_method_base<ReturnType, ArgType...>
    {
    private:
        typedef java_method_base<ReturnType, ArgType...> super;
        typedef java_type_traits<ReturnType> traits;
        typedef typename traits::return_type return_type;
    public:
        java_static_method() = default;
        template<typename ClassType>
        java_static_method(JNIEnv * jenv, const java_class<ClassType> & clazz, const char* name):
            super(super::get_static_method_id(jenv, clazz.c_ptr(), name, super::get_signature())),
            m_holder(clazz.holder())
        {
        }
        
        return_type operator()(JNIEnv * jenv, typename java_type_traits<ArgType>::arg_type... params) const
        {
            ReturnType ret = traits::call_static_method(jenv, this->m_holder->c_ptr(), this->m_id, argument_to_java(params)...);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
    private:
        std::shared_ptr<java_class_holder> m_holder;
    };
    
    template<typename... ArgType>
    class java_static_method<void, ArgType...> : public java_method_base<void, ArgType...>
    {
    private:
        typedef java_method_base<void, ArgType...> super;
        typedef java_type_traits<void> traits;
        typedef typename traits::return_type return_type;
    public:
        java_static_method() = default;
        template<typename ClassType>
        java_static_method(JNIEnv * jenv, const java_class<ClassType> & clazz, const char* name):
            super(super::get_static_method_id(jenv, clazz.c_ptr(), name, super::get_signature())),
            m_holder(clazz.holder())
        {
        }
        
        void operator()(JNIEnv * jenv, typename java_type_traits<ArgType>::arg_type... params) const
        {
            traits::call_static_method(jenv, this->m_holder->c_ptr(), this->m_id, argument_to_java(params)...);
            java_exception::check(jenv);
        }
    private:
        std::shared_ptr<java_class_holder> m_holder;
    };
    
    template<typename ReturnType, typename... ArgType>
    class java_constructor : public java_method_base<void, ArgType...>
    {
    private:
        typedef java_method_base<void, ArgType...> super;
        typedef java_type_traits<ReturnType> traits;
        typedef typename traits::return_type return_type;
    public:
        java_constructor() = default;
        java_constructor(JNIEnv * jenv, const java_class<ReturnType> & clazz):
            super(super::get_method_id(jenv, clazz.c_ptr(), "<init>", super::get_signature())),
            m_holder(clazz.holder())
        {
        }
        
        return_type operator()(JNIEnv * jenv, typename java_type_traits<ArgType>::arg_type... params) const
        {
            ReturnType ret = traits::new_object(jenv, this->m_holder->c_ptr(), this->m_id, argument_to_java(params)...);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
    private:
        std::shared_ptr<java_class_holder> m_holder;
    };
    
    
}

#endif	//HEADER_JAVA_METHOD_H_INCLUDED


