/* 
 * echoserver.c - A simple connection-based echo server 
 * usage: echoserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFSIZE 1024

#if 0
/* 
 * Structs exported from netinet/in.h (for easy reference)
 */

/* Internet address */
struct in_addr {
  unsigned int s_addr; 
};

/* Internet style socket address */
struct sockaddr_in  {
  unsigned short int sin_family; /* Address family */
  unsigned short int sin_port;   /* Port number */
  struct in_addr sin_addr;	 /* IP address */
  unsigned char sin_zero[...];   /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
  char    *h_name;        /* official name of host */
  char    **h_aliases;    /* alias list */
  int     h_addrtype;     /* host address type */
  int     h_length;       /* length of address */
  char    **h_addr_list;  /* list of addresses */
}
#endif

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int receive_file(int,FILE*);

int main(int argc, char **argv) {
  int listenfd; /* listening socket */
  int connfd; /* connection socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buffer */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */


  /* check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* socket: create a socket */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /* build the server's internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET; /* we are using the Internet */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* accept reqs to any IP addr */
  serveraddr.sin_port = htons((unsigned short)portno); /* port to listen on */

  /* bind: associate the listening socket with a port */
  if (bind(listenfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* listen: make it a listening socket ready to accept connection requests */
  if (listen(listenfd, 5) < 0) /* allow 5 requests to queue up */ 
    error("ERROR on listen");

  /* main loop: wait for a connection request, echo input line, 
     then close connection. */
  clientlen = sizeof(clientaddr);
  while (1) {

    /* accept: wait for a connection request */
    connfd = accept(listenfd, (struct sockaddr*) &clientaddr,(socklen_t*) &clientlen);
    if (connfd < 0) 
      error("ERROR on accept");
    
    /* gethostbyaddr: determine who sent the message */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL){
      error("ERROR on inet_ntoa\n");
    }
    printf("server established connection with %s (%s)\n", hostp->h_name, hostaddrp);
      time_t now = time(NULL);         // Get current time
  struct tm *t = localtime(&now);  // Convert to local time structure
    File *log = fopen("log.txt","a");//updates the log for each user
    fprintf(log,"%d:", t->tm_year + 1900);  
    fprintf(log,"%d:", t->tm_mon + 1);     
    fprintf(log,"%d", t->tm_mday);
    fprintf(log,"%d:", t->tm_hour);
    fprintf(log,"%d:", t->tm_min);
    fprintf(log,"%d:", t->tm_sec);
    char client_ip[20];
    inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip, 20);
    fprintf(log,"%s\n", client_ip);
    FILE *qr;
    int r = receive_file(connfd,qr);
    
    /* read: read input string from the client */
    // bzero(buf, BUFSIZE);
    // n = read(connfd, buf, BUFSIZE);
    // if (n < 0) 
    //   error("ERROR reading from socket");
    // printf("server received %d bytes: %s", n, buf);
    
    // /* write: echo the input string back to the client */
    // n = write(connfd, buf, strlen(buf));
    // if (n < 0) 
    //   error("ERROR writing to socket");

    // close(connfd);
  }
  close(connfd);
}

int receive_file(int socket_fd, FILE* image) {
    image = fopen("image.jpg", "wb");
    if (image == NULL) {
        perror("[-] Error opening destination file");
        return -1;
    }
    char buffer[BUFSIZE];
    int size;
    read(socket_fd, buffer, BUFSIZE);
    size = atoi(buffer);//reads the size from the client
    // // Keep receiving data chunks until the peer closes the connection
    // while (1) {
    //     n = recv(socket_fd, buffer, size, 0);
    //     if (n <= 0) {
    //         // n == 0 means peer closed connection gracefully; < 0 means error
    //         break; 
    //     }

    //     // write(socket_fd, buffer, strlen(buffer));
    //     bzero(buffer, BUFSIZE);
    // }
    int total_bytes = 0;
    int bytes_received = 0;
    while (total_bytes < size) {//reads until the specified size
        bytes_received = recv(socket_fd, buffer, BUFSIZE, 0);
        if (bytes_received < 0) {
            perror("[-] Error reading stream");
            break;
        } else if (bytes_received == 0) {
            printf("[-] Connection closed prematurely by client\n");
            break;
        }

        fwrite(buffer, 1, bytes_received, image);
        total_bytes += bytes_received;
    }
    
    return 0;
}