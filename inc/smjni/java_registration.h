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

#ifndef HEADER_JAVA_REGISTRATION_H_INCLUDED
#define	HEADER_JAVA_REGISTRATION_H_INCLUDED

#include <vector>

#include <smjni/java_method.h>
#include <smjni/java_class.h>

namespace smjni
{
    class java_registration_base
    {
    private:
        typedef std::tuple<std::string, std::string, void*> entry;
    protected:
        void add_method(const char * name, const char * signature, void * func)
        {
            m_entries.emplace_back(entry(name, signature, func));
        }
        
        void perform(JNIEnv * jenv, jclass clazz) const;
    private:
        std::vector<entry> m_entries;
    };
    
    template<typename T>
    class [[deprecated("Please use registration facilities from java_class")]] java_registration : private java_registration_base
    {
    public:
        template<typename ReturnType, typename... ArgType>
        void add_static_method(const char * name, ReturnType (JNICALL *func)(JNIEnv *, jclass, ArgType...))
        {
            const char * signature = java_method_base<ReturnType, ArgType...>::get_signature();
            java_registration_base::add_method(name, signature, reinterpret_cast<void*>(func));
        }
        
        template<typename ReturnType, typename... ArgType>
        void add_instance_method(const char * name, ReturnType (JNICALL *func)(JNIEnv *, T, ArgType...))
        {
            const char * signature = java_method_base<ReturnType, ArgType...>::get_signature();
            java_registration_base::add_method(name, signature, reinterpret_cast<void*>(func));
        }
        
        void perform(JNIEnv * jenv, const java_class<T> & clazz) const
        {
            java_registration_base::perform(jenv, clazz.c_ptr());
        }
    };

    
}

#endif	//HEADER_JAVA_REGISTRATION_H_INCLUDED

