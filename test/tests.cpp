///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009 Benaka Moorthi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#define BOOST_TEST_MODULE annex_tests
#include <boost/test/included/unit_test.hpp>
#include "plugin.hpp"

BOOST_AUTO_TEST_CASE( plugin_load_unload )
{
    annex::plugin<my_plugin_interface> p1("plugin1.dll");
}

BOOST_AUTO_TEST_CASE( multiple_plugin_load_unload )
{
    annex::plugin<my_plugin_interface> p1("plugin1.dll");
    annex::plugin<my_plugin_interface> p2("plugin2.dll");
}

BOOST_AUTO_TEST_CASE( multiple_plugin_calls )
{
    annex::plugin<my_plugin_interface> p1("plugin1.dll");
    annex::plugin<my_plugin_interface> p2("plugin2.dll");

    BOOST_CHECK( p1->function_1() == plugin1::function_1_result );
    BOOST_CHECK( p1->function_2() == plugin1::function_2_result );

    BOOST_CHECK( p2->function_1() == plugin2::function_1_result );
    BOOST_CHECK( p2->function_2() == plugin2::function_2_result );
}
