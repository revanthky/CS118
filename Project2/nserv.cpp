// NAME: REVANTH KUMAR YAMANI
//UID: 104971055
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <getopt.h>
using namespace std;
#define MAX_BUFFER 2048

#include "revpacket.h"

static int port_num;

int main(int a, char **b)
{
    int curr_client = 1;

    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    port_num = atoi(b[1]); // R first arg passed in is the port number
    // R Init local socket address for listening
    struct sockaddr_in server_addr;

    struct sockaddr_in client_addr; // client address

    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY allows to connect to any one of the host’s IP address
    // *** Socket Bind ***
    if (bind(socket_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("bind");
        exit(1);
    };
    while (true)
    {
        //  unsigned int sin_size;
        char buffer[MAX_BUFFER] = {0};
        socklen_t len = sizeof(client_addr);
        size_t reclen;

        struct Packet syn_from_client;

        reclen = recvfrom(socket_fd, &syn_from_client, sizeof(syn_from_client), 0, (struct sockaddr *)&client_addr, &len);
        syn_from_client.get_SYN_msg();

        short start_seq_num = 24;
        int rand_s = rand() % 25600;
        short curr_ack_num = 0;
        start_seq_num = short(rand_s);
        short curr_seq_num = start_seq_num;

        curr_ack_num = syn_from_client.packet_header_vals.seq_num + 1;
        struct Packet syn_ack_sent;

        syn_ack_sent.init_fields(start_seq_num, curr_ack_num, 1, 1, 0, 0);

        syn_ack_sent.send_SYNACK_msg();
        sendto(socket_fd, &syn_ack_sent, sizeof(syn_ack_sent), 0, (const struct sockaddr *)&client_addr, len);

        struct Packet received_packet;
        string total_content = "";

        string file_name = std::to_string(curr_client);
        file_name += ".file";
        FILE *file_start = fopen(file_name.c_str(), "w+");
        bool received_ack_bool = true;
        while ((recvfrom(socket_fd, &received_packet, sizeof(received_packet), 0, (struct sockaddr *)&client_addr, &len)) != 0)
        {
            if (received_packet.packet_header_vals.fin_bit == true)
            {
                received_packet.get_FIN_msg();
                break;
            }

            if (received_ack_bool)
            {
                received_packet.get_ack_msg();
                received_ack_bool = false;
            }
            else
            {
                received_packet.get_only_data();
            }

            curr_ack_num += received_packet.packet_header_vals.data_size;

            curr_seq_num = received_packet.packet_header_vals.ack_num;
            struct Packet ack_packet;
            if (curr_ack_num > 25600)
            {
                curr_ack_num = curr_ack_num % 25600;
            }
            ack_packet.init_fields(curr_seq_num, curr_ack_num, true, false, false, 0);
            sendto(socket_fd, &ack_packet, sizeof(ack_packet), 0, (const struct sockaddr *)&client_addr, len);
            ack_packet.send_ACK_msg();

            fwrite(received_packet.file_data, sizeof(char), received_packet.packet_header_vals.data_size, file_start);
        }
        fclose(file_start);
        curr_ack_num = received_packet.packet_header_vals.seq_num + 1;
        struct Packet fin_ack_sent;
        fin_ack_sent.init_fields(curr_seq_num, curr_ack_num, true, false, false, 0);
        fin_ack_sent.send_ACK_msg();
        sendto(socket_fd, &fin_ack_sent, sizeof(fin_ack_sent), 0, (const struct sockaddr *)&client_addr, len);

        struct Packet fin_sent;
        fin_sent.init_fields(curr_seq_num, 0, false, false, true, 1);

        fin_sent.send_FIN_msg();
        sendto(socket_fd, &fin_sent, sizeof(fin_sent), 0, (const struct sockaddr *)&client_addr, len);

        struct Packet fin_ack_received;
        reclen = recvfrom(socket_fd, &fin_ack_received, sizeof(fin_ack_received), 0, (struct sockaddr *)&client_addr, &len);
        fin_ack_received.get_ack_msg();
        curr_client += 1;
    }
    close(socket_fd);
    return 0;
}
