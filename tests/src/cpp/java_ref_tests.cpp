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

#include <type_traits>

#include <iostream>

using namespace smjni;

struct TypeListEnd;

template<class H, class... Rest>
struct TypeList
{
    using Head = H;
    using Tail = TypeList<Rest...>;
};


template<class H>
struct TypeList<H>
{
    using Head = H;
    using Tail = TypeListEnd;
};


template<class TL, class Func>
void ForEach(Func func)
{
    if constexpr (std::is_same_v<TL, TypeListEnd>)
    {
        return;
    }
    else
    {
        func.template operator()<typename TL::Head>();
        ForEach<typename TL::Tail>(func);
    }
}

void JNICALL TestJavaRef::test(JNIEnv * env, jTestJavaRef)
{
    NATIVE_PROLOG

        using RefTypes = TypeList<auto_java_ref<jobject>, local_java_ref<jobject>, global_java_ref<jobject>, weak_java_ref<jobject>>;

        ForEach<RefTypes>([env] <class T>() {

            ASSERT_TRUE(std::is_default_constructible_v<T>);
            ASSERT_TRUE(std::is_nothrow_default_constructible_v<T>);

            ASSERT_TRUE((std::is_constructible_v<T, std::nullptr_t>));
            ASSERT_TRUE((std::is_nothrow_constructible_v<T, std::nullptr_t>));

            ASSERT_TRUE(std::is_copy_constructible_v<T>);
            ASSERT_TRUE(std::is_nothrow_move_constructible_v<T>);

            ASSERT_TRUE(std::is_copy_assignable_v<T>);
            ASSERT_TRUE(std::is_nothrow_move_assignable_v<T>);

            ASSERT_TRUE(std::is_nothrow_destructible_v<T>);
            ASSERT_TRUE(std::is_nothrow_swappable_v<T>);


        });

        ASSERT_TRUE((std::is_nothrow_constructible_v<auto_java_ref<jobject>, jobject>));
        ASSERT_FALSE((std::is_constructible_v<auto_java_ref<jobject>, int *>));
        ASSERT_FALSE((std::is_constructible_v<local_java_ref<jobject>, jobject>));
        ASSERT_FALSE((std::is_constructible_v<global_java_ref<jobject>, jobject>));
        ASSERT_FALSE((std::is_constructible_v<weak_java_ref<jobject>, jobject>));

        ForEach<RefTypes>([env] <class T>() {

            ForEach<RefTypes>([env] <class Y>() {

                ASSERT_TRUE((std::is_nothrow_constructible_v<T, Y>));
                ASSERT_TRUE((std::is_nothrow_constructible_v<T, Y&&>));
                ASSERT_TRUE((std::is_nothrow_constructible_v<T, const Y&>));
                ASSERT_TRUE((std::is_nothrow_constructible_v<T, Y&>));
                ASSERT_TRUE((std::is_nothrow_constructible_v<T, const Y&&>));
                //ASSERT_TRUE((std::is_nothrow_constructible_v<auto_java_ref<jobject>, auto_java_ref<jstring>>));
                //ASSERT_TRUE((std::is_nothrow_constructible_v<auto_java_ref<jobject>, auto_java_ref<jstring>&&>));

            });

        });


    NATIVE_EPILOG
}