#define MAX_PAYLOAD 512
#include <cstring>
#include <string>

using namespace std;

struct Header
{
  short seq_num;
  short ack_num;

  bool ack_bit;
  bool syn_bit;
  bool fin_bit;
  int data_size;
};

struct Packet
{
  Header packet_header_vals;
  void send_SYN_msg() const
  {
    cout << "SEND " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " SYN" << endl;
    //printf("\n");
  }
  void send_SYNACK_msg() const
  {
    //printf("SEND %d %d", p_header.seq_num, p_header.ack_num);
    cout << "SEND " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " SYN ACK" << endl;
    // if (p_header.syn_bit == true)
    //   cout<<" SYN";
    // if (p_header.fin_bit == true)
    //   cout<<" FIN";
    // if (p_header.ack_bit == true)
    //   cout<<" ACK";
    // printf("\n");
  }
  void send_FIN_msg()
  {
    packet_header_vals.ack_num = (short(0)); //ALWAYS 0 when sending a FIN Message; need this to explicitly make sure?
    cout << "SEND " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " FIN" << endl;
  }
  void send_ACK_msg() const
  {
    cout << "SEND " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " ACK" << endl;
  } // this should be used to send the actual data as well
  void send_only_data() const
  {
    cout << "SEND " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << endl;
  }

  //RECEVER FUNCTIONS FOR PRINTING PACKET TRANSMISSION:
  void get_SYN_msg() const
  {
    cout << "RECV " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " SYN" << endl;
  }
  void get_SYNACK_msg() const
  {
    cout << "RECV " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " SYNACK" << endl;
  }
  void get_FIN_msg()
  {
    packet_header_vals.ack_num = (short(0));
    cout << "RECV " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " FIN" << endl;
  }
  void get_ack_msg() const
  {
    cout << "RECV " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << " ACK" << endl;
  }
  void get_only_data() const
  {
    cout << "RECV " << packet_header_vals.seq_num << " " << packet_header_vals.ack_num << endl;
  }
  // void recv_pprint() const
  // {
  //   printf("RECV %d %d", packet_header_vals.seq_num, p_header.ack_num);
  //   if (p_header.syn_bit)
  //     printf(" SYN");
  //   if (p_header.fin_bit)
  //     printf(" FIN");
  //   if (p_header.ack_bit)
  //     printf(" ACK");
  //   //cout << "SIZE SENT: " << p_header.data_size << endl;
  //   //cout << "ACTUAL PAYLOAD SIZE " << strlen(data) << endl;
  //   // printf("size is %d ", data.size());
  //   //dup-ack//?
  //   printf("\n");
  // }

  // void rsnd_pprint() const
  // {
  //   printf("RESEND %d %d", p_header.seq_num, p_header.ack_num);
  //   if (p_header.syn_bit)
  //     printf(" SYN");
  //   if (p_header.fin_bit)
  //     printf(" FIN");
  //   if (p_header.ack_bit)
  //     printf(" ACK");
  //   //dup-ack//?
  //   printf("\n");
  // }
  // void tout_pprint() const
  // {
  //   printf("TIMEOUT %d\n", p_header.seq_num);
  // }
  void init_fields(short s, short a, bool aBit, bool sBit, bool fBit, int dSize)
  {
    packet_header_vals.seq_num = s;
    packet_header_vals.ack_num = a;
    packet_header_vals.syn_bit = sBit;
    packet_header_vals.ack_bit = aBit;
    packet_header_vals.fin_bit = fBit;
    if (fBit)
    {
      packet_header_vals.ack_num = 0;
    }
    packet_header_vals.data_size = dSize;
  }

  char file_data[MAX_PAYLOAD];
};

struct Final_Packet
{
  Header p_header;
  // string data;
  char *last_data;

  void send_pprint() const
  {
    printf("SEND %d %d", p_header.seq_num, p_header.ack_num);
    if (p_header.syn_bit)
      printf(" SYN");
    if (p_header.fin_bit)
      printf(" FIN");
    if (p_header.ack_bit)
      printf(" ACK");
    printf("\n");
  }
  void recv_pprint() const
  {
    printf("RECV %d %d", p_header.seq_num, p_header.ack_num);
    if (p_header.syn_bit)
      printf(" SYN");
    if (p_header.fin_bit)
      printf(" FIN");
    if (p_header.ack_bit)
      printf(" ACK");
    // printf("size is %d ", data.size());
    //dup-ack//?
    printf("\n");
  }

  void rsnd_pprint() const
  {
    printf("RESEND %d %d", p_header.seq_num, p_header.ack_num);
    if (p_header.syn_bit)
      printf(" SYN");
    if (p_header.fin_bit)
      printf(" FIN");
    if (p_header.ack_bit)
      printf(" ACK");
    //dup-ack//?
    printf("\n");
  }
  void tout_pprint() const
  {
    printf("TIMEOUT %d\n", p_header.seq_num);
  }
  void set_data(short seq, short ack, bool ackBit, bool synBit, bool finBit, int dataSize)
  {
    p_header.seq_num = seq;
    p_header.ack_num = ack;
    p_header.syn_bit = synBit;
    p_header.ack_bit = ackBit;
    p_header.fin_bit = finBit;
    p_header.data_size = dataSize;
  }
};