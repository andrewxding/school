#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <mcrypt.h>
struct termios oldtio, newtio;
int logfd = -1;
int logfile = 0, encrypt = 0;
int encryptfd = -1;
char end[1] = {0x0A};
MCRYPT td, td1;
char* IV, *IV1, *key;
int keysize = 16;

void writefd(int fd, char buf[], int bytes){
	if (write(fd, buf, bytes) == -1) {//write char to shell
	    int errsv = errno;
		fprintf(stderr, "Error writing to stdout: %s\n", strerror(errsv));
		exit(1);
	}
}
void closefd(int fd){
	if(close(fd)==-1){
		int errsv = errno;
		fprintf(stderr, "Client Error closing fd: %d %s \n", fd, strerror(errsv));
		exit(1);
	}
}
void reset_input_mode(void){
	if(tcsetattr(0,TCSANOW, &oldtio)){ /* save current serial port settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd %s\r\n", strerror(errsv));
		exit(1);
	}
	if(encrypt==1){
		mcrypt_generic_deinit(td);
		mcrypt_module_close(td);
	}
}

void set_input_mode(void){

	if(!isatty (0)){
		fprintf(stderr, "Not a terminal");
		exit(1);
	}

	if(tcgetattr(0, &oldtio)){ /* save current serial port settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd %s\n", strerror(errsv));
		exit(1);//CHECK FOR EXIT CODE CORRECT
	}
	newtio = oldtio;
	newtio.c_iflag &= ~ISTRIP;
	newtio.c_iflag |= ISTRIP;	/* only lower 7 bits	*/
	newtio.c_oflag = 0;		/* no processing	*/
	newtio.c_lflag = 0;		/* no processing	*/
	if(tcsetattr(0,TCSANOW, &newtio)){ /* change terminal settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd  %s\r\n", strerror(errsv));
		exit(1);//CHECK FOR ECORRECT EXIT CODE
	}
}

void threadfunc(void *ptr){
	int *fd = (int *) ptr;
	char buf[256];
	char intbuf[4] = {0};
	int bytes  = 0;
	while((bytes = read(*fd, buf, 255)) > 0){
		if(logfile == 1){
			writefd(logfd, "RECEIVED ", 9);
			memset(intbuf, 0, sizeof(intbuf));
			snprintf(intbuf, sizeof bytes, "%d", bytes);
			writefd(logfd, intbuf, strlen(intbuf));
			writefd(logfd, " bytes: ", 8);
			writefd(logfd, buf, bytes);
			writefd(logfd, &end[0], 1);
		}
		int i = 0;
		for(i = 0; i < bytes; i ++){
			if(encrypt == 1){
				mdecrypt_generic(td1, &buf[i], 1);
			}

			if(buf[i] == 0x04){//dont know if i need this
				closefd(*fd);
				exit(1);
			}
			else if(buf[i] == 0x0A){
        		writefd(1, end, 2);
			}
			else{
				writefd(1, &buf[i], 1);
			}
		}
	}

	//eof
	closefd(*fd);
	exit(1);
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
int main(int argc, char ** argv){
	int sockfd = -1, portno = -1;
	struct sockaddr_in server;
	char buf[256];
	char end[2] = {0x0D, 0x0A};
	char* logname;
	int bytes = 0;
	int opt = -1;
	atexit(reset_input_mode);
	static struct option long_opts[] =
	{
		{"port", required_argument, 0, 'p'},
		{"log", required_argument, 0, 'l'},
		{"encrypt", required_argument, 0, 'e'},
		{0, 0, 0, 0}
	};
	while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 'p':
				portno = atoi(optarg);
				break;
			case 'l':
				logfile = 1;
				logname = optarg;
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
				reset_input_mode();
				exit(1); //Exit with status of failure
		}
	}
	if(portno < 0){
		int errsv = errno;
		fprintf(stderr, "Port is a mandatory argument: %s\r\n", strerror(errsv));
	}
	if (logfile == 1){
		logfd = creat(logname, S_IRUSR | S_IWUSR);
		if (logfd < 0){
			logfd = open(logname, O_WRONLY | O_TRUNC);
			if (logfd < 0){
				int errsv = errno;
				fprintf(stderr, "Creat log file error: %s, exiting 1\r\n", strerror(errsv));
				exit(1);
			}
		}
	}
	if (portno < 0){
		fprintf(stderr, "Please enter a valid portno\r\n");
		exit(1);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		fprintf(stderr, "Error opening socket");
		exit(1);
	}
	set_input_mode();
	memset((void *) &server, 0, sizeof(server));//----------------------atexit free server?
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *)&server, sizeof(server))){
		fprintf(stderr, "Error connecting to socket\r\n");
		exit(1);
	}
	pthread_t threadid;
	pthread_create(&threadid, NULL, (void *) &threadfunc, (void*) &sockfd);

	while((bytes = read(0, buf, 255)) > 0){
		int i  =0;
		for (i = 0; i < bytes; i ++){
			if(buf[i] == 0x0D || buf[i] == 0x0A){
				writefd(1, end, 2);
				buf[i] = end[1];
				if(encrypt == 1){
					mcrypt_generic(td, &buf[i], 1);//error check it later
				}
				writefd(sockfd, &buf[i], 1);
			}
			else if(buf[i] == 0x04 || buf[i] == 0x03){//delete later
				if(encrypt == 1){
					mcrypt_generic(td, &buf[i], 1);
				}
				writefd(sockfd, &buf[i], 1);
			}
			else{
				writefd(1, &buf[i], 1);
				if(encrypt == 1){
					mcrypt_generic(td, &buf[i], 1);
				}
				writefd(sockfd, &buf[i], 1);
			}
			if(logfile == 1){
				writefd(logfd, "SENT 1 bytes: ", 14);
				writefd(logfd, &buf[i], 1);
				writefd(logfd, &end[1], 1);
			}
		}
	}
	close(sockfd);
	exit(0);
}