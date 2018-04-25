/*
 *  XMBurner for Uzebox
 *  Copyright (C) 2018 Sandor Zsuga (Jubatian)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <xmburner.h>
#include "xmb_if.h"



/* Tile indices of boxes indicating XMBurner component runs and errors */
#define BOX_GREEN  0x60U
#define BOX_YELLOW 0x61U
#define BOX_RED    0x62U
#define BOX_GRAY   0x63U

/* XMBurner component list origin on the VRAM */
#define CLIST_X    3U
#define CLIST_Y    6U


/* XMBurner logo image for the VRAM, 18 x 12 */
static const u8 xmb_logo_data[18U * 12U] PROGMEM = {
 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x68U, 0x69U, 0x6AU, 0x6BU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
 0x00U, 0x00U, 0x6CU, 0x6DU, 0x6EU, 0x6FU, 0x70U, 0x71U, 0x72U, 0x73U, 0x74U, 0x75U, 0x76U, 0x77U, 0x78U, 0x79U, 0x00U, 0x00U,
 0x00U, 0x7AU, 0x7BU, 0x7CU, 0x7DU, 0x7EU, 0x7FU, 0x80U, 0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U, 0x88U, 0x89U, 0x00U,
 0x00U, 0x8AU, 0x8BU, 0x8CU, 0x8DU, 0x8EU, 0x8FU, 0x90U, 0x91U, 0x92U, 0x93U, 0x94U, 0x95U, 0x96U, 0x97U, 0x98U, 0x99U, 0x00U,
 0x00U, 0x9AU, 0x9BU, 0x9CU, 0x9DU, 0x9EU, 0x9FU, 0xA0U, 0xA1U, 0xA2U, 0xA3U, 0xA4U, 0xA5U, 0xA6U, 0xA7U, 0xA8U, 0xA9U, 0x00U,
 0xAAU, 0xABU, 0xACU, 0xADU, 0xAEU, 0xAFU, 0xB0U, 0xB1U, 0xB2U, 0xB3U, 0xB4U, 0xB5U, 0xB6U, 0xB7U, 0xB8U, 0xB9U, 0xBAU, 0xBBU,
 0xBCU, 0xBDU, 0xBEU, 0xBFU, 0xC0U, 0xC1U, 0xC2U, 0xC3U, 0xC4U, 0xC5U, 0xC6U, 0xC7U, 0xC8U, 0xC9U, 0xCAU, 0xCBU, 0xCCU, 0xCDU,
 0x00U, 0xCEU, 0xCFU, 0xD0U, 0xD1U, 0xD2U, 0xD3U, 0xD4U, 0xD5U, 0xD6U, 0xD7U, 0xD8U, 0xD9U, 0xDAU, 0xDBU, 0xDCU, 0xDDU, 0x00U,
 0x00U, 0x00U, 0x00U, 0xDEU, 0xDFU, 0xE0U, 0xE1U, 0xE2U, 0xE3U, 0xE4U, 0xE5U, 0xE6U, 0xE7U, 0xE8U, 0xE9U, 0x00U, 0x00U, 0x00U,
 0x00U, 0x00U, 0x00U, 0x00U, 0xEAU, 0xEBU, 0xECU, 0xEDU, 0xEEU, 0xEFU, 0xF0U, 0xF1U, 0xF2U, 0xF3U, 0x00U, 0x00U, 0x00U, 0x00U,
 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0xF4U, 0xF5U, 0xF6U, 0xF7U, 0xF8U, 0xF9U, 0xFAU, 0xFBU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0xFCU, 0xFDU, 0xFEU, 0xFFU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
};


/* Text descriptions */
static const u8 text_data[] PROGMEM =
 "      XMBurner for UzeBox\n"
 "  by: Sandor Zsuga (Jubatian)\n"
 "   Library version: ........\n"
 "\n"
 "Uzebox & comps. license : GPLv3\n"
 "XMBurner lib. license ..: MPLv2\n";


/* Box legend (0x84 is the down->left corner, 0x85 the arrow head) */
static const u8 text_legend[] PROGMEM =
 "\x84----\x85 Unknown fault code\n"
 "|\x84---\x85 Execution chain fault\n"
 "|| \x84-\x85 Detected fault\n"
 "|| |\n";


/* XMBurner component list printout for the VRAM */
static const u8 xmb_comp_list[4U * 20U] PROGMEM = {
 'c' - 0x20U, 'r' - 0x20U, 'e' - 0x20U, 'g' - 0x20U,
 'c' - 0x20U, 'o' - 0x20U, 'n' - 0x20U, 'd' - 0x20U,
 'j' - 0x20U, 'u' - 0x20U, 'm' - 0x20U, 'p' - 0x20U,
 ' ' - 0x20U, 'c' - 0x20U, 'r' - 0x20U, 'c' - 0x20U,
 ' ' - 0x20U, 'r' - 0x20U, 'a' - 0x20U, 'm' - 0x20U,
 ' ' - 0x20U, 'l' - 0x20U, 'o' - 0x20U, 'g' - 0x20U,
 ' ' - 0x20U, 's' - 0x20U, 'u' - 0x20U, 'b' - 0x20U,
 ' ' - 0x20U, 'a' - 0x20U, 'd' - 0x20U, 'd' - 0x20U,
 'a' - 0x20U, 'l' - 0x20U, 'e' - 0x20U, 'x' - 0x20U,
 'w' - 0x20U, 'o' - 0x20U, 'p' - 0x20U, 's' - 0x20U,
 ' ' - 0x20U, 'b' - 0x20U, 'i' - 0x20U, 't' - 0x20U,
 ' ' - 0x20U, 'm' - 0x20U, 'u' - 0x20U, 'l' - 0x20U,
 'a' - 0x20U, 'b' - 0x20U, 's' - 0x20U, 'a' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
 ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U, ' ' - 0x20U,
};


/* XMBurner component fault code counts */
static const u8 xmb_comp_fidcnt[20U] PROGMEM = {
 XMB_FID_CNT_CREG,
 XMB_FID_CNT_COND,
 XMB_FID_CNT_JUMP,
 XMB_FID_CNT_CRC,
 XMB_FID_CNT_RAM,
 XMB_FID_CNT_LOG,
 XMB_FID_CNT_SUB,
 XMB_FID_CNT_ADD,
 XMB_FID_CNT_ALEX,
 XMB_FID_CNT_WOPS,
 XMB_FID_CNT_BIT,
 XMB_FID_CNT_MUL,
 XMB_FID_CNT_ABSA,
 0,
 0,
 0,
 0,
 0,
 0,
 0
};


/* Draw display frame for positioning */
static void draw_frame(void)
{
 u8 i;
 u8 j;
 j = 0U;
 for (i = 0U; i < 60U; i++){
  vram[(u16)( 0U) * VRAM_TILES_H + i] = j + '0' - 0x20U;
  vram[(u16)(27U) * VRAM_TILES_H + i] = '-' - 0x20U;
  j = j + 1U;
  if (j >= 10U){ j = 0U; }
 }
 j = 1U;
 for (i = 1U; i < 28U; i++){
  vram[(u16)(i) * VRAM_TILES_H +  0U] = j + '0' - 0x20U;
  vram[(u16)(i) * VRAM_TILES_H + 59U] = '|' - 0x20U;
  j = j + 1U;
  if (j >= 10U){ j = 0U; }
 }
 vram[(u16)(27U) * VRAM_TILES_H + 59U] = 0x86U - 0x20U; /* Bottom left corner */
}



/* Output the logo image on VRAM */
static void draw_logo(u8 x, u8 y)
{
 u8 i;
 u8 j;
 for (j = 0U; j < 12U; j++){
  for (i = 0U; i < 18U; i++){
   vram[(u16)(j + y) * VRAM_TILES_H + (i + x)] =
       pgm_read_byte(&xmb_logo_data[j * 18U + i]);
  }
 }
}



/* Generic text output by pointer */
static void draw_generic_text(u8 const* sptr, u8 x, u8 y)
{
 u8 chr;
 u8 xpos = x;
 do{
  chr = pgm_read_byte(sptr);
  if (chr >= 0x20U){
   vram[(u16)(y) * VRAM_TILES_H + xpos] = chr - 0x20U;
  }
  sptr ++;
  xpos ++;
  if (chr == '\n'){
   xpos = x;
   y ++;
  }
 }while (chr != 0U);
}



/* Outputs a single numeric digit */
static void draw_digit(u8 digit, u8 x, u8 y)
{
 vram[(u16)(y) * VRAM_TILES_H + x] = ((digit & 0xFU) + (u8)('0')) - 0x20U;
}



/* Outputs text data */
static void draw_text(u8 x, u8 y)
{
 u32 ver = xmb_version();
 u8  i;

 draw_generic_text(&text_data[0], x, y);

 for (i = 0U; i < 8U; i++){
  draw_digit(ver, x + 27U - i, y + 2U);
  ver >>= 4;
 }
}



/* Output the component list in VRAM */
static void draw_comp(void)
{
 u8 i;
 u8 j;
 draw_generic_text(&text_legend[0], CLIST_X + 7U, CLIST_Y - 4U);
 for (j = 0U; j < XMB_COMPONENT_COUNT; j++){
  for (i = 0U; i < 4U; i++){
   vram[(u16)(j + CLIST_Y) * VRAM_TILES_H + (i + CLIST_X + 2U)] =
       pgm_read_byte(&xmb_comp_list[j * 4U + i]);
  }
 }
}



/* Output an indicator box at a given XMBurner component position */
static void draw_comp_box(u8 comp, u8 pos, u8 coldef)
{
 if (comp >= XMB_COMPONENT_COUNT){ return; }
 if (pos == 0xFFU){ /* Chain column */
  vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (8U + CLIST_X)] = coldef;
 }else{             /* Normal indicators */
  if (pos >= pgm_read_byte(&(xmb_comp_fidcnt[comp]))){
   vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (7U + CLIST_X)] = coldef;
  }else{
   vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (10U + pos + CLIST_X)] = coldef;
  }
 }
}



/* Retrieve indicator box color definition */
static u8 get_comp_box(u8 comp, u8 pos)
{
 u8 ret = BOX_GRAY;
 if (comp < XMB_COMPONENT_COUNT){
  if (pos == 0xFFU){ /* Chain column */
   ret = vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (8U + CLIST_X)];
  }else{             /* Normal indicators */
   if (pos >= pgm_read_byte(&(xmb_comp_fidcnt[comp]))){
    ret = vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (7U + CLIST_X)];
   }else{
    ret = vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + (10U + pos + CLIST_X)];
   }
  }
 }
 if ( (ret != BOX_GREEN) &&
      (ret != BOX_YELLOW) &&
      (ret != BOX_RED) ){ ret = BOX_GRAY; }
 return ret;
}



/* Output component indicator */
static void draw_comp_ind(u8 comp)
{
 u8 i;
 if (comp >= XMB_COMPONENT_COUNT){ return; }
 for (i = 0U; i < XMB_COMPONENT_COUNT; i++){
  vram[(u16)(i + CLIST_Y) * VRAM_TILES_H + CLIST_X] = ' ' - 0x20U;
 }
 vram[(u16)(comp + CLIST_Y) * VRAM_TILES_H + CLIST_X] = '+' - 0x20U;
}



/* Clear component list boxes to non-executed, keeping red and yellow boxes
** (preserving detected errors). */
static void draw_comp_clear(void)
{
 u8 i;
 u8 j;
 u8 col;
 for (j = 0U; j < XMB_COMPONENT_COUNT; j++){
  for (i = 0xFFU; i != (pgm_read_byte(&(xmb_comp_fidcnt[j])) + 1U); i++){
   col = get_comp_box(j, i);
   if (col == BOX_GREEN){ col = BOX_GRAY; }
   draw_comp_box(j, i, col);
  }
 }
}



/* Clear a component row to executed, correct, but keeping yellow boxes, and
** turning red boxes to yellow (error indicators) */
static void draw_comp_passed(u8 comp)
{
 u8 i;
 u8 col;
 if (comp >= XMB_COMPONENT_COUNT){ return; }
 for (i = 0xFFU; i != (pgm_read_byte(&(xmb_comp_fidcnt[comp])) + 1U); i++){
  col = get_comp_box(comp, i);
  if (col == BOX_GRAY){ col = BOX_GREEN; }
  if (col == BOX_RED){  col = BOX_YELLOW; }
  draw_comp_box(comp, i, col);
 }
}



/* Inits or re-inits (used at boot and when XMBurner detects an error) */
static void init(void)
{
 /* Initially prepare or rebuild display */

 draw_frame();
 draw_logo(32U,  6U);
 draw_text(25U, 20U);
 draw_comp();
 draw_comp_clear();

 /* Init or reinit XMBurner */

 xmb_init();
 xmbu_error_clear();
}



/* Main entry point */
int main(){

 u8 comp;

 ClearVram();
 init();

 /* Run main loop, XMBurner tests */

 while(1){

  WaitVsync(1U);

  comp = xmb_next();
  xmb_run(); /* Run XMBurner tests */

  draw_comp_ind(comp);
  if (xmbu_iserror()){ /* XMBurner detected an error */
   draw_comp_box(xmbu_geterror() >> 8, xmbu_geterror(), BOX_RED);
   init();
  }else{
   draw_comp_passed(comp);
  }

 }

}
