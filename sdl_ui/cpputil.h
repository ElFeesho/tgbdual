/**
 * $Id: cpputil.h,v 1.1 2003/09/28 16:37:44 i Exp $
 *
 * Copyright (C) shinichiro.h <g940455@mail.ecc.u-tokyo.ac.jp>
 *  http://user.ecc.u-tokyo.ac.jp/~g940455/wp/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef cpputil_h_
#define cpputil_h_

#include <string>
#include <sstream>
#include <iostream>

inline void makeErrorOutput_(std::ostream &os, const std::string &str, const char *f, const int l) {
    os << f << ':' << l << ": " << str << std::endl;
}

inline void error_(const std::string &str, const char *f, const int l) {
    std::ostringstream oss;
    makeErrorOutput_(oss, str, f, l);
    throw std::runtime_error(oss.str());
}
#define error(expr) error_(expr, __FILE__, __LINE__);
inline void check_(bool t, const char *str, const char *f, const int l) {
    if (!t) {
        error_(std::string(str), f, l);
    }
}
inline void check_(bool t, const std::string &str, const char *f, int l) {
    if (!t) {
        error_(str, f, l);
    }
}
#define check(expr1, expr2) check_(expr1, expr2, __FILE__, __LINE__);

#endif // ! cpputil_h_
