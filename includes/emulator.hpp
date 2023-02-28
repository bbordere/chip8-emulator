#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstring>
#include <chrono>
#include <iomanip>

#define START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x50
#define WIDTH_SCREEN 64
#define HEIGHT_SCREEN 32
#define PIXEL_SIZE 16

#define BG_COLOR {0, 0, 0}
#define FG_COLOR {0, 196, 49}

#endif