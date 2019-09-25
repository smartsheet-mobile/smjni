/*
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
package smjni.jnigen

internal class UniqueName(original: String, index: Int) {

    private val value: String = original + if (index > 0) "$index" else ""

    override fun toString(): String {
        return value
    }


}

internal class NameTable {

    private val nameCounts: MutableMap<String, Int> = HashMap()

    init {
        //C++ keywords that are not keywords in Java
        arrayOf("alignas",
        "alignof",
        "and",
        "and_eq",
        "asm",
        "atomic_cancel",
        "atomic_commit",
        "atomic_noexcept",
        "auto",
        "bitand",
        "bitor",
        "bool",
        "char16_t",
        "char32_t",
        "compl",
        "concept",
        "constexpr",
        "const_cast",
        "co_await",
        "co_return",
        "co_yield",
        "decltype",
        "delete",
        "dynamic_cast",
        "explicit",
        "export",
        "extern",
        "float",
        "friend",
        "inline",
        "module",
        "mutable",
        "namespace",
        "noexcept",
        "not",
        "not_eq",
        "nullptr",
        "operator",
        "or",
        "or_eq",
        "register",
        "reinterpret_cast",
        "requires",
        "signed",
        "sizeof",
        "static_assert",
        "static_cast",
        "struct",
        "switch",
        "template",
        "thread_local",
        "typedef",
        "typeid",
        "typename",
        "union",
        "unsigned",
        "using",
        "virtual",
        "wchar_t",
        "xor",
        "xor_eq").forEach { nameCounts[it] = 1 }
    }

    internal fun allocateName(name: String): UniqueName {

        val count = nameCounts[name] ?: 0
        nameCounts[name] = count + 1
        return UniqueName(name, count)
    }

}