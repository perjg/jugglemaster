/*
 * AAJM - AA JuggleMaster
 * (C) Gary Briggs 2003
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include "./aajm-remote.h"

int do_command(int port, char *host, char *command, char *data) {
	char command_string[MAX_SOCKET_BUFFER];
	int sockfd;
	struct hostent *he;
	struct sockaddr_in their_addr;

	memset((void *)&their_addr, 0, sizeof(struct sockaddr_in));
	memset((void *)command_string, 0, sizeof(struct sockaddr_in));

	if((he = gethostbyname(host)) == NULL) {
		perror("gethostbyname() failed");
		return(-1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed");
		return(-1);
	}

	their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(port);
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&their_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("connect() failed");
		return(-1);
	}

	snprintf(command_string,MAX_SOCKET_BUFFER,"%s=%s;\n",command, data);
	if((int)write(sockfd, command_string, strlen(command_string)) >= (int)strlen(command_string)) {
		printf("Command successfully sent:\n%s\n",command_string);
	} else {
		printf("Command not successfully sent:\n%s\n",command_string);
	}
	close(sockfd);

	return(0);

}

int main(int argc, char **argv) {
        char options[] = "hqo:e:p:t:s:";
	char optch;
	int help_flag = 0;
	int quit_flag = 0;
	int option_index = 0;
	int port_num = DEFPORT;
	char command[MAX_SOCKET_BUFFER];
	char data[MAX_SOCKET_BUFFER];
	char hostname[MAXHOSTNAME];

	memset((void *)command, 0, MAX_SOCKET_BUFFER);
	memset((void *)data, 0, MAX_SOCKET_BUFFER);
	memset((void *)hostname, 0, MAX_SOCKET_BUFFER);
	strcpy(hostname,"localhost");

        static struct option long_options[] =
        {
                {"help", no_argument, &help_flag, 1},
                {"quit", no_argument, &quit_flag, 1},
                {"port", required_argument, 0, 'p'},
                {"speed", required_argument, 0, 'e'},
                {"siteswap", required_argument, 0, 's'},
                {"style", required_argument, 0, 't'},
                {"hostname", required_argument, 0, 'o'},
                {0,0,0,0}
        };

	while( (optch = getopt_long(argc,argv,options,
		long_options,&option_index)) != -1) {
		switch(optch) {
			case 'h':
				help_flag=1;
				break;
			case 'p':
				port_num=atoi(optarg);
				break;
			case 'e':
				strcpy(command,"Speed");
				if(!strcmp(optarg,"Up")) {
					strcpy(data,"Up");
				} else if(!strcmp(optarg,"Down")) {
					strcpy(data,"Down");
				} else if(!strcmp(optarg,"Reset")) {
					strcpy(data,"Reset");
				}
				break;
			case 's':
				strcpy(command,"Site");
				strncpy(data,optarg,MAX_SOCKET_BUFFER);
				break;
			case 't':
				strcpy(command,"Style");
				strncpy(data,optarg,MAX_SOCKET_BUFFER);
				break;
			case 'q':
				quit_flag=1;
				break;
			case 'o':
				strncpy(hostname,optarg,MAXHOSTNAME);
				break;
		}
		if(strlen(command) > 0) {
			if(!do_command(port_num,hostname,command,data)) {
			}
			memset((void *)command, 0, MAX_SOCKET_BUFFER);
			memset((void *)data, 0, MAX_SOCKET_BUFFER);
		}
			
	}

	if(help_flag) {
		printf("AAJM-Remote, Remote control for AAJM\n");
		printf("Usage: ./aajm-remote [OPTIONS]\n");

		printf("AAJM-Remote Options:\n");
		printf("  -s, --siteswap=XX     show siteswap XX\n");
		printf("  -t, --style=XX        use style XX\n");
		printf("  -p, --port=XX         use port XX for IPC (%i)\n",DEFPORT);
		printf("  -e, --speed=XX        change speed [XX = \"Up\", \"Down\", or \"Reset\"]\n");
		printf("  -o, --hostname=XX     connect to aajm running on host XX\n");
		printf("  -h, --help            get help [this screen]\n");
		printf("  -q, --quit            get aajm to gracefully quit\n");
	}

	if(quit_flag) {
		memset((void *)command, 0, MAX_SOCKET_BUFFER);
		memset((void *)data, 0, MAX_SOCKET_BUFFER);
		strcpy(command,"Quit");
		do_command(port_num,hostname,command,data);
	}
}
