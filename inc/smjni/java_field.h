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

#ifndef HEADER_JAVA_FIELD_H_INCLUDED
#define HEADER_JAVA_FIELD_H_INCLUDED

#include <smjni/java_type_traits.h>
#include <smjni/java_class.h>
#include <smjni/java_exception.h>

namespace smjni
{
    class java_field_core
    {
    protected:
        java_field_core() noexcept : m_id(0)
        {
        }
        java_field_core(jfieldID id) noexcept: m_id(id)
        {
        }
        
        java_field_core(const java_field_core &) noexcept = default;
        java_field_core(java_field_core &&) noexcept = default;
        java_field_core & operator=(const java_field_core &) noexcept = default;
        java_field_core & operator=(java_field_core &&) noexcept = default;
    
        static jfieldID get_field_id(JNIEnv * jenv, jclass clazz, const char * name, const std::string & signature);
        static jfieldID get_static_field_id(JNIEnv * jenv, jclass clazz, const char * name, const std::string & signature);
        
    protected:
        jfieldID m_id;
    };
    
    template<typename Type>
    class java_field_base : public java_field_core
    {
    public:
        typedef Type field_type;
        typedef typename java_type_traits<Type>::return_type return_type;
    public:
        static std::string get_signature()
        {
            return java_type_traits<Type>::signature();
        }
        
    protected:
        java_field_base(jfieldID id):
            java_field_core(id)
        {}
        java_field_base() noexcept = default;
        java_field_base(const java_field_base &) noexcept = default;
        java_field_base(java_field_base &&) noexcept = default;
        java_field_base & operator=(const java_field_base &) noexcept = default;
        java_field_base & operator=(java_field_base &&) noexcept = default;
        
    };
    
    template<typename Type, typename ThisType>
    class java_field : public java_field_base<Type>
    {
    private:
        typedef java_field_base<Type> super;
        typedef java_type_traits<Type> traits;
        typedef typename super::return_type return_type;
    public:
        java_field() = default;
        java_field(JNIEnv * jenv, java_class<ThisType> clazz, const char* name):
            super(super::get_field_id(jenv, clazz.c_ptr(), name, super::get_signature()))
        {
        }
        
        return_type get(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object) const
        {
            Type ret = traits::get_field(jenv, argument_to_java(object), this->m_id);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
        
        void set(JNIEnv * jenv, typename java_type_traits<ThisType>::arg_type object, typename java_type_traits<Type>::arg_type val) const
        {
            traits::set_field(jenv, argument_to_java(object), this->m_id, argument_to_java(val));
            java_exception::check(jenv);
        }
    };
    
    template<typename Type>
    class java_static_field : public java_field_base<Type>
    {
    private:
        typedef java_field_base<Type> super;
        typedef java_type_traits<Type> traits;
        typedef typename super::return_type return_type;
    public:
        java_static_field() = default;
        
        template<typename ClassType>
        java_static_field(JNIEnv * jenv, const java_class<ClassType> & clazz, const char* name):
            super(super::get_static_field_id(jenv, clazz.c_ptr(), name, super::get_signature())),
            m_holder(clazz.holder())
        {
        }
        
        
        return_type get(JNIEnv * jenv) const
        {
            Type ret = traits::get_static_field(jenv, this->m_holder->c_ptr(), this->m_id);
            if (!ret)
                java_exception::check(jenv);
            return return_value_from_java(jenv, ret);
        }
        
        void set(JNIEnv * jenv, typename java_type_traits<Type>::arg_type val) const
        {
            java_type_traits<Type>::set_static_field(jenv, this->m_holder->c_ptr(), this->m_id, argument_to_java(val));
            java_exception::check(jenv);
        }
    private:
        std::shared_ptr<java_class_holder> m_holder;
    };
}

#endif //HEADER_JAVA_FIELD_H_INCLUDED
