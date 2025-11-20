
#include "wrapper.h"

#include <unistd.h>

#include <string.h>
#include <stdlib.h>

int send_daemon ( int method, char *restrict session, int fd, char *restrict content, ssize_t len ){

	register int ret = -1;

	register char *buf;

	static const char *restrict sep = ": ";

	register size_t slen;

	switch ( method ){
		case 0:
			if ( !session || fd < 0 ){
				return ret;
			}
			slen = strlen( session ) + strlen( sep ) + len;
			buf = malloc( slen );
			strncpy(
					strcpy(
						strcpy( buf, session ),
						sep
					),
					content,
					slen
			);
			len = write( fd, buf, slen );
			free( buf );

			ret = slen != len;

			break;
	}

	return ret;
}
