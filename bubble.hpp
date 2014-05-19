/*
 * Bubble is a simple dialog library built on a text-based API.
 * Copyright (c) 2014 Mario 'rlyeh' Rodriguez

 * Callstack code is based on code by Magnus Norddahl (See http://goo.gl/LM5JB)
 * Mem/CPU OS code is based on code by David Robert Nadeau (See http://goo.gl/8P5Jqv)

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * - rlyeh
 */

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <iomanip>

namespace bubble {
    struct string : std::wstring {
        string() : std::wstring()
        {}
        template<typename T>
        string( const T &t ) : std::wstring() {
            std::wstringstream ss;
            if( ss << std::setprecision(20) << t )
                this->assign( ss.str() );
        }
        template<unsigned N>
        string( const std::string::value_type (&str)[N] ) : std::wstring() {
            std::string s(str);
            this->assign( s.begin(), s.end() );
        }
        template<unsigned N>
        string( const std::wstring::value_type (&str)[N] ) : std::wstring( str )
        {}
        string( const std::string &t ) : std::wstring( t.begin(), t.end() )
        {}
        string( const std::wstring &t ) : std::wstring( t )
        {}
        template<typename T>
        bool operator>>( T &t ) const {
            std::wstringstream ss( *this );
            return ss >> t ? true : (t = T(), false);
        }
        template<typename T>
        string &operator<<( const T &t ) {
            std::wstringstream ss;
            return ss << t ? ( this->assign( *this + ss.str() ), *this ) : *this;
        }
        template<typename T>
        T as() const {
            T t;
            std::wstringstream ss( *this );
            return ss >> t ? t : T();
        }
        std::wstring str() const {
            return *this;
        }
        operator int() const {
            return as<int>();
        }
    };

    using vars = std::map<bubble::string, bubble::string>;

    int show( const bubble::string &options, const std::function<void(bubble::vars &)> &cb2 = std::function<void(bubble::vars &)>() );
    int show( const bubble::vars &map, const std::function<void(bubble::vars &)> &cb2 = std::function<void(bubble::vars &)>() );
}
