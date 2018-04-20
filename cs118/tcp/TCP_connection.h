#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H


#include "globals.h"
#include "packet.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <sys/time.h>
#include "stdint.h"
#include <iostream>
#include <netinet/in.h>
using namespace std;

class TCP_connection {
	private:
		struct sockaddr_in m_hostaddr;

		uint8_t m_rec_buffer[MSS];
		uint8_t m_send_buffer[MSS];
		int m_sockfd;
		bool m_server;


		uint16_t m_expectedSeq;
		uint16_t m_nextSeq;
		uint16_t m_baseSeq;
		uint16_t m_sendBase;

		ssize_t m_recvSize;

		struct sockaddr_in m_remaddr;
		socklen_t m_addrlen;
		// ssize_t m_ssthresh = SSTHRESH;
	public:
		TCP_connection();
		TCP_connection(bool isServer, int sockfd, struct sockaddr_in & hostaddr);
		void ReceiveHandshake();
        void InitiateHandshake();

        void ReceiveFin();
        void SendFin();

        void sendData(uint8_t* data, ssize_t data_size = MSS);
        bool receiveData();

        uint8_t* getRecBuffer(){ return m_rec_buffer; }
        ssize_t getRecvSize(){ return m_recvSize; }



        uint16_t getNextSeq(){ return m_nextSeq; }
        void setNextSeq(uint16_t i){ m_nextSeq = i; }

        uint16_t getExpectedSeq(){ return m_expectedSeq; }
        void setExpectedSeq(uint16_t i){ m_expectedSeq = i; }

};

#endif
