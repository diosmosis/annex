///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2014 Benaka Moorthi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#if !defined( ANNEX_ANNEX_HPP )
#define ANNEX_ANNEX_HPP

#include <memory>
#include <type_traits>
#include <string>
#include <assert.h>

// TODO: Should throw an exception to get human readable error messages on errors.
#if defined(WIN32) || defined(_WIN32) // windows
#   define ANNEX_DECL __declspec(dllexport)

namespace annex { namespace detail
{
    extern "C" __declspec(dllimport) void *__stdcall LoadLibraryA(const char *);
    extern "C" __declspec(dllimport) int   __stdcall FreeLibrary(void *);
    extern "C" __declspec(dllimport) void *__stdcall GetProcAddress(void *, const char*);

    typedef void * shared_library_handle;

    inline shared_library_handle load_shared_library(std::string const& file)
    {
        return LoadLibraryA(file.c_str());
    }

    inline bool unload_shared_library(shared_library_handle handle)
    {
        assert(handle);

        return FreeLibrary(handle) != 0;
    }

    inline void * get_function_ptr(shared_library_handle handle, char const* str)
    {
        assert(handle && str);

        return GetProcAddress(handle, str);
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
        return dlopen(file.c_str(), RTLD_LAZY);
    }

    inline bool unload_shared_library(shared_library_handle handle)
    {
        assert(handle);

        return dlclose(handle) == 0;
    }

    inline void * get_function_ptr(shared_library_handle handle, char const* str)
    {
        assert(handle && str);

        return dlsym(handle, str);
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
namespace annex
{
    struct shared_library
    {
        shared_library(std::string const& file)
        {
            handle = detail::load_shared_library(file);
        }

        shared_library(shared_library && other)
            : handle(other.handle)
        {}

        ~shared_library()
        {
            detail::unload_shared_library(handle);
        }

        template <typename Signature, typename... ArgumentTypes>
        typename std::result_of<Signature>::type execute(char const* str, ArgumentTypes... args) const
        {
            Signature * fptr = get_as<Signature>(str);
            return fptr(args...);
        }

        explicit operator bool() const
        {
            return handle;
        }

        void * get(char const* str) const
        {
            return detail::get_function_ptr(handle, str);
        }

        template <typename Signature>
        Signature * get_as(char const* str) const
        {
            return static_cast<Signature *>(get(str));
        }

    private:
        shared_library(shared_library const&);

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

    private:
        std::shared_ptr<impl_type> pimpl;
    };
}

#endif // #if !defined( ANNEX_ANNEX_HPP )