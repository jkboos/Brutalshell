
#include "wrapper.h"

#include "yaml.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <alloca.h>
#include <limits.h>

#ifndef BUFLEN
#define BUFLEN 256
#endif

#ifndef BSH_CONFIGPATH
#define BSH_CONFIGPATH "/etc/brutalshell/config.conf"
#endif

struct config _get_yaml( void *, struct config );

struct config get_configure( int argc, char **restrict argv ){

	static const char *arg_default[] = { "/bin/sh", NULL };

	register struct config cfg = {};

	register int fd;
	register void *file;

	register char *restrict cfg_path = BSH_CONFIGPATH;

	register char *restrict cfg_home;

	cfg.logfd = logfd = STDERR_FILENO;

	if ( argc < 2 ){
		cfg.argv = (void *)arg_default;
		memset( cfg.argv, 0, sizeof( *cfg.argv ) * 2 );
		*cfg.argv = "/bin/sh";
	} else {

		if ( !strcmp( "c", *( argv + 1 ) ) && argc > 2 ){
			cfg_path = *( argv + 2 );
			cfg.argv = argv + 3;
		} else if ( !strcmp( "?", *( argv + 1 ) ) ){
			usage( *argv );
			return cfg;
		}else{
			cfg.argv = argv + 1;
		}

		fd = open( cfg_path, O_RDONLY );
		if ( fd < 0 ){

			if ( ( fd = open( getenv( "BSH_CFG" ), O_RDONLY ) ) < 0 ){

				cfg.desc = getenv( "HOME" );
				if ( !cfg.desc ){
					return cfg;
				}
				cfg_home = malloc( PATH_MAX );
				memset( cfg_home, 0, PATH_MAX );
				cfg.desc = strcpy( cfg_home, cfg.desc );
				strncpy( cfg.desc, "/.config/bsh/config.conf", PATH_MAX - strlen( cfg_home ) - 1 );

				cfg.desc = NULL;

				fd = open( cfg_home, O_RDONLY );
				free( cfg_home );
				if ( fd < 0 ){
					return cfg;
				}

			}
		}

		file = fdopen( fd, "r" );

		cfg = _get_yaml( file, cfg );

		fclose( file );

	}

	return cfg;

}

struct config _get_yaml( void *f, struct config cfg ){

	register int is_val;
	register void *k, *v;

	register int done;

	yaml_parser_t par;
	yaml_event_t eve;

	if ( !yaml_parser_initialize( &par ) ){
		goto RET;
	}

	yaml_parser_set_input_file( &par, f );

	done = 0;
	k = NULL;
	v = NULL;

	while ( !done ){
		if ( !yaml_parser_parse( &par, &eve ) ){
			break;
		}

		switch ( eve.type ){
			case YAML_SCALAR_EVENT:
				v = eve.data.scalar.value;

				if ( !is_val ){

					if ( k ){
						free( k );
					}

					k = v;
					is_val = 1;

				} else {

					if ( !strcmp( k, "method" ) ){
						cfg.daemon_method = atoi( v );
					} else if ( !strcmp( k, "path" ) ){
						cfg.desc = strdup( v );
						cfg.len = strlen( v );
					}
				}

				break;
			case YAML_SEQUENCE_START_EVENT:
			case YAML_MAPPING_START_EVENT:
				break;

			case YAML_STREAM_END_EVENT:
				done = 1;
				break;

			default:
				break;
		}

		yaml_event_delete( &eve );
	}

	if ( k ){
		free( k );
	}

	yaml_parser_delete( &par );

RET:

	return cfg;
}
