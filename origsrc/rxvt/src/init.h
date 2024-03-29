/*
 * $Id: init.h,v 1.6 2001/04/02 12:11:26 gcw Exp $
 */

#ifndef _INIT_H_
#define _INIT_H_

#ifdef HAVE_XSETLOCALE
# define X_LOCALE
# include <X11/Xlocale.h>
#else
# ifdef HAVE_SETLOCALE
#  include <locale.h>
# endif
#endif				/* HAVE_XLOCALE */

#if defined(PTYS_ARE_PTMX) && !defined(__CYGWIN32__)
# include <sys/resource.h>	/* for struct rlimit */
# define _NEW_TTY_CTRL		/* to get proper defines in <termios.h> */
#endif

#ifdef __QNX__			/* autoconf someday */
# include <process.h>
#endif

#ifdef TTY_GID_SUPPORT
# include <grp.h>
#endif

/* ways to deal with getting/setting termios structure */

#ifdef HAVE_TERMIOS_H
/* termios interface */
# ifdef TCSANOW			/* POSIX */
#  define GET_TERMIOS(fd,tios)	tcgetattr (fd, tios)
#  define SET_TERMIOS(fd,tios)		\
	cfsetospeed (tios, BAUDRATE),	\
	cfsetispeed (tios, BAUDRATE),	\
	tcsetattr (fd, TCSANOW, tios)
# else
#  ifdef TIOCSETA
#   define GET_TERMIOS(fd,tios)	ioctl (fd, TIOCGETA, tios)
#   define SET_TERMIOS(fd,tios)		\
	tios->c_cflag |= BAUDRATE,	\
	ioctl (fd, TIOCSETA, tios)
#  else
#   define GET_TERMIOS(fd,tios)	ioctl (fd, TCGETS, tios)
#   define SET_TERMIOS(fd,tios)		\
	tios->c_cflag |= BAUDRATE,	\
	ioctl (fd, TCSETS, tios)
#  endif
# endif
# define SET_TTYMODE(fd,tios)		SET_TERMIOS (fd, tios)
#else
/* sgtty interface */

# define SET_TTYMODE(fd,tt)				\
	tt->sg.sg_ispeed = tt->sg.sg_ospeed = BAUDRATE,	\
	ioctl (fd, TIOCSETP, &(tt->sg)),		\
	ioctl (fd, TIOCSETC, &(tt->tc)),		\
	ioctl (fd, TIOCSLTC, &(tt->lc)),		\
	ioctl (fd, TIOCSETD, &(tt->line)),		\
	ioctl (fd, TIOCLSET, &(tt->local))
#endif				/* HAVE_TERMIOS_H */

/* use the fastest baud-rate */
#ifdef B38400
# define BAUDRATE	B38400
#else
# ifdef B19200
#  define BAUDRATE	B19200
# else
#  define BAUDRATE	B9600
# endif
#endif

/* Disable special character functions */
#ifdef _POSIX_VDISABLE
# define VDISABLE	_POSIX_VDISABLE
#else
# define VDISABLE	255
#endif

/*----------------------------------------------------------------------*
 * system default characters if defined and reasonable
 */
#ifndef CINTR
# define CINTR		'\003'	/* ^C */
#endif
#ifndef CQUIT
# define CQUIT		'\034'	/* ^\ */
#endif
#ifndef CERASE
# ifdef linux
#  define CERASE	'\177'	/* ^? */
# else
#  define CERASE	'\010'	/* ^H */
# endif
#endif
#ifndef CKILL
# define CKILL		'\025'	/* ^U */
#endif
#ifndef CEOF
# define CEOF		'\004'	/* ^D */
#endif
#ifndef CSTART
# define CSTART		'\021'	/* ^Q */
#endif
#ifndef CSTOP
# define CSTOP		'\023'	/* ^S */
#endif
#ifndef CSUSP
# define CSUSP		'\032'	/* ^Z */
#endif
#ifndef CDSUSP
# define CDSUSP		'\031'	/* ^Y */
#endif
#ifndef CRPRNT
# define CRPRNT		'\022'	/* ^R */
#endif
#ifndef CFLUSH
# define CFLUSH		'\017'	/* ^O */
#endif
#ifndef CWERASE
# define CWERASE	'\027'	/* ^W */
#endif
#ifndef CLNEXT
# define CLNEXT		'\026'	/* ^V */
#endif

#ifndef VDISCRD
# ifdef VDISCARD
#  define VDISCRD	VDISCARD
# endif
#endif

#ifndef VWERSE
# ifdef VWERASE
#  define VWERSE	VWERASE
# endif
#endif

#ifndef O_NOCTTY
# define O_NOCTTY	0
#endif
#ifndef O_NDELAY
# define O_NDELAY	O_NONBLOCK	/* QNX, at least */
#endif
#ifndef ONLCR
# define ONLCR		0		/* QNX, at least */
#endif

#define CONSOLE		"/dev/console"	/* console device */

#include "init.intpro"	/* PROTOS for internal routines */
#endif	/* _INIT_H_ */
