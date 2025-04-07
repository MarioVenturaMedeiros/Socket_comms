#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Function to handle errors by printing the error message and exiting the program
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// argc is the total number of command line arguments
// argv is an array of strings representing the command line arguments
int main(int argc, char *argv[]) {
    // Declare variables for socket file descriptors, port number, buffer, and address structures
    // sockfd is the file descriptor for the socket, newsockfd is the file descriptor for the accepted connection
    int sockfd, newsockfd, portno, n;
    // datatype, give us the internet address family, and the size of the address structure
    socklen_t clilen;
    // buffer is used to store the message received from the client
    char buffer[256];
    // serv_addr is the address structure for the server, cli_addr is the address structure for the client
    struct sockaddr_in serv_addr, cli_addr;

    // If the user does not provide a port number, print an error message and exit (has to provide it as an argument to run the program)
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // Create a socket using the socket() function, specifying the address family (AF_INET for IPv4), socket type (SOCK_STREAM for TCP), and protocol (0 for default)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // If the socket creation fails, print an error message and exit
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // Initialize the server address structure to zero and set the address family, port number, and IP address
    // bzero is used to clear the memory of the structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //atoi converts the port number from a string to an integer
    portno = atoi(argv[1]);
    // Set the address family to IPv4, the port number to the provided port number, and the IP address to INADDR_ANY (binds to all available interfaces)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // htons converts the port number from host byte order to network byte order
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the specified address and port using the bind() function
    // If the binding fails, print an error message and exit
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // Listen for incoming connections on the socket using the listen() function
    // The second argument specifies the maximum limit of clients that can be queued for connection
    // If the listening fails, print an error message and exit
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        error("ERROR on accept");
    }

    while (1) {
        // Clear the buffer before reading data from the socket
        // Read data from the socket using the read() function
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("Here is the message: %s\n", buffer);
        // Send a response back to the client using the write() function
        bzero(buffer, 255);
        // reads the message from the standard input (keyboard) and stores it in the buffer
        fgets(buffer, 255, stdin);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }
        // If the message is "exit", break the loop and close the connection
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Server exiting...\n");
            break;
        }
    }
    // Close the sockets after the communication is done
    // close(newsockfd) closes the connection with the client
    close(newsockfd);
    close(sockfd);
    return 0;
}