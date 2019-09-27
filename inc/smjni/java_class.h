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
#include <smjni/java_exception.h>

namespace smjni
{
    class java_class_holder 
    { 
    public:
        java_class_holder(const auto_java_ref<jclass> & clazz):
            m_class(clazz)
        {}
            
        jclass c_ptr() const 
        {
            return m_class.c_ptr();
        }

        bool is_instance_of(JNIEnv * jenv, const auto_java_ref<jobject> & obj) const
        {
            return jenv->IsInstanceOf(obj.c_ptr(), c_ptr());
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
        java_class(const auto_java_ref<jclass> & clazz):
            m_holder(init(clazz))
        {}
            
        java_class(JNIEnv * jenv, std::function<local_java_ref<jclass> (JNIEnv *)> loader):
            m_holder(init(jenv, loader))
        {}
        
        jclass c_ptr() const
        {
            return m_holder->c_ptr();
        }
        
        bool is_instance_of(JNIEnv * jenv, const auto_java_ref<jobject> & obj) const
        {
            return m_holder->is_instance_of(jenv, obj);
        }
    private:
        static std::shared_ptr<java_class_holder> init(JNIEnv * jenv, std::function<local_java_ref<jclass> (JNIEnv *)> loader)
        {
            return do_init([loader, jenv] () -> global_java_ref<jclass> { return loader(jenv); });
        }
        static std::shared_ptr<java_class_holder> init(const auto_java_ref<jclass> & clazz)
        {
            return do_init([clazz] () -> global_java_ref<jclass> { return clazz; });
        }

        static std::shared_ptr<java_class_holder> do_init(std::function<global_java_ref<jclass> ()> loader)
        {
            std::lock_guard<std::mutex> lock(s_holder_mutex);
            auto ret = s_holder.lock();
            if (!ret)
            {
                auto clazz = loader();
                ret = std::make_shared<java_class_holder>(clazz);
                s_holder = ret;
            }
            return ret;
        }
        
        const std::shared_ptr<java_class_holder> & holder() const
        {
            return m_holder;
        }
    private:
        const std::shared_ptr<java_class_holder> m_holder;
        
        static std::mutex s_holder_mutex;
        static std::weak_ptr<java_class_holder> s_holder;
    };
    template<typename T>
    std::mutex java_class<T>::s_holder_mutex;
    template<typename T>
    std::weak_ptr<java_class_holder> java_class<T>::s_holder;
}

#endif	//HEADER_JAVA_CLASS_H_INCLUDED


