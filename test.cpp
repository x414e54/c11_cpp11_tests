/*
 C++ 11 printf and vnsprintf style functions adapted from
 http://en.cppreference.com/w/cpp/string/multibyte/wcrtomb
 http://en.cppreference.com/w/cpp/string/multibyte/mbrtowc
 https://en.wikipedia.org/wiki/Variadic_template
*/

#include <iostream>
#include <clocale>
#include <cstring>
#include <uchar.h> 
#include <sstream>
#include <ostream>

template<typename C>
void cpp11_printf(std::basic_ostream<C>& stream, const C* fmt)
{
        stream << fmt;
}

void cpp11_printf(std::basic_ostream<char>& stream, const char16_t& c)
{
        std::mbstate_t state = std::mbstate_t();
        std::string mb(MB_CUR_MAX, '\0');
        int ret = c16rtomb(&mb[0], c, &state);
        stream << mb;
}

void cpp11_printf(std::basic_ostream<char>& stream, const char16_t* ptr)
{
        std::mbstate_t state = std::mbstate_t();
        size_t len = std::char_traits<char16_t>::length(ptr);
        for (int i = 0; i < len; ++i) {
                std::string mb(MB_CUR_MAX, '\0');
                int ret = c16rtomb(&mb[0], *ptr++, &state);
                stream << mb;
        }
}

void cpp11_printf(std::basic_ostream<char16_t>& stream, const char* ptr)
{
        std::mbstate_t state = std::mbstate_t(); // initial state
        const char* end = ptr + std::strlen(ptr);
        int len;
        char16_t wc;
        while((len = mbrtoc16(&wc, ptr, end-ptr, &state)) > 0) {
                stream << wc;
                ptr += len;
        }
}

template<typename C, typename T>
void cpp11_printf(std::basic_ostream<C>& stream, const T& value)
{
	stream << value;
}

template<typename C, typename T, typename... Args>
void cpp11_printf(std::basic_ostream<C>& stream, const char16_t* fmt,
                  T value, Args... args)
{		
        while (*fmt) {
                if (*fmt == u'%') {
                        ++fmt;
                        cpp11_printf(stream, value);
                        if (*fmt) {
                                cpp11_printf(stream, fmt + 1, args...);
                        }
                        return;
                }
                cpp11_printf(stream, *fmt++);
	}
}

void print_mb(const char* ptr)
{
        std::mbstate_t state = std::mbstate_t(); // initial state
        const char* end = ptr + std::strlen(ptr);
        int len;
        char16_t wc;
        while((len = mbrtoc16(&wc, ptr, end-ptr, &state)) > 0) {
                std::cout << wc;
                ptr += len;
        }
}

void print_16(const char16_t* ptr)
{
        std::mbstate_t state = std::mbstate_t();
        size_t len = std::char_traits<char16_t>::length(ptr);
        for (int i = 0; i < len; ++i) {
                std::string mb(MB_CUR_MAX, '\0');
                int ret = c16rtomb(&mb[0], *ptr++, &state);
                std::cout << mb;
        }
}

int main()
{
        std::setlocale(LC_ALL,"");
        // UTF-8 narrow multibyte encoding
        const char* str = u8"z\u00df\u6c34\U0001d10b"; // or u8"zß水𝄋"
                      // or "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b";
        const char16_t* str2 = u"z\u00df\u6c34\U0001d10b";
        print_mb(str);
        std::cout << std::endl;
        print_16(str2);
        std::cout << std::endl;
        cpp11_printf(std::cout, u"Test%s%d%o,test", u"tester", "test", 13);
        std::cout << std::endl;
        std::basic_stringstream<char16_t> sstr;
        cpp11_printf(sstr, u"Bloopp%d,doo%t", u"er", "dle");
        std::basic_string<char16_t> str16 = sstr.str();
        const char16_t* ret_str = str16.c_str();
        print_16(ret_str);
        std::cout << std::endl;
}
