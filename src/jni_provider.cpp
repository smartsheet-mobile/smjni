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

#include "stdpch.h"

#include <pthread.h>

#include <smjni/jni_provider.h>
#include <smjni/java_externals.h>

using namespace smjni;

namespace smjni
{
    namespace internal
    {
        class jni_record
        {
        public:
            jni_record();
            ~jni_record();

            JNIEnv * env() const
                { return m_env; }
        private:
            JNIEnv * m_env;
            bool m_attached;
        };

        class jni_provider_tls
        {
        public:
            jni_provider_tls():
                m_key{0}
            {
                int err = pthread_key_create(&m_key, [] (void * value) {
                    delete(static_cast<jni_record*>(value));
                });
                if (err != 0)
                    THROW_JAVA_PROBLEM("cannot create thread local key");
            }
            ~jni_provider_tls() noexcept
            {
                pthread_key_delete(m_key);
            }

            jni_record & get() const
            {
                jni_record * ptr = static_cast<jni_record *>(pthread_getspecific(m_key));
                if (!ptr)
                {
                    ptr = new jni_record;
                    do_set(ptr);
                }
                return *ptr;
            }
        private:
            void do_set(jni_record * new_ptr) const
            {
                int err = pthread_setspecific(m_key, new_ptr);
                if (err != 0)
                {
                    delete new_ptr;
                    THROW_JAVA_PROBLEM("cannot set thread local value");
                }
            }
        private:
            pthread_key_t m_key;
        };

        jni_provider * g_provider = nullptr;
    }
}

jni_provider::jni_provider(JavaVM * vm) :
    m_vm(vm),
    m_tls(new internal::jni_provider_tls)
{}

jni_provider::~jni_provider()
{}

void jni_provider::init(JNIEnv * initialEnv)
{
    if (internal::g_provider)
        return;
    
    JavaVM * vm = nullptr;
    jint res = initialEnv->GetJavaVM(&vm);
    if (res != 0)
        THROW_JAVA_PROBLEM("failed to obtain Java VM, error %d", res);
    internal::g_provider = new jni_provider(vm);
}

void jni_provider::init(JavaVM * vm)
{
    if (internal::g_provider)
        return;
    
    internal::g_provider = new jni_provider(vm);
}

JNIEnv * jni_provider::get_jni()
{ 
    return internal::g_provider->m_tls->get().env();
}

template<typename T>
T ** AttachCurrentThreadAsDaemonOutputTypeDetector(jint (JavaVM::*)(T **, void *));

internal::jni_record::jni_record():
    m_env(nullptr),
    m_attached(false)
{ 
    JavaVM * vm = g_provider->vm();

    jint get_res = vm->GetEnv(reinterpret_cast<void**>(&m_env), JNI_VERSION_1_6);
    if (get_res != JNI_OK)
    {
        JavaVMAttachArgs args = {
            JNI_VERSION_1_6,
            nullptr,
            nullptr
        };

        typedef decltype(AttachCurrentThreadAsDaemonOutputTypeDetector(&JavaVM::AttachCurrentThreadAsDaemon)) env_ret_type;
        jint attach_res = vm->AttachCurrentThreadAsDaemon((env_ret_type)&m_env, &args);
        if (attach_res != JNI_OK)
            THROW_JAVA_PROBLEM("failed to obtain JNIEnv, error %d and failed to attach Java VM to current thread, error %d", get_res, attach_res);
        m_attached = true;
    } 
}

internal::jni_record::~jni_record()
{
    if (m_attached && g_provider)
    {
        g_provider->vm()->DetachCurrentThread();
    }
}