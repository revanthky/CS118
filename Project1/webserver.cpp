#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>

#include <string>
#include <arpa/inet.h>

#include <unistd.h>
#include <netdb.h>

#include <fstream>
#include <iostream>
using namespace std;

// convert to c++: make string parsing easier??

string getftype(string file_name)
{
    string temp = "binary";
    int name_len = file_name.length();
    int dot_ind = file_name.find(".");

    if (dot_ind == string::npos)
    {
        return temp;
    }
    else
    {
        int ext_length = name_len - dot_ind; // will give the content length after the dot

        temp = file_name.substr(dot_ind + 1, ext_length);
        return temp;
    }
}

int main(int argc, char **argv)
{
    int socket_fd;
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("socket"); //error message for when the socket cannot be created
        exit(1);          // based on  errno, which is set in the cause of failure
                          // may not even need the string message here?
    }

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    memset(&serv_addr, 0, sizeof(serv_addr)); //set initially to 000s then set up
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8064); //pick any arbitrary port that is not 80 or 8080?

    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(socket_fd, 20) == -1) // 20: Max # of Client connections (arbitrary??)
    {
        perror("listen");
        exit(1);
    }

    printf("%s\n", "Successfully running the server, waiting for client connections");

    int connfd;
    int n;

    //pid_t child_pid;

    socklen_t sin_size; //necessary??

    while (1)
    {
        // should buffers be set outside of the loop? memset inside
        char req_buf[2000000]; //buffer to hold HTTP request; typical request size is 700/800 bytes
        int req_buf_size;

        char resp_buf[2000000]; //buffer to hold HTTP response
        int resp_buf_size = 2000000;

        // char file_name[1024];
        int file_len;

        sin_size = sizeof(cli_addr);
        connfd = accept(socket_fd, (struct sockaddr *)&cli_addr, &sin_size);
        if (connfd == -1)
        {
            perror("accept");
            continue;
        }
        printf("Request received, connection accepted from %s\n", inet_ntoa(cli_addr.sin_addr));

        memset(req_buf, 0, 2000000);
        memset(resp_buf, 0, 2000000);
        //memset(file_name, 0, 1024);

        while ((req_buf_size = read(connfd, req_buf, 1024)) > 0)
        {
            if (req_buf_size < 0)
            {
                printf("Error reading in request");
                exit(1);
            }

            req_buf[req_buf_size - 1] = '\0';
            // printf("HTTP Request received from the server: \n%s", req_buf);

            // Need to error check for anything other than GET requests??
            // Which error codes do we need?

            string req_buff_str = req_buf;

            printf("%s\n", req_buf);

            int file_start = 5;

            int file_end = req_buff_str.find(" HTTP");

            string file_name = req_buff_str.substr(file_start, file_end - file_start);

            const char *file_n = file_name.c_str();
            printf("file name parsed: %s\n", file_n);

            string file_type = getftype(file_n);

            const char *file_t = file_type.c_str();

            printf("File type: %s\n", file_t);

            //FILE *file = fopen()

            // char *start = req_buf[0]; // first line
            // //can we assume that we are getting a properly formatted GET request??

            // char *file_start = req_buf[5]; // should be the first character of the file name
            // char *end = strstr("HTTP/");   //two characters behind the end of the file
            // char *file_end = end - 1;

            // file_len = file_end - file_start;
            // strncpy(file_name, file_start, file_len);
            // file_name[file_len] = '\0';

            // printf("file name parsed: %s\n", file_name);

            // char file_type[10];

            string carriage_return = "\r\n";
            string response = "HTTP/1.1 200 OK";
            response += carriage_return;

            string file_line;
            // string file_content;

            ifstream file_input(file_name);

            string file_content((istreambuf_iterator<char>(file_input)), istreambuf_iterator<char>());

            //cout << file_content;
            // ifstream file_input(file_name);

            // while (getline(file_input, file_line))
            // {
            //     cout << file_line << "\n";
            //     file_content += file_line;
            //     file_content += "\n";
            // }

            // file_content = file_content.substr(0, file_content.size() - 1);

            int f_size = file_content.size(); //convert this to a string, is the content length number
            printf("size of file: %d\n", f_size);

            string length_str = "Content-Length: ";
            length_str += to_string(f_size);
            length_str += "\r\n";

            response += length_str;
            string type_str = "Content-Type: ";

            if (file_type.compare("jpg") == 0)
            {
                type_str += "image/jpg"; //png? or jpg??
            }
            else if (file_type.compare("png") == 0)
            {
                type_str += "image/png";
            }
            else if (file_type.compare("txt") == 0)
            {
                type_str += "text/plain";
            }
            else if (file_type.compare("html") == 0)
            {
                type_str += "text/html";
            }

            type_str += "\r\n";

            if (file_type.compare("binary") == 0)
            {
                printf("binary file");
                type_str = "Content-Type: application/octet-stream\r\n";
            }

            response += type_str;
            response += "\r\n";

            response += file_content;
            //response += "Content-Length"

            //strcpy(resp_buf, response.c_str());

            //printf(response);

            //const char *file_r = response.c_str();

            //printf("File response: %s\n", file_r);

            //printf("HTTP RESPONSE MESSAGE: \n %s", response.c_str());
            //resp_buf = response.c_str();

            write(connfd, response.c_str(), response.size());

            //write(connfd, resp_buf, resp_buf_size);
        }
    }
}
