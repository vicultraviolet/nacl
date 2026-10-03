#ifndef NACL_MACROS_HPP

#if defined(__has_cpp_attribute) && __has_cpp_attribute(msvc::no_unique_address)
    #define NACL_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address)
    #define NACL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define NACL_NO_UNIQUE_ADDRESS
#endif

#endif // NACL_MACROS_HPP
