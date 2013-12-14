#ifndef __BDF_H
#define __BDF_H

// This file needs to be preprocessed with the 'ragel' tool.
// Run it like this:
//
//   ragel -s -T0 bdf.rl -o bdf.h
//

#include <string>
#include <stdexcept>
#include <iostream>

#include <string.h>
#include <stdio.h>

#include <stdint.h>

#include <vector>
#include <unordered_map>


namespace bdf {

#include "bdf_types.h"


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

inline void hexchar(uint8_t& c, char fc) {
    c = c << 4;
    if (fc >= '0' && fc <= '9') {
        c |= (fc - '0');
    } else if (fc >= 'A' && fc <= 'F') {
        c |= (fc - 'A' + 10);
    }
}

inline void check_h(const Font& f, size_t h) {
    if (h != f.h)
        throw std::runtime_error("Height mismatch; Only monospace fonts are supported!");
}

inline void check_w(const Font& f, size_t w) {
    if (w != f.w && w != f.w / 2)
        throw std::runtime_error("Width mismatch; Only monospace fonts are supported!");
}

void parse_bdf(const std::string& filename, Font& out) {

    /** File reading cruft. **/

    char buf[4096];

    FILE* fn = ::fopen(filename.c_str(), "r");

    if (fn == NULL) {
        throw std::runtime_error("Could not open: \"" + filename + "\"");
    }

    ragel_state state;

    uint8_t bitmapchar = 0;

    out.glyphs.clear();
    out.w = 0;
    out.h = 0;

    bitmap* glyph = &(out.glyphs[0]);

    %%{

        machine BDFParser;

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

        junk = [^\n]+ '\n';
        optjunk = [^\n]* '\n';

        startchar = 'STARTCHAR' junk;

        encoding = 
            'ENCODING' 
            ws1 number %{ glyph = &(out.glyphs[toint(state.match)]); }
            '\n'
        ;

        bbox = 
            'BBX' 
            ws1 number %{ glyph->w = toint(state.match); check_w(out, glyph->w); }
            ws1 number %{ check_h(out, toint(state.match)); }
            ws1 snumber 
            ws1 snumber 
            '\n'
        ;

        bitmapchar = 
            ([0-9A-F]) ${ hexchar(bitmapchar, fc); }
            ([0-9A-F]) ${ hexchar(bitmapchar, fc); }
            %{ glyph->bm.push_back(bitmapchar); }
            ;

        bitmapline = bitmapchar+ '\n';
        bitmap = 
            'BITMAP' optjunk
            (bitmapline+)
             ;

        swidth = 'SWIDTH' junk ;
        dwidth = 'DWIDTH' junk ;
        vvector = 'VVECTOR' junk ;

        glyphline = 
            ( encoding |
              bbox |
              swidth | dwidth | vvector )
            ;

        glyph = 
            startchar
            glyphline* :>> bitmap
            'ENDCHAR' optjunk
        ;

        fontboundingbox = 
            'FONTBOUNDINGBOX' 
            ws1 number %{ out.w = toint(state.match); }
            ws1 number %{ out.h = toint(state.match); }
            ws1 snumber 
            ws1 snumber '\n'
        ;

        startfont = 'STARTFONT' junk ;
        comment = 'COMMENT' junk ;
        contentversion = 'CONTENTVERSION' junk ;
        xfont = 'FONT' junk ;
        size = 'SIZE' junk ;
        metricsset = 'METRICSSET' junk ;

        properties = (any)* :>> ('ENDPROPERTIES\n');

        startproperties = 'STARTPROPERTIES' junk properties;

        header =
            ( fontboundingbox |
              startfont | comment | contentversion | xfont | size | metricsset |
              swidth | dwidth | vvector | startproperties )
            ;

        font = 
           (header+)
           'CHARS' junk
           (glyph+) 
           'ENDFONT' optjunk
        ;

      main := font ;
        
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
        (void)BDFParser_first_final;
        (void)BDFParser_en_main;

        if (state.cs == BDFParser_error) {
            throw std::runtime_error("Parse error. Unconsumed input: " + std::string(state.p, state.pe));
        }
    }

    for (auto& i : out.glyphs) {
        auto& g = i.second;
            
        if ((g.w % 8) != 0) {
            g.pitch = (g.w / 8) + 1;
        } else {
            g.pitch = g.w / 8;
        }
    }
}

}


#endif
