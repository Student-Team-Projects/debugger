/*
 This file is a modified version of source code published under the GNU Lesser General Public License (LGPL) version 2.
 This modified code is licensed under the GNU General Public License (GPL) version 3 or later.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
*/

/*
 NOTICE FROM THE ORIGINAL SOURCE CODE:
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license version 2 or later (LGPLv2+),
 in which case the provisions of LGPL License are applicable instead of
 those above.

 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , ziz_at_mailbox.org
*/
#include "html/AnsiParser.h"
#include <cassert>
#include <fmt/format.h>
#include <string>

//enables a hotfix for inputs using cursor manipulations like htop.
// It's a hotfix and may not work for all such inputs
const bool LINE_BREAK_FIX = true;
const bool IGNORE_CR = false;
const char ansi_vt220_character_set[256][16] =
{
	"&#x2400;","&#x2401;","&#x2402;","&#x2403;","&#x2404;","&#x2405;","&#x2406;","&#x2407;", //00..07
	"&#x2408;","&#x2409;","&#x240a;","&#x240b;","&#x240c;","&#x240d;","&#x240e;","&#x240f;", //08..0f
	"&#x2410;","&#x2411;","&#x2412;","&#x2413;","&#x2414;","&#x2415;","&#x2416;","&#x2417;", //10..17
	"&#x2418;","&#x2419;","&#x241a;","&#x241b;","&#x241c;","&#x241d;","&#x241e;","&#x241f;", //18..1f
	" "       ,"!"       ,"\""      ,"#"       ,"$"       ,"%"       ,"&"       ,"'"       , //20..27
	"("       ,")"       ,"*"       ,"+"       ,","       ,"-"       ,"."       ,"/"       , //28..2f
	"0"       ,"1"       ,"2"       ,"3"       ,"4"       ,"5"       ,"6"       ,"7"       , //30..37
	"8"       ,"9"       ,":"       ,";"       ,"&lt;"    ,"="       ,"&gt;"    ,"?"       , //38..3f
	"@"       ,"A"       ,"B"       ,"C"       ,"D"       ,"E"       ,"F"       ,"G"       , //40..47
	"H"       ,"I"       ,"J"       ,"K"       ,"L"       ,"M"       ,"N"       ,"O"       , //48..4f
	"P"       ,"Q"       ,"R"       ,"S"       ,"T"       ,"U"       ,"V"       ,"W"       , //50..57
	"X"       ,"Y"       ,"Z"       ,"["       ,"\\"      ,"]"       ,"^"       ,"_"       , //58..5f
	"`"       ,"a"       ,"b"       ,"c"       ,"d"       ,"e"       ,"f"       ,"g"       , //60..67
	"h"       ,"i"       ,"j"       ,"k"       ,"l"       ,"m"       ,"n"       ,"o"       , //68..6f
	"p"       ,"q"       ,"r"       ,"s"       ,"t"       ,"u"       ,"v"       ,"w"       , //70..77
	"x"       ,"y"       ,"z"       ,"{"       ,"|"       ,"}"       ,"~"       ,"&#x2421;", //78..7f
	"&#x25c6;","&#x2592;","&#x2409;","&#x240c;","&#x240d;","&#x240a;","&#x00b0;","&#x00b1;", //80..87
	"&#x2400;","&#x240b;","&#x2518;","&#x2510;","&#x250c;","&#x2514;","&#x253c;","&#x23ba;", //88..8f
	"&#x23bb;","&#x2500;","&#x23bc;","&#x23bd;","&#x251c;","&#x2524;","&#x2534;","&#x252c;", //90..97
	"&#x2502;","&#x2264;","&#x2265;","&pi;    ","&#x2260;","&pound;" ,"&#x0095;","&#x2421;", //98..9f
	"&#x2588;","&#x00a1;","&#x00a2;","&#x00a3;"," "       ,"&yen;"   ," "       ,"&#x00a7;", //a0..a7
	"&#x00a4;","&#x00a9;","&#x00ba;","&#x00qb;"," "       ," "       ," "       ," "       , //a8..af
	"&#x23bc;","&#x23bd;","&#x00b2;","&#x00b3;","&#x00b4;","&#x00b5;","&#x00b6;","&#x00b7;", //b0..b7
	"&#x00b8;","&#x00b9;","&#x00ba;","&#x00bb;","&#x00bc;","&#x00bd;","&#x00be;","&#x00bf;", //b8..bf
	"&#x00c0;","&#x00c1;","&#x00c2;","&#x00c3;","&#x00c4;","&#x00c5;","&#x00c6;","&#x00c7;", //c0..c7
	"&#x00c8;","&#x00c9;","&#x00ca;","&#x00cb;","&#x00cc;","&#x00cd;","&#x00ce;","&#x00cf;", //c8..cf
	" "       ,"&#x00d1;","&#x00d2;","&#x00d3;","&#x00d4;","&#x00d5;","&#x00d6;","&#x0152;", //d0..d7
	"&#x00d8;","&#x00d9;","&#x00da;","&#x00db;","&#x00dc;","&#x0178;"," "       ,"&#x00df;", //d8..df
	"&#x00e0;","&#x00e1;","&#x00e2;","&#x00e3;","&#x00e4;","&#x00e5;","&#x00e6;","&#x00e7;", //e0..e7
	"&#x00e8;","&#x00e9;","&#x00ea;","&#x00eb;","&#x00ec;","&#x00ed;","&#x00ee;","&#x00ef;", //e8..ef
	" "       ,"&#x00f1;","&#x00f2;","&#x00f3;","&#x00f4;","&#x00f5;","&#x00f6;","&#x0153;", //f0..f7
	"&#x00f8;","&#x00f9;","&#x00fa;","&#x00fb;","&#x00fc;","&#x00ff;"," "       ,"&#x2588;", //f8..ff
};

int getNextChar(std::string_view s, size_t &i)
{
	if(i<s.size())
		return s[i++];
	return EOF;
}

typedef struct selem *pelem;
typedef struct selem {
	unsigned char digit[8];
	unsigned char digitcount;
	long int value;
	pelem next;
} telem;

pelem parseInsert(char* s)
{
	pelem firstelem=NULL;
	pelem momelem=NULL;

	unsigned char digit[8];
	unsigned char digitcount=0;
	long int value=0;

	int pos=0;
	for (pos=0;pos<1024;pos++)
	{
		if (s[pos]=='[')
			continue;
		if (s[pos]==';' || s[pos]==':' || s[pos]==0)
		{
			if (digitcount==0)
			{
				digit[0]=0;
				digitcount=1;
			}

			pelem newelem=(pelem)malloc(sizeof(telem));
			if (newelem==NULL)
			{
				perror("Failed to allocate memory for parseInsert()");
				exit(EXIT_FAILURE);
			}

			memcpy(newelem->digit, digit, sizeof(digit));
			newelem->digitcount=digitcount;
			newelem->value=value;
			newelem->next=NULL;

			if (momelem==NULL)
				firstelem=newelem;
			else
				momelem->next=newelem;
			momelem=newelem;

			digitcount=0;
			memset(digit,0,sizeof(digit));
			value=0;

			if (s[pos]==0)
				break;
		}
		else
		if (digitcount < sizeof(digit))
		{
			digit[digitcount]=s[pos]-'0';
			value=(value*10)+digit[digitcount];
			digitcount++;
		}
	}
	return firstelem;
}

void deleteParse(pelem elem)
{
	while (elem!=NULL)
	{
		pelem temp=elem->next;
		free(elem);
		elem=temp;
	}
}

std::string make_rgb (int color_id){
	assert(color_id >= 16 && color_id <= 255);
	if (color_id >= 232)
	{
		// color_id is encoded as 232 + c, where c is in [0, 23]
		int index = color_id - 232;
		int grey = index * 255 / 23;
		return fmt::format("{:02x}{:02x}{:02x}", grey, grey, grey);
	}
	// color_id is encoded as 16 + 36 * r + 6 * g + b, where r, g, b are in [0, 5]
	int index = color_id - 16;
	int b = (index % 6) * 255 / 5;
	index /= 6;
	int g = (index % 6) * 255 / 5;
	index /= 6;
	int r = index * 255 / 5;
	return fmt::format("{:02x}{:02x}{:02x}", r, g, b);
}

enum ColorMode {
	MODE_3BIT,
	MODE_8BIT,
	MODE_24BIT
};

struct State {
	int fc, bc;
	int bold;
	int italic;
	int underline;
	int blink;
	int crossedout;
	enum ColorMode fc_colormode;
	enum ColorMode bc_colormode;
	int highlighted; //for fc AND bc although not correct...
};

void swapColors(struct State *const state) {
	if (state->bc_colormode == MODE_3BIT && state->bc == -1)
		state->bc = 8;

	if (state->fc_colormode == MODE_3BIT && state->fc == -1)
		state->fc = 9;

	int temp = state->bc;
	state->bc = state->fc;
	state->fc = temp;

	enum ColorMode temp_colormode = state->bc_colormode;
	state->bc_colormode = state->fc_colormode;
	state->fc_colormode = temp_colormode;
}


const struct State default_state = {
	.fc = -1, //Standard Foreground Color //IRC-Color+8
	.bc = -1, //Standard Background Color //IRC-Color+8
	.bold = 0,
	.italic = 0,
	.underline = 0,
	.blink = 0,
	.crossedout = 0,
	.fc_colormode = MODE_3BIT,
	.bc_colormode = MODE_3BIT,
	.highlighted = 0,
};

int statesDiffer(const struct State *const a, const struct State *const b) {
	return
		(a->fc != b->fc) ||
		(a->bc != b->bc) ||
		(a->bold != b->bold) ||
		(a->italic != b->italic) ||
		(a->underline != b->underline) ||
		(a->blink != b->blink) ||
		(a->crossedout != b->crossedout) ||
		(a->fc_colormode != b->fc_colormode) ||
		(a->bc_colormode != b->bc_colormode) ||
		(a->highlighted != b->highlighted);
}

std::string convert(std::string_view input)
{
	char const* fcstyle[10] = {
		"color:dimgray;", //Black
		"color:red;", //Red
		"color:green;", //Green
		"color:olive;", //Yellow
		"color:blue;", //Blue
		"color:purple;", //Purple
		"color:teal;", //Cyan
		"color:gray;", //White
		"color:white;", //Background
		"color:black;" //Foreground
	};

	char const* bcstyle[10] = {
		"background-color:black;", //Black
		"background-color:red;", //Red
		"background-color:green;", //Green
		"background-color:olive;", //Yellow
		"background-color:blue;", //Blue
		"background-color:purple;", //Purple
		"background-color:teal;", //Cyan
		"background-color:gray;", //White
		"background-color:white;", //Background
		"background-color:black;", //Foreground
	};

    auto buf = fmt::memory_buffer();
    auto iter = std::back_inserter(buf);
	//Begin of Conversion
	struct State state = default_state;
	struct State oldstate;
	int c;
	int negative = 0; //No negative image
	int special_char = 0; //No special characters
	int line=0;
	int newline=-1;
	size_t i=0;
	int line_break=0;
	while (i<input.size())
	{
		c=getNextChar(input, i);
		if (c=='\033')
		{
			oldstate = state;
			//Searching the end (a letter) and safe the insert:
			c=getNextChar(input, i);
			if ( c == '[' ) // CSI code, see https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
			{
				char buffer[1024];
				buffer[0] = '[';
				int counter=1;
				while (((c<'A') || ((c>'Z') && (c<'a')) || (c>'z')) && c!=EOF)
				{
					c=getNextChar(input, i);
					buffer[counter]=c;
					if (c=='>') //end of htop
						break;
					counter++;
					if (counter>1022)
						break;
				}
				buffer[counter-1]=0;
				pelem elem;
				switch (c)
				{
					case 'm':
					{
						elem=parseInsert(buffer);
						pelem momelem=elem;
						while (momelem!=NULL)
						{
							switch (momelem->value)
							{
								case 0: // 0 - Reset all
									state = default_state;
									negative=0; special_char=0;
									break;

								case 1: // 1 - Enable Bold
									state.bold=1;
									break;

								case 3: // 3 - Enable Italic
									state.italic=1;
									break;

								case 4: // 4 - Enable underline
									state.underline=1;
									break;

								case 5: // 5 - Slow Blink
									state.blink=1;
									break;

								case 7: // 7 - Inverse video
									swapColors(&state);
									negative = !negative;
									break;

								case 9: // 9 - Enable Crossed-out
									state.crossedout=1;
									break;

								case 21: // 21 - Reset bold
								case 22: // 22 - Not bold, not "high intensity" color
									state.bold=0;
									break;

								case 23: // 23 - Reset italic
									state.italic=0;
									break;

								case 24: // 23 - Reset underline
									state.underline=0;
									break;

								case 25: // 25 - Reset blink
									state.blink=0;
									break;

								case 27: // 27 - Reset Inverted
									if (negative)
									{
										swapColors(&state); //7, 7X is not defined (and supported)
										negative = 0;
									}
									break;

								case 29: // 29 - Reset crossed-out
									state.crossedout=0;
									break;

								case 30:
								case 31:
								case 32:
								case 33:
								case 34:
								case 35:
								case 36:
								case 37:
								case 38: // 3X - Set foreground color
									{
										int *dest = &(state.fc);
										if (negative != 0)
											dest=&(state.bc);
										if (momelem->value == 38 &&
											momelem->next &&
											momelem->next->value == 5 &&
											momelem->next->next)// 38;5;<n> -> 8 Bit
										{
											momelem = momelem->next->next;
											state.fc_colormode = MODE_8BIT;
											if (momelem->value >=8 && momelem->value <=15)
											{
												state.highlighted = 1;
												*dest = momelem->value-8;
											}
											else
											{
												state.highlighted = 0;
												*dest = momelem->value;
											}
										}
										else
										if (momelem->value == 38 &&
											momelem->next &&
											momelem->next->value == 2 &&
											momelem->next->next)// 38;2;<n> -> 24 Bit
										{
											momelem = momelem->next->next;
											pelem r,g,b;
											r = momelem;
											momelem = momelem->next;
											g = momelem;
											if ( momelem )
												momelem = momelem->next;
											b = momelem;
											if ( r && g && b )
											{
												state.highlighted = 0;
												state.fc_colormode = MODE_24BIT;
												*dest =
													(r->value & 255) * 65536 +
													(g->value & 255) * 256 +
													(b->value & 255);
											}
										}
										else
										{
											state.fc_colormode = MODE_3BIT;
											state.highlighted = 0;
											*dest=momelem->value-30;
										}
									}
									break;
								case 39: // Set foreground color to default
									state.fc_colormode = MODE_3BIT;
									state.highlighted = 0;
									state.fc = -1;
									break;
								case 40:
								case 41:
								case 42:
								case 43:
								case 44:
								case 45:
								case 46:
								case 47:
								case 48: // 4X - Set background color
									{
										int *dest = &(state.bc);
										if (negative != 0)
											dest=&(state.fc);
										if (momelem->value == 48 &&
											momelem->next &&
											momelem->next->value == 5 &&
											momelem->next->next)// 48;5;<n> -> 8 Bit
										{
											momelem = momelem->next->next;
											state.bc_colormode = MODE_8BIT;
											if (momelem->value >=8 && momelem->value <=15)
											{
												state.highlighted = 1;
												*dest = momelem->value-8;
											}
											else
											{
												state.highlighted = 0;
												*dest = momelem->value;
											}
										}
										else
										if (momelem->value == 48 &&
											momelem->next &&
											momelem->next->value == 2 &&
											momelem->next->next)// 48;2;<n> -> 24 Bit
										{
											momelem = momelem->next->next;
											pelem r,g,b;
											r = momelem;
											momelem = momelem->next;
											g = momelem;
											if ( momelem )
												momelem = momelem->next;
											b = momelem;
											if ( r && g && b )
											{
												state.bc_colormode = MODE_24BIT;
												state.highlighted = 0;
												*dest =
													(r->value & 255) * 65536 +
													(g->value & 255) * 256 +
													(b->value & 255);
											}
										}
										else
										{
											state.bc_colormode = MODE_3BIT;
											state.highlighted = 0;
											*dest=momelem->value-40;
										}
									}
									break;
								case 49: // Set background color to default
									state.bc_colormode = MODE_3BIT;
									state.highlighted = 0;
									state.bc = -1;
									break;
								case 90:
								case 91:
								case 92:
								case 93:
								case 94:
								case 95:
								case 96:
								case 97: // 9X - Set foreground color highlighted
									{
										int *dest = &(state.fc);
										if (negative != 0)
											dest=&(state.bc);
										state.fc_colormode = MODE_3BIT;
										state.highlighted = 1;
										*dest=momelem->value-90;
									}
									break;

								case 100:
								case 101:
								case 102:
								case 103:
								case 104:
								case 105:
								case 106:
								case 107: // 10X - Set background color highlighted
									{
										int *dest = &(state.bc);
										if (negative != 0)
											dest=&(state.fc);
										state.bc_colormode = MODE_3BIT;
										state.highlighted = 1;
										*dest=momelem->value-100;
									}
									break;
							}
							momelem=momelem->next;
						}
						deleteParse(elem);
					}
					break;
					case 'H':
						if (LINE_BREAK_FIX) //a little dirty ...
						{
							elem=parseInsert(buffer);
							pelem second=elem->next;
							if (second==NULL)
								second=elem;
							newline=second->digit[0]-1;
							if (second->digitcount>1)
								newline=(newline+1)*10+second->digit[1]-1;
							if (second->digitcount>2)
								newline=(newline+1)*10+second->digit[2]-1;
							deleteParse(elem);
							if (newline<line)
								line_break=1;
						}
					break;
				}
				if (LINE_BREAK_FIX)
					if (line_break)
					{
						for (;line<80;line++)
							fmt::format_to(iter," ");
					}
				if ( statesDiffer(&state, &oldstate) ) //ANY Change
				{
					if (statesDiffer(&oldstate, &default_state))
						fmt::format_to(iter,"</span>");
					if (statesDiffer(&state, &default_state))
					{
						fmt::format_to(iter,"<span style=\"");
						if (state.underline)
						{
							fmt::format_to(iter,"text-decoration:underline;");
						}
						if (state.bold)
						{
							fmt::format_to(iter,"font-weight:bold;");
						}
						if (state.italic)
						{
							fmt::format_to(iter,"font-style:italic;");
						}
						if (state.blink)
						{
							fmt::format_to(iter,"text-decoration:blink;");
						}
						if (state.crossedout)
						{
							fmt::format_to(iter,"text-decoration:line-through;");
						}
						if (state.highlighted)
						{
							fmt::format_to(iter,"filter: contrast(70%) brightness(190%);");
						}
						switch (state.fc_colormode)
						{
							case MODE_3BIT:
								if (state.fc>=0 && state.fc<=9) fmt::format_to(iter, fmt::runtime(fcstyle[state.fc]));
								break;
							case MODE_8BIT:
								if (state.fc>=0 && state.fc<=7)
									fmt::format_to(iter, fmt::runtime(fcstyle[state.fc]));
								else
								{
									fmt::format_to(iter,"color:#{};", make_rgb(state.fc));
								}
								break;
							case MODE_24BIT:
								fmt::format_to(iter,"color:#{:06x};",state.fc);
								break;
						};
						switch (state.bc_colormode)
						{
							case MODE_3BIT:
								if (state.bc>=0 && state.bc<=9) fmt::format_to(iter, fmt::runtime(bcstyle[state.bc]));
								break;
							case MODE_8BIT:
								if (state.bc>=0 && state.bc<=7)
									fmt::format_to(iter, fmt::runtime(bcstyle[state.bc]));
								else
								{
									fmt::format_to(iter,"background-color:#{};", make_rgb(state.bc));
								}
								break;
							case MODE_24BIT:
								fmt::format_to(iter,"background-color:#{:06x};",state.bc);
								break;
						};
						fmt::format_to(iter,"\">");
					}
				}
			}
			else
			if ( c == ']' ) //Operating System Command (OSC), ignoring for now
			{
				while (c != 2 && c != 7 && c!= 27 && c!=EOF) //STX, BEL or ESC end an OSC.
					c = getNextChar(input, i);
				if ( c == 27 ) // expecting \ after ESC
					c = getNextChar(input, i);
			}
			else
			if ( c == '(' ) //Some VT100 ESC sequences, which should be ignored
			{
				//Reading (and ignoring!) one character should work for "(B"
				//(US ASCII character set), "(A" (UK ASCII character set) and
				//"(0" (Graphic). This whole "standard" is fucked up. Really...
				c = getNextChar(input, i);
				if (c == '0') //we do not ignore ESC(0 ;)
					special_char=1;
				else
					special_char=0;
			}
		}
		else
		if (c==13 && LINE_BREAK_FIX)
		{
			for (;line<80;line++)
				fmt::format_to(iter," ");
			line=0;
			fmt::format_to(iter,"\n");
		}
		else
		if (c==13 && IGNORE_CR)
		{
		}
		else if (c!=8)
		{
			line++;
			if (line_break)
			{
				fmt::format_to(iter,"\n");
				line=0;
				line_break=0;
			}
			if (newline>=0)
			{
				while (newline>line)
				{
					fmt::format_to(iter," ");
					line++;
				}
				newline=-1;
			}
			switch (c)
			{
				case '&':	fmt::format_to(iter,"&amp;"); break;
				case '\"':	fmt::format_to(iter,"&quot;"); break;
				case '<':	fmt::format_to(iter,"&lt;"); break;
				case '>':	fmt::format_to(iter,"&gt;"); break;
				case '\n':
				case 13:
					line=0;
					[[fallthrough]];
				default:
					if (special_char)
						fmt::format_to(iter, "{}", ansi_vt220_character_set[((int)c+32) & 255]);
					else
						fmt::format_to(iter,"{:c}",c);
			}
		}
	}
	if (statesDiffer(&state, &default_state))
		fmt::format_to(iter,"</span>");
	return fmt::to_string(buf);
}
