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

#ifndef HEADER_JAVA_STRING_INCLUDED
#define HEADER_JAVA_STRING_INCLUDED

#include <smjni/java_ref.h>
#include <smjni/java_exception.h>

namespace smjni
{
    class java_string
    {
    public:
        java_string() = delete;
        java_string(const java_string & str) = delete;
        java_string & operator=(const java_string & src) = delete;
        
        static local_java_ref<jstring> create(JNIEnv * env, const jchar * str, jsize len)
        {
            jstring ret = env->NewString(str, len);
            if (!ret)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("cannot create java string");
            }
            return jattach(env, ret);
        } 
        
        static local_java_ref<jstring> create(JNIEnv * env, const char * str);
        
        static jsize get_length(JNIEnv * env, jstring str)
        {
            if (!str)
                return 0;
            jsize ret = env->GetStringLength(str);
            if (ret < 0)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("invalid string size");
            }
            return ret;
        }
        
        static void get_region(JNIEnv * env, jstring str, jsize start, jsize len, jchar * buf)
        {
            env->GetStringRegion(str, start, len, buf);
            java_exception::check(env);
        }  
    };
    
    template<typename InIt, typename Out>
    Out utf32_to_utf16(InIt first, InIt last, Out dest)
    {
        while(first != last)
        {
            char32_t c = *first++;
            
            if (c > 0x010FFFF || (c > 0x00D7FF && c < 0x00E000))
            {
                *dest++ = u'\uFFFD';
            }
            else if (c >= 0x010000)
            {
                c -= 0x010000;
                *dest++ = char16_t((c >> 10)    + 0x00D800);
                *dest++ = char16_t((c & 0x03FF) + 0x00DC00);
            }
            else
            {
                *dest++ = char16_t(c);
            }
        }
        
        return dest;
    }
    
    template<typename InIt, typename OutIt>
    OutIt utf16_to_utf32(InIt first, InIt last, OutIt dest)
    {
        while(first != last)
        {
            char32_t lead = *first++;
            if ((lead >= 0xD800u) && (lead <= 0xDBFFu))
            {
                if (first < last)
                {
                    char32_t trail = *first++;
                    *dest++ = (((lead - 0xD800u) << 10) + (trail - 0xDC00u) + 0x0010000u);
                }
                else
                {
                    *dest++ = U'\U0000FFFD';
                }
            }
            else
            {
                *dest++ = lead;
            }
        }
        return dest;
    }
    
    template<typename InIt, typename Out>
    Out utf8_to_utf16(InIt first, InIt last, Out dest)
    {
        uint32_t min_for_length[] = {
            0,
            0x000080,
            0x000800,
            0x010000
        };
        
        for( ; ; )
        {
        resync:
        
            if (first == last)
                break;
        
            uint8_t lead = *first++;
            uint8_t tail[3];
            int tail_len;
            
            if (lead <= 0b01111111)
            {
                tail_len = 0;
            }
            else if (lead <= 0b11011111)
            {
                tail_len = 1;
                lead &= 0b00011111;
            }
            else if (lead <= 0b11101111)
            {
                tail_len = 2;
                lead &= 0b00001111;
            }
            else if (lead <= 0b11110111)
            {
                tail_len = 3;
                lead &= 0b00000111;
            }
            else
            {
                *dest++ = u'\uFFFD';
                goto resync;
            }
            
            for(int i = 0; i < tail_len; ++i)
            {
                if (first == last)
                {
                    *dest++ = u'\uFFFD';
                    goto resync;
                }
                tail[i] = *first;
                if (tail[i] < 0b10000000 || tail[i] > 0b10111111)
                {
                    *dest++ = u'\uFFFD';
                    goto resync;
                }
                tail[i] &= 0b00111111;
                ++first;
            }
            
            uint32_t c = uint32_t(lead);
            for(int i = 0; i < tail_len; ++i)
            {
                c <<= 6;
                c |= tail[i];
            }
            
            if (c < min_for_length[tail_len] || c > 0x010FFFF || (c > 0x00D7FF && c < 0x00E000))
            {
                *dest++ = u'\uFFFD';
                goto resync;
            }
            
            if (c >= 0x010000)
            {
                c -= 0x010000;
                *dest++ = char16_t((c >> 10)    + 0x00D800);
                *dest++ = char16_t((c & 0x03FF) + 0x00DC00);
            }
            else
            {
                *dest++ = char16_t(c);
            }
        }
        
        return dest;
    }
    
    template<typename InIt, typename OutIt>
    OutIt utf16_to_utf8(InIt first, InIt last, OutIt dest)
    {
        while(first != last)
        {
            char32_t val;

            char32_t lead = *first++;
            if ((lead >= 0xD800u) && (lead <= 0xDBFFu))
            {
                if (first < last)
                {
                    char32_t trail = *first++;
                    val = (((lead - 0xD800u) << 10) + (trail - 0xDC00u) + 0x0010000u);
                }
                else
                {
                    val = U'\U0000FFFD';
                }
            }
            else
            {
                val = lead;
            }

            if (val <= 0x00007f)
            {
                *dest++ = static_cast<uint8_t>(val);
            }
            else if (val <= 0x0007FF)
            {
                *dest++ = static_cast<uint8_t>(0b11000000u | ((val >> 6) & 0b00011111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | (val        & 0b00111111u));
            }
            else if (val <= 0x00FFFF)
            {
                *dest++ = static_cast<uint8_t>(0b11100000u | ((val >> 12) & 0b00001111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((val >> 6)  & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | (val         & 0b00111111u));
            }
            else
            {
                *dest++ = static_cast<uint8_t>(0b11110000u | (val         & 0b00000111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((val >> 6)  & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((val >> 12) & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((val >> 18) & 0b00111111u));
            }
        }
        return dest;
    }
}

#endif
