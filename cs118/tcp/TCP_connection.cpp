#include "TCP_connection.h"
#include <cstring>
#include <iostream>
#include <time.h>
#include <unistd.h>

TCP_connection::TCP_connection(){}
TCP_connection::TCP_connection(bool isServer, int sockfd, struct sockaddr_in & hostaddr){
	m_sockfd = sockfd;
	m_server = isServer;
	m_hostaddr = hostaddr;
	m_addrlen = sizeof(m_remaddr);
}

void TCP_connection::sendData(uint8_t* data, ssize_t data_size){
 // 	cout << "sending data!!!!!!!!!!\n";
	// if (!data){
	// 	cout << "no data";
	// 	return;
	// }
    // Clear out the send buffer
    memset(m_send_buffer, '\0', sizeof(m_send_buffer));
    //cout << "Here\n";
    // Copy the encoded data to the send buffer
    copy(data, data + data_size, m_send_buffer);
    //cout << "Here1\n" << write(m_sockfd, m_send_buffer, data_size);
    if(m_server){
    	if(sendto(m_sockfd, m_send_buffer, data_size, 0, (struct sockaddr *)&m_remaddr, m_addrlen) <= 0){
    	cout << "bad data!\n";
    // // if(sendto(m_sockfd, m_send_buffer, data_size, 0, (struct sockaddr*)&m_hostaddr, sizeof(m_hostaddr)) <= 0){
    //     perror("error sending to socket");
    	}
    }
    else{
	    if(sendto(m_sockfd, m_send_buffer, data_size, 0, (struct sockaddr *)&m_hostaddr, sizeof(m_hostaddr)) <= 0){
	    	cout << "bad data!!!!!!!!!!\n";
	    // // if(sendto(m_sockfd, m_send_buffer, data_size, 0, (struct sockaddr*)&m_hostaddr, sizeof(m_hostaddr)) <= 0){
	    //     perror("error sending to socket");
	    }
	}
    //cout << "sendt data!!!!!!!!!!\n";
}

bool TCP_connection::receiveData(){
	//cout << "trying to receive data\n";
    // Clear out old content of receive buffer
    memset(m_rec_buffer, '\0', sizeof(m_rec_buffer));
    // socklen_t len = sizeof(m_hostaddr);
    // Default recvfrom, that should block
    
    // ssize_t m_recvSize = recvfrom(m_sockfd, m_rec_buffer, MSS, 0, (struct sockaddr*)&m_hostaddr, &len);
    m_recvSize = recvfrom(m_sockfd, m_rec_buffer, MSS, 0, (struct sockaddr *) &m_remaddr, &m_addrlen);
    if(m_recvSize < 0){
        // We have timed out based on the set timer on the socket
        // if(errno == EWOULDBLOCK){
        //     if (finishing) {
        //         finished = true;
        //     }
        // }
        perror("receive data error");
        return false;
    }

    return (m_recvSize > 0) ? true: false;
}
void TCP_connection::InitiateHandshake(){

    // // Set Sending timeout
    // setTimeout(0, RTO * 1000, 1);

    // // Set Receiving timeout
    // setTimeout(0, RTO * 1000, 0);


	//ssize_t len;

	//char buf[PACKET_SIZE];

	// bool retransmit = false;
	// _sendBase = 0;
	// _nextSeqNum = 0;

	srand(time(NULL));
	m_sendBase = rand() % MAX_SEQ + 1;
	
	//create tcp packet with syn flag set
	TCP_Packet packet_syn = TCP_Packet(m_sendBase, 0, PACKET_SIZE, false, true, false);
	m_nextSeq = m_sendBase + 1;	

	cout << "Sending packet " << m_sendBase << " SYN" << endl;
	// cout << "\npacket with" << packet_syn.getHeader().fields[SEQ];
	// cout << "\n" << packet_syn.getHeader().fields[ACK];

	//need to decode the packet header too
	// uint8_t* data_bytes = packet_syn.getPacketBytes();
	// sendData(data_bytes);
	sendData(packet_syn.encode());
	// fprintf(stdout, "Sent packet 0\n");

	//receive data loop

	while(!receiveData());
	TCP_Packet rec_synack = TCP_Packet(m_rec_buffer);
	cout << "Receiving packet " << rec_synack.getHeader().fields[SEQ] << " SYN ACK\n";
	// cout << "\npacket with" << rec_synack.getHeader().fields[SEQ];
	// cout << " " << rec_synack.getHeader().fields[ACK] << "\n";
	// // //send ack
	TCP_Packet packet_ack = TCP_Packet(m_nextSeq, 1, PACKET_SIZE, true, false, false);
	cout << "Sending packet " << m_nextSeq << " ACK\n";
	// cout << "packet with" << packet_ack.getHeader().fields[SEQ];
	// cout << " " << packet_ack.getHeader().fields[ACK] << "\n";
	m_nextSeq++;
	sendData(packet_ack.encode());

};
void TCP_connection::ReceiveHandshake(){
	cout << "attempting to receive handshake\n";
	//read header for length
	//read header
	//read data
	while(!receiveData());
	TCP_Packet syn_rec = TCP_Packet(m_rec_buffer);
	cout << "Receiving packet " <<  syn_rec.getHeader().fields[SEQ] << " SYN\n";
	m_expectedSeq = syn_rec.getHeader().fields[SEQ] + 1;
	// cout << syn_rec.getLength();
	// cout << "\n" << syn_rec.getHeader().fields[SEQ];
	// cout << " " << syn_rec.getHeader().fields[ACK] << "\n";



	//send syn ack
	m_sendBase = rand() % MAX_SEQ + 1;
	TCP_Packet packet_synack = TCP_Packet(m_sendBase, m_expectedSeq, PACKET_SIZE, true, true, false);
	cout << "Sending packet " << m_sendBase << " SYN ACK\n";
	m_nextSeq = m_sendBase + 1;
	// cout << "packet with" << packet_synack.getHeader().fields[SEQ];
	// cout << " " << packet_synack.getHeader().fields[ACK] << "\n";
	sendData(packet_synack.encode());
	m_expectedSeq++;
	

	while(!receiveData());
	TCP_Packet ack_rec = TCP_Packet(m_rec_buffer);
	cout << "Receiving packet " << ack_rec.getHeader().fields[SEQ] << " SYN\n";
	m_expectedSeq++;
	// cout << syn_rec.getLength();
	// cout << "" << ack_rec.getHeader().fields[SEQ];
	// cout << " " << ack_rec.getHeader().fields[ACK] << "\n";

};
