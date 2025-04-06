#pragma once
#include <iostream>
#include <string>

enum Colour
{
    YELLOW = 1,
    GREEN,
    BLUE,
    RED,
    MAGENTA,
    CYAN,
    WHITE,
    BLACK
};

// Template function definition
template <typename T>
static void debug(const T& msg, int colour = 1)
{
#ifdef DJ
    std::string color_code;

    if (colour == 1)
        color_code = "\033[1;33m"; // Yellow
    else if (colour == 2)
        color_code = "\033[1;32m"; // Green
    else if (colour == 3)
        color_code = "\033[1;34m"; // Blue
    else if (colour == 4)
        color_code = "\033[1;31m"; // Red
    else if (colour == 5)
        color_code = "\033[1;35m"; // Magenta
    else if (colour == 6)
        color_code = "\033[1;36m"; // Cyan
    else if (colour == 7)
        color_code = "\033[1;37m"; // White
    else if (colour == 8)
        color_code = "\033[1;30m"; // Black
    else
        color_code = ""; // Default color

    std::cerr << color_code << msg << "\033[0m" << std::endl;
#endif
}

static void debug_null(void* ptr, const std::string& msg)
{
    if(ptr == nullptr){
        debug(msg + " is null", RED);
    } else {
        debug(msg + " is not null", GREEN);
    }
}
