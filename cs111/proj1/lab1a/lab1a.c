#include <sys/types.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <poll.h>

//NAME:Andrew Ding
//EMAIL:andrewxding@ucla.edu
//ID:504748356

int BUF_SIZE = 256;
struct termios oldtio,newtio;

int fd = 0; ///i think should be stdin????
int debug = 0;
pid_t shell_pid;
char cr_lf[2] = {0x0D, 0x0A};
int toshellfd[2];//[1] is write end, [0] is read end
int fromshellfd[2];



void reset_input_mode(void){
	if(tcsetattr(fd,TCSANOW, &oldtio)){ /* save current serial port settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd %d: %s\r\n", fd, strerror(errsv));
		exit(1);
	}
}

void handler(int signal_num)
{
	if(signal_num == SIGPIPE){
	  	fprintf(stderr, "Caguht sigpipe error.");
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
void dupfd(int fd){
	if(dup(fd)==-1){
		int errsv = errno;
		fprintf(stderr, "Error duping fd: %d %s \r\n", fd, strerror(errsv));
		exit(1);
	}
}
// //gets the keyboard input, buf conatins int count bytes read already, to is the output fd
int process_input(char buf[], int count, int to){
	int i;
    for(i = 0; i < count; i++){//read char by char from buffer
        if(buf[i] == 0x04){//EOF
        	if (debug)
        		fprintf(stderr, "Received control D from parent\r\n");
        	return 0;
        }
        else if(buf[i] == 0x0D || buf[i] == 0x0A){//cr or lf
        	writefd(1, cr_lf, 2);
        	writefd(to, &cr_lf[1], 1);
        }
        else if(buf[i] == 0x03){//ctrl c
        	kill(shell_pid, SIGINT);
		return 0;
        }
        else{
        	writefd(to, &buf[i], 1);
        	writefd(1, &buf[i], 1);
        }
	}
	return 1;
}
void process_output(char buf[], int count, int to){
	int i;
    for(i = 0; i < count; i++){//read char by char from shell output
        if(buf[i] == 0x0A)
        	writefd(to, cr_lf, 2);
        else{
        	writefd(to, &buf[i], 1);
        }
	}
}
//from = listening to shell, to is writing to shell
void poll_loop(int from, int to){
	struct pollfd pollfds[2];
	int count;
	char buf[BUF_SIZE+1];
	pollfds[0].fd = 0;//read from keyboard, parent input
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
					if (debug)
						printf("closing pipe to shell\n");
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
				if (debug)
					fprintf(stderr, "read from shell returns EOF\r\n");
				break;
			}
		}
		if(pollfds[1].revents & (POLLHUP+POLLERR)){//poll fd close for shell
			int stat;
			if(debug)
				fprintf(stderr, "poll or shell returns hup/err\r\n");
			closefd(from);
			waitpid(shell_pid, &stat, 0);
			fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", stat&0x7F, (stat&0xff00)>>8);
	  		break;
		}
	}
}

int main(int argc, char** argv)
{
	char buf[BUF_SIZE+1];
	int shell = 0;
	int opt = 0;
	atexit(reset_input_mode);
	static struct option long_opts[] =
	{
		{"shell", no_argument, 		0, 's'},
		{0, 0, 0, 0}
	};
	while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 's':
				shell = 1;
				break;
			default:
				fprintf(stderr, "correct usage: ./lab0 --shell\r\n");
				exit(1); //Exit with status of failure
		}
	}
    if(tcgetattr(fd,&oldtio)){ /* save current serial port settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd %d: %s\r\n", fd,  strerror(errsv));
		exit(1);//CHECK FOR EXIT CODE CORRECT
	}
	newtio = oldtio;
	newtio.c_iflag &= ~ISTRIP;
	newtio.c_iflag |= ISTRIP;	/* only lower 7 bits	*/
	newtio.c_oflag = 0;		/* no processing	*/
	newtio.c_lflag = 0;		/* no processing	*/
	if(tcsetattr(fd,TCSANOW, &newtio)){ /* change terminal settings */
    	int errsv = errno;
		fprintf(stderr, "Unable to get terminal modes from fd %d: %s\r\n", fd, strerror(errsv));
		exit(1);//CHECK FOR ECORRECT EXIT CODE
	}

	
	if(shell){
		if(pipe(toshellfd)){//unable tto create pipes
			int errsv = errno;
			fprintf(stderr, "Unable to create pipe from shell: %s\r\n", strerror(errsv));
			exit(1);
		}
		if(pipe(fromshellfd)){
			int errsv = errno;
			fprintf(stderr, "Unable to create pipe from shell: %s\r\n", strerror(errsv));
			exit(1);
		}
		shell_pid = fork();
		if(shell_pid < 0){
			int errsv = errno;
			fprintf(stderr, "Error forking shell process: %s\r\n", strerror(errsv));
		}
		else if(shell_pid == 0){//shell child process
			closefd(toshellfd[1]);//close writing to shell and listening from shell
			closefd(fromshellfd[0]);
			closefd(0);
			dupfd(toshellfd[0]);//stdin is terminal output
			closefd(toshellfd[0]);

			closefd(1);//stdout/err is now pipe for send to parentfd
			dupfd(fromshellfd[1]);
			closefd(2);
			dupfd(fromshellfd[1]);
			closefd(fromshellfd[1]);

			char *name[] = {
	        "/bin/bash",
	        NULL
	      	};
	      	//execute shell in child ps
	      	execvp(name[0], name);
		}
		else{//parent process
			signal(SIGPIPE, &handler);
			//close unneeded ends of pipe
			closefd(fromshellfd[1]);
			closefd(toshellfd[0]);

			if (debug)
				fprintf(stderr, "%d spawned %s, pi=%d\r\n", getpid(), "/bin/bash", shell_pid);
			poll_loop(fromshellfd[0], toshellfd[1]);
		}
	}

	else{
		int bytes, i;
		while ((bytes = read(fd, &buf, BUF_SIZE)) > 0) {
	        if (bytes == -1) {
	        	int errsv = errno;
	            fprintf(stderr, "Error reading from stdin: %s\r\n", strerror(errsv));
	            exit(1);
	        }
	        for(i = 0; i < bytes; i++){
	        	if(buf[i] == 0x04){
	        		exit(0);
	        	}
	        	else if(buf[i] == 0x0D || buf[i] == 0x0A){
	        		writefd(1, cr_lf, 2);
	        	}
	        	else if(buf[i] == 0x03){
	        		exit(0);
	        	}
	        	else{
	        		writefd(1, &buf[i], 1);
		        }
	        }
	    }
	}


	
    exit(0); 
}
