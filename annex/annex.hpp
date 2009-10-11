///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009 Benaka Moorthi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#if !defined( ANNEX_ANNEX_HPP )
#define ANNEX_ANNEX_HPP

#include <boost/function_types/result_type.hpp>

#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>

#include <string>

// TODO: This should throw boost::system_error based on a #define.
#if defined(BOOST_WINDOWS) // windows
#   define ANNEX_DECL __declspec(dllexport)

namespace annex { namespace detail
{
    extern "C" __declspec(dllimport) void *__stdcall LoadLibraryA(const char *);
    extern "C" __declspec(dllimport) int   __stdcall FreeLibrary(void *);
    extern "C" __declspec(dllimport) void *__stdcall GetProcAddress(void *, const char*);

    typedef void * shared_library_handle;

    inline shared_library_handle load_shared_library(std::string const& file)
    {
        void * result = LoadLibraryA(file.c_str());

        BOOST_ASSERT(result);

        return result;
    }

    inline void unload_shared_library(shared_library_handle handle)
    {
        BOOST_ASSERT(handle);

        int result = FreeLibrary(handle);

        BOOST_ASSERT(result);
    }

    inline void * get_function_ptr(shared_library_handle handle, char const* str)
    {
        BOOST_ASSERT(handle && str);

        void * result = GetProcAddress(handle, str);

        BOOST_ASSERT(result);

        return result;
    }
}}
#else // linux
#   include <dlfcn.h>

#   define ANNEX_DECL

namespace annex { namespace detail
{
    typedef void * shared_library_handle;

    inline shared_library_handle load_shared_library(std::string const& file)
    {
        void * result = dlopen(file.c_str(), RTLD_LAZY);

        // FIXME: if !result throw here

        return result;
    }

    inline void unload_shared_library(shared_library_handle handle)
    {
        BOOST_ASSERT(handle);

        int result = dlclose(handle);

        BOOST_ASSERT(result == 0);
    }

    inline void * get_function_ptr(shared_library_handle handle, char const* str)
    {
        BOOST_ASSERT(handle && str);

        void * result = dlsym(handle, str);

        BOOST_ASSERT(result);

        return result;
    }
}}
#endif

// TODO: Put ability to specify more than one construction signature.
#define ANNEX_IMPLEMENT_PLUGIN(impl)                                                    \
    extern "C"                                                                          \
    {                                                                                   \
        ANNEX_DECL impl * annex_plugin_init() { return new impl(); }                    \
        ANNEX_DECL void annex_plugin_cleanup(impl * lib) { delete lib; }                \
    }

// TODO: put debug parts to ensure type safety, at least in debug mode
// TODO: put some way of passing arguments to plugin
namespace annex
{
    struct shared_library
    {
        shared_library(std::string const& file)
        {
            handle = detail::load_shared_library(file);
        }

        ~shared_library()
        {
            detail::unload_shared_library(handle);
        }

        // TODO: Use file iteration
        template <typename Signature>
        typename boost::function_types::result_type<Signature>::type
            execute(char const* str)
        {
            Signature * fptr = (Signature *) detail::get_function_ptr(handle, str);

            return fptr();
        }

        template <typename Signature, typename A0>
        typename boost::function_types::result_type<Signature>::type
            execute(char const* str, A0 & a0)
        {
            Signature * fptr = (Signature *) detail::get_function_ptr(handle, str);

            return fptr(a0);
        }

        detail::shared_library_handle handle;
    };

    template <typename Impl>
    struct plugin
    {
        struct impl_type
        {
            impl_type(std::string const& file) : lib(file)
            {
                impl = lib.execute<Impl * ()>("annex_plugin_init");
            }

            ~impl_type()
            {
                lib.execute<void (Impl *)>("annex_plugin_cleanup", impl);
            }

            shared_library lib;
            Impl * impl;
        };

        plugin() {}

        plugin(std::string const& file)
        {
            load(file);
        }

        void load(std::string const& file)
        {
            pimpl.reset(new impl_type(file));
        }

        Impl * operator -> () const { return pimpl->impl; }

        boost::shared_ptr<impl_type> pimpl;
    };
}

#endif // #if !defined( ANNEX_ANNEX_HPP )
