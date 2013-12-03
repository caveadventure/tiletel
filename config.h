
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


    
#line 146 "config.rl"

/*

*/

    
#line 79 "config.h"
static const int ConfigParser_start = 1;
static const int ConfigParser_first_final = 68;
static const int ConfigParser_error = 0;

static const int ConfigParser_en_strchar_escape = 67;
static const int ConfigParser_en_main = 1;


#line 152 "config.rl"
    
#line 90 "config.h"
	{
	 state.cs = ConfigParser_start;
	 state.top = 0;
	}

#line 153 "config.rl"

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

        
#line 115 "config.h"
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
		case 68: goto st68;
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
		case 69: goto st69;
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
		case 115: goto st41;
		case 116: goto st55;
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
		case 34: goto tr16;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st12;
	goto st0;
tr16:
#line 77 "config.rl"
	{
            state.match.clear();
        }
	goto st13;
tr17:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st13;
tr19:
#line 108 "config.rl"
	{{ state.stack[ state.top++] = 13; goto st67;}}
	goto st13;
st13:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof13;
case 13:
#line 325 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st14;
		case 92: goto tr19;
	}
	goto tr17;
st14:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof14;
case 14:
	switch( (*( state.p)) ) {
		case 13: goto tr20;
		case 32: goto tr20;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr20;
	goto st0;
tr20:
#line 135 "config.rl"
	{ out.host = state.match; }
	goto st15;
st15:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof15;
case 15:
#line 350 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st15;
		case 32: goto st15;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr22;
	} else if ( (*( state.p)) >= 9 )
		goto st15;
	goto st0;
tr24:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st16;
tr22:
#line 77 "config.rl"
	{
            state.match.clear();
        }
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st16;
st16:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof16;
case 16:
#line 381 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr23;
		case 32: goto tr23;
		case 59: goto tr25;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr24;
	} else if ( (*( state.p)) >= 9 )
		goto tr23;
	goto st0;
tr23:
#line 136 "config.rl"
	{ out.port = toint(state.match); }
	goto st17;
tr54:
#line 118 "config.rl"
	{ out.fullscreen = false; }
	goto st17;
tr56:
#line 117 "config.rl"
	{ out.fullscreen = true; }
	goto st17;
tr73:
#line 124 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st17;
tr89:
#line 130 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st17;
st17:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof17;
case 17:
#line 417 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st17;
		case 32: goto st17;
		case 59: goto st68;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st17;
	goto st0;
tr25:
#line 136 "config.rl"
	{ out.port = toint(state.match); }
	goto st68;
tr39:
#line 114 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st68;
tr55:
#line 118 "config.rl"
	{ out.fullscreen = false; }
	goto st68;
tr57:
#line 117 "config.rl"
	{ out.fullscreen = true; }
	goto st68;
tr75:
#line 124 "config.rl"
	{ out.screen_height = toint(state.match); }
	goto st68;
tr91:
#line 130 "config.rl"
	{ out.tile_height = toint(state.match); }
	goto st68;
st68:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof68;
case 68:
#line 454 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st68;
		case 32: goto st68;
		case 99: goto st2;
		case 102: goto st18;
		case 115: goto st41;
		case 116: goto st55;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st68;
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
		case 34: goto tr34;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st23;
	goto st0;
tr34:
#line 77 "config.rl"
	{
            state.match.clear();
        }
	goto st24;
tr35:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st24;
tr37:
#line 108 "config.rl"
	{{ state.stack[ state.top++] = 24; goto st67;}}
	goto st24;
st24:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof24;
case 24:
#line 539 "config.h"
	switch( (*( state.p)) ) {
		case 34: goto st25;
		case 92: goto tr37;
	}
	goto tr35;
st25:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof25;
case 25:
	switch( (*( state.p)) ) {
		case 13: goto tr38;
		case 32: goto tr38;
		case 59: goto tr39;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr38;
	goto st0;
tr38:
#line 114 "config.rl"
	{ out.fonts.push_back(state.match); }
	goto st26;
st26:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof26;
case 26:
#line 565 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st26;
		case 32: goto st26;
		case 34: goto tr34;
		case 59: goto st68;
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
		case 13: goto tr54;
		case 32: goto tr54;
		case 59: goto tr55;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr54;
	goto st0;
st40:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof40;
case 40:
	switch( (*( state.p)) ) {
		case 13: goto tr56;
		case 32: goto tr56;
		case 59: goto tr57;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto tr56;
	goto st0;
st41:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof41;
case 41:
	if ( (*( state.p)) == 99 )
		goto st42;
	goto st0;
st42:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof42;
case 42:
	if ( (*( state.p)) == 114 )
		goto st43;
	goto st0;
st43:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof43;
case 43:
	if ( (*( state.p)) == 101 )
		goto st44;
	goto st0;
st44:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof44;
case 44:
	if ( (*( state.p)) == 101 )
		goto st45;
	goto st0;
st45:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof45;
case 45:
	if ( (*( state.p)) == 110 )
		goto st46;
	goto st0;
st46:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof46;
case 46:
	if ( (*( state.p)) == 115 )
		goto st47;
	goto st0;
st47:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof47;
case 47:
	if ( (*( state.p)) == 105 )
		goto st48;
	goto st0;
st48:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof48;
case 48:
	if ( (*( state.p)) == 122 )
		goto st49;
	goto st0;
st49:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof49;
case 49:
	if ( (*( state.p)) == 101 )
		goto st50;
	goto st0;
st50:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof50;
case 50:
	switch( (*( state.p)) ) {
		case 13: goto st51;
		case 32: goto st51;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st51;
	goto st0;
st51:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof51;
case 51:
	switch( (*( state.p)) ) {
		case 13: goto st51;
		case 32: goto st51;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr68;
	} else if ( (*( state.p)) >= 9 )
		goto st51;
	goto st0;
tr70:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st52;
tr68:
#line 77 "config.rl"
	{
            state.match.clear();
        }
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st52;
st52:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof52;
case 52:
#line 802 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr69;
		case 32: goto tr69;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr70;
	} else if ( (*( state.p)) >= 9 )
		goto tr69;
	goto st0;
tr69:
#line 123 "config.rl"
	{ out.screen_width = toint(state.match); }
	goto st53;
st53:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof53;
case 53:
#line 821 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st53;
		case 32: goto st53;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr72;
	} else if ( (*( state.p)) >= 9 )
		goto st53;
	goto st0;
tr74:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st54;
tr72:
#line 77 "config.rl"
	{
            state.match.clear();
        }
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st54;
st54:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof54;
case 54:
#line 852 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr73;
		case 32: goto tr73;
		case 59: goto tr75;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr74;
	} else if ( (*( state.p)) >= 9 )
		goto tr73;
	goto st0;
st55:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof55;
case 55:
	if ( (*( state.p)) == 105 )
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
	if ( (*( state.p)) == 101 )
		goto st58;
	goto st0;
st58:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof58;
case 58:
	if ( (*( state.p)) == 115 )
		goto st59;
	goto st0;
st59:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof59;
case 59:
	if ( (*( state.p)) == 105 )
		goto st60;
	goto st0;
st60:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof60;
case 60:
	if ( (*( state.p)) == 122 )
		goto st61;
	goto st0;
st61:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof61;
case 61:
	if ( (*( state.p)) == 101 )
		goto st62;
	goto st0;
st62:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof62;
case 62:
	switch( (*( state.p)) ) {
		case 13: goto st63;
		case 32: goto st63;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st63;
	goto st0;
st63:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof63;
case 63:
	switch( (*( state.p)) ) {
		case 13: goto st63;
		case 32: goto st63;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr84;
	} else if ( (*( state.p)) >= 9 )
		goto st63;
	goto st0;
tr86:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st64;
tr84:
#line 77 "config.rl"
	{
            state.match.clear();
        }
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st64;
st64:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof64;
case 64:
#line 958 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr85;
		case 32: goto tr85;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr86;
	} else if ( (*( state.p)) >= 9 )
		goto tr85;
	goto st0;
tr85:
#line 129 "config.rl"
	{ out.tile_width = toint(state.match); }
	goto st65;
st65:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof65;
case 65:
#line 977 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto st65;
		case 32: goto st65;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr88;
	} else if ( (*( state.p)) >= 9 )
		goto st65;
	goto st0;
tr90:
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st66;
tr88:
#line 77 "config.rl"
	{
            state.match.clear();
        }
#line 81 "config.rl"
	{
            state.match += (*( state.p));
        }
	goto st66;
st66:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof66;
case 66:
#line 1008 "config.h"
	switch( (*( state.p)) ) {
		case 13: goto tr89;
		case 32: goto tr89;
		case 59: goto tr91;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr90;
	} else if ( (*( state.p)) >= 9 )
		goto tr89;
	goto st0;
st67:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof67;
case 67:
	switch( (*( state.p)) ) {
		case 34: goto tr92;
		case 49: goto tr93;
		case 50: goto tr94;
		case 51: goto tr95;
		case 92: goto tr96;
		case 110: goto tr97;
	}
	goto st0;
tr92:
#line 99 "config.rl"
	{ state.match += '"';  { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
tr93:
#line 101 "config.rl"
	{ state.match += '\1'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
tr94:
#line 102 "config.rl"
	{ state.match += '\2'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
tr95:
#line 103 "config.rl"
	{ state.match += '\3'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
tr96:
#line 104 "config.rl"
	{ state.match += '\\'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
tr97:
#line 100 "config.rl"
	{ state.match += '\n'; { state.cs =  state.stack[-- state.top];goto _again;} }
	goto st69;
st69:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof69;
case 69:
#line 1061 "config.h"
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
	_test_eof68:  state.cs = 68; goto _test_eof; 
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
	_test_eof69:  state.cs = 69; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 171 "config.rl"

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
