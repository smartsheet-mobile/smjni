/*
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

#include <smjni/smjni.h>

#include "test_util.h"

using namespace smjni;


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    try
    {
        jni_provider::init(vm);
        JNIEnv * env = jni_provider::get_jni();
        java_runtime::init(env);

        NATIVE_PROLOG
            java_classes::init(env);

            return JNI_VERSION_1_6;
        NATIVE_EPILOG
    }
    catch(std::exception & ex)
    {
        //If we are here there is no way to communicate with
        //Java - something really bad happened.
        //Let's just log and report failure
        fprintf(stderr, "%s\n", ex.what());

    }
    return 0;
}

jboolean JNICALL TestSmJNI::doTestNativeMethodImplementation(JNIEnv * env, jTestSmJNI, jboolean bl, jbyte b, jchar c, jshort s, jint i, jlong l, jfloat f, jdouble d, jstring str,
                                     jbooleanArray bla, jbyteArray ba, jcharArray ca, jshortArray sa, jintArray ia, jlongArray la, jfloatArray fa, jdoubleArray da, jstringArray stra)
{
    NATIVE_PROLOG
        ASSERT_TRUE(bl);
        ASSERT_EQUAL(42, b);
        ASSERT_EQUAL(u'q', c);
        ASSERT_EQUAL(17, s);
        ASSERT_EQUAL(64, i);
        ASSERT_EQUAL(59, l);
        ASSERT_EQUAL(0.42f, f);
        ASSERT_EQUAL(0.756, d);

        auto cppstr = java_string_to_cpp(env, str);
        ASSERT_EQUAL("hello👶🏻", cppstr);

        {
            java_array_access access(env, bla);
            auto expected = { java_true, java_false };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, ba);
            auto expected = { 3, 4 };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, ca);
            auto expected = { u'm', u'p' };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, sa);
            auto expected = { 9, 10 };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, ia);
            auto expected = { 545, 212 };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, la);
            auto expected = { -1, -3 };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, fa);
            auto expected = { 0.1f, 0.2f };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, da);
            auto expected = { 0.25, 0.26 };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));
        }

        {
            java_array_access access(env, stra);
            auto expected = { "abc" , "xyz" };
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end(),
                                   [env] (const auto & lhs, const auto & rhs) {
                return rhs == java_string_to_cpp(env, local_java_ref<jstring>(lhs));
            }));
        }

        return java_true;

    NATIVE_EPILOG
    return java_false;
}

void JNICALL TestSmJNI::testString(JNIEnv * env, jTestSmJNI self)
{
    NATIVE_PROLOG
        jchar chars[] = {u'h', u'e', u'l', u'l', u'o'};
        auto str1 = java_string_create(env, chars, size_to_java(std::size(chars)));
        ASSERT_EQUAL(5, java_string_get_length(env, str1));
        ASSERT_EQUAL("hello", java_string_to_cpp(env, str1));
        auto str2 = java_string_create(env, "hello");
        ASSERT_EQUAL(5, java_string_get_length(env, str2));
        ASSERT_EQUAL("hello", java_string_to_cpp(env, str2));
        auto str3 = java_string_create(env, std::string("hello"));
        ASSERT_EQUAL(5, java_string_get_length(env, str3));
        ASSERT_EQUAL("hello", java_string_to_cpp(env, str3));

        auto empty = java_string_create(env, nullptr);
        ASSERT_EQUAL(0, java_string_get_length(env, empty));
        ASSERT_EQUAL("", java_string_to_cpp(env, empty));

        ASSERT_EQUAL(0, java_string_get_length(env, nullptr));
        ASSERT_EQUAL("", java_string_to_cpp(env, nullptr));

        jchar buf[5] = {};
        java_string_get_region(env, str1, 1, 2, buf);
        ASSERT_EQUAL(u'e', buf[0]);
        ASSERT_EQUAL(u'l', buf[1]);
        ASSERT_EQUAL(0, buf[2]);

        java_string_access access(env, str2);
        ASSERT_EQUAL(5, access.size());
        for(int i = 0; i < 5; ++i)
            ASSERT_EQUAL(chars[i], access[i]);
        ASSERT_TRUE(std::equal(access.begin(), access.end(), std::begin(chars), std::end(chars)));

        java_string_access empty_access(env, empty);
        ASSERT_EQUAL(0, empty_access.size());
        ASSERT_TRUE(empty_access.begin() == empty_access.end());

        java_string_access null_access(env, nullptr);
        ASSERT_EQUAL(0, null_access.size());
        ASSERT_TRUE(null_access.begin() == null_access.end());
    NATIVE_EPILOG
}

jcharArray JNICALL TestSmJNI::doTestPrimitiveArray(JNIEnv * env, jTestSmJNI self, jintArray array)
{
    NATIVE_PROLOG

        {
            java_array_access access(env, array);
            ASSERT_EQUAL(5, access.size());
            auto expected = {1, 2, 3, 4, 5};
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));

            access[1] = 72; //this should be aborted
        }
        {
            java_array_access access(env, array);
            auto expected = {1, 2, 3, 4, 5};
            ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end()));

            std::reverse(access.begin(), access.end());
            access.commit(false);

            ASSERT_TRUE(access.begin() != nullptr);

            access.commit();
            ASSERT_TRUE(access.begin() == nullptr);
        }
        {
            java_array_access<jbyteArray> access(env, nullptr);
            ASSERT_EQUAL(0, access.size());
            ASSERT_TRUE(access.begin() == access.end());
        }

        std::vector<jchar> res = {u'a', u'b'};
        auto ret = java_array_create<jchar>(env, res.begin(), res.end());
        return ret.release();
    NATIVE_EPILOG
    return nullptr;
}

jstringArray JNICALL TestSmJNI::doTestObjectArray(JNIEnv * env, jTestSmJNI self, jstringArray array)
{
    NATIVE_PROLOG

       java_array_access access(env, array);
       ASSERT_EQUAL(5, access.size());
       auto expected = {"a", "b", "c", "d", "e"};
       ASSERT_TRUE(std::equal(access.begin(), access.end(), expected.begin(), expected.end(),
                               [env] (const auto & lhs, const auto & rhs) {
            return rhs == java_string_to_cpp(env, local_java_ref<jstring>(lhs));
        }));

       std::reverse(access.begin(), access.end());
       auto string_class = java_class<jstring>(env, [] (JNIEnv * env) { return java_runtime::get_class<jstring>(env); });
       auto ret = java_array_create(env, string_class, 2, java_string_create(env, "a"));

       return ret.release();

    NATIVE_EPILOG
    return nullptr;
}

jByteBuffer JNICALL TestSmJNI::doTestDirectBuffer(JNIEnv * env, jTestSmJNI self, jByteBuffer buffer)
{
    NATIVE_PROLOG

        java_direct_buffer<uint8_t> buf(env, buffer);
        auto expected = {1, 2, 3, 4, 5};
        ASSERT_EQUAL(5, buf.size());
        for(int i = 0; i < 5; ++i)
            ASSERT_EQUAL(i + 1, buf[i]);
        ASSERT_TRUE(std::equal(buf.begin(), buf.end(), expected.begin(), expected.end()));
        std::reverse(buf.begin(), buf.end());

        static uint8_t bytes[] = { 1, 2 }; //this must exist forever
        java_direct_buffer<uint8_t> ret(bytes, size_to_java(std::size(bytes)));
        return ret.to_java(env).release();

    NATIVE_EPILOG
    return nullptr;
}

void JNICALL TestSmJNI::testCallingJava(JNIEnv * env, jTestSmJNI self)
{
    NATIVE_PROLOG
        auto derived_class = java_classes::get<Derived>();
        auto base_class = java_classes::get<Base>();

        auto derived = derived_class.ctor(env, 42);

        ASSERT_EQUAL(42, base_class.get_value(env, derived));
        base_class.set_value(env, derived, -42);
        ASSERT_EQUAL(-42, base_class.get_value(env, derived));

        ASSERT_EQUAL(15, base_class.get_staticValue(env));
        base_class.set_staticValue(env, -15);
        ASSERT_EQUAL(-15, base_class.get_staticValue(env));

        ASSERT_EQUAL(74, base_class.staticMethod(env, 74));

        ASSERT_EQUAL(5, base_class.instanceMethod(env, derived, 3));

        ASSERT_EQUAL(4, base_class.instanceMethod(env, derived, base_class, 3));
    NATIVE_EPILOG
}