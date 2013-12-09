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

    std::string host;
    unsigned int port;

    Config() : 
        tile_width(16), tile_height(16),
        screen_width(80), screen_height(25),
        fullscreen(true), port(23)
        {}
};


#endif
