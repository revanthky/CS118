//NAME: REVANTH YAMANI
//UID: 104971055
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
using namespace std;
#define MAX_BUFFER 2048
#include "revpacket.h"

static int port_num;
int main(int a, char **b)
{
  // *** Initialize socket for listening ***
  int socket_fd;
  if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }

  short start_seq = 23;
  int init_s = rand() % 25600;
  start_seq = short(init_s);

  port_num = atoi(b[2]);
  // *** Initialize local listening socket address ***
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_num);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY allows to connect to any one of the host’s IP address
  socklen_t len;

  // directly from man page for getaddrinfo
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  int recv_val;
  if ((recv_val = getaddrinfo(b[1], b[2], &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(recv_val));
    return 1;
  }

  char buffer[MAX_BUFFER] = {0};
  struct Packet syn_sent;
  syn_sent.init_fields(start_seq, 0, 0, 1, 0, 0);

  syn_sent.send_SYN_msg();
  sendto(socket_fd, &syn_sent, sizeof(syn_sent), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

  size_t recvlen;
  struct Packet syn_ack_received;
  recvlen = recvfrom(socket_fd, &syn_ack_received, sizeof(syn_ack_received), 0, (struct sockaddr *)&server_addr, &len);

  int valid_bool = 0;
  int ack_bool = 0;
  if (syn_ack_received.packet_header_vals.ack_bit == 1 and syn_ack_received.packet_header_vals.syn_bit == 1)
  {

    syn_ack_received.get_SYNACK_msg();
    valid_bool = 1;
    ack_bool = 1;
  }
  short curr_seq_num = syn_ack_received.packet_header_vals.ack_num;
  short curr_ack_num = syn_ack_received.packet_header_vals.seq_num + 1;

  FILE *fp = fopen(b[3], "r");
  int file_source_length;
  //  char* source = source = malloc(sizeof(char) * (fsize + 1));
  long fsize;
  char *source = NULL;
  if (fseek(fp, 0L, SEEK_END) == 0)
  {
    fsize = ftell(fp);

    source = (char *)malloc(sizeof(char) * (fsize + 1));

    if (fseek(fp, 0L, SEEK_SET) != 0)
    {
      printf("Error reading in file contents");
      return 0;
    }

    file_source_length = fread(source, sizeof(char), fsize, fp);

    source[file_source_length] = '\0';
  }
  fclose(fp);

  struct Packet cli_ack;
  int remaining = fsize;
  int offset = 0;

  bool ack_flag = true;
  while (remaining > 0)
  {
    if (remaining < 512)
    {
      memcpy(cli_ack.file_data, source + offset, file_source_length - offset);
      cli_ack.packet_header_vals.data_size = remaining;
      cli_ack.init_fields(curr_seq_num, curr_ack_num, ack_flag, 0, 0, remaining);
      //ack_flag = false;
    }
    else
    {
      memcpy(cli_ack.file_data, source + offset, 512);
      cli_ack.packet_header_vals.data_size = 512;

      cli_ack.init_fields(curr_seq_num, curr_ack_num, ack_flag, 0, 0, 512);
      //ack_flag = false;
    }

    if (valid_bool)
    {

      sendto(socket_fd, &cli_ack, sizeof(cli_ack), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
      if (ack_flag)
      {
        cli_ack.send_ACK_msg();
        ack_flag = false;
      }
      else
      {
        cli_ack.send_only_data();
      }

      struct Packet ack_received;
      recvlen = recvfrom(socket_fd, &ack_received, sizeof(ack_received), 0, (struct sockaddr *)&server_addr, &len);
      ack_received.get_ack_msg();
      curr_seq_num = ack_received.packet_header_vals.ack_num;
      curr_ack_num += 1; // when does this update??
      if (curr_ack_num > 25600)
      {
        curr_ack_num = 0;
      }
      cli_ack.init_fields(ack_received.packet_header_vals.ack_num, ack_received.packet_header_vals.seq_num + 1, false, false, false, 0);
    }
    offset += 512;
    remaining = file_source_length - offset;
  }

  struct Packet fin;
  fin.init_fields(curr_seq_num, 0, false, false, true, 1);
  sendto(socket_fd, &fin, sizeof(fin), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
  fin.send_FIN_msg();

  struct Packet fin_ack_received;
  recvlen = recvfrom(socket_fd, &fin_ack_received, sizeof(fin_ack_received), 0, (struct sockaddr *)&server_addr, &len);

  fin_ack_received.get_ack_msg();

  struct Packet fin_received;
  recvlen = recvfrom(socket_fd, &fin_received, sizeof(fin_received), 0, (struct sockaddr *)&server_addr, &len);
  fin_received.get_FIN_msg();

  struct Packet fin_ack;
  fin_ack.packet_header_vals.seq_num = curr_seq_num + 1;
  fin_ack.packet_header_vals.ack_num = fin_received.packet_header_vals.seq_num + 1;
  fin_ack.packet_header_vals.ack_bit = 1;
  fin_ack.packet_header_vals.syn_bit = 0;
  fin_ack.packet_header_vals.fin_bit = 0;
  sendto(socket_fd, &fin_ack, sizeof(fin_ack), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
  fin_ack.send_ACK_msg();

  close(socket_fd);
  return 0;
}
