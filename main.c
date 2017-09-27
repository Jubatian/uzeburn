/*
 *  XMBurner for Uzebox
 *  Copyright (C) 2017 Sandor Zsuga (Jubatian)
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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <xmburner.h>
#include "xmb_if.h"



int main(){

	unsigned int i;

	/* Init XMBurner */

	xmb_init();

	/* Clear the screen (fills the vram with tile zero) */

	ClearVram();

	/* Prints some stuff on the screen */

	Print( 0,  0, PSTR("<---------------------------------------------------------->"));
	Print( 0, 11, PSTR("<---------------------------------------------------------->"));
	Print( 0, 12, PSTR("<---------------------------------------------------------->"));
	Print( 0, 13, PSTR("                  60 x 28 code tile display                 "));
	Print( 0, 14, PSTR("                      (360 x 224 pixels)                    "));
	Print( 0, 27, PSTR("<---------------------------------------------------------->"));

	for (i = 1 * 60U; i < 12 * 60U; i++){
		if ((i & 0xFFU) < 96U){ vram[i] = i & 0xFFU; }
	}

	/* Clear errors */

	xmbu_error_clear();

	while(1){
		WaitVsync(1U);
		xmb_run(); /* Run XMBurner tests */
		*((u8*)(0x0039U)) = xmb_next();
		*((u8*)(0x003AU)) = xmbu_iserror();
	}

}
