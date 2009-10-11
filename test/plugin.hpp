///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009 Benaka Moorthi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#if !defined( ANNEX_PLUGIN_TEST_HPP )
#define ANNEX_PLUGIN_TEST_HPP

#include <annex/annex.hpp>

struct my_plugin_interface
{
    my_plugin_interface() {}
    virtual ~my_plugin_interface() {}

    virtual int function_1() = 0;
    virtual int function_2() = 0;
};

namespace plugin1
{
    int const function_1_result = 1;
    int const function_2_result = 2;
}

namespace plugin2
{
    int const function_1_result = 3;
    int const function_2_result = 4;
}

#endif // #if !defined( ANNEX_PLUGIN_TEST_HPP )
