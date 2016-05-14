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

#ifndef HEADER_JAVA_EXCEPTION_H_INCLUDED
#define HEADER_JAVA_EXCEPTION_H_INCLUDED

#include <string>

#include <smjni/java_ref.h>

namespace smjni
{

    class java_exception final : public std::exception
    {
    public:
        java_exception(jthrowable ex) noexcept :
            m_throwable(jglobal_ref(ex))
        {
            
        }
        ~java_exception() noexcept
        {
            
        }
        
        const char * what() const noexcept override;
        
        jthrowable throwable() const
            { return m_throwable.c_ptr(); }
        
        void raise(JNIEnv * jenv) const
        {
            raise(jenv, m_throwable.c_ptr());
        }
        
        static void check(JNIEnv * jenv)
        {
            jthrowable ex = jenv->ExceptionOccurred();
            if (ex)
            {
                jenv->ExceptionClear();
                throw java_exception(ex);
            }
        }
        
        static void raise(JNIEnv * jenv, jthrowable jex)
        {
            jint res = jenv->Throw(jex);
            if (res != 0)
                THROW_JAVA_PROBLEM("Cannot throw java exception");
        }
        
        static void translate(JNIEnv * env, const std::exception & ex);
        
    protected:
        std::string do_what() const;
        
    private:
        global_java_ref<jthrowable> m_throwable;
        mutable std::string m_what;
    };
    
    
}



#endif //HEADER_JAVA_EXCEPTION_H_INCLUDED
