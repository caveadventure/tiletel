
#line 1 "bdf.rl"
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

    Font::glyph* glyph = &(out.glyphs[0]);

    
#line 204 "bdf.rl"

/*

*/

    
#line 106 "bdf.h"
static const int BDFParser_start = 1;
static const int BDFParser_first_final = 187;
static const int BDFParser_error = 0;

static const int BDFParser_en_main = 1;


#line 210 "bdf.rl"
    
#line 116 "bdf.h"
	{
	 state.cs = BDFParser_start;
	}

#line 211 "bdf.rl"

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

        
#line 140 "bdf.h"
	{
	if ( ( state.p) == ( state.pe) )
		goto _test_eof;
	switch (  state.cs )
	{
case 1:
	switch( (*( state.p)) ) {
		case 67: goto st2;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	goto st0;
st0:
 state.cs = 0;
	goto _out;
st2:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof2;
case 2:
	if ( (*( state.p)) == 79 )
		goto st3;
	goto st0;
st3:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof3;
case 3:
	switch( (*( state.p)) ) {
		case 77: goto st4;
		case 78: goto st176;
	}
	goto st0;
st4:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof4;
case 4:
	if ( (*( state.p)) == 77 )
		goto st5;
	goto st0;
st5:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof5;
case 5:
	if ( (*( state.p)) == 69 )
		goto st6;
	goto st0;
st6:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof6;
case 6:
	if ( (*( state.p)) == 78 )
		goto st7;
	goto st0;
st7:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof7;
case 7:
	if ( (*( state.p)) == 84 )
		goto st8;
	goto st0;
st8:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof8;
case 8:
	if ( (*( state.p)) == 10 )
		goto st0;
	goto st9;
st9:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof9;
case 9:
	if ( (*( state.p)) == 10 )
		goto st10;
	goto st9;
st10:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof10;
case 10:
	switch( (*( state.p)) ) {
		case 67: goto st11;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	goto st0;
st11:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof11;
case 11:
	switch( (*( state.p)) ) {
		case 72: goto st12;
		case 79: goto st3;
	}
	goto st0;
st12:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof12;
case 12:
	if ( (*( state.p)) == 65 )
		goto st13;
	goto st0;
st13:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof13;
case 13:
	if ( (*( state.p)) == 82 )
		goto st14;
	goto st0;
st14:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof14;
case 14:
	if ( (*( state.p)) == 83 )
		goto st15;
	goto st0;
st15:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof15;
case 15:
	if ( (*( state.p)) == 10 )
		goto st0;
	goto st16;
st16:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof16;
case 16:
	if ( (*( state.p)) == 10 )
		goto st17;
	goto st16;
st17:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof17;
case 17:
	if ( (*( state.p)) == 83 )
		goto st18;
	goto st0;
st18:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof18;
case 18:
	if ( (*( state.p)) == 84 )
		goto st19;
	goto st0;
st19:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof19;
case 19:
	if ( (*( state.p)) == 65 )
		goto st20;
	goto st0;
st20:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof20;
case 20:
	if ( (*( state.p)) == 82 )
		goto st21;
	goto st0;
st21:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof21;
case 21:
	if ( (*( state.p)) == 84 )
		goto st22;
	goto st0;
st22:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof22;
case 22:
	if ( (*( state.p)) == 67 )
		goto st23;
	goto st0;
st23:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof23;
case 23:
	if ( (*( state.p)) == 72 )
		goto st24;
	goto st0;
st24:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof24;
case 24:
	if ( (*( state.p)) == 65 )
		goto st25;
	goto st0;
st25:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof25;
case 25:
	if ( (*( state.p)) == 82 )
		goto st26;
	goto st0;
st26:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof26;
case 26:
	if ( (*( state.p)) == 10 )
		goto st0;
	goto st27;
st27:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof27;
case 27:
	if ( (*( state.p)) == 10 )
		goto st28;
	goto st27;
tr95:
#line 129 "bdf.rl"
	{ glyph = &(out.glyphs[toint(state.match)]); }
	goto st28;
st28:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof28;
case 28:
#line 359 "bdf.h"
	switch( (*( state.p)) ) {
		case 66: goto st29;
		case 68: goto st66;
		case 69: goto st71;
		case 83: goto st66;
		case 86: goto st81;
	}
	goto st0;
st29:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof29;
case 29:
	switch( (*( state.p)) ) {
		case 66: goto st30;
		case 73: goto st42;
	}
	goto st0;
st30:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof30;
case 30:
	if ( (*( state.p)) == 88 )
		goto st31;
	goto st0;
st31:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof31;
case 31:
	switch( (*( state.p)) ) {
		case 13: goto st32;
		case 32: goto st32;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st32;
	goto st0;
st32:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof32;
case 32:
	switch( (*( state.p)) ) {
		case 13: goto st32;
		case 32: goto st32;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr42;
	} else if ( (*( state.p)) >= 9 )
		goto st32;
	goto st0;
tr42:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st33;
tr44:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st33;
st33:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof33;
case 33:
#line 429 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto tr43;
		case 32: goto tr43;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr44;
	} else if ( (*( state.p)) >= 9 )
		goto tr43;
	goto st0;
tr43:
#line 135 "bdf.rl"
	{ glyph->w = toint(state.match); check_w(out, glyph->w); }
	goto st34;
st34:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof34;
case 34:
#line 448 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st34;
		case 32: goto st34;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr46;
	} else if ( (*( state.p)) >= 9 )
		goto st34;
	goto st0;
tr46:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st35;
tr48:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st35;
st35:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof35;
case 35:
#line 479 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto tr47;
		case 32: goto tr47;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr48;
	} else if ( (*( state.p)) >= 9 )
		goto tr47;
	goto st0;
tr47:
#line 136 "bdf.rl"
	{ check_h(out, toint(state.match)); }
	goto st36;
st36:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof36;
case 36:
#line 498 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st36;
		case 32: goto st36;
		case 45: goto tr50;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr51;
	} else if ( (*( state.p)) >= 9 )
		goto st36;
	goto st0;
tr50:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st37;
st37:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof37;
case 37:
#line 524 "bdf.h"
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr52;
	goto st0;
tr51:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st38;
tr52:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st38;
st38:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof38;
case 38:
#line 548 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st39;
		case 32: goto st39;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr52;
	} else if ( (*( state.p)) >= 9 )
		goto st39;
	goto st0;
st39:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof39;
case 39:
	switch( (*( state.p)) ) {
		case 13: goto st39;
		case 32: goto st39;
		case 45: goto tr54;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr55;
	} else if ( (*( state.p)) >= 9 )
		goto st39;
	goto st0;
tr54:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st40;
st40:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof40;
case 40:
#line 588 "bdf.h"
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr56;
	goto st0;
tr55:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st41;
tr56:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st41;
st41:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof41;
case 41:
#line 612 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto st28;
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr56;
	goto st0;
st42:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof42;
case 42:
	if ( (*( state.p)) == 84 )
		goto st43;
	goto st0;
st43:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof43;
case 43:
	if ( (*( state.p)) == 77 )
		goto st44;
	goto st0;
st44:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof44;
case 44:
	if ( (*( state.p)) == 65 )
		goto st45;
	goto st0;
st45:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof45;
case 45:
	if ( (*( state.p)) == 80 )
		goto st46;
	goto st0;
st46:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof46;
case 46:
	if ( (*( state.p)) == 10 )
		goto st47;
	goto st46;
st47:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof47;
case 47:
	if ( (*( state.p)) > 57 ) {
		if ( 65 <= (*( state.p)) && (*( state.p)) <= 70 )
			goto tr62;
	} else if ( (*( state.p)) >= 48 )
		goto tr62;
	goto st0;
tr62:
#line 143 "bdf.rl"
	{ hexchar(bitmapchar, (*( state.p))); }
	goto st48;
tr65:
#line 145 "bdf.rl"
	{ glyph->bitmap.push_back(bitmapchar); }
#line 143 "bdf.rl"
	{ hexchar(bitmapchar, (*( state.p))); }
	goto st48;
st48:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof48;
case 48:
#line 677 "bdf.h"
	if ( (*( state.p)) > 57 ) {
		if ( 65 <= (*( state.p)) && (*( state.p)) <= 70 )
			goto tr63;
	} else if ( (*( state.p)) >= 48 )
		goto tr63;
	goto st0;
tr63:
#line 144 "bdf.rl"
	{ hexchar(bitmapchar, (*( state.p))); }
	goto st49;
st49:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof49;
case 49:
#line 692 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto tr64;
	if ( (*( state.p)) > 57 ) {
		if ( 65 <= (*( state.p)) && (*( state.p)) <= 70 )
			goto tr65;
	} else if ( (*( state.p)) >= 48 )
		goto tr65;
	goto st0;
tr64:
#line 145 "bdf.rl"
	{ glyph->bitmap.push_back(bitmapchar); }
	goto st50;
st50:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof50;
case 50:
#line 709 "bdf.h"
	if ( (*( state.p)) == 69 )
		goto tr66;
	if ( (*( state.p)) > 57 ) {
		if ( 65 <= (*( state.p)) && (*( state.p)) <= 70 )
			goto tr62;
	} else if ( (*( state.p)) >= 48 )
		goto tr62;
	goto st0;
tr66:
#line 143 "bdf.rl"
	{ hexchar(bitmapchar, (*( state.p))); }
	goto st51;
st51:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof51;
case 51:
#line 726 "bdf.h"
	if ( (*( state.p)) == 78 )
		goto st52;
	if ( (*( state.p)) > 57 ) {
		if ( 65 <= (*( state.p)) && (*( state.p)) <= 70 )
			goto tr63;
	} else if ( (*( state.p)) >= 48 )
		goto tr63;
	goto st0;
st52:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof52;
case 52:
	if ( (*( state.p)) == 68 )
		goto st53;
	goto st0;
st53:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof53;
case 53:
	if ( (*( state.p)) == 67 )
		goto st54;
	goto st0;
st54:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof54;
case 54:
	if ( (*( state.p)) == 72 )
		goto st55;
	goto st0;
st55:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof55;
case 55:
	if ( (*( state.p)) == 65 )
		goto st56;
	goto st0;
st56:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof56;
case 56:
	if ( (*( state.p)) == 82 )
		goto st57;
	goto st0;
st57:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof57;
case 57:
	if ( (*( state.p)) == 10 )
		goto st58;
	goto st57;
st58:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof58;
case 58:
	switch( (*( state.p)) ) {
		case 69: goto st59;
		case 83: goto st18;
	}
	goto st0;
st59:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof59;
case 59:
	if ( (*( state.p)) == 78 )
		goto st60;
	goto st0;
st60:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof60;
case 60:
	if ( (*( state.p)) == 68 )
		goto st61;
	goto st0;
st61:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof61;
case 61:
	if ( (*( state.p)) == 70 )
		goto st62;
	goto st0;
st62:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof62;
case 62:
	if ( (*( state.p)) == 79 )
		goto st63;
	goto st0;
st63:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof63;
case 63:
	if ( (*( state.p)) == 78 )
		goto st64;
	goto st0;
st64:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof64;
case 64:
	if ( (*( state.p)) == 84 )
		goto st65;
	goto st0;
st65:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof65;
case 65:
	if ( (*( state.p)) == 10 )
		goto st187;
	goto st65;
st187:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof187;
case 187:
	goto st0;
st66:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof66;
case 66:
	if ( (*( state.p)) == 87 )
		goto st67;
	goto st0;
st67:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof67;
case 67:
	if ( (*( state.p)) == 73 )
		goto st68;
	goto st0;
st68:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof68;
case 68:
	if ( (*( state.p)) == 68 )
		goto st69;
	goto st0;
st69:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof69;
case 69:
	if ( (*( state.p)) == 84 )
		goto st70;
	goto st0;
st70:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof70;
case 70:
	if ( (*( state.p)) == 72 )
		goto st26;
	goto st0;
st71:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof71;
case 71:
	if ( (*( state.p)) == 78 )
		goto st72;
	goto st0;
st72:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof72;
case 72:
	if ( (*( state.p)) == 67 )
		goto st73;
	goto st0;
st73:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof73;
case 73:
	if ( (*( state.p)) == 79 )
		goto st74;
	goto st0;
st74:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof74;
case 74:
	if ( (*( state.p)) == 68 )
		goto st75;
	goto st0;
st75:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof75;
case 75:
	if ( (*( state.p)) == 73 )
		goto st76;
	goto st0;
st76:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof76;
case 76:
	if ( (*( state.p)) == 78 )
		goto st77;
	goto st0;
st77:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof77;
case 77:
	if ( (*( state.p)) == 71 )
		goto st78;
	goto st0;
st78:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof78;
case 78:
	switch( (*( state.p)) ) {
		case 13: goto st79;
		case 32: goto st79;
	}
	if ( 9 <= (*( state.p)) && (*( state.p)) <= 10 )
		goto st79;
	goto st0;
st79:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof79;
case 79:
	switch( (*( state.p)) ) {
		case 13: goto st79;
		case 32: goto st79;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr94;
	} else if ( (*( state.p)) >= 9 )
		goto st79;
	goto st0;
tr94:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st80;
tr96:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st80;
st80:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof80;
case 80:
#line 969 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto tr95;
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr96;
	goto st0;
st81:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof81;
case 81:
	if ( (*( state.p)) == 86 )
		goto st82;
	goto st0;
st82:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof82;
case 82:
	if ( (*( state.p)) == 69 )
		goto st83;
	goto st0;
st83:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof83;
case 83:
	if ( (*( state.p)) == 67 )
		goto st84;
	goto st0;
st84:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof84;
case 84:
	if ( (*( state.p)) == 84 )
		goto st85;
	goto st0;
st85:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof85;
case 85:
	if ( (*( state.p)) == 79 )
		goto st25;
	goto st0;
st86:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof86;
case 86:
	if ( (*( state.p)) == 87 )
		goto st87;
	goto st0;
st87:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof87;
case 87:
	if ( (*( state.p)) == 73 )
		goto st88;
	goto st0;
st88:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof88;
case 88:
	if ( (*( state.p)) == 68 )
		goto st89;
	goto st0;
st89:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof89;
case 89:
	if ( (*( state.p)) == 84 )
		goto st90;
	goto st0;
st90:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof90;
case 90:
	if ( (*( state.p)) == 72 )
		goto st8;
	goto st0;
st91:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof91;
case 91:
	if ( (*( state.p)) == 79 )
		goto st92;
	goto st0;
st92:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof92;
case 92:
	if ( (*( state.p)) == 78 )
		goto st93;
	goto st0;
st93:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof93;
case 93:
	if ( (*( state.p)) == 84 )
		goto st94;
	goto st0;
st94:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof94;
case 94:
	switch( (*( state.p)) ) {
		case 10: goto st0;
		case 66: goto st95;
	}
	goto st9;
st95:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof95;
case 95:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 79: goto st96;
	}
	goto st9;
st96:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof96;
case 96:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 85: goto st97;
	}
	goto st9;
st97:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof97;
case 97:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 78: goto st98;
	}
	goto st9;
st98:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof98;
case 98:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 68: goto st99;
	}
	goto st9;
st99:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof99;
case 99:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 73: goto st100;
	}
	goto st9;
st100:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof100;
case 100:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 78: goto st101;
	}
	goto st9;
st101:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof101;
case 101:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 71: goto st102;
	}
	goto st9;
st102:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof102;
case 102:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 66: goto st103;
	}
	goto st9;
st103:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof103;
case 103:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 79: goto st104;
	}
	goto st9;
st104:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof104;
case 104:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 88: goto st105;
	}
	goto st9;
st105:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof105;
case 105:
	switch( (*( state.p)) ) {
		case 9: goto st106;
		case 10: goto st107;
		case 13: goto st106;
		case 32: goto st106;
	}
	goto st9;
st106:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof106;
case 106:
	switch( (*( state.p)) ) {
		case 9: goto st106;
		case 10: goto st107;
		case 13: goto st106;
		case 32: goto st106;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr121;
	goto st9;
st107:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof107;
case 107:
	switch( (*( state.p)) ) {
		case 13: goto st108;
		case 32: goto st108;
		case 67: goto st11;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr123;
	} else if ( (*( state.p)) >= 9 )
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
			goto tr123;
	} else if ( (*( state.p)) >= 9 )
		goto st108;
	goto st0;
tr123:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st109;
tr125:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st109;
st109:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof109;
case 109:
#line 1243 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto tr124;
		case 32: goto tr124;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr125;
	} else if ( (*( state.p)) >= 9 )
		goto tr124;
	goto st0;
tr124:
#line 172 "bdf.rl"
	{ out.w = toint(state.match); }
	goto st110;
st110:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof110;
case 110:
#line 1262 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st110;
		case 32: goto st110;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr127;
	} else if ( (*( state.p)) >= 9 )
		goto st110;
	goto st0;
tr127:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st111;
tr129:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st111;
st111:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof111;
case 111:
#line 1293 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto tr128;
		case 32: goto tr128;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr129;
	} else if ( (*( state.p)) >= 9 )
		goto tr128;
	goto st0;
tr128:
#line 173 "bdf.rl"
	{ out.h = toint(state.match); }
	goto st112;
st112:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof112;
case 112:
#line 1312 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st112;
		case 32: goto st112;
		case 45: goto tr131;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr132;
	} else if ( (*( state.p)) >= 9 )
		goto st112;
	goto st0;
tr131:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st113;
st113:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof113;
case 113:
#line 1338 "bdf.h"
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr133;
	goto st0;
tr132:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st114;
tr133:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st114;
st114:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof114;
case 114:
#line 1362 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st115;
		case 32: goto st115;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr133;
	} else if ( (*( state.p)) >= 9 )
		goto st115;
	goto st0;
st115:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof115;
case 115:
	switch( (*( state.p)) ) {
		case 13: goto st115;
		case 32: goto st115;
		case 45: goto tr135;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr136;
	} else if ( (*( state.p)) >= 9 )
		goto st115;
	goto st0;
tr135:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st116;
st116:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof116;
case 116:
#line 1402 "bdf.h"
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr137;
	goto st0;
tr136:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st117;
tr137:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st117;
st117:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof117;
case 117:
#line 1426 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto st10;
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr137;
	goto st0;
st118:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof118;
case 118:
	if ( (*( state.p)) == 69 )
		goto st119;
	goto st0;
st119:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof119;
case 119:
	if ( (*( state.p)) == 84 )
		goto st120;
	goto st0;
st120:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof120;
case 120:
	if ( (*( state.p)) == 82 )
		goto st121;
	goto st0;
st121:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof121;
case 121:
	if ( (*( state.p)) == 73 )
		goto st122;
	goto st0;
st122:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof122;
case 122:
	if ( (*( state.p)) == 67 )
		goto st123;
	goto st0;
st123:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof123;
case 123:
	if ( (*( state.p)) == 83 )
		goto st124;
	goto st0;
st124:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof124;
case 124:
	if ( (*( state.p)) == 83 )
		goto st125;
	goto st0;
st125:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof125;
case 125:
	if ( (*( state.p)) == 69 )
		goto st7;
	goto st0;
st126:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof126;
case 126:
	switch( (*( state.p)) ) {
		case 73: goto st127;
		case 84: goto st129;
		case 87: goto st87;
	}
	goto st0;
st127:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof127;
case 127:
	if ( (*( state.p)) == 90 )
		goto st128;
	goto st0;
st128:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof128;
case 128:
	if ( (*( state.p)) == 69 )
		goto st8;
	goto st0;
st129:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof129;
case 129:
	if ( (*( state.p)) == 65 )
		goto st130;
	goto st0;
st130:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof130;
case 130:
	if ( (*( state.p)) == 82 )
		goto st131;
	goto st0;
st131:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof131;
case 131:
	if ( (*( state.p)) == 84 )
		goto st132;
	goto st0;
st132:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof132;
case 132:
	switch( (*( state.p)) ) {
		case 70: goto st133;
		case 80: goto st134;
	}
	goto st0;
st133:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof133;
case 133:
	if ( (*( state.p)) == 79 )
		goto st6;
	goto st0;
st134:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof134;
case 134:
	if ( (*( state.p)) == 82 )
		goto st135;
	goto st0;
st135:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof135;
case 135:
	if ( (*( state.p)) == 79 )
		goto st136;
	goto st0;
st136:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof136;
case 136:
	if ( (*( state.p)) == 80 )
		goto st137;
	goto st0;
st137:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof137;
case 137:
	if ( (*( state.p)) == 69 )
		goto st138;
	goto st0;
st138:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof138;
case 138:
	if ( (*( state.p)) == 82 )
		goto st139;
	goto st0;
st139:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof139;
case 139:
	if ( (*( state.p)) == 84 )
		goto st140;
	goto st0;
st140:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof140;
case 140:
	if ( (*( state.p)) == 73 )
		goto st141;
	goto st0;
st141:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof141;
case 141:
	if ( (*( state.p)) == 69 )
		goto st142;
	goto st0;
st142:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof142;
case 142:
	if ( (*( state.p)) == 83 )
		goto st143;
	goto st0;
st143:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof143;
case 143:
	if ( (*( state.p)) == 10 )
		goto st0;
	goto st144;
st144:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof144;
case 144:
	if ( (*( state.p)) == 10 )
		goto st145;
	goto st144;
st145:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof145;
case 145:
	if ( (*( state.p)) == 69 )
		goto st146;
	goto st145;
st146:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof146;
case 146:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 78: goto st147;
	}
	goto st145;
st147:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof147;
case 147:
	switch( (*( state.p)) ) {
		case 68: goto st148;
		case 69: goto st146;
	}
	goto st145;
st148:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof148;
case 148:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 80: goto st149;
	}
	goto st145;
st149:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof149;
case 149:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 82: goto st150;
	}
	goto st145;
st150:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof150;
case 150:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 79: goto st151;
	}
	goto st145;
st151:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof151;
case 151:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 80: goto st152;
	}
	goto st145;
st152:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof152;
case 152:
	if ( (*( state.p)) == 69 )
		goto st153;
	goto st145;
st153:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof153;
case 153:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 78: goto st147;
		case 82: goto st154;
	}
	goto st145;
st154:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof154;
case 154:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 84: goto st155;
	}
	goto st145;
st155:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof155;
case 155:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 73: goto st156;
	}
	goto st145;
st156:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof156;
case 156:
	if ( (*( state.p)) == 69 )
		goto st157;
	goto st145;
st157:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof157;
case 157:
	switch( (*( state.p)) ) {
		case 69: goto st146;
		case 78: goto st147;
		case 83: goto st158;
	}
	goto st145;
st158:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof158;
case 158:
	switch( (*( state.p)) ) {
		case 10: goto st10;
		case 69: goto st146;
	}
	goto st145;
st159:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof159;
case 159:
	if ( (*( state.p)) == 86 )
		goto st160;
	goto st0;
st160:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof160;
case 160:
	if ( (*( state.p)) == 69 )
		goto st161;
	goto st0;
st161:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof161;
case 161:
	if ( (*( state.p)) == 67 )
		goto st162;
	goto st0;
st162:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof162;
case 162:
	if ( (*( state.p)) == 84 )
		goto st163;
	goto st0;
st163:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof163;
case 163:
	if ( (*( state.p)) == 79 )
		goto st164;
	goto st0;
st164:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof164;
case 164:
	if ( (*( state.p)) == 82 )
		goto st8;
	goto st0;
tr121:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st165;
tr184:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st165;
st165:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof165;
case 165:
#line 1810 "bdf.h"
	switch( (*( state.p)) ) {
		case 9: goto tr182;
		case 10: goto tr183;
		case 13: goto tr182;
		case 32: goto tr182;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr184;
	goto st9;
tr182:
#line 172 "bdf.rl"
	{ out.w = toint(state.match); }
	goto st166;
st166:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof166;
case 166:
#line 1828 "bdf.h"
	switch( (*( state.p)) ) {
		case 9: goto st166;
		case 10: goto st167;
		case 13: goto st166;
		case 32: goto st166;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr187;
	goto st9;
tr183:
#line 172 "bdf.rl"
	{ out.w = toint(state.match); }
	goto st167;
st167:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof167;
case 167:
#line 1846 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st110;
		case 32: goto st110;
		case 67: goto st11;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr127;
	} else if ( (*( state.p)) >= 9 )
		goto st110;
	goto st0;
tr187:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st168;
tr190:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st168;
st168:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof168;
case 168:
#line 1883 "bdf.h"
	switch( (*( state.p)) ) {
		case 9: goto tr188;
		case 10: goto tr189;
		case 13: goto tr188;
		case 32: goto tr188;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr190;
	goto st9;
tr188:
#line 173 "bdf.rl"
	{ out.h = toint(state.match); }
	goto st169;
st169:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof169;
case 169:
#line 1901 "bdf.h"
	switch( (*( state.p)) ) {
		case 9: goto st169;
		case 10: goto st170;
		case 13: goto st169;
		case 32: goto st169;
		case 45: goto tr193;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr194;
	goto st9;
tr189:
#line 173 "bdf.rl"
	{ out.h = toint(state.match); }
	goto st170;
st170:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof170;
case 170:
#line 1920 "bdf.h"
	switch( (*( state.p)) ) {
		case 13: goto st112;
		case 32: goto st112;
		case 45: goto tr131;
		case 67: goto st11;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr132;
	} else if ( (*( state.p)) >= 9 )
		goto st112;
	goto st0;
tr193:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st171;
st171:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof171;
case 171:
#line 1952 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto st10;
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr195;
	goto st9;
tr194:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st172;
tr195:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st172;
st172:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof172;
case 172:
#line 1978 "bdf.h"
	switch( (*( state.p)) ) {
		case 9: goto st173;
		case 10: goto st174;
		case 13: goto st173;
		case 32: goto st173;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr195;
	goto st9;
st173:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof173;
case 173:
	switch( (*( state.p)) ) {
		case 9: goto st173;
		case 10: goto st174;
		case 13: goto st173;
		case 32: goto st173;
		case 45: goto tr198;
	}
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr198;
	goto st9;
st174:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof174;
case 174:
	switch( (*( state.p)) ) {
		case 13: goto st115;
		case 32: goto st115;
		case 45: goto tr135;
		case 67: goto st11;
		case 68: goto st86;
		case 70: goto st91;
		case 77: goto st118;
		case 83: goto st126;
		case 86: goto st159;
	}
	if ( (*( state.p)) > 10 ) {
		if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
			goto tr136;
	} else if ( (*( state.p)) >= 9 )
		goto st115;
	goto st0;
tr198:
#line 104 "bdf.rl"
	{
            state.match.clear();
        }
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st175;
tr199:
#line 108 "bdf.rl"
	{
            state.match += (*( state.p));
        }
	goto st175;
st175:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof175;
case 175:
#line 2043 "bdf.h"
	if ( (*( state.p)) == 10 )
		goto st10;
	if ( 48 <= (*( state.p)) && (*( state.p)) <= 57 )
		goto tr199;
	goto st9;
st176:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof176;
case 176:
	if ( (*( state.p)) == 84 )
		goto st177;
	goto st0;
st177:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof177;
case 177:
	if ( (*( state.p)) == 69 )
		goto st178;
	goto st0;
st178:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof178;
case 178:
	if ( (*( state.p)) == 78 )
		goto st179;
	goto st0;
st179:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof179;
case 179:
	if ( (*( state.p)) == 84 )
		goto st180;
	goto st0;
st180:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof180;
case 180:
	if ( (*( state.p)) == 86 )
		goto st181;
	goto st0;
st181:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof181;
case 181:
	if ( (*( state.p)) == 69 )
		goto st182;
	goto st0;
st182:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof182;
case 182:
	if ( (*( state.p)) == 82 )
		goto st183;
	goto st0;
st183:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof183;
case 183:
	if ( (*( state.p)) == 83 )
		goto st184;
	goto st0;
st184:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof184;
case 184:
	if ( (*( state.p)) == 73 )
		goto st185;
	goto st0;
st185:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof185;
case 185:
	if ( (*( state.p)) == 79 )
		goto st186;
	goto st0;
st186:
	if ( ++( state.p) == ( state.pe) )
		goto _test_eof186;
case 186:
	if ( (*( state.p)) == 78 )
		goto st8;
	goto st0;
	}
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
	_test_eof187:  state.cs = 187; goto _test_eof; 
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
	_test_eof149:  state.cs = 149; goto _test_eof; 
	_test_eof150:  state.cs = 150; goto _test_eof; 
	_test_eof151:  state.cs = 151; goto _test_eof; 
	_test_eof152:  state.cs = 152; goto _test_eof; 
	_test_eof153:  state.cs = 153; goto _test_eof; 
	_test_eof154:  state.cs = 154; goto _test_eof; 
	_test_eof155:  state.cs = 155; goto _test_eof; 
	_test_eof156:  state.cs = 156; goto _test_eof; 
	_test_eof157:  state.cs = 157; goto _test_eof; 
	_test_eof158:  state.cs = 158; goto _test_eof; 
	_test_eof159:  state.cs = 159; goto _test_eof; 
	_test_eof160:  state.cs = 160; goto _test_eof; 
	_test_eof161:  state.cs = 161; goto _test_eof; 
	_test_eof162:  state.cs = 162; goto _test_eof; 
	_test_eof163:  state.cs = 163; goto _test_eof; 
	_test_eof164:  state.cs = 164; goto _test_eof; 
	_test_eof165:  state.cs = 165; goto _test_eof; 
	_test_eof166:  state.cs = 166; goto _test_eof; 
	_test_eof167:  state.cs = 167; goto _test_eof; 
	_test_eof168:  state.cs = 168; goto _test_eof; 
	_test_eof169:  state.cs = 169; goto _test_eof; 
	_test_eof170:  state.cs = 170; goto _test_eof; 
	_test_eof171:  state.cs = 171; goto _test_eof; 
	_test_eof172:  state.cs = 172; goto _test_eof; 
	_test_eof173:  state.cs = 173; goto _test_eof; 
	_test_eof174:  state.cs = 174; goto _test_eof; 
	_test_eof175:  state.cs = 175; goto _test_eof; 
	_test_eof176:  state.cs = 176; goto _test_eof; 
	_test_eof177:  state.cs = 177; goto _test_eof; 
	_test_eof178:  state.cs = 178; goto _test_eof; 
	_test_eof179:  state.cs = 179; goto _test_eof; 
	_test_eof180:  state.cs = 180; goto _test_eof; 
	_test_eof181:  state.cs = 181; goto _test_eof; 
	_test_eof182:  state.cs = 182; goto _test_eof; 
	_test_eof183:  state.cs = 183; goto _test_eof; 
	_test_eof184:  state.cs = 184; goto _test_eof; 
	_test_eof185:  state.cs = 185; goto _test_eof; 
	_test_eof186:  state.cs = 186; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 229 "bdf.rl"

        // Avoid spurious gcc warnings.
        (void)BDFParser_first_final;
        (void)BDFParser_en_main;

        if (state.cs == BDFParser_error) {
            throw std::runtime_error("Parse error. Unconsumed input: " + std::string(state.p, state.pe));
        }
    }

    std::cout << "Done parsing font." << std::endl;

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
