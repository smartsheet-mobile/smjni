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

#ifndef HEADER_JAVA_DIRECT_BUFFER_H_INCLUDED
#define HEADER_JAVA_DIRECT_BUFFER_H_INCLUDED

#include <smjni/java_ref.h>
#include <smjni/java_exception.h>


namespace smjni
{
    template<typename T>
    class java_direct_buffer
    {
    public:
        typedef T * iterator;
        typedef const T * const_iterator;
        typedef jsize size_type;
        typedef T value_type;
    public:
        java_direct_buffer(T * ptr, jsize size):
            m_ptr(ptr),
            m_size(size)  
        {
        }
            
        java_direct_buffer(JNIEnv * env, jobject obj)
        {
            m_ptr = static_cast<T *>(env->GetDirectBufferAddress(obj));
            if (!m_ptr)
            {
                java_exception::check(env); 
                THROW_JAVA_PROBLEM("invalid buffer");
            }
            jsize byte_size = env->GetDirectBufferCapacity(obj);
            if (byte_size == -1)
            {
                java_exception::check(env); 
                THROW_JAVA_PROBLEM("invalid buffer");
            }
            m_size = byte_size / sizeof(T);
        }
        
        local_java_ref<jobject> to_java(JNIEnv * env)
        {
            jobject ret = env->NewDirectByteBuffer(m_ptr, m_size * sizeof(T));
            if (!ret)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("cannot create java string");
            }
            return jattach(env, ret);
        } 
        
        const value_type * begin() const
        {
            return m_ptr;
        }
        value_type * begin()
        {
            return m_ptr;
        }
        const value_type * end() const
        {
            return m_ptr + m_size;
        }
        value_type * end()
        {
            return m_ptr + m_size;
        }
        jsize size() const
        {
            return m_size;
        }
        const value_type & operator[](jsize idx) const
        {
            return m_ptr[idx];
        }
        value_type & operator[](jsize idx) 
        {
            return m_ptr[idx];
        }
    private:
        T * m_ptr = nullptr;
        jsize m_size = 0;    
    };
}

#endif