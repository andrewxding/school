#include "TCP_server.h"
#include <cstring>

#include <time.h>
#include <unistd.h>
#include <sys/stat.h> 

TCP_server::TCP_server(TCP_connection* connection){
	m_connection = connection;

}
// void TCP_server::setServer(TCP_connection connection){
// 	m_connection = connection;
// }	




void TCP_server::setFile(char* path){ 
	// struct stat attr;
 //    ifstream m_file(path, ifstream::binary);
 //    if (not m_file){
 //      cerr << "file: " << path << " failed to open" << endl;
 //      exit(1);
 //    }
 //  	int res = stat(path, &attr);
	// //m_file = file; 
	// m_fileSize = attr.st_size; 

	// cout << "set file";
	m_file.open(path, ios::in | ios::binary);
    // Server will ensure this is effectively handled
    // That is, exit the program if this happens
    if(!m_file || !m_file.is_open()){
        perror("File failed to open\n");
        // m_status = false;
    }
    // Get length of file while restoring file pointer state
    m_file.seekg(0, m_file.end);
    m_fileSize = m_file.tellg();
    m_file.seekg(0, m_file.beg);
}
void TCP_server::chunkToPacket(){//ssize_t numchunks){
	ssize_t total_size = m_fileSize;
	size_t total_chunks = total_size / PACKET_SIZE;
  size_t last_chunk_size = total_size % PACKET_SIZE;

	if (last_chunk_size != 0){
    	++total_chunks;
    }
  	else{
      last_chunk_size = PACKET_SIZE;
    }
    cout << "total chunks: " <<  total_chunks<< endl;
  /* the loop of chunking */
  	for (size_t chunk = 0; chunk < total_chunks; ++chunk){
      	size_t this_chunk_size =
        chunk == total_chunks - 1 /* if last chunk */
        ? last_chunk_size /* then fill chunk with remaining bytes */
        : PACKET_SIZE; /* else fill entire chunk */

      /* if needed, we also have the position of this chunk in the file
         size_t start_of_chunk = chunk * chunk_size; */

      /* dapt this portion as necessary, this is the fast C++ way */
	    char chunk_data[this_chunk_size];

	    memset(chunk_data, '\0', sizeof(chunk_data));
        // ssize_t file_pos = m_file.tellg();
        //     // There is no longer a fixed chunk of data left
        //     if(file_pos + PACKET_SIZE > m_bytes){
        //         // Get the remaining bytes
        //         ssize_t remaining = m_bytes - m_file.tellg();
        //         m_file.read(data, remaining);
        //         data_size = remaining;
        //         shouldEnd = true;
        //     } else {
        //         // Read fixed chunks
        //         m_file.read(data, PACKET_SIZE);
        //         data_size = PACKET_SIZE;
        //     }



	  	m_file.read(chunk_data, this_chunk_size); 
	    //TCP_Packet newPacket = TCP_Packet(tcp_server.getnextSeq(), 0, (int)m_cwnd*PACKET_SIZE, 0, 0, 0);
	    TCP_Packet newPacket = TCP_Packet(m_connection->getNextSeq(), 0, 20, 0, 0, 0);
      m_connection->setNextSeq(m_connection->getNextSeq() + 1);
	    newPacket.setData(chunk_data, this_chunk_size);
	    cout << chunk_data;
	    cout << endl;
	    m_file_packets.push_back(newPacket);

	    //cout << "chunk" << newPacket.getHeader().fields[SEQ] << " " << newPacket.getLength() << " " << this_chunk_size << endl;
	}
      /* do something with chunk_data before next iteration */
}

void TCP_server::sendPacket(){
	cout <<  m_file_packets.size() << endl;
	for(ssize_t i = 0; i < m_file_packets.size(); i++){
		//cout << "packet " << i <<  " " << m_file_packets[i].getHeader().fields[SEQ]<< endl;
		m_connection->sendData(m_file_packets[i].encode(), m_file_packets[i].getLength());
		// //m_file_packets[i].encode();
		// cout << "sent packet " << i << endl;
	}
}


void TCP_server::ackPackets(){
  while(1){
      if(m_connection->receiveData()){
          TCP_Packet packet = TCP_Packet(m_connection->getRecBuffer(), m_connection->getRecvSize());

          uint16_t seqnum = packet.getHeader().fields[SEQ];
          // if(packet.getHeader().fields[FLAGS])
            cout << "Received packet ack for " << seqnum;
            cout << packet.getHeader().fields[SEQ] << " " << packet.getLength() << " " << m_connection->getRecvSize() << endl;
          // vector<uint8_t> data = packet.getData();
          // for(ssize_t i = 0; i < data.size(); i ++){
          //   cout << (char)data[i];
          // }
      }
    }
}