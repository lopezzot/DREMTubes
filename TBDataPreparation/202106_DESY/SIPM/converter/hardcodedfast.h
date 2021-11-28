#pragma once
#include <map>
#include <stdint.h>

static constexpr bool VERBOSE = false;

static constexpr uint8_t NCHANNELS = 64;
static constexpr uint8_t MAX_BOARDS = 5;
static constexpr uint8_t FILE_HEADER_SIZE = 14;

static constexpr uint32_t EVENT_HEADER_SIZE[] = {27, 21, 27, 27};
static constexpr uint32_t EVENTS_SIZE[] = {6, 7, 12, 5};

static constexpr uint8_t MAPPING_LUT[] = {
    0,  40, 8,  32, // 0 - 3
    1,  41, 9,  33, // 4 - 7
    2,  42, 10, 34, // 8 - 11
    3,  43, 11, 35, // 12 - 15
    4,  44, 12, 36, // 16 - 19
    5,  45, 13, 37, // 20 - 23
    6,  46, 14, 38, // 24 - 27
    7,  47, 15, 39, // 28 - 31
    24, 48, 16, 56, // 32 - 35
    25, 49, 17, 57, // 36 - 39
    26, 50, 18, 58, // 40 - 43
    27, 51, 19, 59, // 44 - 47
    28, 52, 20, 60, // 48 - 51
    29, 53, 21, 61, // 52 - 55
    30, 54, 22, 62, // 56 - 59
    31, 55, 23, 63  // 60 - 63
};
