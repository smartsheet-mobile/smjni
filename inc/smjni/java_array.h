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

#ifndef HEADER_JAVA_ARRAY_H_INCLUDED
#define HEADER_JAVA_ARRAY_H_INCLUDED

#include <smjni/jni_provider.h>
#include <smjni/java_ref.h>

namespace smjni
{
    template<typename T, bool IsObject = std::is_convertible<typename java_type_traits<T>::element_type, jobject>::value>
    class java_array;
    
    template<typename T>
    class java_array<T, true>
    {
    public:
        typedef typename java_type_traits<T>::element_type element_type;
        
        class access
        {
        public:
            class const_iterator;
            class iterator;
        private:
            
            class proxy
            {
                friend class const_iterator;
                friend class iterator;
                friend class access;
            public:
                operator local_java_ref<element_type>() const
                {
                    return jattach(m_parent->m_env, m_parent->get(m_idx));
                }
                void operator=(element_type el)
                {
                    m_parent->set(m_idx, el);
                }
                void operator=(const local_java_ref<element_type> & el)
                {
                    m_parent->set(m_idx, el.c_ptr());
                }
            private:
                proxy(): 
                    m_parent(nullptr),
                    m_idx(0)
                {}
                proxy(access & parent, jsize idx):
                    m_parent(&parent),
                    m_idx(idx)
                {}
                
            private:
                access * const m_parent;
                jsize m_idx;
            };
        public:
            class iterator 
            {
                friend class const_iterator;
                friend class access;
            public:
                typedef std::random_access_iterator_tag  iterator_category;
                typedef local_java_ref<element_type>  value_type;
                typedef std::make_signed_t<jsize>  difference_type;
                typedef void pointer;
                typedef proxy reference;
            public:
                iterator()
                {}
                
                proxy operator*() const
                {
                    return proxy(*m_parent, m_idx);
                }
                proxy operator[](jsize dist) const
                {
                    return proxy(*m_parent, m_idx + dist);
                }
                iterator & operator++()
                {
                    ++m_idx;
                    return *this;
                }
                iterator & operator--()
                {
                    --m_idx;
                    return *this;
                }
                iterator & operator+=(jsize dist)
                {
                    m_idx += dist;
                    return *this;
                }
                iterator & operator-=(jsize dist)
                {
                    m_idx -= dist;
                    return *this;
                }
                iterator operator++(int)
                {
                    const_iterator ret(*this);
                    ++m_idx;
                    return ret;
                }
                iterator operator--(int)
                {
                    const_iterator ret(*this);
                    --m_idx;
                    return ret;
                }
                iterator operator+(jsize dist) const
                {
                    return iterator(*m_parent, m_idx + dist);
                }
                iterator operator-(jsize dist) const
                {
                    return iterator(*m_parent, m_idx - dist);
                }
                jsize operator-(const iterator & rhs) const
                {
                    return m_idx - rhs.m_idx;
                }
                
                bool operator==(const iterator & rhs) const
                {
                    return m_idx == rhs.m_idx;
                }
                bool operator!=(const iterator & rhs) const
                {
                    return !(*this == rhs);
                }
            private:
                iterator(access & parent, jsize idx):
                    m_parent(&parent), 
                    m_idx(idx)
                {}
            private:
                access * const m_parent;
                jsize m_idx;
            };
            
            class const_iterator 
            {
                friend class access;
            public:
                typedef std::random_access_iterator_tag  iterator_category;
                typedef local_java_ref<element_type>  value_type;
                typedef std::make_signed_t<jsize>  difference_type;
                typedef void pointer;
                typedef local_java_ref<element_type> reference;
            public:
                const_iterator()
                {}
                const_iterator(const iterator & it):
                    m_parent(it.m_parent),
                    m_idx(it.m_idx)
                {}
                local_java_ref<element_type> operator*() const
                {
                    return jattach(m_parent->m_env, m_parent->get(m_idx));
                }
                local_java_ref<element_type> operator[](jsize dist) const
                {
                    return jattach(m_parent->m_env, m_parent->get(m_idx + dist));
                }
                const_iterator & operator++()
                {
                    ++m_idx;
                    return *this;
                }
                const_iterator & operator--()
                {
                    --m_idx;
                    return *this;
                }
                const_iterator & operator+=(jsize dist)
                {
                    m_idx += dist;
                    return *this;
                }
                const_iterator & operator-=(jsize dist)
                {
                    m_idx -= dist;
                    return *this;
                }
                const_iterator operator++(int)
                {
                    const_iterator ret(*this);
                    ++m_idx;
                    return ret;
                }
                const_iterator operator--(int)
                {
                    const_iterator ret(*this);
                    --m_idx;
                    return ret;
                }
                const_iterator operator+(jsize dist) const
                {
                    return const_iterator(m_parent, m_idx + dist);
                }
                const_iterator operator-(jsize dist) const
                {
                    return const_iterator(m_parent, m_idx - dist);
                }
                jsize operator-(const const_iterator & rhs) const
                {
                    return m_idx - rhs.m_idx;
                }
                bool operator==(const const_iterator & rhs) const
                {
                    return m_idx == rhs.m_idx;
                }
                bool operator!=(const const_iterator & rhs) const
                {
                    return !(*this == rhs);
                }
            private:
                const_iterator(const access & parent, jsize idx):
                    m_parent(&parent), 
                    m_idx(idx)
                {}
            private:
                const access * const m_parent;
                jsize m_idx;
            };
            
            typedef jsize size_type;
            typedef local_java_ref<element_type> value_type;
        public:
            access(JNIEnv * env, T array):
                m_env(env),
                m_array(array),
                m_length(m_env->GetArrayLength(m_array))
            {
                if (!m_array)
                {
                    java_exception::check(env);
                    THROW_JAVA_PROBLEM("cannot access java array");
                }
            }
            access(const access &) = delete;
            access & operator=(const access &) = delete;
            
            
            const_iterator begin() const
            {
                return const_iterator(*this, 0);
            }
            iterator begin()
            {
                return iterator(*this, 0);
            }
            const_iterator end() const
            {
                return const_iterator(*this, m_length);
            }
            iterator end()
            {
                return iterator(*this, m_length);
            }
            jsize size() const
            {
                return m_length;
            }
            local_java_ref<element_type> operator[](jsize idx) const
            {
                return jattach(m_env, get(idx));
            }
            proxy operator[](jsize idx) 
            {
                return proxy(*this, idx);
            }
        
        private:
            element_type get(jsize index) const
            {
                element_type ret = static_cast<element_type>(m_env->GetObjectArrayElement(m_array, index));
                if (!ret)
                    java_exception::check(m_env);
                return ret;
            }
            
            void set(jsize index, element_type value)
            {
                m_env->SetObjectArrayElement(m_array, index, value);
                java_exception::check(m_env);   
            }
        private:
            JNIEnv * const m_env;
            const T m_array;
            const jsize m_length;
        };
    public:
        java_array() = delete;
        java_array(const java_array & array) = delete;
        java_array & operator=(const java_array & src) = delete;
        
        static local_java_ref<T> create(JNIEnv * env, const java_class<element_type> & cls, jsize size, element_type initial_value)
        {
            T ret = static_cast<T>(env->NewObjectArray(size, cls.c_ptr(), initial_value));
            if (!ret)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("cannot create java array");
            }
            return jattach(env, ret);
        }      
    };
    
    template<typename T>
    class java_array<T, false>
    {
    public:
        typedef typename java_type_traits<T>::element_type element_type;
        
        class access
        {
        public:
            typedef element_type * iterator;
            typedef const element_type * const_iterator;
            typedef jsize size_type;
            typedef element_type value_type;
        public:
            access(JNIEnv * env, T array):
                m_array(array),
                m_length(env->GetArrayLength(m_array)),
                m_data(java_type_traits<T>::get_array_elements(env, m_array, nullptr))
            {
                if (!m_array)
                {
                    java_exception::check(env);
                    THROW_JAVA_PROBLEM("cannot access java array");
                }
            }
            access(const access &) = delete;
            access & operator=(const access &) = delete;
            ~access()
            {
                if (m_data)
                    java_type_traits<T>::release_array_elements(jni_provider::get_jni(), m_array, m_data, JNI_ABORT);
            }
            void commit(JNIEnv * env, bool done = true)
            {
                if (done)
                {
                    java_type_traits<T>::release_array_elements(env, m_array, m_data, 0);
                    m_data = nullptr;
                }
                else
                {
                    java_type_traits<T>::release_array_elements(env, m_array, m_data, JNI_COMMIT);
                }
            }
            
            const element_type * begin() const
            {
                return m_data;
            }
            element_type * begin()
            {
                return m_data;
            }
            const  element_type * end() const
            {
                return m_data + m_length;
            }
            element_type * end()
            {
                return m_data + m_length;
            }
            jsize size() const
            {
                return m_length;
            }
            const element_type & operator[](jsize idx) const
            {
                return m_data[idx];
            }
            element_type & operator[](jsize idx) 
            {
                return m_data[idx];
            }
        private:
            T m_array;
            jsize m_length;
            element_type * m_data;
        };
    public:
        java_array() = delete;
        java_array(const java_array & array) = delete;
        java_array & operator=(const java_array & src) = delete;
        
        static local_java_ref<T> create(JNIEnv * env, jsize size)
        {
            T array = java_type_traits<element_type>::new_array(env, size);
            if (!array)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("cannot create java array");
            }
            return jattach(env, array);
        }   
    };
}

#endif //HEADER_JAVA_ARRAY_H_INCLUDED
