//
// Created by rudyc on 2019/9/6.
//

#ifndef MEOW_CORE_HPP
#define MEOW_CORE_HPP

#define FORMAT meow::fmt::Format::format
// #define PRINT_TRACE meow::core::printStack

#pragma region MACRO

#define println(...)                \
    do {                            \
        std::cout << FORMAT(__VA_ARGS__) << std::endl;        \
    } while(0)

#define eprintf(...)            \
    do {        \
        std::cerr << FORMAT(__VA_ARGS__); \
    } while(0)

#define eprintln(...)   \
    do {    \
        eprintf(__VA_ARGS__);   \
        eprintf("\n");  \
    } while(0)  \

#define panic(...)                  \
    do {                            \
        eprintf("panic at {}:{} \n", __FILE__, __LINE__);   \
        eprintf("\t");              \
        eprintf(__VA_ARGS__);        \
        eprintf("\n");    \
        exit(-1);        \
    } while(0)

#ifdef _DEBUG
#define debugln(...)      \
    do {        \
        eprintf("DEBUG[ {}:{} ]\t", __FILE__, __LINE__);  \
        eprintf(__VA_ARGS__);   \
        eprintf("\n");       \
    } while(0)
#else
#define debugln(...)
#endif

#pragma endregion 

#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <iostream>
#include <regex>

#include "interface.hpp"
#include "fmt/display.hpp"
#include "fmt/format.hpp"
#include "result.hpp"
#include "util.hpp"
#include "enum.hpp"

namespace meow {
  
}

using namespace meow;


#endif