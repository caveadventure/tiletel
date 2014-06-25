#ifndef __CONFIG_TYPES_H
#define __CONFIG_TYPES_H

struct Config {

    std::vector<std::string> fonts;
    std::string tiles;

    size_t tile_width;
    size_t tile_height;
    size_t screen_width;
    size_t screen_height;
    bool fullscreen;
    bool cursor;

    unsigned int polling_rate;

    std::string host;
    unsigned int port;

    std::vector<std::string> command;

    std::string palette;
    std::unordered_map<uint32_t, uint32_t> tile_mapping;

    bool compression;

    Config() : 
        tile_width(16), tile_height(16),
        screen_width(80), screen_height(25),
        fullscreen(true), cursor(true), polling_rate(50),
        port(23), compression(true)
        {}
};


#endif
