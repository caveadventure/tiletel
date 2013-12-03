#ifndef __CONFIG_H
#define __CONFIG_H

// This file needs to be preprocessed with the 'ragel' tool.
// Run it like this:
//
//   ragel -s -T0 config.rl -o config.h
//

#include <string>
#include <stdexcept>
#include <iostream>

#include <string.h>
#include <stdio.h>

#include <stdint.h>

#include <vector>


namespace config {

#include "config_types.h"


struct ragel_state {

    /** Ragel scanner cruft. **/

    // Iterators for start and end of buffer data.
    char* p;
    char* pe;

    // Iterator for 'end of file'.
    char* eof;

    // Internal state and rollback variables.
    int cs, act;

    // Stack for ragel machine states.
    int stack[256];
    int top;

    // Not part of ragel's variables; this is a buffer to hold the current match.
    std::string match;
};

inline int toint(const std::string& s) {
    return ::atoi(s.c_str());
}

void parse_config(const std::string& filename, Config& out) {

    /** File reading cruft. **/

    char buf[4096];

    FILE* fn = ::fopen(filename.c_str(), "r");

    if (fn == NULL) {
        throw std::runtime_error("Could not open: \"" + filename + "\"");
    }

    ragel_state state;


    %%{

        machine ConfigParser;

        access state.;
        variable p state.p;
        variable pe state.pe;
        variable eof state.eof;

        action start {
            state.match.clear();
        }

        action push {
            state.match += fc;
        }

        onews = [ \t\r\n];
        ws = onews*;
        ws1 = onews+;

        number = [0-9]+ 
            >start $push;

        snumber = ('-'? [0-9]+)
            >start $push;

        #/* 34 is the double quote character, the only reason for writing it like this is to make Emacs happy */
        quote = 34;

        strchar_escape := 
            ( ('"'   ${ state.match += '"';  fret; }) |
              ('n'   ${ state.match += '\n'; fret; }) |
              ('1'   ${ state.match += '\1'; fret; }) |
              ('2'   ${ state.match += '\2'; fret; }) |
              ('3'   ${ state.match += '\3'; fret; }) |
              ('\\'  ${ state.match += '\\'; fret; }) )
            ;

        strchar = ( (^ (quote | '\\') $push ) |
                    ('\\'             ${fcall strchar_escape;} ) );

        strdata = strchar*;

        string = '"' >start strdata '"';

        fonts = 'fonts' (ws1 string %{ out.fonts.push_back(state.match); })+ ws ';';

        fullscreen = 'fullscreen' ws1
            (('on'  %{ out.fullscreen = true; }) |
             ('off' %{ out.fullscreen = false; }))
            ws ';' 
            ;
        
        screensize = 'screensize'
            ws1 number %{ out.screen_width = toint(state.match); }
            ws1 number %{ out.screen_height = toint(state.match); }
            ws ';'
            ;

        tilesize = 'tilesize' 
            ws1 number %{ out.tile_width = toint(state.match); }
            ws1 number %{ out.tile_height = toint(state.match); }
            ws ';'
            ;

        connect_to = 'connect_to'
            ws1 string %{ out.host = state.match; }
            ws1 number %{ out.port = toint(state.match); }
            ws ';'
            ;

        config = 
            fonts | tilesize | screensize | fullscreen | connect_to
            ;

      main := (ws config ws)+;
        
    }%%
/*

*/

    %% write data;
    %% write init;

    bool done = false;

    while (!done) {
        int space = sizeof(buf) ;

        state.p = buf;
        int len = ::fread(state.p, 1, space, fn);
        
        state.pe = state.p + len;
        state.eof = 0;
        
        if (len == 0) {
            state.eof = state.pe;
            done = true;
        }

        %% write exec;

        // Avoid spurious gcc warnings.
        (void)ConfigParser_en_main;
        (void)ConfigParser_first_final;
        (void)ConfigParser_en_strchar_escape;

        if (state.cs == ConfigParser_error) {
            throw std::runtime_error("Parse error. Unconsumed input: " + std::string(state.p, state.pe));
        }
    }

    std::cout << "Done parsing config." << std::endl;

    for (const auto& f : out.fonts)
        std::cout << "font " << f << std::endl;

    std::cout << "tilesize " << out.tile_width << " " << out.tile_height << std::endl;
    std::cout << "screensize " << out.screen_width << " " << out.screen_height << std::endl;
    std::cout << "fullscreen " << out.fullscreen << std::endl;
}

}


#endif
