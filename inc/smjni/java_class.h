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

#ifndef HEADER_JAVA_CLASS_H_INCLUDED
#define	HEADER_JAVA_CLASS_H_INCLUDED

#include <memory>
#include <mutex>

#include <smjni/java_ref.h>
#include <smjni/java_type_traits.h>

namespace smjni
{
    class java_class_holder 
    { 
    public:
        java_class_holder(jclass clazz):
            m_class(jglobal_ref(clazz))
        {}
            
        jclass c_ptr() const 
        {
            return m_class.c_ptr();
        }

        bool is_instance_of(JNIEnv * jenv, jobject obj) const
        {
            return jenv->IsInstanceOf(obj, c_ptr());
        }
    private:
        static bool find(JNIEnv * jenv, std::string name)
        {
            for(char & c: name) 
                if (c == '.') c ='/';
            return jenv->FindClass(name.c_str());
        }
        
    private:
        global_java_ref<jclass> m_class;
    };
    
    template<typename T>
    class java_class 
    { 
    template<typename ReturnType, typename ThisType, typename... ArgType> friend class java_method;
    template<typename ReturnType, typename... ArgType> friend class java_static_method;
    template<typename ReturnType, typename... ArgType> friend class java_constructor;
    template<typename Type> friend class java_static_field;
    public:
        java_class(jclass clazz):
            m_holder(init(clazz))
        {}
            
        java_class(JNIEnv * jenv, std::function<jclass (JNIEnv *)> loader):
            m_holder(init(jenv, loader))
        {}
        
        static jclass find(JNIEnv * jenv)
        {
            std::string name = java_type_traits<T>::class_name();
            for(char & c: name) 
                if (c == '.') c ='/';
            return jenv->FindClass(name.c_str());
        }

        jclass c_ptr() const
        {
            return m_holder->c_ptr();
        }
        
        bool is_instance_of(JNIEnv * jenv, jobject obj) const
        {
            return m_holder->is_instance_of(jenv, obj);
        }
    private:
        static std::shared_ptr<java_class_holder> init(JNIEnv * jenv, std::function<jclass (JNIEnv *)> loader)
        {
            std::lock_guard<std::mutex> lock(s_holder_mutex);
            if (!s_holder)
            {
                jclass clazz = loader(jenv);
                s_holder = std::make_shared<java_class_holder>(clazz);
            }
            return s_holder;
        }
        static std::shared_ptr<java_class_holder> init(jclass clazz)
        {
            std::lock_guard<std::mutex> lock(s_holder_mutex);
            if (!s_holder)
            {
                s_holder = std::make_shared<java_class_holder>(clazz);
            }
            return s_holder;
        }
        
        const std::shared_ptr<java_class_holder> & holder() const
        {
            return m_holder;
        }
    private:
        const std::shared_ptr<java_class_holder> m_holder;
        
        static std::mutex s_holder_mutex;
        static std::shared_ptr<java_class_holder> s_holder;
    };
    template<typename T>
    std::mutex java_class<T>::s_holder_mutex;
    template<typename T>
    std::shared_ptr<java_class_holder> java_class<T>::s_holder;
}

#endif	//HEADER_JAVA_CLASS_H_INCLUDED


