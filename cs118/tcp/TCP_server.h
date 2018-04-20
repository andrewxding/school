#include "globals.h"
#include <sys/types.h>
#include <vector>
#include "packet.h"
#include "TCP_connection.h"
#include <iostream>
#include <fstream>
using namespace std;

class TCP_server {
	private:
		ifstream m_file;
		// uint16_t m_nextSeq;
		// uint16_t m_baseSeq;
		// uint16_t m_sendBase;
		TCP_connection* m_connection;
		ssize_t m_fileSize;
		vector <TCP_Packet> m_file_packets;
		// ssize_t m_ssthresh = SSTHRESH;
	public:
		TCP_server(TCP_connection* connection);
		void setServer(TCP_connection conn);
		void setFile(char* file_name);
		// void getFileSize(){ return m_fileSize; }

		// uint16_t getnextSeq(){ return m_nextSeq; }
		// ifstream getFile(){ return m_file; }



		void chunkToPacket();
		void sendPacket();

		void ackPackets();
		// void ReceiveHandshake();
  //       void InitiateHandshake();

  //       void ReceiveFin();
  //       void SendFin();

  //       void sendData(uint8_t* data, ssize_t data_size = MSS);
  //       bool receiveData();

  //       uint8_t* getRecBuffer(){ return m_rec_buffer; }
  //       uint16_t getnextSeq(){ return m_nextSeq; }
        
};