
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
        std::cout << "? " << (int)c << std::endl;

        int x = tsm_utf8_mach_feed(utf8_mach, c);

        if (x == TSM_UTF8_ACCEPT || x == TSM_UTF8_REJECT) {
            uint32_t ucs = tsm_utf8_mach_get(utf8_mach);
            out.tile_mapping[ucs] = tile;
            std::cout << " map " << ucs << " " << tile << std::endl;
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

    
#line 185 "config.rl"

/*

*/

    
#line 104 "config.h"
static const int ConfigParser_start = 1;
static const int ConfigParser_first_final = 93;
static const int ConfigParser_error = 0;

static const int ConfigParser_en_strchar_escape = 92;
static const int ConfigParser_en_main = 1;


#line 191 "config.rl"
    
#line 115 "config.h"
	{
	 state.cs = ConfigParser_start;
	 state.top = 0;
	}

#line 192 "config.rl"

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

        
#line 140 "config.h"
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
		case 13: goto st13;
		case 14: goto st14;
		case 15: goto st15;
		case 16: goto st16;
		case 17: goto st17;
		case 93: goto st93;
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
		case 94: goto st94;
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
		case 102: goto st18;
		case 109: goto st41;
		case 112: goto st49;
		case 115: goto st63;
		case 116: goto st77;
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
	if ( (*( state.p)) == 111 )
		goto st3;
	goto st0;
st3:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof3;
case 3:
	if ( (*( state.p)) == 110 )
		goto st4;
	goto st0;
st4:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof4;
case 4:
	if ( (*( state.p)) == 110 )
		goto st5;
	goto st0;
st5:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof5;
case 5:
	if ( (*( state.p)) == 101 )
		goto st6;
	goto st0;
st6:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof6;
case 6:
	if ( (*( state.p)) == 99 )
		goto st7;
	goto st0;
st7:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof7;
case 7:
	if ( (*( state.p)) == 116 )
		goto st8;
	goto st0;
st8:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof8;
case 8:
	if ( (*( state.p)) == 95 )
		goto st9;
	goto st0;
st9:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof9;
case 9:
	if ( (*( state.p)) == 116 )
		goto st10;
	goto st0;
st10:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof10;
case 10:
	if ( (*( state.p)) == 111 )
		goto st11;
	goto st0;
st11:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof11;
case 11:
	switch( (*( state.p)) ) {
		case 13: goto st12;
		case 32: goto st12;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st12;
	goto st0;
st12:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof12;
case 12:
	switch( (*( state.p)) ) {
		case 13: goto st12;
		case 32: goto st12;
		case 34: goto tr18;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st12;
	goto st0;
tr18:
#line 102 "config.rl"
	{
            state.match.clear();
        }
	goto st13;
tr19:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st13;
tr21:
#line 133 "config.rl"
	{{ state.stack[ state.top++] = 13; goto st92;}}
	goto st13;
st13:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof13;
case 13:
#line 377 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st14;
		case 92: goto tr21;
	}
	goto tr19;
st14:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof14;
case 14:
	switch( (*( state.p)) ) {
		case 13: goto tr22;
		case 32: goto tr22;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr22;
	goto st0;
tr22:
#line 162 "config.rl"
	{ out.host = state.match; }
	goto st15;
st15:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof15;
case 15:
#line 402 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st15;
		case 32: goto st15;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr24;
	} else if ( (*( state.p)) >= 9 )
		goto st15;
	goto st0;
tr26:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st16;
tr24:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st16;
st16:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof16;
case 16:
#line 433 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr25;
		case 32: goto tr25;
		case 59: goto tr27;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr26;
	} else if ( (*( state.p)) >= 9 )
		goto tr25;
	goto st0;
tr25:
#line 163 "config.rl"
	{ out.port = toint(state.match); }
	goto st17;
tr56:
#line 145 "config.rl"
	{ out.fullscreen = false; }
	goto st17;
tr58:
#line 144 "config.rl"
	{ out.fullscreen = true; }
	goto st17;
tr70:
#line 174 "config.rl"
	{ add_map(out, utf8_glyph, utf8_mach, toint(state.match)); }
	goto st17;
tr86:
#line 168 "config.rl"
	{ out.polling_rate = toint(state.match); }
	goto st17;
tr104:
#line 151 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st17;
tr117:
#line 141 "config.rl"
	{ out.tiles = state.match; }
	goto st17;
tr127:
#line 157 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st17;
st17:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof17;
case 17:
#line 481 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st17;
		case 32: goto st17;
		case 59: goto st93;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st17;
	goto st0;
tr27:
#line 163 "config.rl"
	{ out.port = toint(state.match); }
	goto st93;
tr41:
#line 139 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st93;
tr57:
#line 145 "config.rl"
	{ out.fullscreen = false; }
	goto st93;
tr59:
#line 144 "config.rl"
	{ out.fullscreen = true; }
	goto st93;
tr72:
#line 174 "config.rl"
	{ add_map(out, utf8_glyph, utf8_mach, toint(state.match)); }
	goto st93;
tr88:
#line 168 "config.rl"
	{ out.polling_rate = toint(state.match); }
	goto st93;
tr106:
#line 151 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st93;
tr118:
#line 141 "config.rl"
	{ out.tiles = state.match; }
	goto st93;
tr129:
#line 157 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st93;
st93:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof93;
case 93:
#line 530 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st93;
		case 32: goto st93;
		case 99: goto st2;
		case 102: goto st18;
		case 109: goto st41;
		case 112: goto st49;
		case 115: goto st63;
		case 116: goto st77;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st93;
	goto st0;
st18:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof18;
case 18:
	switch( (*( state.p)) ) {
		case 111: goto st19;
		case 117: goto st27;
	}
	goto st0;
st19:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof19;
case 19:
	if ( (*( state.p)) == 110 )
		goto st20;
	goto st0;
st20:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof20;
case 20:
	if ( (*( state.p)) == 116 )
		goto st21;
	goto st0;
st21:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof21;
case 21:
	if ( (*( state.p)) == 115 )
		goto st22;
	goto st0;
st22:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof22;
case 22:
	switch( (*( state.p)) ) {
		case 13: goto st23;
		case 32: goto st23;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st23;
	goto st0;
st23:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof23;
case 23:
	switch( (*( state.p)) ) {
		case 13: goto st23;
		case 32: goto st23;
		case 34: goto tr36;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st23;
	goto st0;
tr36:
#line 102 "config.rl"
	{
            state.match.clear();
        }
	goto st24;
tr37:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st24;
tr39:
#line 133 "config.rl"
	{{ state.stack[ state.top++] = 24; goto st92;}}
	goto st24;
st24:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof24;
case 24:
#line 617 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st25;
		case 92: goto tr39;
	}
	goto tr37;
st25:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof25;
case 25:
	switch( (*( state.p)) ) {
		case 13: goto tr40;
		case 32: goto tr40;
		case 59: goto tr41;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr40;
	goto st0;
tr40:
#line 139 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st26;
st26:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof26;
case 26:
#line 643 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st26;
		case 32: goto st26;
		case 34: goto tr36;
		case 59: goto st93;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st26;
	goto st0;
st27:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof27;
case 27:
	if ( (*( state.p)) == 108 )
		goto st28;
	goto st0;
st28:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof28;
case 28:
	if ( (*( state.p)) == 108 )
		goto st29;
	goto st0;
st29:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof29;
case 29:
	if ( (*( state.p)) == 115 )
		goto st30;
	goto st0;
st30:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof30;
case 30:
	if ( (*( state.p)) == 99 )
		goto st31;
	goto st0;
st31:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof31;
case 31:
	if ( (*( state.p)) == 114 )
		goto st32;
	goto st0;
st32:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof32;
case 32:
	if ( (*( state.p)) == 101 )
		goto st33;
	goto st0;
st33:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof33;
case 33:
	if ( (*( state.p)) == 101 )
		goto st34;
	goto st0;
st34:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof34;
case 34:
	if ( (*( state.p)) == 110 )
		goto st35;
	goto st0;
st35:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof35;
case 35:
	switch( (*( state.p)) ) {
		case 13: goto st36;
		case 32: goto st36;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st36;
	goto st0;
st36:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof36;
case 36:
	switch( (*( state.p)) ) {
		case 13: goto st36;
		case 32: goto st36;
		case 111: goto st37;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st36;
	goto st0;
st37:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof37;
case 37:
	switch( (*( state.p)) ) {
		case 102: goto st38;
		case 110: goto st40;
	}
	goto st0;
st38:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof38;
case 38:
	if ( (*( state.p)) == 102 )
		goto st39;
	goto st0;
st39:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof39;
case 39:
	switch( (*( state.p)) ) {
		case 13: goto tr56;
		case 32: goto tr56;
		case 59: goto tr57;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr56;
	goto st0;
st40:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof40;
case 40:
	switch( (*( state.p)) ) {
		case 13: goto tr58;
		case 32: goto tr58;
		case 59: goto tr59;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr58;
	goto st0;
st41:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof41;
case 41:
	if ( (*( state.p)) == 97 )
		goto st42;
	goto st0;
st42:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof42;
case 42:
	if ( (*( state.p)) == 112 )
		goto st43;
	goto st0;
st43:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof43;
case 43:
	switch( (*( state.p)) ) {
		case 13: goto st44;
		case 32: goto st44;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st44;
	goto st0;
st44:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof44;
case 44:
	switch( (*( state.p)) ) {
		case 13: goto st44;
		case 32: goto st44;
		case 34: goto tr63;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st44;
	goto st0;
tr63:
#line 102 "config.rl"
	{
            state.match.clear();
        }
	goto st45;
tr64:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st45;
tr66:
#line 133 "config.rl"
	{{ state.stack[ state.top++] = 45; goto st92;}}
	goto st45;
st45:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof45;
case 45:
#line 829 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st46;
		case 92: goto tr66;
	}
	goto tr64;
st46:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof46;
case 46:
	switch( (*( state.p)) ) {
		case 13: goto tr67;
		case 32: goto tr67;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr67;
	goto st0;
tr67:
#line 173 "config.rl"
	{ utf8_glyph = state.match; }
	goto st47;
st47:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof47;
case 47:
#line 854 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st47;
		case 32: goto st47;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr69;
	} else if ( (*( state.p)) >= 9 )
		goto st47;
	goto st0;
tr71:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st48;
tr69:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st48;
st48:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof48;
case 48:
#line 885 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr70;
		case 32: goto tr70;
		case 59: goto tr72;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr71;
	} else if ( (*( state.p)) >= 9 )
		goto tr70;
	goto st0;
st49:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof49;
case 49:
	if ( (*( state.p)) == 111 )
		goto st50;
	goto st0;
st50:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof50;
case 50:
	if ( (*( state.p)) == 108 )
		goto st51;
	goto st0;
st51:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof51;
case 51:
	if ( (*( state.p)) == 108 )
		goto st52;
	goto st0;
st52:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof52;
case 52:
	if ( (*( state.p)) == 105 )
		goto st53;
	goto st0;
st53:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof53;
case 53:
	if ( (*( state.p)) == 110 )
		goto st54;
	goto st0;
st54:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof54;
case 54:
	if ( (*( state.p)) == 103 )
		goto st55;
	goto st0;
st55:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof55;
case 55:
	if ( (*( state.p)) == 95 )
		goto st56;
	goto st0;
st56:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof56;
case 56:
	if ( (*( state.p)) == 114 )
		goto st57;
	goto st0;
st57:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof57;
case 57:
	if ( (*( state.p)) == 97 )
		goto st58;
	goto st0;
st58:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof58;
case 58:
	if ( (*( state.p)) == 116 )
		goto st59;
	goto st0;
st59:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof59;
case 59:
	if ( (*( state.p)) == 101 )
		goto st60;
	goto st0;
st60:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof60;
case 60:
	switch( (*( state.p)) ) {
		case 13: goto st61;
		case 32: goto st61;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st61;
	goto st0;
st61:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof61;
case 61:
	switch( (*( state.p)) ) {
		case 13: goto st61;
		case 32: goto st61;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr85;
	} else if ( (*( state.p)) >= 9 )
		goto st61;
	goto st0;
tr87:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st62;
tr85:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st62;
st62:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof62;
case 62:
#line 1019 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr86;
		case 32: goto tr86;
		case 59: goto tr88;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr87;
	} else if ( (*( state.p)) >= 9 )
		goto tr86;
	goto st0;
st63:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof63;
case 63:
	if ( (*( state.p)) == 99 )
		goto st64;
	goto st0;
st64:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof64;
case 64:
	if ( (*( state.p)) == 114 )
		goto st65;
	goto st0;
st65:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof65;
case 65:
	if ( (*( state.p)) == 101 )
		goto st66;
	goto st0;
st66:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof66;
case 66:
	if ( (*( state.p)) == 101 )
		goto st67;
	goto st0;
st67:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof67;
case 67:
	if ( (*( state.p)) == 110 )
		goto st68;
	goto st0;
st68:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof68;
case 68:
	if ( (*( state.p)) == 115 )
		goto st69;
	goto st0;
st69:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof69;
case 69:
	if ( (*( state.p)) == 105 )
		goto st70;
	goto st0;
st70:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof70;
case 70:
	if ( (*( state.p)) == 122 )
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
	switch( (*( state.p)) ) {
		case 13: goto st73;
		case 32: goto st73;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st73;
	goto st0;
st73:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof73;
case 73:
	switch( (*( state.p)) ) {
		case 13: goto st73;
		case 32: goto st73;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr99;
	} else if ( (*( state.p)) >= 9 )
		goto st73;
	goto st0;
tr101:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st74;
tr99:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st74;
st74:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof74;
case 74:
#line 1139 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr100;
		case 32: goto tr100;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr101;
	} else if ( (*( state.p)) >= 9 )
		goto tr100;
	goto st0;
tr100:
#line 150 "config.rl"
	{ out.screen_width = toint(state.match); }
	goto st75;
st75:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof75;
case 75:
#line 1158 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st75;
		case 32: goto st75;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr103;
	} else if ( (*( state.p)) >= 9 )
		goto st75;
	goto st0;
tr105:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st76;
tr103:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st76;
st76:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof76;
case 76:
#line 1189 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr104;
		case 32: goto tr104;
		case 59: goto tr106;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr105;
	} else if ( (*( state.p)) >= 9 )
		goto tr104;
	goto st0;
st77:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof77;
case 77:
	if ( (*( state.p)) == 105 )
		goto st78;
	goto st0;
st78:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof78;
case 78:
	if ( (*( state.p)) == 108 )
		goto st79;
	goto st0;
st79:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof79;
case 79:
	if ( (*( state.p)) == 101 )
		goto st80;
	goto st0;
st80:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof80;
case 80:
	if ( (*( state.p)) == 115 )
		goto st81;
	goto st0;
st81:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof81;
case 81:
	switch( (*( state.p)) ) {
		case 13: goto st82;
		case 32: goto st82;
		case 105: goto st85;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st82;
	goto st0;
st82:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof82;
case 82:
	switch( (*( state.p)) ) {
		case 13: goto st82;
		case 32: goto st82;
		case 34: goto tr113;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st82;
	goto st0;
tr113:
#line 102 "config.rl"
	{
            state.match.clear();
        }
	goto st83;
tr114:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st83;
tr116:
#line 133 "config.rl"
	{{ state.stack[ state.top++] = 83; goto st92;}}
	goto st83;
st83:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof83;
case 83:
#line 1273 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st84;
		case 92: goto tr116;
	}
	goto tr114;
st84:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof84;
case 84:
	switch( (*( state.p)) ) {
		case 13: goto tr117;
		case 32: goto tr117;
		case 59: goto tr118;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr117;
	goto st0;
st85:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof85;
case 85:
	if ( (*( state.p)) == 122 )
		goto st86;
	goto st0;
st86:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof86;
case 86:
	if ( (*( state.p)) == 101 )
		goto st87;
	goto st0;
st87:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof87;
case 87:
	switch( (*( state.p)) ) {
		case 13: goto st88;
		case 32: goto st88;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st88;
	goto st0;
st88:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof88;
case 88:
	switch( (*( state.p)) ) {
		case 13: goto st88;
		case 32: goto st88;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr122;
	} else if ( (*( state.p)) >= 9 )
		goto st88;
	goto st0;
tr124:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st89;
tr122:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st89;
st89:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof89;
case 89:
#line 1350 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr123;
		case 32: goto tr123;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr124;
	} else if ( (*( state.p)) >= 9 )
		goto tr123;
	goto st0;
tr123:
#line 156 "config.rl"
	{ out.tile_width = toint(state.match); }
	goto st90;
st90:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof90;
case 90:
#line 1369 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st90;
		case 32: goto st90;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr126;
	} else if ( (*( state.p)) >= 9 )
		goto st90;
	goto st0;
tr128:
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st91;
tr126:
#line 102 "config.rl"
	{
            state.match.clear();
        }
#line 106 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st91;
st91:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof91;
case 91:
#line 1400 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr127;
		case 32: goto tr127;
		case 59: goto tr129;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr128;
	} else if ( (*( state.p)) >= 9 )
		goto tr127;
	goto st0;
st92:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof92;
case 92:
	switch( (*( state.p)) ) {
		case 34: goto tr130;
		case 49: goto tr131;
		case 50: goto tr132;
		case 51: goto tr133;
		case 92: goto tr134;
		case 110: goto tr135;
	}
	goto st0;
tr130:
#line 124 "config.rl"
	{ state.match += '"';  { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
tr131:
#line 126 "config.rl"
	{ state.match += '\1'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
tr132:
#line 127 "config.rl"
	{ state.match += '\2'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
tr133:
#line 128 "config.rl"
	{ state.match += '\3'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
tr134:
#line 129 "config.rl"
	{ state.match += '\\'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
tr135:
#line 125 "config.rl"
	{ state.match += '\n'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st94;
st94:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof94;
case 94:
#line 1453 "config.h"
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
	_test_eof13:  state.cs = 13; goto _test_eof; 
	_test_eof14:  state.cs = 14; goto _test_eof; 
	_test_eof15:  state.cs = 15; goto _test_eof; 
	_test_eof16:  state.cs = 16; goto _test_eof; 
	_test_eof17:  state.cs = 17; goto _test_eof; 
	_test_eof93:  state.cs = 93; goto _test_eof; 
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
	_test_eof94:  state.cs = 94; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 210 "config.rl"

        // Avoid spurious gcc warnings.
        (void)ConfigParser_en_main;
        (void)ConfigParser_first_final;
        (void)ConfigParser_en_strchar_escape;

        if (state.cs == ConfigParser_error) {
            throw std::runtime_error("Parse error. Unconsumed input: " + std::string(state.p, state.pe));
        }
    }

    tsm_utf8_mach_free(utf8_mach);

    std::cout << "Done parsing config." << std::endl;

    for (const auto& f : out.fonts)
        std::cout << "font " << f << std::endl;

    std::cout << "tilesize " << out.tile_width << " " << out.tile_height << std::endl;
    std::cout << "screensize " << out.screen_width << " " << out.screen_height << std::endl;
    std::cout << "fullscreen " << out.fullscreen << std::endl;
}

}


#endif
