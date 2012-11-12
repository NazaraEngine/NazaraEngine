
#ifndef BOOST_MPL_AUX_TEST_DATA_HPP_INCLUDED
#define BOOST_MPL_AUX_TEST_DATA_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: data.hpp 49239 2008-10-10 09:10:26Z agurtovoy $
// $Date: 2008-10-10 11:10:26 +0200 (Fr, 10. Okt 2008) $
// $Revision: 49239 $

#include <boost/noncopyable.hpp>

enum enum_ {};
struct UDT {};
struct incomplete;
class abstract { public: virtual ~abstract() = 0; };
using boost::noncopyable;

#endif // BOOST_MPL_AUX_TEST_DATA_HPP_INCLUDED
