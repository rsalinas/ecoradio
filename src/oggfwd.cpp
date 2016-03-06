#include "oggfwd.h"


///*
// * oggfwd    -- Forward an Ogg stream from stdin to an Icecast server
// *		A useful demonstration of the libshout API
// *
// * This program is distributed under the GNU General Public License, version 2.
// * A copy of this license is included with this source.
// *
// * This program is provided "as-is", with no explicit or implied warranties of
// * any kind.
// *
// * Copyright (C) 2003-2006,	J <j@v2v.cc>,
// *				rafael2k <rafael(at)riseup(dot)net>,
// *				Moritz Grimm <gtgbr@gmx.net>
// */
///* thanx to rhatto <rhatto (AT) riseup (DOT) net> and others at submidialogia :-P */

//#include <sys/types.h>
//#include <sys/param.h>
//#include <errno.h>
//#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#ifndef NO_UNISTD_H
//#  include <unistd.h>
//#endif /* no-NO_UNISTD_H */

//#include <shout/shout.h>

//extern char    *__progname;
//extern char    *optarg;
//extern int	optind;
//extern int	errno;

//volatile sig_atomic_t	print_total = 0;
//volatile sig_atomic_t	quit = 0;

//#define BUFFERSIZE	4096

//#if defined(__dead)
//__dead void
//#else
//void
//#endif /* __dead */
//usage(void)
//{
//    printf("usage: %s "
//           "[-hp] "
//           "[-d description] "
//           "[-g genre] "
//           "[-n name] "
//           "[-u URL]\n"
//           "              address port password mountpoint\n",
//           __progname);
//    exit(1);
//}

//void
//sig_handler(int sig)
//{
//    switch (sig) {
//    case SIGHUP:
//        print_total = 1;
//        break;
//    case SIGTERM:
//    case SIGINT:
//        quit = 1;
//        break;
//    default:
//        /* ignore */
//        break;
//    }
//}

//void
//set_argument_string(char **param, char *opt, char optname)
//{
//    size_t siz;

//    if (*param) {
//        fprintf(stderr, "%s: Parameter -%c given multiple times\n",
//            __progname, optname);
//        usage();
//    }

//    siz = strlen(opt) + 1;
//    if (siz >= MAXPATHLEN) {
//        fprintf(stderr, "%s: Argument for parameter -%c too long\n",
//            __progname, optname);
//        exit(1);
//    }

//    if ((*param = static_cast<char*>(malloc(siz))) == NULL) {
//        fprintf(stderr, "%s: %s\n", __progname, strerror(errno));
//        exit(1);
//    }

//    snprintf(*param, siz, "%s", opt);
//}

//int
//main_(int argc, char **argv)
//{
//    shout_t        *shout;
//    unsigned char	buff[BUFFERSIZE];
//    int		ret, ch;
//    unsigned int	pFlag;
//    char	       *description, *genre, *name, *url;
//    size_t		bytes_read;
//    unsigned short	port;
//    unsigned long long total;

//    pFlag = 0;
//    description = genre = name = url = NULL;
//    while ((ch = getopt(argc, argv, "d:g:hn:pu:")) != -1) {
//        switch (ch) {
//        case 'd':
//            set_argument_string(&description, optarg, 'D');
//            break;
//        case 'g':
//            set_argument_string(&genre, optarg, 'g');
//            break;
//        case 'n':
//            set_argument_string(&name, optarg, 'n');
//            break;
//        case 'p':
//            pFlag = 1;
//            break;
//        case 'u':
//            set_argument_string(&url, optarg, 'u');
//            break;
//        case 'h':
//        default:
//            usage();
//        }
//    }
//    argc -= optind;
//    argv += optind;

//    if (argc != 4) {
//        fprintf(stderr, "%s: Wrong number of arguments\n", __progname);
//        usage();
//    }

//    if ((shout = shout_new()) == NULL) {
//        fprintf(stderr, "%s: Could not allocate shout_t\n",
//            __progname);
//        return (1);
//    }

//    if (shout_set_host(shout, argv[0]) != SHOUTERR_SUCCESS) {
//        fprintf(stderr, "%s: Error setting hostname: %s\n", __progname,
//            shout_get_error(shout));
//        return (1);
//    }

//    if (sscanf(argv[1], "%hu", &port) != 1) {
//        fprintf(stderr, "Invalid port `%s'\n", argv[1]);
//        usage();
//    }
//    if (shout_set_port(shout, port) != SHOUTERR_SUCCESS) {
//        fprintf(stderr, "%s: Error setting port: %s\n", __progname,
//            shout_get_error(shout));
//        return (1);
//    }

//    if (shout_set_password(shout, argv[2]) != SHOUTERR_SUCCESS) {
//        fprintf(stderr, "%s: Error setting password: %s\n", __progname,
//            shout_get_error(shout));
//        return (1);
//    }

//    if (shout_set_mount(shout, argv[3]) != SHOUTERR_SUCCESS) {
//        fprintf(stderr, "%s: Error setting mount: %s\n", __progname,
//            shout_get_error(shout));
//        return 1;
//    }

//    shout_set_format(shout, SHOUT_FORMAT_VORBIS);

//    shout_set_public(shout, pFlag);

//    if (description)
//        shout_set_description(shout, description);

//    if (genre)
//        shout_set_genre(shout, genre);

//    if (name)
//        shout_set_name(shout, name);

//    if (url)
//        shout_set_url(shout, url);

//    if (shout_open(shout) == SHOUTERR_SUCCESS) {
//        printf("%s: Connected to server\n", __progname);
//        total = 0;

//        signal(SIGHUP, sig_handler);
//        signal(SIGTERM, sig_handler);
//        signal(SIGINT, sig_handler);

//        while (quit == 0) {
//            bytes_read = fread(buff, 1, sizeof(buff), stdin);
//            total += bytes_read;

//            if (bytes_read > 0) {
//                ret = shout_send(shout, buff, bytes_read);
//                if (ret != SHOUTERR_SUCCESS) {
//                    printf("%s: Send error: %s\n",
//                           __progname,
//                           shout_get_error(shout));
//                    quit = 1;
//                }
//            } else
//                quit = 1;

//            if (quit) {
//                printf("%s: Quitting ...\n", __progname);
//                print_total = 1;
//            }

//            if (print_total) {
//                printf("%s: Total bytes read: %llu\n",
//                       __progname, total);
//                print_total = 0;
//            }

//            shout_sync(shout);
//        }
//    } else {
//        fprintf(stderr, "%s: Error connecting: %s\n", __progname,
//               shout_get_error(shout));
//        return 1;
//    }

//    shout_close(shout);

//    return (0);
//}



OggFwd::OggFwd() {

}

int OggFwd::writeBytes(const char * buf, int length) {

}

ProcessOggFwd::ProcessOggFwd() {

}


int ProcessOggFwd::writeBytes(const char * buf, int length) {

}