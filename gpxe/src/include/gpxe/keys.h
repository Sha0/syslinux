#ifndef _GPXE_KEYS_H
#define _GPXE_KEYS_H

/** @file
 *
 * Key definitions
 *
 */

FILE_LICENCE ( GPL2_OR_LATER );

/*
 * Symbolic names for some standard ASCII characters
 *
 */

#define NUL		0x00
#define CTRL_A		0x01
#define CTRL_B		0x02
#define CTRL_C		0x03
#define CTRL_D		0x04
#define CTRL_E		0x05
#define CTRL_F		0x06
#define CTRL_G		0x07
#define CTRL_H		0x08
#define CTRL_I		0x09
#define CTRL_J		0x0a
#define CTRL_K		0x0b
#define CTRL_L		0x0c
#define CTRL_M		0x0d
#define CTRL_N		0x0e
#define CTRL_O		0x0f
#define CTRL_P		0x10
#define CTRL_Q		0x11
#define CTRL_R		0x12
#define CTRL_S		0x13
#define CTRL_T		0x14
#define CTRL_U		0x15
#define CTRL_V		0x16
#define CTRL_W		0x17
#define CTRL_X		0x18
#define CTRL_Y		0x19
#define CTRL_Z		0x1a

#define BACKSPACE	CTRL_H
#define TAB		CTRL_I
#define LF		CTRL_J
#define CR		CTRL_M
#define ESC		0x1b

/*
 * Special keys outside the normal ASCII range 
 *
 *
 * The names are chosen to match those used by curses.  The values are
 * chosen to facilitate easy conversion from a received ANSI escape
 * sequence to a KEY_XXX constant.
 */

#define KEY_ANSI( n, terminator ) ( 0x100 * ( (n) + 1 ) + (terminator) )

#define KEY_MIN		0x101
#define KEY_UP		KEY_ANSI ( 0, 'A' )	/**< Up arrow */
#define KEY_DOWN	KEY_ANSI ( 0, 'B' )	/**< Down arrow */
#define KEY_RIGHT	KEY_ANSI ( 0, 'C' )	/**< Right arrow */
#define KEY_LEFT	KEY_ANSI ( 0, 'D' )	/**< Left arrow */
#define KEY_END		KEY_ANSI ( 0, 'F' )	/**< End */
#define KEY_HOME	KEY_ANSI ( 0, 'H' )	/**< Home */
#define KEY_IC		KEY_ANSI ( 2, '~' )	/**< Insert */
#define KEY_DC		KEY_ANSI ( 3, '~' )	/**< Delete */
#define KEY_PPAGE	KEY_ANSI ( 5, '~' )	/**< Page up */
#define KEY_NPAGE	KEY_ANSI ( 6, '~' )	/**< Page down */
#define KEY_F8		KEY_ANSI ( 19, '~' )	/**< F8 (for PXE) */

/* Not in the [KEY_MIN,KEY_MAX] range; terminals seem to send these as
 * normal ASCII values.
 */
#define KEY_BACKSPACE	BACKSPACE
#define KEY_ENTER	LF

#endif /* _GPXE_KEYS_H */
