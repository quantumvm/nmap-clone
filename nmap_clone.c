#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int get_socket();
void start_scan(int socketfd, char *host, int start_port, int end_port);

int main(int argc, char *argv[]){
    unsigned int start_port = 0;
    unsigned int end_port = 0;
    char * host;

    if(argc < 3){
        puts("nmap_clone: use \"nmap_clone -h\" for help");
        exit(1);
    }

    else if(argc == 3){

        //handle user help
        if(strcmp(argv[1],"-h")==0){
            puts("Use: nmap_clone [start_port] [end_port]");
            exit(1);
        }

        //second argument as start and end port.
        else{
            host = argv[1];
            start_port = atoi(argv[2]);
            end_port = atoi(argv[2]);
        }

    }


    //If user has specified a range of ports
    else if(argc == 4){
        host = argv[1];
        start_port = atoi(argv[2]);
        end_port = atoi(argv[3]);

        if(start_port>end_port){
            puts("Start port must be less than end port!");
            exit(1);
        }

    }
    else{
        puts("Too many arguments use -h for help");
        exit(1);
    }

    //Set up a socket to use.
    int socket = get_socket();

    //check that end port is < 65535
    if(start_port <= 0){
        puts("Invalid start_port");
        exit(1);
    }
    if(end_port > 65535){
        puts("Invalid end_port");
        exit(1);
    }


    //start scanning
    start_scan(socket,host,start_port,end_port);
}



int get_socket(){
    if(socket(AF_INET, SOCK_STREAM, 6)==-1) //6=tcp
        return -1;
}


void start_scan(int socketfd, char *host, int start_port, int end_port){

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 6; //tcp
    hints.ai_flags = 0;

    //for converting port int to string
    char port_s[6];
    sprintf(port_s,"%d",start_port);
    int address_info = getaddrinfo(host, port_s, &hints, &result);
    struct sockaddr_in * port_number = (struct sockaddr_in *) result->ai_addr;

    for(int port = start_port; port<=end_port; port++){
        port_number->sin_port = htons(port);

        int connection = connect(socketfd,result->ai_addr, result->ai_addrlen);

        if(connection == 0){
            printf("Port %d is open\n",port);
            close(socketfd);
            socketfd = get_socket();
        }else{
            //printf("Port %d is closed\n",port);
        }

    }
    freeaddrinfo(result);

}
