//NAME:Andrew Ding
//EMAIL:andrewxding@ucla.edu
//ID:504748356

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

int BUF_SIZE=1000;
//Signal handler that prints error message if SIGSEGV
void signal_handler(int signal_num)
{
  if(signal_num == SIGSEGV)
  {
	int errsv = errno;
	fprintf(stderr, "Segmentation fault caught: %s\n", strerror(errsv));
    exit(4);
  }
}

int main(int argc, char** argv)
{
  int opt = 0;
  //valid arguments for program
  static struct option long_opts[] =
  {
	{"input",	 required_argument, 0, 'i'},
	{"output", 	 required_argument, 0, 'o'},
	{"segfault", no_argument, 		0, 's'},
	{"catch", 	 no_argument, 		0, 'c'},
	{0, 0, 0, 0}
  };
  char buf[BUF_SIZE + 1];
  char* n = NULL; //Used to cause a Segmentation fault
  int segfault = 0; //Used to register a segmentation fault for handler
  int bytes=0;
  int ifd, ofd;
  int counter = 0;
  while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1)
  {
        counter += 1;
	switch(opt)
	{
	  case 'i':
		ifd = open(optarg, O_RDONLY);
		//If successful open, clone input file descriptor as stdin
		if(ifd >= 0)
		{
		  close(0);
		  dup(ifd);
		  close(ifd);
		}
		else
		{//unsuccessful open
		  int errsv = errno;
		  fprintf(stderr, "Could not open input file %s: %s\n", optarg, strerror(errsv));
		  //fprintf("Opening file produced error.");
		  exit(2);
		}
		break;
	  case 'o':
		//add read and write priviledge to output file
		ofd = creat(optarg, 0666);
		//If successful creat, then clone output file descriptor as stdout
		if(ofd >= 0)
		{
		  close(1);
		  dup(ofd);
		  close(ofd);
		}
		else
		{//unable to create output file
                  int errsv = errno;
                  fprintf(stderr, "Couldn't create output file %s: %s\n", optarg, strerror(errsv));
		  //strerror("Creating file produced error.");
		  exit(3);
		}
		break;
	  case 's':
		segfault = 1;
		break;
	  case 'c'://catch sigsegv and produce error
		signal(SIGSEGV, signal_handler);
		break;
	  default://unknown argument caught
		fprintf(stderr, "\ncorrect usage: ./lab0 --input='filename' --output='filename' --segfault --catch\n");
		exit(1); //Exit with status of failure
	}
  }

  if(counter != argc-1){
  	fprintf(stderr, "./lab0: unrecognized argument\ncorrect usage: ./lab0 --input='filename' --output='filename' --segfault --catch\n");
        exit(1);
  }
  //if segfault option passed, derefernce null pointer to cause segfault
  if(segfault){
  	*n = 'A';
  }
  //read from new input fd to output fd, copies it
  while ((bytes = read(0, &buf, BUF_SIZE)) > 0) {
        if (bytes == -1) {//couldnt read from stdin
            int errsv = errno;
            fprintf(stderr, "Error reading from stdin: %s\n", strerror(errsv));
            exit(1);
        }
        if (write(1, &buf, bytes) == -1) {//error writing to stdout
            int errsv = errno;
            fprintf(stderr, "Error writing to stdout: %s\n", strerror(errsv));
            exit(2);
        }
    } 
  //Successful exit of 0
  exit(0);
}
