#include "chessGame.h"
int main() {
    ChessGame game;

    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1000];

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options to reuse the IP address and IP port if either is already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        return -1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    INFO("Server listening on port %d", PORT);
    // Accept incoming connection
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    initialize_game(&game);

    INFO("Server accepted connection");

    while (1) {
        read(connfd, buffer, 1000);
        int result = receive_command(&game, buffer, connfd, false);
        if(result == 1002){
            close(connfd);
            exit(1);
        }
        printf("Next input: ");
        scanf("%s", buffer);
        int result2 = send_command(&game, buffer, connfd, false);
        while(result2 == COMMAND_ERROR || result2 == COMMAND_UNKNOWN){
            printf("Next input: ");
            scanf("%s", buffer);
            result2 = send_command(&game, buffer, connfd, false);
        }
        if(strncmp(buffer, "/forfeit", 8) == 0){
            close(connfd);
            exit(1);
        }

        // Fill this in
    }

    close(listenfd);
    return 0;
}