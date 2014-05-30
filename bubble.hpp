/* Bubble is a simple dialog library built on a text-based API.
 * Copyright (c) 2014, 2015, Mario 'rlyeh' Rodriguez, zlib/libpng licensed.

 * Callstack code is based on code by Magnus Norddahl (See http://goo.gl/LM5JB)
 * Mem/CPU OS code is based on code by David Robert Nadeau (See http://goo.gl/8P5Jqv)

 * - rlyeh
 */

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <iomanip>

#define BUBBLE_VERSION "1.0.0" /* (2015/06/12) Diverse enhancements and clean ups
#define BUBBLE_VERSION "0.0.0" // (2014/05/20) Initial commit */

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
        string &operator<<( const std::string &t ) {
            return operator<<( std::wstring( t.begin(), t.end() ));
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
