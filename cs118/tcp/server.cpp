// #include <stdio.h>
// #include <sys/socket.h>
// #include <stdlib.h>
// #include <string.h>
// #include <netinet/in.h>
// #include "TCP_connection.h"
// using namespace std;

// int main(int argc, char** argv)
// {
//     // test();
//     int sockfd, portno;
//     struct sockaddr_in serv_addr;
//     int serv_addr_len = sizeof(serv_addr);
//     if (argc < 2)
//     {
//         fprintf(stderr, "ERROR, no port provided\n");
//         exit(1);
//     }

//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0)
//         perror("ERROR opening socket");
//     memset(&serv_addr, '\0', sizeof(serv_addr));
//     portno = atoi(argv[1]);
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     serv_addr.sin_port = htons(portno);

//     if (::bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//         perror("ERROR on binding");
//     listen(sockfd, 5);

//     while (true){
//     	int newsockfd = accept(sockfd, (struct sockaddr*)&serv_addr, (socklen_t*)&serv_addr_len);
//         // fprintf(stdout, "Initiating RDTP1 connection as server.");
//         TCP_connection server = TCP_connection(true, newsockfd, serv_addr);
//         // break;
//     }
// }

#include <stdio.h>
#include <sys/types.h> // Data types used in socket.h and netinet/in.h
#include <sys/socket.h> // Struct definitions for sockets
#include <netinet/in.h> // Constants and structs for IP addresses
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h> // waitpid()
#include <signal.h> // Signal name macros and kill()
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <iostream> // cout (for debugging)

#include <sstream> // stringstream
#include <algorithm> // transform


#include "TCP_connection.h"
#include "TCP_server.h"

using namespace std;

void sigchld_handler(int s)
{
    (void)s;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Print error message and then exit
void error(string msg)
{
    perror(msg.c_str());
    exit(1);
}

// Main function for processing and responding to TCP requests
// Each child process calls this function once to process the request it's handling
void respondToClient(int sockfd, struct sockaddr_in hostaddr);
void formFilePacket();
void sendPacket();
int main(int argc, char** argv)
{
    int sockfd, newsockfd, portno, pid;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    // struct sigaction sa;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);



    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");


    // listen(sockfd, 5);

    // cli_len = sizeof(cli_addr);


    // Kill zombie processes
    // sa.sa_handler = sigchld_handler;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = SA_RESTART;
    // if (sigaction(SIGCHLD, &sa, nullptr) == -1)
    //     error("sigaction");

    while (true){
        // newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);

        // if (newsockfd < 0)
        //     error("ERROR on accept");

        // Fork to handle incoming requests
        // pid = fork();
        // if (pid < 0)
        //     error("ERROR on fork");

        // // Child process: close old socket, handle request, close new socket
        // if (pid == 0)
        // {
        //     close(sockfd);
            respondToClient(sockfd, serv_addr);
            close(sockfd);
            exit(0);
        // }
        // else // Parent closes new socket fd
        //     close(newsockfd);
    }


    return 0;
}
void respondToClient(int sockfd, struct sockaddr_in hostaddr){
	cout << "server accepted connection\n";
	TCP_connection* session = new TCP_connection(true, sockfd, hostaddr);
	session->ReceiveHandshake();
	cout << "\nHANDSHAKE DONE\n";
	// //read filename
	while(!session->receiveData());
	TCP_server server = TCP_server(session);
	server.setFile((char*)session->getRecBuffer());
	server.chunkToPacket();
	server.sendPacket();

    server.ackPackets();
	// while(1);
	// struct stat attr;
	// char* path = ((char*)session.getRecBuffer());
	// cout << path;

	// int res = stat(path, &attr);
 // //    ifstream ifs(path);
 // //    //cout << request_uri << endl;
 // //    if (ifs.fail() || (res == 0 && S_ISDIR(attr.st_mode))) // If the file couldn't be opened or is a directory, use the failure header (404 Not Found)
 // //    {
 // //        perror("file not found");
 // //    }
 //    ifstream file(path, ifstream::binary);

  /* basic sanity check */
  	// if (not file)
   //    cerr << "file: " << path << " failed to open" << endl;
    // TCP_server server = TCP_server(session);
	// server.setFile(file, attr.st_size);
	//send filename

    
}
