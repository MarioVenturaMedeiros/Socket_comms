#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> // for gethostbyname

// Function to handle errors by printing the error message and exiting the program
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// argv[0] is the name of the program
// argv[1] is the server_ipaddress
// argv[2] is the port number

int main (int argc, char *argv[]) {
    // Declare variables for socket file descriptors, port number, buffer, and address structures
    // sockfd is the file descriptor for the socket, newsockfd is the file descriptor for the accepted connection
    int sockfd, portno, n;
    // datatype, give us the internet address family, and the size of the address structure
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // buffer is used to store the message received from the client
    char buffer[255];

    // If the user does not provide a port number, print an error message and exit (has to provide it as an argument to run the program)
    if (argc < 3) {
        fprintf(stderr,"ERROR, usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    // Create a socket using the socket() function, specifying the address family (AF_INET for IPv4), socket type (SOCK_STREAM for TCP), and protocol (0 for default)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // If the socket creation fails, print an error message and exit    
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // Get the server IP address using gethostbyname() function
    server = gethostbyname(argv[1]);
    // If the server IP address is not found, print an error message and exit
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }
    // Initialize the server address structure to zero and set the address family, port number, and IP address
    // bzero is used to clear the memory of the structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // Set the address family to IPv4, the port number to the provided port number, and the IP address to INADDR_ANY (binds to all available interfaces)
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    // htons converts the port number from host byte order to network byte order
    serv_addr.sin_port = htons(portno);
    
    // Connect to the server using the connect() function
    // If the connection fails, print an error message and exit
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    while (1) {
        // Prompt the user for input and read it into the buffer
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        // Send the message to the server using the write() function
        n = write(sockfd,buffer,strlen(buffer));
        // If the write fails, print an error message and exit
        if (n < 0) {
            error("ERROR writing to socket");
        }
        // Clear the buffer and read the response from the server using the read() function
        bzero(buffer,255);
        n = read(sockfd,buffer,255);
        // If the read fails, print an error message and exit
        if (n < 0) {
            error("ERROR reading from socket");
        }
        // Print the response from the server
        printf("%s\n",buffer);

        // If the user types "exit", break the loop and close the connection
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
    }
    // Close the socket using the close() function
    close(sockfd);
    return 0;
}