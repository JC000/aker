 /**
  * Copyright 2017 Comcast Cable Communications Management, LLC
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include <libparodus.h>
#include <msgpack.h>

#include "aker_log.h"
#include "schedule.h"
#include "wrp_interface.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void sig_handler(int sig);
static int main_loop(libpd_cfg_t *cfg, char *firewall_cli, char *data_file,
                     char *md5_file );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char **argv)
{
    static const struct option options[] = {
        { "parodus-url",  required_argument, 0, 'p' },
        { "client-url",   required_argument, 0, 'c' },
        { "firewall-cli", required_argument, 0, 'w' },
        { "data-file",    required_argument, 0, 'd' },
        { "md5-file",     required_argument, 0, 'm' },
        { 0, 0, 0, 0 }
    };

    libpd_cfg_t cfg = { .service_name = "parental-control",
                        .receive = true,
                        .keepalive_timeout_secs = 64,
                        .parodus_url = NULL,
                        .client_url = NULL
                      };

    char *firewall_cli = NULL;
    char *data_file = NULL;
    char *md5_file = NULL;
    int item = 0;
    int opt_index = 0;
    int rv = -1;

    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGBUS, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGALRM, sig_handler);

    while( -1 != (item = getopt_long(argc, argv, "p:c:w:d:m", options, &opt_index)) ) {
        switch( item ) {
            case 'p':
                cfg.parodus_url = strdup(optarg);
                break;
            case 'c':
                cfg.client_url = strdup(optarg);
                break;
            case 'w':
                firewall_cli = strdup(optarg);
                break;
            case 'd':
                data_file = strdup(optarg);
                break;
            case 'm':
                md5_file = strdup(optarg);
                break;
            default:
                break;
        }
    }

    if( (NULL != cfg.parodus_url) &&
        (NULL != cfg.client_url) &&
        (NULL != firewall_cli) &&
        (NULL != data_file) &&
        (NULL != md5_file) )
    {
        main_loop(&cfg, firewall_cli, data_file, md5_file);
        rv = 0;
    }

    if( NULL != md5_file )          free( md5_file );
    if( NULL != data_file )         free( data_file );
    if( NULL != firewall_cli )      free( firewall_cli );
    if( NULL != cfg.parodus_url )   free( (char*) cfg.parodus_url );
    if( NULL != cfg.client_url )    free( (char*) cfg.client_url );

    return rv;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static void sig_handler(int sig)
{
    if( sig == SIGINT ) {
        signal(SIGINT, sig_handler); /* reset it to this function */
        debug_info("SIGINT received!\n");
        exit(0);
    } else if( sig == SIGUSR1 ) {
        signal(SIGUSR1, sig_handler); /* reset it to this function */
        debug_info("SIGUSR1 received!\n");
    } else if( sig == SIGUSR2 ) {
        debug_info("SIGUSR2 received!\n");
    } else if( sig == SIGCHLD ) {
        signal(SIGCHLD, sig_handler); /* reset it to this function */
        debug_info("SIGHLD received!\n");
    } else if( sig == SIGPIPE ) {
        signal(SIGPIPE, sig_handler); /* reset it to this function */
        debug_info("SIGPIPE received!\n");
    } else if( sig == SIGALRM ) {
        signal(SIGALRM, sig_handler); /* reset it to this function */
        debug_info("SIGALRM received!\n");
    } else {
        debug_info("Signal %d received!\n", sig);
        exit(0);
    }
}


static int main_loop(libpd_cfg_t *cfg, char *firewall_cli, char *data_file,
                     char *md5_file )
{
    int rv;
    wrp_msg_t *wrp_msg;
    libpd_instance_t hpd_instance;


    rv = libparodus_init( &hpd_instance, cfg );
    if( 0 != rv ) {
        debug_error("Init for parodus (url %s) failed: '%s'\n", cfg->parodus_url, libparodus_strerror(rv) );
        return -1;
    }
    debug_info("Init for parodus Success..!!\n");

    debug_print("starting the main loop...\n");
    while( 1 ) {
        rv = libparodus_receive(hpd_instance, &wrp_msg, 2000);

        if( 0 == rv ) {
            uint8_t *bytes = NULL;
            debug_info("Got something from parodus.\n");
            wrp_to_object(wrp_msg, &bytes);
            // TODO process the request, then write to the file

        } else if( 1 == rv || 2 == rv ) {
            debug_info("Timed out or message closed.\n");
            continue;
        } else {
            debug_info("Libparodus failed to receive message: '%s'\n",libparodus_strerror(rv));
        }

        if( NULL != wrp_msg ) {
            free(wrp_msg);
        }
    }

    libparodus_shutdown(&hpd_instance);
    debug_print("End of parodus_upstream\n");
    return 0;
}
