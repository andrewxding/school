#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // define structures like hostent
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "globals.h"
#include "TCP_connection.h"
#include "packet.h"//for temp testing packet 
using namespace std;

void receiveFilePacket(TCP_connection* session);
// int main(int argc, char** argv){
// 	int sockfd, portno;
//     struct sockaddr_in serv_addr;

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
// }

// int main(int argc, char const *argv[])
// {
// 	if (argc < 2){
//         fprintf(stderr, "ERROR, no port provided\n");
//         exit(1);
//     }

// 	int PORT = atoi(argv[1]);
//     //struct sockaddr_in address;
//     int sock = 0, valread;
//     struct sockaddr_in serv_addr;
//     char buffer[1024] = {0};
//     if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
//         printf("\n Socket creation error \n");
//         return -1;
//     }
  
//     memset(&serv_addr, '0', sizeof(serv_addr));
  
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);
      
//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
//         printf("\nInvalid address/ Address not supported \n");
//         return -1;
//     }
  
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
//         printf("\nConnection Failed \n");
//         return -1;
//     }
//     send(sock , "hello", 5 , 0 );
//     printf("Hello message sent\n");
//     // valread = read( sock , buffer, 1024);
//     // printf("%s\n",buffer );
//     return 0;
// }
void error(string msg)
{
    perror(msg.c_str());
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd;  // socket descriptor
    int portno;//, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;  // contains tons of information, including the server's IP address

    // char buffer[PACKET_SIZE+1];
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port filename\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // create a new socket
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);  // takes a string like "www.yahoo.com", and returns a struct hostent which contains information, as IP address, address type, the length of the addresses...
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //initialize server's address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) //establish a connection to the server
        error("ERROR connecting");

    // printf("Please enter the message: ");
    // memset(buffer,0, 256);
    // fgets(buffer,255,stdin);  // read message

    // n = write(sockfd,buffer,strlen(buffer));  // write to the socket
    // if (n < 0)
    //      error("ERROR writing to socket");

    TCP_connection* session = new TCP_connection(false, sockfd, serv_addr);
    session->InitiateHandshake();

    session->sendData((uint8_t*)argv[3]);
    // int i = 0;
    // while(1){
    // 	cout << ++i <<  "just ran\n";
    // 	sleep(1);
    // 	// cout << "just 123ran\n";
    // 	if(session.receiveData()){
    // 		TCP_Packet packet = TCP_Packet(session.getRecBuffer(), session.getRecvSize());
    // 		cout << packet.getHeader().fields[SEQ] << " " << packet.getLength() << " " << session.getRecvSize() << endl;
	   //  	vector<uint8_t> data = packet.getData();
	   //  	for(ssize_t i = 0; i < data.size(); i ++){
	   //  		cout << (char)data[i];
	   //  	}
    // 	}
    // 	else{
    // 		cout << "nop data";
    // 	}
    	
    // }
    receiveFilePacket(session);
    close(sockfd);  // close socket

    return 0;
}

void receiveFilePacket(TCP_connection *session){
    while(1){
        if(session->receiveData()){
            TCP_Packet packet = TCP_Packet(session->getRecBuffer(), session->getRecvSize());
            uint16_t seqnum = packet.getHeader().fields[SEQ];
            cout << "Received packet and acking" << seqnum << endl;

            vector<uint8_t> data = packet.getData();
            ofstream myfile;
            myfile.open ("example.txt", ofstream::out | ofstream::app);
            for(ssize_t i = 0; i < data.size(); i ++)
                myfile << (char)data[i];
            myfile.close();
            
            
            //need global variable of ack num for window
            packet.getHeader().fields[ACK] = seqnum;
            packet.getHeader().setFlags(true, false, false);
            session->sendData(packet.encode());
        }

        // TCP_Packet syn_rec = TCP_Packet(session->getRecBuffer());
        // fprintf(stdout, "Receiving packet 0 SYN\n");
        // cout << syn_rec.getLength();
        // cout << "\n" << syn_rec.getHeader().fields[SEQ];
        // cout << " " << syn_rec.getHeader().fields[ACK] << "\n";

        // //send syn ack
        // TCP_Packet packet_synack = TCP_Packet(m_sendBase, 0, PACKET_SIZE, true, true, false);
        // fprintf(stdout, "Sending packet 1 SYN ACK\n");
        // cout << "packet with" << packet_synack.getHeader().fields[SEQ];
        // cout << " " << packet_synack.getHeader().fields[ACK] << "\n";
        // sendData(packet_synack.encode());
    }
}