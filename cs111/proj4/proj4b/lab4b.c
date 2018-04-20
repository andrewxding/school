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

char timeBuffer[9];
char scale = 'F';
mraa_aio_context tempSensor;
mraa_gpio_context button;
int logging;
FILE* logFile;
int interval = 1;
int first = 1;

// //reads the new period value and checks if it is valid and sets the new period
int changePeriod(char *buf, int length){
	long period;
  char *str = buf + length;//str points to beginning of period value
  char *end;

  //try to convert it into a long
  period = strtol(str, &end, 10);
  //check for valid value, exit if invalid
  if(period == LONG_MAX || period == LONG_MIN || end == str) {
    fprintf(stderr, "Error: invalid period");
    exit(1);
  }
  else if(period <= 0) {
    fprintf(stderr, "Error: not period value");
    exit(1);
  }
  else {
    if(logFile) {
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
		int x = read(STDIN_FILENO, buf, 1024);
		if(x < 0){
      fprintf(stderr, "Error: reading from buffer fail\n");
      exit(1);
    }
		if (x > 0){
			int i; 
      int start = 0; //starting index of the current command we are processing
			for(i = 0; i < x; i++){
				if(buf[i] == '\n'){ 
					buf[i] = 0 ; //set null byte to where \n is
					if(strcmp(buf + start, "OFF") == 0){
            char curtime[10];
          	getTime(curtime);
          	if(logFile){
          		fprintf(logFile, "%s\n", "OFF");
          		fprintf(logFile, "%s %s\n", curtime, "SHUTDOWN");
          	}
            fprintf(stdout, "%s %s\n", curtime, "SHUTDOWN");
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
            interval = changePeriod(buf + start, strlen("PERIOD="));
					start = i + 1; //begin command processing (if we have more) after the \n
				}
			}
		}
	}
	return NULL;
}

//thread that checks for button input
void * btnFunc(){
  //fprintf(stdout, "btn");
	while(1) {
		if(mraa_gpio_read(button)) {
			char curtime[10];
			getTime(curtime);
			if(logFile)
        fprintf(logFile, "%s %s\n", curtime, "SHUTDOWN");
      fprintf(stdout, "%s %s\n", curtime, "SHUTDOWN");
			exit(0);
		}
	}
	return NULL;
}

//thread that reads temperature from sensors
void * tempFunc(){
  //fprintf(stdout, "temp");

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
    fprintf(stdout, "%s %.1f\n", curtime, scale=='F' ? fahrenheit : celsius);
    if(first){
      first = 0;
      return NULL;
    }
    //take measurements per period
    sleep(interval);
  }
  return NULL;
}

int main(int argc, char** argv){
  //init the sensor pinout
  tempSensor = mraa_aio_init(1);
  button = mraa_gpio_init(62);

  int opt;
  static struct option long_opts[] = {
  		{"period", required_argument, 0, 'p'},
  		{"scale", required_argument, 0, 's'},
  		{"log", required_argument, 0, 'l'},
  		{0, 0, 0, 0}
  };

  while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1){
		switch(opt){
			case 'p':
        interval = changePeriod(optarg, 0);
        break;
      case 'l':
        logFile = fopen(optarg, "w");
        if (!logFile){
          fprintf(stderr, "Error: opening log file\n");
          exit(1);
        }
				logging = 1;
        break;
      case 's':
        if (optarg[0] == 'C')
          scale = 'C';
				else if(optarg[0] == 'F')
          scale = 'F';
				else {
					fprintf(stderr, "Error: scale must be C or F \n");
					exit(1);
				}
        break;
      default:
        fprintf(stderr, "Usage: ./lab4b [--period=p] [--scale={C,F}] [--log=filename]");
        exit(1);
    }
  }
  //first reading
  tempFunc();
  //take input non blocking
  pthread_t *threads = malloc(3 * sizeof(pthread_t));
  pthread_create(threads + 0, NULL, tempFunc, NULL);
  pthread_create(threads + 1, NULL, btnFunc, NULL);
  pthread_create(threads + 2, NULL, inputFunc, NULL);
  int i;
  for(i = 0 ; i < 3; i++){
    	pthread_join(threads[i], NULL);
  }

  if(logging){
    	if (fclose(logFile)) {
        fprintf(stderr, "Error: closing log file\n");
        exit(1);
      }
  }

  free(threads);
  mraa_aio_close(tempSensor);
  mraa_gpio_close(button);
  exit(0);
}
