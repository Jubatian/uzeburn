/*
** Interface to XMBurner
**
** Copyright (C) 2017 Sandor Zsuga (Jubatian)
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY** without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**
** Interfacing with XMBurner, application side components.
*/


#ifndef XMB_IF_H
#define XMB_IF_H


#include <uzebox.h>


/*
** Clears error report
*/
void  xmbu_error_clear(void);


/*
** Returns whether XMBurner detected something (nonzero if so)
*/
u8    xmbu_iserror(void);


/*
** Returns error code as reported by XMBurner
*/
u16   xmbu_geterror(void);


#endif
