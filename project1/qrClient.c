/* 
 * echoclient.c - A simple connection-based client
 * usage: echoclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

void send_file(int);

int main(int argc, char **argv) {
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    

    /* connect: create a connection with the server */
    if (connect(sockfd,(const sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
      error("ERROR connecting");
    }
    int cmd = 0;
    printf("enter a -1 to exit");
    while (cmd != -1){
        printf("enter a -1 to exit or 0 to continue: ");
        bzero(buf, BUFSIZE);
        fgets(buf, BUFSIZE, stdin);
        cmd = atoi(buf);
        if(cmd == 0){
            send_file(sockfd);
            bzero(buf, BUFSIZE);
            n = read(sockfd, buf, BUFSIZE);
            if (n < 0) {
                error("ERROR reading from socket");
            }
            printf("url: %s", buf);
        }
        else if(cmd !=-1){
            printf("invalid comand");
        }
    }
    /* get message line from the user */
    // printf("Please enter msg: ");
    // bzero(buf, BUFSIZE);
    // fgets(buf, BUFSIZE, stdin);

    // /* write: send the message line to the server */
    // n = write(sockfd, buf, strlen(buf));
    // if (n < 0) 
    //   error("ERROR writing to socket");

    /* read: print the server's reply */

    close(sockfd);
    return 0;
}

void send_file(int socket_fd) {
    char buf[BUFSIZE];
    int fileS;
    printf("Please enter file size: ");
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);
    send(socket_fd, buf, BUFSIZE, 0);//sends the image size
    printf("Please enter file name: ");
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);
    FILE* fp = fopen(buf,"r");
    int bytes_read;
    while ((bytes_read = fread(buf, 1, BUFSIZE, fp)) > 0) {
        if (send(socket_fd, buf, bytes_read, 0) < 0) {
            perror("[-] Error sending image data");
            break;
        }
    }
    // // Read from file and send chunks until End-Of-File (EOF)
    // while (fgets(data, BUFSIZE, fp) != NULL) {
    //     if (send(socket_fd, data, sizeof(data), 0) == -1) {
    //         perror("[-] Error in sending file.");
    //         exit(1);
    //     }
    //     // Clear the buffer for the next chunk
    //     bzero(data, BUFSIZE); 
    // }
}
    