#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <mcrypt.h>
struct termios oldtio, newtio;
pid_t shellpid = -1;
int sockfd = -1;
int fromshell[2];
int toshell[2];
int encrypt = 0;
int encryptfd = -1;
char end[2] = {0x0D, 0x0A};
MCRYPT td, td1;
char* IV, *IV1, *key;
int keysize = 16;

void dupfd(int fd1, int fd2){
	if(dup2(fd1, fd2)==-1){
		int errsv = errno;
		fprintf(stderr, "Error duping fd: %s\r\n", strerror(errsv));
		exit(1);
	}
}
void writefd(int fd, char buf[], int bytes){
	if (write(fd, buf, bytes) == -1) {//write char to shell
	    int errsv = errno;
		fprintf(stderr, "Error writing to stdout: %s\r\n", strerror(errsv));
		exit(1);
	}
}
void closefd(int fd){
	if(close(fd)==-1){
		int errsv = errno;
		fprintf(stderr, "Error closing fd: %d %s \r\n", fd, strerror(errsv));
		exit(1);
	}
}

void handler(int signo){
	if (signo == SIGPIPE){
		//fprintf(stderr, "called sig handler \r\n");
		closefd(sockfd);
		kill(shellpid, SIGINT);
		closefd(fromshell[0]);
		closefd(toshell[1]);
		int stat = 0;
		waitpid(shellpid, &stat, 0);
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", stat&0xFF, (stat&0xff00)>>8);
		exit(0);
	}
}
// void encrypt() {
// 	if (td  = module opening)

// 	if (i = moduel inti)

// 		moduel generdic
// }
int process_input(char buf[], int count, int to){
	int i;
    for(i = 0; i < count; i++){//read char by char from buffer
    	if(encrypt == 1){
		 	mdecrypt_generic(td, &buf[i], 1);
		}
    	// writefd(2, &buf[i], 1);
    	
        if(buf[i] == 0x04){//EOF
        	return 0;
        }
        else if(buf[i] == 0x0D || buf[i] == 0x0A){//cr or lf
        	writefd(to, &end[1], 1);
        }
        else if(buf[i] == 0x03){//ctrl c
        	kill(shellpid, SIGINT);
			return 0;
        }
        else{ 	
        	writefd(to, &buf[i], 1);
        }
	}
	return 1;
}
void process_output(char buf[], int count, int to){
	int i;
    for(i = 0; i < count; i++){//read char by char from shell output
        if(buf[i] == 0x0A){
        	char tmp[2] = {0x0D, 0x0A};
        	if(encrypt == 1){
		 		mcrypt_generic(td1, &tmp[0], 1);
		 		mcrypt_generic(td1, &tmp[1], 1);
		 		writefd(to, &tmp[0], 1);
		 		writefd(to, &tmp[1], 1);
			}
        	else{
        		writefd(to, end, 2);
        	}
        }
        else{
        	if(encrypt == 1){
		 		mcrypt_generic(td1, &buf[i], 1);
			}
        	writefd(to, &buf[i], 1);
        }
	}
}
void poll_loop(int from, int to){
	struct pollfd pollfds[2];
	int count;
	char buf[256];
	pollfds[0].fd = 0;//read from socket
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;
	pollfds[1].fd = from;//input from shell output
	pollfds[1].events = POLLIN | POLLHUP | POLLERR;
	while (1){
		poll(pollfds, 2, -1);
		if (pollfds[0].revents & POLLIN){//keyoard input
			count = read(0, buf, sizeof(buf));//read from keyboard
			if(count == -1){
				int errsv = errno;
				fprintf(stderr, "Reading error %s\r\n", strerror(errsv));
				exit(1);
			}
			if (count > 0){
				if (process_input(buf, count, to) == 0){//WHEN SHOULD IT RETURN 0
					closefd(to);
				}
			}
		}
		if (pollfds[1].revents & POLLIN){//read from shell
			count = read(from, buf, sizeof(buf));
			if(count == -1){
				int errsv = errno;
				fprintf(stderr, "Reading error %s \r\n", strerror(errsv));
				exit(1);
			}
			if (count > 0){
				process_output(buf, count, 1);
			}
			else{
				fprintf(stderr, "read from shell returns EOF\r\n");
				break;
			}
		}
		if(pollfds[1].revents & (POLLHUP+POLLERR)){//poll fd close for shell
			int stat;
			//fprintf(stderr, "poll or shell returns hup/err\r\n");
			closefd(from);
			waitpid(shellpid, &stat, 0);
			fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", stat&0xFF, (stat&0xff00)>>8);
	  		exit(0);///--------check if right
		}
	}
}
void prepare_encryption(){
	int i, bytes;
	key = calloc(1, keysize);

	int keyfd = encryptfd;//open("my.key", O_RDONLY);//----------------change this to encrypt fd
	if(keyfd < 0){
		int errsv = errno;
		fprintf(stderr, "Open file error %s\n", strerror(errsv));
		exit(1);
	}
	bytes = read(keyfd, key, keysize);
	closefd(keyfd);
	if(bytes < 0){
		int errsv = errno;
		fprintf(stderr, "Read file error %s\n", strerror(errsv));
		exit(1);	
	}

	td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	if(td == MCRYPT_FAILED){
		int errsv = errno;
		fprintf(stderr, "Error while preparing encryption %s\n", strerror(errsv));
		exit(1);
	}
	IV = malloc(mcrypt_enc_get_iv_size(td));
	for(i = 0; i < mcrypt_enc_get_iv_size(td); i++){
		IV[i]=0x0A;
	}
	i = mcrypt_generic_init(td, key, keysize, IV);
	if(i < 0){
		mcrypt_perror(i);
		int errsv = errno;
		fprintf(stderr, "Error while prearing encryption %s\n", strerror(errsv));
		//free(key);
		exit(1);
	}

	td1 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	if(td1 == MCRYPT_FAILED){
		int errsv = errno;
		fprintf(stderr, "Error while preparing encryption %s\n", strerror(errsv));
		exit(1);
	}
	IV1 = malloc(mcrypt_enc_get_iv_size(td1));
	for(i = 0; i < mcrypt_enc_get_iv_size(td1); i++){
		IV1[i]=0x0A;
	}
	i = mcrypt_generic_init(td1, key, keysize, IV1);
	if(i < 0){
		mcrypt_perror(i);
		int errsv = errno;
		fprintf(stderr, "Error while prearing encryption %s\n", strerror(errsv));
		//free(key);
		exit(1);
	}
}

void cleanup(){
	if(encrypt==1){
		mcrypt_generic_deinit(td);
		mcrypt_module_close(td);
		mcrypt_generic_deinit(td1);
		mcrypt_module_close(td1);
	}
}
int main(int argc, char ** argv){
	atexit(cleanup);
	int newsockfd = -1, portno = -1;
	socklen_t clilen;
	struct sockaddr_in server, client;
	//char buffer[128];
	//int bytes = 0;
	int opt = -1;
	static struct option long_opts[] =
	{
		{"port", required_argument, 0, 'p'},
		{"encrypt", required_argument, 0, 'e'},
		{0, 0, 0, 0}
	};
	while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 'p':
				portno = atoi(optarg);
				break;
			case 'e':
				encrypt = 1;
				encryptfd = open(optarg, O_RDONLY);

				if (encryptfd < 0){
					int errsv = errno;
					fprintf(stderr, "Error accessing encryption file: %s\r\n", strerror(errsv));
					exit(1);
				}
				prepare_encryption();
				break;
			default:
				fprintf(stderr, "correct usage: ./lab0 --port=portno --encrypt\r\n");
				exit(1); //Exit with status of failure
		}
	}
	if(portno < 0){
		int errsv = errno;
		fprintf(stderr, "Port is a mandatory argument: %s\r\n", strerror(errsv));
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		fprintf(stderr, "Error opening socket");
		exit(1);
	}

	memset((void *) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);

	//bind port and socket
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))){
		int errsv = errno;
		fprintf(stderr, "Server error binding %s \n", strerror(errsv));
		exit(1);
	}
	listen(sockfd, 5);
	clilen = sizeof(client);
	//accept actual connection
	newsockfd = accept(sockfd, (struct sockaddr *)&client, &clilen);
	if(newsockfd < 0){
		int errsv = errno;
		fprintf(stderr, "Error on accept %s \n", strerror(errsv));
		exit(1);
	}
	dupfd(newsockfd, 0);
	dupfd(newsockfd, 1);
	//dupfd(newsockfd, 2);
	closefd(newsockfd);
	if(pipe(fromshell)==-1){
		int errsv = errno;
		fprintf(stderr, "Error piping from shell %s \n", strerror(errsv));
		exit(1);
	}
	if(pipe(toshell)==-1){
		int errsv = errno;
		fprintf(stderr, "Error piping to shell %s \n", strerror(errsv));
		exit(1);
	}
	shellpid = fork();
	if(shellpid > 0){
		signal(SIGPIPE, handler);
		closefd(fromshell[1]);
		closefd(toshell[0]);
		poll_loop(fromshell[0], toshell[1]);
	}
	else if(shellpid == 0){
		closefd(fromshell[0]);
		closefd(toshell[1]);

		dupfd(fromshell[1], 1);
		dupfd(fromshell[1], 2);
		closefd(fromshell[1]);

		dupfd(toshell[0], 0);
		closefd(toshell[0]);
		
		char *name[] = {
	        "/bin/bash",
	        NULL
	    };
	    if (execvp(name[0], name) == -1){
	    	int errsv = errno;
			fprintf(stderr, "Error execing new shell %s \n", strerror(errsv));
			exit(1);
	    }		
	}
	else{
		int errsv = errno;
		fprintf(stderr, "Error forking %s \n", strerror(errsv));
		exit(1);
	}
}