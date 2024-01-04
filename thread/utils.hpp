#include <iostream>

#ifndef UTILS_HPP
#define UTILS_HPP

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"


#define PRINT_INFO(msg) \
    std::cout << COLOR_GREEN << "[INFO] " << COLOR_RESET << msg << std::endl

#define PRINT_INFO2(msg0, msg1) \
    std::cout << COLOR_GREEN << "[INFO] " << COLOR_RESET << msg0 << msg1 << std::endl

#define PRINT_ERROR(msg) \
    std::cout << COLOR_RED << "[ERROR] " << COLOR_RESET << msg << std::endl


// function : parse_uint
// description : get a uint64 value from string (support hexadecimal and decimal)
int parse_uint (char *string, uint64_t *pvalue) {
    if ( string[0] == '0'  &&  string[1] == 'x' )                // HEX format "0xXXXXXXXX"
        return sscanf( &(string[2]), "%lx", pvalue);
    else                                                         // DEC format
        return sscanf(   string    , "%lu", pvalue);
}
#endif