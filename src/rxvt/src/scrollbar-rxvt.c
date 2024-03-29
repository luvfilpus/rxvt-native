/*--------------------------------*-C-*---------------------------------*
 * File:	scrollbar-rxvt.c
 *----------------------------------------------------------------------*
 * $Id: scrollbar-rxvt.c,v 1.11 2003/06/24 07:18:36 gcw Exp $
 *
 * Copyright (c) 1997,1998 mj olesen <olesen@me.QueensU.CA>
 * Copyright (c) 1999-2001 Geoff Wing <gcw@pobox.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *----------------------------------------------------------------------*/

#include "../config.h"		/* NECESSARY */
#include "rxvt.h"		/* NECESSARY */
#include "scrollbar-rxvt.intpro"	/* PROTOS for internal routines */

/*----------------------------------------------------------------------*/
#if defined(RXVT_SCROLLBAR)

/* draw triangular button with a shadow of SHADOW (1 or 2) pixels */
/* INTPROTO */
void
rxvt_Draw_button(rxvt_t *r, int x, int y, int state, int dirn)
{
    unsigned int    sz, sz2;
    XPoint          pt[3];
    GC              top, bot;

    sz = r->scrollBar.width;
    sz2 = sz / 2;
    switch (state) {
    case +1:
	top = r->h->topShadowGC;
	bot = r->h->botShadowGC;
	break;
    case -1:
	top = r->h->botShadowGC;
	bot = r->h->topShadowGC;
	break;
    default:
	top = bot = r->h->scrollbarGC;
	break;
    }

/* fill triangle */
    pt[0].x = x;
    pt[1].x = x + sz - 1;
    pt[2].x = x + sz2;
    if (dirn == UP) {
	pt[0].y = pt[1].y = y + sz - 1;
	pt[2].y = y;
    } else {
	pt[0].y = pt[1].y = y;
	pt[2].y = y + sz - 1;
    }
    XFillPolygon(r->Xdisplay, r->scrollBar.win, r->h->scrollbarGC,
		 pt, 3, Convex, CoordModeOrigin);

/* draw base */
    XDrawLine(r->Xdisplay, r->scrollBar.win, (dirn == UP ? bot : top),
	      pt[0].x, pt[0].y, pt[1].x, pt[1].y);

/* draw shadow on left */
    pt[1].x = x + sz2 - 1;
    pt[1].y = y + (dirn == UP ? 0 : sz - 1);
    XDrawLine(r->Xdisplay, r->scrollBar.win, top,
	      pt[0].x, pt[0].y, pt[1].x, pt[1].y);

#if (SHADOW > 1)
/* doubled */
    pt[0].x++;
    if (dirn == UP) {
	pt[0].y--;
	pt[1].y++;
    } else {
	pt[0].y++;
	pt[1].y--;
    }
    XDrawLine(r->Xdisplay, r->scrollBar.win, top,
	      pt[0].x, pt[0].y, pt[1].x, pt[1].y);
#endif
/* draw shadow on right */
    pt[1].x = x + sz - 1;
/*  pt[2].x = x + sz2; */
    pt[1].y = y + (dirn == UP ? sz - 1 : 0);
    pt[2].y = y + (dirn == UP ? 0 : sz - 1);
    XDrawLine(r->Xdisplay, r->scrollBar.win, bot,
	      pt[2].x, pt[2].y, pt[1].x, pt[1].y);
#if (SHADOW > 1)
/* doubled */
    pt[1].x--;
    if (dirn == UP) {
	pt[2].y++;
	pt[1].y--;
    } else {
	pt[2].y--;
	pt[1].y++;
    }
    XDrawLine(r->Xdisplay, r->scrollBar.win, bot,
	      pt[2].x, pt[2].y, pt[1].x, pt[1].y);
#endif
}

/* EXTPROTO */
int
rxvt_scrollbar_show_rxvt(rxvt_t *r, int update __attribute__((unused)), int last_top, int last_bot, int scrollbar_len)
{
    int             sbshadow = r->sb_shadow;
    int             sbwidth = (int)r->scrollBar.width;

    if ((r->scrollBar.init & R_SB_RXVT) == 0) {
	XGCValues       gcvalue;

	r->scrollBar.init |= R_SB_RXVT;
	gcvalue.foreground = r->PixColors[Color_trough];
	if (sbshadow) {
	    XSetWindowBackground(r->Xdisplay, r->scrollBar.win,
				 gcvalue.foreground);
	    XClearWindow(r->Xdisplay, r->scrollBar.win);
	}
    } else {
/* instead of XClearWindow (r->Xdisplay, r->scrollBar.win); */
	if (last_top < r->scrollBar.top)
	    XClearArea(r->Xdisplay, r->scrollBar.win,
		       sbshadow, last_top,
		       sbwidth, (r->scrollBar.top - last_top),
		       False);

	if (r->scrollBar.bot < last_bot)
	    XClearArea(r->Xdisplay, r->scrollBar.win,
		       sbshadow, r->scrollBar.bot,
		       sbwidth, (last_bot - r->scrollBar.bot),
		       False);
    }

/* scrollbar slider */
#ifdef SB_BORDER
    {
	int             xofs;

	if (r->Options & Opt_scrollBar_right)
	    xofs = 0;
	else
	    xofs = sbshadow ? sbwidth : sbwidth - 1;

	XDrawLine(r->Xdisplay, r->scrollBar.win, r->h->botShadowGC,
		  xofs, 0, xofs, r->scrollBar.end + sbwidth);
    }
#endif
    XFillRectangle(r->Xdisplay, r->scrollBar.win, r->h->scrollbarGC,
		   sbshadow, r->scrollBar.top, sbwidth,
		   scrollbar_len);

    if (sbshadow)
	/* trough shadow */
	rxvt_Draw_Shadow(r->Xdisplay, r->scrollBar.win,
			 r->h->botShadowGC, r->h->topShadowGC,
			 0, 0,
			 sbwidth + 2 * sbshadow, /* scrollbar_TotalWidth() */
			 r->scrollBar.end + (sbwidth + 1) + sbshadow);
/* shadow for scrollbar slider */
    rxvt_Draw_Shadow(r->Xdisplay, r->scrollBar.win,
		     r->h->topShadowGC, r->h->botShadowGC,
		     sbshadow, r->scrollBar.top, sbwidth,
		     scrollbar_len);

/*
 * Redraw scrollbar arrows
 */
    rxvt_Draw_button(r, sbshadow, sbshadow,
		     (scrollbar_isUp() ? -1 : +1), UP);
    rxvt_Draw_button(r, sbshadow, (r->scrollBar.end + 1),
		     (scrollbar_isDn() ? -1 : +1), DN);
    return 1;
}
#endif				/* RXVT_SCROLLBAR */
/*----------------------- end-of-file (C source) -----------------------*/
