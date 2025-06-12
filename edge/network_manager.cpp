#include "network_manager.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>

#include "opcode.h"
using namespace std;

NetworkManager::NetworkManager() 
{
  this->sock = -1;
  this->addr = NULL;
  this->port = -1;
}

NetworkManager::NetworkManager(const char *addr, int port)
{
  this->sock = -1;
  this->addr = addr;
  this->port = port;
}

void NetworkManager::setAddress(const char *addr)
{
  this->addr = addr;
}

const char *NetworkManager::getAddress()
{
  return this->addr;
}

void NetworkManager::setPort(int port)
{
  this->port = port;
}

int NetworkManager::getPort()
{
  return this->port;
}

int NetworkManager::init()
{
	struct sockaddr_in serv_addr;

	this->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (this->sock == FAILURE)
  {
    cout << "[*] Error: socket() error" << endl;
    cout << "[*] Please try again" << endl;
    exit(1);
  }

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(this->addr);
	serv_addr.sin_port = htons(this->port);

	if (connect(this->sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == FAILURE)
  {
    cout << "[*] Error: connect() error" << endl;
    cout << "[*] Please try again" << endl;
    exit(1);
  }
	
  cout << "[*] Connected to " << this->addr << ":" << this->port << endl;

  return sock;
}

// TODO: You should revise the following code
int NetworkManager::sendData(uint8_t *data, int dlen)
{
  int sock, tbs, sent, offset, num, jlen;
  unsigned char opcode;
  uint8_t vector_type;
  uint8_t n[4];
  uint8_t *p;

  sock = this->sock;
  
  // Step 1: Send opcode (OPCODE_DATA)
  opcode = OPCODE_DATA;
  tbs = 1; offset = 0;
  while (offset < tbs)
  {
    sent = write(sock, &opcode + offset, tbs - offset);
    if (sent > 0)
      offset += sent;
  }
  assert(offset == tbs);

  // Step 2: Send vector type (1 byte)
  // Vector types:
  // 1 = AGG_AVG: [temp(1), humid(1), power(2), month(1)] = 5 bytes
  // 2 = AGG_CUSTOM: [max_power(2), humid_bucket(1), weekday(1)] = 4 bytes  
  // 3 = AGG_COMFORT: [temp(1), humid(1), discomfort_index(1), weekday(1)] = 4 bytes
  if (dlen == 5) {
    vector_type = 1;  // AGG_AVG
  } else if (dlen == 4) {
    // Improved detection logic: Check the third byte to distinguish vector types
    // AGG_CUSTOM: [max_power(2), humid_bucket(0-2), weekday(0-6)]
    // AGG_COMFORT: [temp(-20 to 50), humid(0-100), discomfort_index(15-100), weekday(0-6)]
    
    uint8_t third_byte = data[2];
    uint8_t fourth_byte = data[3];
    
    // In AGG_CUSTOM, third byte is humid_bucket (0, 1, or 2)
    // In AGG_COMFORT, third byte is discomfort_index (typically 15-100)
    // Fourth byte is always weekday (0-6) in both cases
    
    if (third_byte <= 2 && fourth_byte <= 6) {
      // Likely AGG_CUSTOM: humid_bucket is 0-2, weekday is 0-6
      vector_type = 2;  // AGG_CUSTOM
    } else if (third_byte >= 15 && third_byte <= 100 && fourth_byte <= 6) {
      // Likely AGG_COMFORT: discomfort_index is 15-100, weekday is 0-6
      vector_type = 3;  // AGG_COMFORT
    } else {
      // Fallback: use first two bytes method as backup
      uint16_t first_two_bytes = (data[0] << 8) | data[1];
      if (first_two_bytes >= 200 && first_two_bytes <= 500) {
        vector_type = 2;  // AGG_CUSTOM (max_power range)
      } else {
        vector_type = 3;  // AGG_COMFORT (default for 4-byte vectors)
      }
    }
  } else {
    vector_type = 0;  // Unknown
  }
  
  tbs = 1; offset = 0;
  while (offset < tbs)
  {
    sent = write(sock, &vector_type + offset, tbs - offset);
    if (sent > 0)
      offset += sent;
  }
  assert(offset == tbs);

  // Step 3: Send actual data
  tbs = dlen; offset = 0;
  while (offset < tbs)
  {
    sent = write(sock, data + offset, tbs - offset);
    if (sent > 0)
      offset += sent;
  }
  assert(offset == tbs);

  return 0;
}

// TODO: Please revise or implement this function as you want. You can also remove this function if it is not needed
uint8_t NetworkManager::receiveCommand() 
{
  int sock;
  uint8_t opcode;
  uint8_t *p;

  sock = this->sock;
  opcode = OPCODE_WAIT;

  while (opcode == OPCODE_WAIT)
    read(sock, &opcode, 1);

  assert(opcode == OPCODE_DONE || opcode == OPCODE_QUIT) ;

  return opcode;
}
