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

#ifndef HEADER_UTF_UTIL_INCLUDED
#define HEADER_UTF_UTIL_INCLUDED

namespace smjni
{
    template<typename InIt, typename Out>
    Out utf32_to_utf16(InIt first, InIt last, Out dest)
    {
        while(first != last)
        {
            uint32_t c = *first++;
            
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
            uint32_t value = *first++;
        resync:
            if (value >= 0xD800 && value <= 0xDBFF)
            {
                if (first == last)
                {
                    *dest++ = U'\uFFFD';
                    return dest;
                }
                
                uint32_t trail = *first++;
                if (trail < 0xDC00 || trail > 0xDFFF)
                {
                    *dest++ = U'\uFFFD';
                    value = trail;
                    goto resync;
                }
                else
                {
                    value = char32_t(((value - 0xD800) << 10) + (trail - 0xDC00) + 0x0010000);
                }
            }
            else if (value >= 0xDC00 && value <= 0xDFFF)
            {
                value = U'\uFFFD';
            }
            *dest++ = char32_t(value);
        }
        return dest;
    }
    
    template<typename InIt, typename Out>
    Out utf8_to_utf16(InIt first, InIt last, Out dest)
    {
        while(first != last)
        {
            uint8_t lead = *first++;
            
        resync:
            if (lead <= 0b01111111)
            {
                *dest++ = char16_t(lead);
                continue;
            }
            if (lead <= 0b11000001)
            {
                *dest++ = u'\uFFFD';
                continue;
            }
            
            int tail_len;
            uint8_t tail_lower_bound = 0b10000000;
            uint8_t tail_upper_bound = 0b10111111;
            if (lead <= 0b11011111)
            {
                tail_len = 1;
                lead &= 0b00011111;
            }
            else if (lead <= 0b11101111)
            {
                tail_len = 2;
                
                if (lead == 0b11100000)
                    tail_lower_bound = 0b10100000;
                else if (lead == 0b11101101)
                    tail_upper_bound = 0b10011111;
                
                lead &= 0b00001111;
            }
            else if (lead <= 0b11110100)
            {
                tail_len = 3;
                
                if (lead == 0b11110000)
                    tail_lower_bound = 0b10010000;
                else if (lead == 0b11110100)
                    tail_upper_bound = 0b10001111;
                
                lead &= 0b00000111;
            }
            else
            {
                *dest++ = u'\uFFFD';
                continue;
            }
    
            uint32_t c = uint32_t(lead);
            for(int i = 0; i < tail_len; ++i)
            {
                if (first == last)
                {
                    *dest++ = u'\uFFFD';
                    return dest;
                }
                uint8_t tail = *first++;
                if (tail < tail_lower_bound || tail > tail_upper_bound)
                {
                    *dest++ = u'\uFFFD';
                    lead = tail;
                    goto resync;
                }
                c = (c << 6) | (tail & 0b00111111);
                tail_lower_bound = 0b10000000;
                tail_upper_bound = 0b10111111;
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
            uint32_t value = *first++;
        resync:
            if (value >= 0xD800 && value <= 0xDBFF)
            {
                if (first == last)
                {
                    *dest++ = static_cast<uint8_t>('\xef');
                    *dest++ = static_cast<uint8_t>('\xbf');
                    *dest++ = static_cast<uint8_t>('\xbd');
                    return dest;
                }
                
                
                uint32_t trail = *first++;
                if (trail < 0xDC00 || trail > 0xDFFF)
                {
                    *dest++ = static_cast<uint8_t>('\xef');
                    *dest++ = static_cast<uint8_t>('\xbf');
                    *dest++ = static_cast<uint8_t>('\xbd');
                    value = trail;
                    goto resync;
                }
                else
                {
                    value = char32_t(((value - 0xD800) << 10) + (trail - 0xDC00) + 0x0010000);
                }
            }
            else if (value >= 0xDC00 && value <= 0xDFFF)
            {
                *dest++ = static_cast<uint8_t>('\xef');
                *dest++ = static_cast<uint8_t>('\xbf');
                *dest++ = static_cast<uint8_t>('\xbd');
                continue;
            }

            if (value <= 0x00007f)
            {
                *dest++ = static_cast<uint8_t>(value);
            }
            else if (value <= 0x0007FF)
            {
                *dest++ = static_cast<uint8_t>(0b11000000u | ((value >> 6) & 0b00011111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | (value        & 0b00111111u));
            }
            else if (value <= 0x00FFFF)
            {
                *dest++ = static_cast<uint8_t>(0b11100000u | ((value >> 12) & 0b00001111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((value >> 6)  & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | (value         & 0b00111111u));
            }
            else
            {
                *dest++ = static_cast<uint8_t>(0b11110000u | ((value >> 18) & 0b00000111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((value >> 12) & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | ((value >> 6)  & 0b00111111u));
                *dest++ = static_cast<uint8_t>(0b10000000u | (value         & 0b00111111u));
            }
        }
        return dest;
    }
}


#endif //HEADER_UTF_UTIL_INCLUDED


