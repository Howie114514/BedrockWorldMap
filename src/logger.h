#pragma once

#include <format>
#include <iostream>

#define DEBUG(...)                                                                                                     \
    std::cout << std::format("[BWM] [{}:{} {}] ", __FILE__, __LINE__, __func__) << std::format(__VA_ARGS__) << std::endl