
#include "wrapper.h"
#include <termios.h>
#include <unistd.h>

int set_pty( void ){
	struct termios term = origin;
	term.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON );
	term.c_oflag &= ~( OPOST );
	term.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );
	term.c_cflag |= ( CS8 );
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	return tcsetattr( STDIN_FILENO, TCSAFLUSH, &term );
}
