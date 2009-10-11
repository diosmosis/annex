///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009 Benaka Moorthi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <annex/annex.hpp>
#include "plugin.hpp"

struct concrete_plugin : my_plugin_interface
{
    int function_1()
    {
        return plugin1::function_1_result;
    }

    int function_2()
    {
        return plugin1::function_2_result;
    }
};

ANNEX_IMPLEMENT_PLUGIN(concrete_plugin)
