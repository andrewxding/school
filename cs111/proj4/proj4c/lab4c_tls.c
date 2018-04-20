/*
NAME:Andrew Ding
EMAIL:andrewxding@ucla.edu
ID:504748356
*/
#include <stdio.h>
#include <mraa.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

char timeBuffer[9];
char scale = 'F';
char* id = "";
char* host = "";
mraa_aio_context tempSensor;
int logging;
FILE* logFile;
char* logName;
int interval = 1;
int port = 0;
int sockfd = 0;

SSL *ssl;

void printExit(char* str, int code){
  fprintf(stderr, "ERROR: %s",str);
  exit(code);
}
 //reads the new period value and checks if it is valid and sets the new period
int getNum(char *buf, int length, int isperiod){
	long period;
  char *str = buf + length;//str points to beginning of period value
  char *end;

  //try to convert it into a long
  period = strtol(str, &end, 10);
  //check for valid value, exit if invalid
  if(period == LONG_MAX || period == LONG_MIN || end == str)
    printExit("period not a number\n", 2);

  else if(period <= 0) 
    printExit("not valid period value\n", 2);

  else {
    if(isperiod && logFile) {
      fprintf(logFile, "%s%lu\n", "PERIOD=", period);
    }
  }
  return period;
}

//gets the current time and puts it formatted into the buffer
void getTime(char * formattedTime){
  time_t clock;
  struct tm* timeInfo;
  time(&clock);
  timeInfo = localtime(&clock);
  // formatted time is in buffer
  strftime(formattedTime, 9, "%H:%M:%S", timeInfo);
}


//constantly receives input from stdin
void * inputFunc(){
	char buf[1024];
	while(1) {
		int x = SSL_read(ssl, buf, 1024);
		if(x < 0)
      printExit("reading from buffer fail\n", 2);

		if (x > 0){
			int i; 
      int start = 0; //starting index of the current command we are processing
			for(i = 0; i < x; i++){
				if(buf[i] == '\n'){ 
					buf[i] = 0 ; //set null byte to where \n is
					if(strcmp(buf + start, "OFF") == 0){
            char curtime[10];
          	getTime(curtime);
            char shutdownbuf[64];
            sprintf(shutdownbuf, "%s %s\n", curtime, "SHUTDOWN");
            if (SSL_write(ssl, shutdownbuf, strlen(shutdownbuf))<= 0)
              printExit("writing to ssl\n", 2);
          	if (logFile){
          		fprintf(logFile, "%s\n", "OFF");
          		fprintf(logFile, "%s %s\n", curtime, "SHUTDOWN");
          	}
            exit(0);
          }
					else if(strcmp(buf + start, "STOP") == 0){
            if(logFile){
              fprintf(logFile, "STOP\n");
            }
            logging = 0;
          }
					else if(strcmp(buf + start, "START") == 0){
            if(logFile){
              fprintf(logFile, "START\n");
            }
            logging = 1;
          }
					else if (strcmp(buf + start, "SCALE=C") == 0){
            scale = 'C';
            if (logFile)
              fprintf(logFile, "SCALE=C\n");
          }
					else if(strcmp(buf + start, "SCALE=F") == 0){
            scale = 'F';
            if (logFile)
              fprintf(logFile, "SCALE=F\n");
          }
					else if(strncmp(buf + start, "PERIOD=", strlen("PERIOD=")) == 0)
            interval = getNum(buf + start, strlen("PERIOD="), 1);
					start = i + 1; //begin command processing (if we have more) after the \n
				}
			}
		}
	}
	return NULL;
}

// thread that reads temperature from sensors
void * tempFunc(){

  const int B = 4275;
  while (1){
    int a = mraa_aio_read(tempSensor);

    double R = 1023.0/((double)a) - 1.0;
		R = 100000.0*R;
		double celsius  = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
		double fahrenheit = celsius * 9/5 + 32;

    char curtime[10];
    getTime(curtime);

    if (logging) {
        fprintf(logFile, "%s %.1f\n", curtime, scale=='F' ? fahrenheit : celsius);
    }
      char buf[32];
      sprintf(buf, "%s %.1f\n", curtime, scale=='F' ? fahrenheit : celsius);
      SSL_write(ssl, buf, strlen(buf));
    sleep(interval);
  }
  return NULL;
}

int main(int argc, char** argv){
  //init the sensor pinout
  tempSensor = mraa_aio_init(1);
  if (argc < 2)
    printExit("Must specify port number. Usage: ./lab4c [--period=p] [--scale={C,F}] [--log=filename] [--id=id] [--host=host] portno \n", 1);
  
  int opt;
  static struct option long_opts[] = {
  		{"period", required_argument, 0, 'p'},
  		{"scale", required_argument, 0, 's'},
  		{"log", required_argument, 0, 'l'},
      {"id", required_argument, 0, 'i'},
      {"host", required_argument, 0, 'h'},
  		{0, 0, 0, 0}
  };

  while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 'p':
        interval = getNum(optarg, 0, 1);
        break;
      case 'l':  //NEED TO CHECK IF LOGGING IS MANDATORY
        logName = optarg;
        logging = 1;
        break;
      case 's':
        if (optarg[0] == 'C')
          scale = 'C';
				else if(optarg[0] == 'F')
          scale = 'F';
				else 
					printExit("Error: scale must be C or F \n", 1);

        break;
      case 'i':
        id = optarg;
        break;
      case 'h':
        host = optarg;
        break;
      default:
        printExit("Usage: ./lab4c [--period=p] [--scale={C,F}] [--log=filename] [--id=id] [--host=host] portno\n", 1);
    }
  }
  if (!strcmp(id, "")) 
    printExit("ID is mandatory. Usage: ./lab4c [--period=p] [--scale={C,F}] [--log=filename] [--id=id] [--host=host] portno\n", 1);
  if (!strcmp(host, ""))
    printExit("host is mandatory. Usage: ./lab4c [--period=p] [--scale={C,F}] [--log=filename] [--id=id] [--host=host] portno\n", 1);
  if (!logging)
    printExit("log is mandatory. Usage: ./lab4c [--period=p] [--scale={C,F}] [--log=filename] [--id=id] [--host=host] portno\n", 1);

  
  logFile = fopen(logName, "w");
  if (!logFile)
    printExit("Error: opening log file\n", 1);


  port = getNum(argv[argc - 1], 0, 0);  //error checking is done by getnum
  

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    printExit("making socket\n", 2);
  //server
  struct hostent *server = gethostbyname(host);
  if(server == NULL)
    printExit("no such host\n", 1);

  //connection
  struct sockaddr_in addr;
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  memcpy( (char *) &addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
  if(connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    printExit("connecting to server\n", 2);

  // load the error strings for good error reporting, algorithm for encrypting
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  // create a new SSL_CTX obj for TLS/SSL enabled connections and a new SSL structto hold the TLS/SSL connection data
  SSL_CTX* ctx = SSL_CTX_new(TLSv1_client_method());
  if (ctx == NULL)
    printExit("creating ssl_ctx obj\n", 2);
  ssl = SSL_new(ctx);
  if (ssl == NULL)
    printExit("creating a new ssl structure\n", 2);
  //  set encrypted input/output facility for the TLS/SSL as socket and connect to ssl
  if (SSL_set_fd(ssl, sockfd) == 0) 
    printExit("setting file descriptor as SSL\n", 2);
  if (SSL_connect(ssl) != 1) 
    printExit("TLS/SSL handshake\n", 2);

  //  send ID and log it
  char idbuf[50];
  sprintf(idbuf, "ID=%s\n", id);
  if (SSL_write(ssl, idbuf, strlen(idbuf)) <= 0)
    printExit("writing to ssl\n", 2);
  if(logFile) 
    fprintf(logFile, "ID=%s\n", id);
  
  //take input non blocking
  pthread_t *threads = malloc(2 * sizeof(pthread_t));
  pthread_create(threads + 0, NULL, tempFunc, NULL);
  pthread_create(threads + 1, NULL, inputFunc, NULL);
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  if(logFile){
    	if (fclose(logFile)) 
        printExit("closing log file\n", 2);
  }

  free(threads);
  if(close(sockfd) < 0) 
    printExit("close() on sockfd\n", 2);
  
  SSL_shutdown(ssl);
  mraa_aio_close(tempSensor);
  exit(0);
}
