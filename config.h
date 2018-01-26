
#line 1 "config.rl"
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

extern "C" {
#include "libtsm/src/libtsm_int.h"
}

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

inline void add_map(Config& out, const std::string& utf8_glyph, tsm_utf8_mach* utf8_mach, uint32_t tile) {

    tsm_utf8_mach_reset(utf8_mach);

    for (char c : utf8_glyph) {

        int x = tsm_utf8_mach_feed(utf8_mach, c);

        if (x == TSM_UTF8_ACCEPT || x == TSM_UTF8_REJECT) {
            uint32_t ucs = tsm_utf8_mach_get(utf8_mach);
            out.tile_mapping[ucs] = tile;
            return;
        }
    }
}

void parse_config(const std::string& filename, Config& out) {

    /** File reading cruft. **/

    char buf[4096];

    FILE* fn = ::fopen(filename.c_str(), "r");

    if (fn == NULL) {
        throw std::runtime_error("Could not open: \"" + filename + "\"");
    }

    ragel_state state;

    tsm_utf8_mach* utf8_mach;
    std::string utf8_glyph;

    tsm_utf8_mach_new(&utf8_mach);

    
#line 202 "config.rl"

/*

*/

    
#line 102 "config.h"
static const int ConfigParser_start = 1;
static const int ConfigParser_first_final = 149;
static const int ConfigParser_error = 0;

static const int ConfigParser_en_strchar_escape = 148;
static const int ConfigParser_en_main = 1;


#line 208 "config.rl"
    
#line 113 "config.h"
	{
	 state.cs = ConfigParser_start;
	 state.top = 0;
	}

#line 209 "config.rl"

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

        
#line 138 "config.h"
	{
	if ( ( state.p) == ( state.pe) )
		goto _test_eof;
	goto _resume;

_again:
	switch (  state.cs ) {
		case 1: goto st1;
		case 0: goto st0;
		case 2: goto st2;
		case 3: goto st3;
		case 4: goto st4;
		case 5: goto st5;
		case 6: goto st6;
		case 7: goto st7;
		case 8: goto st8;
		case 9: goto st9;
		case 10: goto st10;
		case 11: goto st11;
		case 12: goto st12;
		case 149: goto st149;
		case 13: goto st13;
		case 14: goto st14;
		case 15: goto st15;
		case 16: goto st16;
		case 17: goto st17;
		case 18: goto st18;
		case 19: goto st19;
		case 20: goto st20;
		case 21: goto st21;
		case 22: goto st22;
		case 23: goto st23;
		case 24: goto st24;
		case 25: goto st25;
		case 26: goto st26;
		case 27: goto st27;
		case 28: goto st28;
		case 29: goto st29;
		case 30: goto st30;
		case 31: goto st31;
		case 32: goto st32;
		case 33: goto st33;
		case 34: goto st34;
		case 35: goto st35;
		case 36: goto st36;
		case 37: goto st37;
		case 38: goto st38;
		case 39: goto st39;
		case 40: goto st40;
		case 41: goto st41;
		case 42: goto st42;
		case 43: goto st43;
		case 44: goto st44;
		case 45: goto st45;
		case 46: goto st46;
		case 47: goto st47;
		case 48: goto st48;
		case 49: goto st49;
		case 50: goto st50;
		case 51: goto st51;
		case 52: goto st52;
		case 53: goto st53;
		case 54: goto st54;
		case 55: goto st55;
		case 56: goto st56;
		case 57: goto st57;
		case 58: goto st58;
		case 59: goto st59;
		case 60: goto st60;
		case 61: goto st61;
		case 62: goto st62;
		case 63: goto st63;
		case 64: goto st64;
		case 65: goto st65;
		case 66: goto st66;
		case 67: goto st67;
		case 68: goto st68;
		case 69: goto st69;
		case 70: goto st70;
		case 71: goto st71;
		case 72: goto st72;
		case 73: goto st73;
		case 74: goto st74;
		case 75: goto st75;
		case 76: goto st76;
		case 77: goto st77;
		case 78: goto st78;
		case 79: goto st79;
		case 80: goto st80;
		case 81: goto st81;
		case 82: goto st82;
		case 83: goto st83;
		case 84: goto st84;
		case 85: goto st85;
		case 86: goto st86;
		case 87: goto st87;
		case 88: goto st88;
		case 89: goto st89;
		case 90: goto st90;
		case 91: goto st91;
		case 92: goto st92;
		case 93: goto st93;
		case 94: goto st94;
		case 95: goto st95;
		case 96: goto st96;
		case 97: goto st97;
		case 98: goto st98;
		case 99: goto st99;
		case 100: goto st100;
		case 101: goto st101;
		case 102: goto st102;
		case 103: goto st103;
		case 104: goto st104;
		case 105: goto st105;
		case 106: goto st106;
		case 107: goto st107;
		case 108: goto st108;
		case 109: goto st109;
		case 110: goto st110;
		case 111: goto st111;
		case 112: goto st112;
		case 113: goto st113;
		case 114: goto st114;
		case 115: goto st115;
		case 116: goto st116;
		case 117: goto st117;
		case 118: goto st118;
		case 119: goto st119;
		case 120: goto st120;
		case 121: goto st121;
		case 122: goto st122;
		case 123: goto st123;
		case 124: goto st124;
		case 125: goto st125;
		case 126: goto st126;
		case 127: goto st127;
		case 128: goto st128;
		case 129: goto st129;
		case 130: goto st130;
		case 131: goto st131;
		case 132: goto st132;
		case 133: goto st133;
		case 134: goto st134;
		case 135: goto st135;
		case 136: goto st136;
		case 137: goto st137;
		case 138: goto st138;
		case 139: goto st139;
		case 140: goto st140;
		case 141: goto st141;
		case 142: goto st142;
		case 143: goto st143;
		case 144: goto st144;
		case 145: goto st145;
		case 146: goto st146;
		case 147: goto st147;
		case 148: goto st148;
		case 150: goto st150;
	default: break;
	}

	if ( ++( state.p) == ( state.pe) )
		goto _test_eof;
_resume:
	switch (  state.cs )
	{
st1:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof1;
case 1:
	switch( (*( state.p)) ) {
		case 13: goto st1;
		case 32: goto st1;
		case 99: goto st2;
		case 102: goto st13;
		case 109: goto st37;
		case 112: goto st45;
		case 115: goto st68;
		case 116: goto st82;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st1;
	goto st0;
st0:
 state.cs = 0;
	goto _out;
st2:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof2;
case 2:
	switch( (*( state.p)) ) {
		case 111: goto st3;
		case 117: goto st138;
	}
	goto st0;
st3:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof3;
case 3:
	switch( (*( state.p)) ) {
		case 109: goto st4;
		case 110: goto st125;
	}
	goto st0;
st4:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof4;
case 4:
	switch( (*( state.p)) ) {
		case 109: goto st5;
		case 112: goto st112;
	}
	goto st0;
st5:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof5;
case 5:
	if ( (*( state.p)) == 97 )
		goto st6;
	goto st0;
st6:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof6;
case 6:
	if ( (*( state.p)) == 110 )
		goto st7;
	goto st0;
st7:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof7;
case 7:
	if ( (*( state.p)) == 100 )
		goto st8;
	goto st0;
st8:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof8;
case 8:
	switch( (*( state.p)) ) {
		case 13: goto st9;
		case 32: goto st9;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st9;
	goto st0;
st9:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof9;
case 9:
	switch( (*( state.p)) ) {
		case 13: goto st9;
		case 32: goto st9;
		case 34: goto tr18;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st9;
	goto st0;
tr18:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st10;
tr19:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st10;
tr21:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 10;goto st148;}}
	goto st10;
st10:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof10;
case 10:
#line 416 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st11;
		case 92: goto tr21;
	}
	goto tr19;
st11:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof11;
case 11:
	switch( (*( state.p)) ) {
		case 13: goto tr22;
		case 32: goto tr22;
		case 59: goto tr23;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr22;
	goto st0;
tr22:
#line 191 "config.rl"
	{ out.command.push_back(state.match); }
	goto st12;
st12:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof12;
case 12:
#line 442 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st12;
		case 32: goto st12;
		case 34: goto tr18;
		case 59: goto st149;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st12;
	goto st0;
tr23:
#line 191 "config.rl"
	{ out.command.push_back(state.match); }
	goto st149;
tr37:
#line 137 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st149;
tr53:
#line 143 "config.rl"
	{ out.fullscreen = false; }
	goto st149;
tr56:
#line 142 "config.rl"
	{ out.fullscreen = true; }
	goto st149;
tr69:
#line 177 "config.rl"
	{ add_map(out, utf8_glyph, utf8_mach, toint(state.match)); }
	goto st149;
tr83:
#line 171 "config.rl"
	{ out.palette = state.match; }
	goto st149;
tr98:
#line 166 "config.rl"
	{ out.polling_rate = toint(state.match); }
	goto st149;
tr116:
#line 149 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st149;
tr132:
#line 193 "config.rl"
	{ out.term_type = state.match; }
	goto st149;
tr145:
#line 139 "config.rl"
	{ out.tiles = state.match; }
	goto st149;
tr149:
#line 181 "config.rl"
	{ out.tiles.clear(); }
	goto st149;
tr160:
#line 155 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st149;
tr174:
#line 188 "config.rl"
	{ out.compression = false; }
	goto st149;
tr176:
#line 187 "config.rl"
	{ out.compression = true;  }
	goto st149;
tr194:
#line 161 "config.rl"
	{ out.port = toint(state.match); }
	goto st149;
tr205:
#line 184 "config.rl"
	{ out.cursor = false; }
	goto st149;
tr207:
#line 183 "config.rl"
	{ out.cursor = true;  }
	goto st149;
st149:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof149;
case 149:
#line 524 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st149;
		case 32: goto st149;
		case 99: goto st2;
		case 102: goto st13;
		case 109: goto st37;
		case 112: goto st45;
		case 115: goto st68;
		case 116: goto st82;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st149;
	goto st0;
st13:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof13;
case 13:
	switch( (*( state.p)) ) {
		case 111: goto st14;
		case 117: goto st22;
	}
	goto st0;
st14:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof14;
case 14:
	if ( (*( state.p)) == 110 )
		goto st15;
	goto st0;
st15:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof15;
case 15:
	if ( (*( state.p)) == 116 )
		goto st16;
	goto st0;
st16:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof16;
case 16:
	if ( (*( state.p)) == 115 )
		goto st17;
	goto st0;
st17:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof17;
case 17:
	switch( (*( state.p)) ) {
		case 13: goto st18;
		case 32: goto st18;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st18;
	goto st0;
st18:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof18;
case 18:
	switch( (*( state.p)) ) {
		case 13: goto st18;
		case 32: goto st18;
		case 34: goto tr32;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st18;
	goto st0;
tr32:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st19;
tr33:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st19;
tr35:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 19;goto st148;}}
	goto st19;
st19:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof19;
case 19:
#line 611 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st20;
		case 92: goto tr35;
	}
	goto tr33;
st20:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof20;
case 20:
	switch( (*( state.p)) ) {
		case 13: goto tr36;
		case 32: goto tr36;
		case 59: goto tr37;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr36;
	goto st0;
tr36:
#line 137 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st21;
st21:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof21;
case 21:
#line 637 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st21;
		case 32: goto st21;
		case 34: goto tr32;
		case 59: goto st149;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st21;
	goto st0;
st22:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof22;
case 22:
	if ( (*( state.p)) == 108 )
		goto st23;
	goto st0;
st23:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof23;
case 23:
	if ( (*( state.p)) == 108 )
		goto st24;
	goto st0;
st24:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof24;
case 24:
	if ( (*( state.p)) == 115 )
		goto st25;
	goto st0;
st25:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof25;
case 25:
	if ( (*( state.p)) == 99 )
		goto st26;
	goto st0;
st26:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof26;
case 26:
	if ( (*( state.p)) == 114 )
		goto st27;
	goto st0;
st27:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof27;
case 27:
	if ( (*( state.p)) == 101 )
		goto st28;
	goto st0;
st28:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof28;
case 28:
	if ( (*( state.p)) == 101 )
		goto st29;
	goto st0;
st29:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof29;
case 29:
	if ( (*( state.p)) == 110 )
		goto st30;
	goto st0;
st30:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof30;
case 30:
	switch( (*( state.p)) ) {
		case 13: goto st31;
		case 32: goto st31;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st31;
	goto st0;
st31:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof31;
case 31:
	switch( (*( state.p)) ) {
		case 13: goto st31;
		case 32: goto st31;
		case 111: goto st32;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st31;
	goto st0;
st32:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof32;
case 32:
	switch( (*( state.p)) ) {
		case 102: goto st33;
		case 110: goto st36;
	}
	goto st0;
st33:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof33;
case 33:
	if ( (*( state.p)) == 102 )
		goto st34;
	goto st0;
st34:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof34;
case 34:
	switch( (*( state.p)) ) {
		case 13: goto tr52;
		case 32: goto tr52;
		case 59: goto tr53;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr52;
	goto st0;
tr52:
#line 143 "config.rl"
	{ out.fullscreen = false; }
	goto st35;
tr55:
#line 142 "config.rl"
	{ out.fullscreen = true; }
	goto st35;
tr67:
#line 177 "config.rl"
	{ add_map(out, utf8_glyph, utf8_mach, toint(state.match)); }
	goto st35;
tr82:
#line 171 "config.rl"
	{ out.palette = state.match; }
	goto st35;
tr96:
#line 166 "config.rl"
	{ out.polling_rate = toint(state.match); }
	goto st35;
tr114:
#line 149 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st35;
tr144:
#line 139 "config.rl"
	{ out.tiles = state.match; }
	goto st35;
tr148:
#line 181 "config.rl"
	{ out.tiles.clear(); }
	goto st35;
tr158:
#line 155 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st35;
tr173:
#line 188 "config.rl"
	{ out.compression = false; }
	goto st35;
tr175:
#line 187 "config.rl"
	{ out.compression = true;  }
	goto st35;
tr192:
#line 161 "config.rl"
	{ out.port = toint(state.match); }
	goto st35;
tr204:
#line 184 "config.rl"
	{ out.cursor = false; }
	goto st35;
tr206:
#line 183 "config.rl"
	{ out.cursor = true;  }
	goto st35;
st35:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof35;
case 35:
#line 814 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st35;
		case 32: goto st35;
		case 59: goto st149;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st35;
	goto st0;
st36:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof36;
case 36:
	switch( (*( state.p)) ) {
		case 13: goto tr55;
		case 32: goto tr55;
		case 59: goto tr56;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr55;
	goto st0;
st37:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof37;
case 37:
	if ( (*( state.p)) == 97 )
		goto st38;
	goto st0;
st38:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof38;
case 38:
	if ( (*( state.p)) == 112 )
		goto st39;
	goto st0;
st39:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof39;
case 39:
	switch( (*( state.p)) ) {
		case 13: goto st40;
		case 32: goto st40;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st40;
	goto st0;
st40:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof40;
case 40:
	switch( (*( state.p)) ) {
		case 13: goto st40;
		case 32: goto st40;
		case 34: goto tr60;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st40;
	goto st0;
tr60:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st41;
tr61:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st41;
tr63:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 41;goto st148;}}
	goto st41;
st41:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof41;
case 41:
#line 892 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st42;
		case 92: goto tr63;
	}
	goto tr61;
st42:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof42;
case 42:
	switch( (*( state.p)) ) {
		case 13: goto tr64;
		case 32: goto tr64;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr64;
	goto st0;
tr64:
#line 176 "config.rl"
	{ utf8_glyph = state.match; }
	goto st43;
st43:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof43;
case 43:
#line 917 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st43;
		case 32: goto st43;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr66;
	} else if ( (*( state.p)) >= 9 )
		goto st43;
	goto st0;
tr68:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st44;
tr66:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st44;
st44:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof44;
case 44:
#line 948 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr67;
		case 32: goto tr67;
		case 59: goto tr69;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr68;
	} else if ( (*( state.p)) >= 9 )
		goto tr67;
	goto st0;
st45:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof45;
case 45:
	switch( (*( state.p)) ) {
		case 97: goto st46;
		case 111: goto st55;
	}
	goto st0;
st46:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof46;
case 46:
	if ( (*( state.p)) == 108 )
		goto st47;
	goto st0;
st47:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof47;
case 47:
	if ( (*( state.p)) == 101 )
		goto st48;
	goto st0;
st48:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof48;
case 48:
	if ( (*( state.p)) == 116 )
		goto st49;
	goto st0;
st49:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof49;
case 49:
	if ( (*( state.p)) == 116 )
		goto st50;
	goto st0;
st50:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof50;
case 50:
	if ( (*( state.p)) == 101 )
		goto st51;
	goto st0;
st51:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof51;
case 51:
	switch( (*( state.p)) ) {
		case 13: goto st52;
		case 32: goto st52;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st52;
	goto st0;
st52:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof52;
case 52:
	switch( (*( state.p)) ) {
		case 13: goto st52;
		case 32: goto st52;
		case 34: goto tr78;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st52;
	goto st0;
tr78:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st53;
tr79:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st53;
tr81:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 53;goto st148;}}
	goto st53;
st53:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof53;
case 53:
#line 1047 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st54;
		case 92: goto tr81;
	}
	goto tr79;
st54:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof54;
case 54:
	switch( (*( state.p)) ) {
		case 13: goto tr82;
		case 32: goto tr82;
		case 59: goto tr83;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr82;
	goto st0;
st55:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof55;
case 55:
	if ( (*( state.p)) == 108 )
		goto st56;
	goto st0;
st56:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof56;
case 56:
	if ( (*( state.p)) == 108 )
		goto st57;
	goto st0;
st57:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof57;
case 57:
	if ( (*( state.p)) == 105 )
		goto st58;
	goto st0;
st58:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof58;
case 58:
	if ( (*( state.p)) == 110 )
		goto st59;
	goto st0;
st59:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof59;
case 59:
	if ( (*( state.p)) == 103 )
		goto st60;
	goto st0;
st60:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof60;
case 60:
	if ( (*( state.p)) == 95 )
		goto st61;
	goto st0;
st61:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof61;
case 61:
	if ( (*( state.p)) == 114 )
		goto st62;
	goto st0;
st62:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof62;
case 62:
	if ( (*( state.p)) == 97 )
		goto st63;
	goto st0;
st63:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof63;
case 63:
	if ( (*( state.p)) == 116 )
		goto st64;
	goto st0;
st64:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof64;
case 64:
	if ( (*( state.p)) == 101 )
		goto st65;
	goto st0;
st65:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof65;
case 65:
	switch( (*( state.p)) ) {
		case 13: goto st66;
		case 32: goto st66;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st66;
	goto st0;
st66:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof66;
case 66:
	switch( (*( state.p)) ) {
		case 13: goto st66;
		case 32: goto st66;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr95;
	} else if ( (*( state.p)) >= 9 )
		goto st66;
	goto st0;
tr97:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st67;
tr95:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st67;
st67:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof67;
case 67:
#line 1180 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr96;
		case 32: goto tr96;
		case 59: goto tr98;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr97;
	} else if ( (*( state.p)) >= 9 )
		goto tr96;
	goto st0;
st68:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof68;
case 68:
	if ( (*( state.p)) == 99 )
		goto st69;
	goto st0;
st69:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof69;
case 69:
	if ( (*( state.p)) == 114 )
		goto st70;
	goto st0;
st70:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof70;
case 70:
	if ( (*( state.p)) == 101 )
		goto st71;
	goto st0;
st71:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof71;
case 71:
	if ( (*( state.p)) == 101 )
		goto st72;
	goto st0;
st72:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof72;
case 72:
	if ( (*( state.p)) == 110 )
		goto st73;
	goto st0;
st73:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof73;
case 73:
	if ( (*( state.p)) == 115 )
		goto st74;
	goto st0;
st74:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof74;
case 74:
	if ( (*( state.p)) == 105 )
		goto st75;
	goto st0;
st75:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof75;
case 75:
	if ( (*( state.p)) == 122 )
		goto st76;
	goto st0;
st76:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof76;
case 76:
	if ( (*( state.p)) == 101 )
		goto st77;
	goto st0;
st77:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof77;
case 77:
	switch( (*( state.p)) ) {
		case 13: goto st78;
		case 32: goto st78;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st78;
	goto st0;
st78:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof78;
case 78:
	switch( (*( state.p)) ) {
		case 13: goto st78;
		case 32: goto st78;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr109;
	} else if ( (*( state.p)) >= 9 )
		goto st78;
	goto st0;
tr111:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st79;
tr109:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st79;
st79:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof79;
case 79:
#line 1300 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr110;
		case 32: goto tr110;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr111;
	} else if ( (*( state.p)) >= 9 )
		goto tr110;
	goto st0;
tr110:
#line 148 "config.rl"
	{ out.screen_width = toint(state.match); }
	goto st80;
st80:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof80;
case 80:
#line 1319 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st80;
		case 32: goto st80;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr113;
	} else if ( (*( state.p)) >= 9 )
		goto st80;
	goto st0;
tr115:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st81;
tr113:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st81;
st81:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof81;
case 81:
#line 1350 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr114;
		case 32: goto tr114;
		case 59: goto tr116;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr115;
	} else if ( (*( state.p)) >= 9 )
		goto tr114;
	goto st0;
st82:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof82;
case 82:
	switch( (*( state.p)) ) {
		case 101: goto st83;
		case 105: goto st95;
	}
	goto st0;
st83:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof83;
case 83:
	if ( (*( state.p)) == 114 )
		goto st84;
	goto st0;
st84:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof84;
case 84:
	if ( (*( state.p)) == 109 )
		goto st85;
	goto st0;
st85:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof85;
case 85:
	if ( (*( state.p)) == 95 )
		goto st86;
	goto st0;
st86:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof86;
case 86:
	if ( (*( state.p)) == 116 )
		goto st87;
	goto st0;
st87:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof87;
case 87:
	if ( (*( state.p)) == 121 )
		goto st88;
	goto st0;
st88:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof88;
case 88:
	if ( (*( state.p)) == 112 )
		goto st89;
	goto st0;
st89:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof89;
case 89:
	if ( (*( state.p)) == 101 )
		goto st90;
	goto st0;
st90:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof90;
case 90:
	switch( (*( state.p)) ) {
		case 13: goto st91;
		case 32: goto st91;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st91;
	goto st0;
st91:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof91;
case 91:
	switch( (*( state.p)) ) {
		case 13: goto st91;
		case 32: goto st91;
		case 34: goto tr127;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st91;
	goto st0;
tr127:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st92;
tr128:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st92;
tr130:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 92;goto st148;}}
	goto st92;
st92:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof92;
case 92:
#line 1463 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st93;
		case 92: goto tr130;
	}
	goto tr128;
st93:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof93;
case 93:
	switch( (*( state.p)) ) {
		case 13: goto tr131;
		case 32: goto tr131;
		case 59: goto tr132;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr131;
	goto st0;
tr131:
#line 193 "config.rl"
	{ out.term_type = state.match; }
	goto st94;
st94:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof94;
case 94:
#line 1489 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st94;
		case 32: goto st94;
		case 34: goto tr127;
		case 59: goto st149;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st94;
	goto st0;
st95:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof95;
case 95:
	if ( (*( state.p)) == 108 )
		goto st96;
	goto st0;
st96:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof96;
case 96:
	if ( (*( state.p)) == 101 )
		goto st97;
	goto st0;
st97:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof97;
case 97:
	if ( (*( state.p)) == 115 )
		goto st98;
	goto st0;
st98:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof98;
case 98:
	switch( (*( state.p)) ) {
		case 13: goto st99;
		case 32: goto st99;
		case 105: goto st105;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st99;
	goto st0;
st99:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof99;
case 99:
	switch( (*( state.p)) ) {
		case 13: goto st99;
		case 32: goto st99;
		case 34: goto tr139;
		case 111: goto st102;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st99;
	goto st0;
tr139:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st100;
tr141:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st100;
tr143:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 100;goto st148;}}
	goto st100;
st100:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof100;
case 100:
#line 1565 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st101;
		case 92: goto tr143;
	}
	goto tr141;
st101:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof101;
case 101:
	switch( (*( state.p)) ) {
		case 13: goto tr144;
		case 32: goto tr144;
		case 59: goto tr145;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr144;
	goto st0;
st102:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof102;
case 102:
	if ( (*( state.p)) == 102 )
		goto st103;
	goto st0;
st103:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof103;
case 103:
	if ( (*( state.p)) == 102 )
		goto st104;
	goto st0;
st104:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof104;
case 104:
	switch( (*( state.p)) ) {
		case 13: goto tr148;
		case 32: goto tr148;
		case 59: goto tr149;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr148;
	goto st0;
st105:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof105;
case 105:
	if ( (*( state.p)) == 122 )
		goto st106;
	goto st0;
st106:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof106;
case 106:
	if ( (*( state.p)) == 101 )
		goto st107;
	goto st0;
st107:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof107;
case 107:
	switch( (*( state.p)) ) {
		case 13: goto st108;
		case 32: goto st108;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st108;
	goto st0;
st108:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof108;
case 108:
	switch( (*( state.p)) ) {
		case 13: goto st108;
		case 32: goto st108;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr153;
	} else if ( (*( state.p)) >= 9 )
		goto st108;
	goto st0;
tr155:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st109;
tr153:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st109;
st109:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof109;
case 109:
#line 1668 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr154;
		case 32: goto tr154;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr155;
	} else if ( (*( state.p)) >= 9 )
		goto tr154;
	goto st0;
tr154:
#line 154 "config.rl"
	{ out.tile_width = toint(state.match); }
	goto st110;
st110:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof110;
case 110:
#line 1687 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st110;
		case 32: goto st110;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr157;
	} else if ( (*( state.p)) >= 9 )
		goto st110;
	goto st0;
tr159:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st111;
tr157:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st111;
st111:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof111;
case 111:
#line 1718 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr158;
		case 32: goto tr158;
		case 59: goto tr160;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr159;
	} else if ( (*( state.p)) >= 9 )
		goto tr158;
	goto st0;
st112:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof112;
case 112:
	if ( (*( state.p)) == 114 )
		goto st113;
	goto st0;
st113:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof113;
case 113:
	if ( (*( state.p)) == 101 )
		goto st114;
	goto st0;
st114:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof114;
case 114:
	if ( (*( state.p)) == 115 )
		goto st115;
	goto st0;
st115:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof115;
case 115:
	if ( (*( state.p)) == 115 )
		goto st116;
	goto st0;
st116:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof116;
case 116:
	if ( (*( state.p)) == 105 )
		goto st117;
	goto st0;
st117:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof117;
case 117:
	if ( (*( state.p)) == 111 )
		goto st118;
	goto st0;
st118:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof118;
case 118:
	if ( (*( state.p)) == 110 )
		goto st119;
	goto st0;
st119:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof119;
case 119:
	switch( (*( state.p)) ) {
		case 13: goto st120;
		case 32: goto st120;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st120;
	goto st0;
st120:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof120;
case 120:
	switch( (*( state.p)) ) {
		case 13: goto st120;
		case 32: goto st120;
		case 111: goto st121;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st120;
	goto st0;
st121:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof121;
case 121:
	switch( (*( state.p)) ) {
		case 102: goto st122;
		case 110: goto st124;
	}
	goto st0;
st122:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof122;
case 122:
	if ( (*( state.p)) == 102 )
		goto st123;
	goto st0;
st123:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof123;
case 123:
	switch( (*( state.p)) ) {
		case 13: goto tr173;
		case 32: goto tr173;
		case 59: goto tr174;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr173;
	goto st0;
st124:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof124;
case 124:
	switch( (*( state.p)) ) {
		case 13: goto tr175;
		case 32: goto tr175;
		case 59: goto tr176;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr175;
	goto st0;
st125:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof125;
case 125:
	if ( (*( state.p)) == 110 )
		goto st126;
	goto st0;
st126:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof126;
case 126:
	if ( (*( state.p)) == 101 )
		goto st127;
	goto st0;
st127:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof127;
case 127:
	if ( (*( state.p)) == 99 )
		goto st128;
	goto st0;
st128:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof128;
case 128:
	if ( (*( state.p)) == 116 )
		goto st129;
	goto st0;
st129:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof129;
case 129:
	if ( (*( state.p)) == 95 )
		goto st130;
	goto st0;
st130:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof130;
case 130:
	if ( (*( state.p)) == 116 )
		goto st131;
	goto st0;
st131:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof131;
case 131:
	if ( (*( state.p)) == 111 )
		goto st132;
	goto st0;
st132:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof132;
case 132:
	switch( (*( state.p)) ) {
		case 13: goto st133;
		case 32: goto st133;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st133;
	goto st0;
st133:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof133;
case 133:
	switch( (*( state.p)) ) {
		case 13: goto st133;
		case 32: goto st133;
		case 34: goto tr185;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st133;
	goto st0;
tr185:
#line 100 "config.rl"
	{
            state.match.clear();
        }
	goto st134;
tr186:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st134;
tr188:
#line 131 "config.rl"
	{{ state.stack[ state.top++] = 134;goto st148;}}
	goto st134;
st134:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof134;
case 134:
#line 1934 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st135;
		case 92: goto tr188;
	}
	goto tr186;
st135:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof135;
case 135:
	switch( (*( state.p)) ) {
		case 13: goto tr189;
		case 32: goto tr189;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr189;
	goto st0;
tr189:
#line 160 "config.rl"
	{ out.host = state.match; }
	goto st136;
st136:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof136;
case 136:
#line 1959 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st136;
		case 32: goto st136;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr191;
	} else if ( (*( state.p)) >= 9 )
		goto st136;
	goto st0;
tr193:
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st137;
tr191:
#line 100 "config.rl"
	{
            state.match.clear();
        }
#line 104 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st137;
st137:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof137;
case 137:
#line 1990 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr192;
		case 32: goto tr192;
		case 59: goto tr194;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr193;
	} else if ( (*( state.p)) >= 9 )
		goto tr192;
	goto st0;
st138:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof138;
case 138:
	if ( (*( state.p)) == 114 )
		goto st139;
	goto st0;
st139:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof139;
case 139:
	if ( (*( state.p)) == 115 )
		goto st140;
	goto st0;
st140:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof140;
case 140:
	if ( (*( state.p)) == 111 )
		goto st141;
	goto st0;
st141:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof141;
case 141:
	if ( (*( state.p)) == 114 )
		goto st142;
	goto st0;
st142:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof142;
case 142:
	switch( (*( state.p)) ) {
		case 13: goto st143;
		case 32: goto st143;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st143;
	goto st0;
st143:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof143;
case 143:
	switch( (*( state.p)) ) {
		case 13: goto st143;
		case 32: goto st143;
		case 111: goto st144;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st143;
	goto st0;
st144:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof144;
case 144:
	switch( (*( state.p)) ) {
		case 102: goto st145;
		case 110: goto st147;
	}
	goto st0;
st145:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof145;
case 145:
	if ( (*( state.p)) == 102 )
		goto st146;
	goto st0;
st146:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof146;
case 146:
	switch( (*( state.p)) ) {
		case 13: goto tr204;
		case 32: goto tr204;
		case 59: goto tr205;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr204;
	goto st0;
st147:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof147;
case 147:
	switch( (*( state.p)) ) {
		case 13: goto tr206;
		case 32: goto tr206;
		case 59: goto tr207;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr206;
	goto st0;
st148:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof148;
case 148:
	switch( (*( state.p)) ) {
		case 34: goto tr208;
		case 49: goto tr209;
		case 50: goto tr210;
		case 51: goto tr211;
		case 92: goto tr212;
		case 110: goto tr213;
	}
	goto st0;
tr208:
#line 122 "config.rl"
	{ state.match += '"';  { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
tr209:
#line 124 "config.rl"
	{ state.match += '\1'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
tr210:
#line 125 "config.rl"
	{ state.match += '\2'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
tr211:
#line 126 "config.rl"
	{ state.match += '\3'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
tr212:
#line 127 "config.rl"
	{ state.match += '\\'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
tr213:
#line 123 "config.rl"
	{ state.match += '\n'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st150;
st150:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof150;
case 150:
#line 2134 "config.h"
	goto st0;
	}
	_test_eof1:  state.cs = 1; goto _test_eof; 
	_test_eof2:  state.cs = 2; goto _test_eof; 
	_test_eof3:  state.cs = 3; goto _test_eof; 
	_test_eof4:  state.cs = 4; goto _test_eof; 
	_test_eof5:  state.cs = 5; goto _test_eof; 
	_test_eof6:  state.cs = 6; goto _test_eof; 
	_test_eof7:  state.cs = 7; goto _test_eof; 
	_test_eof8:  state.cs = 8; goto _test_eof; 
	_test_eof9:  state.cs = 9; goto _test_eof; 
	_test_eof10:  state.cs = 10; goto _test_eof; 
	_test_eof11:  state.cs = 11; goto _test_eof; 
	_test_eof12:  state.cs = 12; goto _test_eof; 
	_test_eof149:  state.cs = 149; goto _test_eof; 
	_test_eof13:  state.cs = 13; goto _test_eof; 
	_test_eof14:  state.cs = 14; goto _test_eof; 
	_test_eof15:  state.cs = 15; goto _test_eof; 
	_test_eof16:  state.cs = 16; goto _test_eof; 
	_test_eof17:  state.cs = 17; goto _test_eof; 
	_test_eof18:  state.cs = 18; goto _test_eof; 
	_test_eof19:  state.cs = 19; goto _test_eof; 
	_test_eof20:  state.cs = 20; goto _test_eof; 
	_test_eof21:  state.cs = 21; goto _test_eof; 
	_test_eof22:  state.cs = 22; goto _test_eof; 
	_test_eof23:  state.cs = 23; goto _test_eof; 
	_test_eof24:  state.cs = 24; goto _test_eof; 
	_test_eof25:  state.cs = 25; goto _test_eof; 
	_test_eof26:  state.cs = 26; goto _test_eof; 
	_test_eof27:  state.cs = 27; goto _test_eof; 
	_test_eof28:  state.cs = 28; goto _test_eof; 
	_test_eof29:  state.cs = 29; goto _test_eof; 
	_test_eof30:  state.cs = 30; goto _test_eof; 
	_test_eof31:  state.cs = 31; goto _test_eof; 
	_test_eof32:  state.cs = 32; goto _test_eof; 
	_test_eof33:  state.cs = 33; goto _test_eof; 
	_test_eof34:  state.cs = 34; goto _test_eof; 
	_test_eof35:  state.cs = 35; goto _test_eof; 
	_test_eof36:  state.cs = 36; goto _test_eof; 
	_test_eof37:  state.cs = 37; goto _test_eof; 
	_test_eof38:  state.cs = 38; goto _test_eof; 
	_test_eof39:  state.cs = 39; goto _test_eof; 
	_test_eof40:  state.cs = 40; goto _test_eof; 
	_test_eof41:  state.cs = 41; goto _test_eof; 
	_test_eof42:  state.cs = 42; goto _test_eof; 
	_test_eof43:  state.cs = 43; goto _test_eof; 
	_test_eof44:  state.cs = 44; goto _test_eof; 
	_test_eof45:  state.cs = 45; goto _test_eof; 
	_test_eof46:  state.cs = 46; goto _test_eof; 
	_test_eof47:  state.cs = 47; goto _test_eof; 
	_test_eof48:  state.cs = 48; goto _test_eof; 
	_test_eof49:  state.cs = 49; goto _test_eof; 
	_test_eof50:  state.cs = 50; goto _test_eof; 
	_test_eof51:  state.cs = 51; goto _test_eof; 
	_test_eof52:  state.cs = 52; goto _test_eof; 
	_test_eof53:  state.cs = 53; goto _test_eof; 
	_test_eof54:  state.cs = 54; goto _test_eof; 
	_test_eof55:  state.cs = 55; goto _test_eof; 
	_test_eof56:  state.cs = 56; goto _test_eof; 
	_test_eof57:  state.cs = 57; goto _test_eof; 
	_test_eof58:  state.cs = 58; goto _test_eof; 
	_test_eof59:  state.cs = 59; goto _test_eof; 
	_test_eof60:  state.cs = 60; goto _test_eof; 
	_test_eof61:  state.cs = 61; goto _test_eof; 
	_test_eof62:  state.cs = 62; goto _test_eof; 
	_test_eof63:  state.cs = 63; goto _test_eof; 
	_test_eof64:  state.cs = 64; goto _test_eof; 
	_test_eof65:  state.cs = 65; goto _test_eof; 
	_test_eof66:  state.cs = 66; goto _test_eof; 
	_test_eof67:  state.cs = 67; goto _test_eof; 
	_test_eof68:  state.cs = 68; goto _test_eof; 
	_test_eof69:  state.cs = 69; goto _test_eof; 
	_test_eof70:  state.cs = 70; goto _test_eof; 
	_test_eof71:  state.cs = 71; goto _test_eof; 
	_test_eof72:  state.cs = 72; goto _test_eof; 
	_test_eof73:  state.cs = 73; goto _test_eof; 
	_test_eof74:  state.cs = 74; goto _test_eof; 
	_test_eof75:  state.cs = 75; goto _test_eof; 
	_test_eof76:  state.cs = 76; goto _test_eof; 
	_test_eof77:  state.cs = 77; goto _test_eof; 
	_test_eof78:  state.cs = 78; goto _test_eof; 
	_test_eof79:  state.cs = 79; goto _test_eof; 
	_test_eof80:  state.cs = 80; goto _test_eof; 
	_test_eof81:  state.cs = 81; goto _test_eof; 
	_test_eof82:  state.cs = 82; goto _test_eof; 
	_test_eof83:  state.cs = 83; goto _test_eof; 
	_test_eof84:  state.cs = 84; goto _test_eof; 
	_test_eof85:  state.cs = 85; goto _test_eof; 
	_test_eof86:  state.cs = 86; goto _test_eof; 
	_test_eof87:  state.cs = 87; goto _test_eof; 
	_test_eof88:  state.cs = 88; goto _test_eof; 
	_test_eof89:  state.cs = 89; goto _test_eof; 
	_test_eof90:  state.cs = 90; goto _test_eof; 
	_test_eof91:  state.cs = 91; goto _test_eof; 
	_test_eof92:  state.cs = 92; goto _test_eof; 
	_test_eof93:  state.cs = 93; goto _test_eof; 
	_test_eof94:  state.cs = 94; goto _test_eof; 
	_test_eof95:  state.cs = 95; goto _test_eof; 
	_test_eof96:  state.cs = 96; goto _test_eof; 
	_test_eof97:  state.cs = 97; goto _test_eof; 
	_test_eof98:  state.cs = 98; goto _test_eof; 
	_test_eof99:  state.cs = 99; goto _test_eof; 
	_test_eof100:  state.cs = 100; goto _test_eof; 
	_test_eof101:  state.cs = 101; goto _test_eof; 
	_test_eof102:  state.cs = 102; goto _test_eof; 
	_test_eof103:  state.cs = 103; goto _test_eof; 
	_test_eof104:  state.cs = 104; goto _test_eof; 
	_test_eof105:  state.cs = 105; goto _test_eof; 
	_test_eof106:  state.cs = 106; goto _test_eof; 
	_test_eof107:  state.cs = 107; goto _test_eof; 
	_test_eof108:  state.cs = 108; goto _test_eof; 
	_test_eof109:  state.cs = 109; goto _test_eof; 
	_test_eof110:  state.cs = 110; goto _test_eof; 
	_test_eof111:  state.cs = 111; goto _test_eof; 
	_test_eof112:  state.cs = 112; goto _test_eof; 
	_test_eof113:  state.cs = 113; goto _test_eof; 
	_test_eof114:  state.cs = 114; goto _test_eof; 
	_test_eof115:  state.cs = 115; goto _test_eof; 
	_test_eof116:  state.cs = 116; goto _test_eof; 
	_test_eof117:  state.cs = 117; goto _test_eof; 
	_test_eof118:  state.cs = 118; goto _test_eof; 
	_test_eof119:  state.cs = 119; goto _test_eof; 
	_test_eof120:  state.cs = 120; goto _test_eof; 
	_test_eof121:  state.cs = 121; goto _test_eof; 
	_test_eof122:  state.cs = 122; goto _test_eof; 
	_test_eof123:  state.cs = 123; goto _test_eof; 
	_test_eof124:  state.cs = 124; goto _test_eof; 
	_test_eof125:  state.cs = 125; goto _test_eof; 
	_test_eof126:  state.cs = 126; goto _test_eof; 
	_test_eof127:  state.cs = 127; goto _test_eof; 
	_test_eof128:  state.cs = 128; goto _test_eof; 
	_test_eof129:  state.cs = 129; goto _test_eof; 
	_test_eof130:  state.cs = 130; goto _test_eof; 
	_test_eof131:  state.cs = 131; goto _test_eof; 
	_test_eof132:  state.cs = 132; goto _test_eof; 
	_test_eof133:  state.cs = 133; goto _test_eof; 
	_test_eof134:  state.cs = 134; goto _test_eof; 
	_test_eof135:  state.cs = 135; goto _test_eof; 
	_test_eof136:  state.cs = 136; goto _test_eof; 
	_test_eof137:  state.cs = 137; goto _test_eof; 
	_test_eof138:  state.cs = 138; goto _test_eof; 
	_test_eof139:  state.cs = 139; goto _test_eof; 
	_test_eof140:  state.cs = 140; goto _test_eof; 
	_test_eof141:  state.cs = 141; goto _test_eof; 
	_test_eof142:  state.cs = 142; goto _test_eof; 
	_test_eof143:  state.cs = 143; goto _test_eof; 
	_test_eof144:  state.cs = 144; goto _test_eof; 
	_test_eof145:  state.cs = 145; goto _test_eof; 
	_test_eof146:  state.cs = 146; goto _test_eof; 
	_test_eof147:  state.cs = 147; goto _test_eof; 
	_test_eof148:  state.cs = 148; goto _test_eof; 
	_test_eof150:  state.cs = 150; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 227 "config.rl"

        // Avoid spurious gcc warnings.
        (void)ConfigParser_en_main;
        (void)ConfigParser_first_final;
        (void)ConfigParser_en_strchar_escape;

        if (state.cs == ConfigParser_error) {
            throw std::runtime_error("Parse error. Unconsumed input: " + std::string(state.p, state.pe));
        }
    }

    tsm_utf8_mach_free(utf8_mach);

    ::fclose(fn);
}

}


#endif
