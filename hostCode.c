#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 6769

int main(void) 
{
    int listen_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    fd_set read_fds;
    char buffer[4096];
    int n;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) 
    {
        perror("socket");
        exit(0);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind");
        close(listen_fd);
        exit(0);
    }

    if (listen(listen_fd, 1) < 0) 
    {
        perror("listen");
        close(listen_fd);
        exit(0);
    }

    printf("[*] Listening on port:%d ...\n", PORT);

    client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) 
    {
        perror("accept");
        close(listen_fd);
        exit(0);
    }

    // Convert the client address to a string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[+] Connection received from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    close(listen_fd);

    // Bidirectional relay: stdin <-> socket
    while (1) 
    {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_fd, &read_fds);

        int maxfd = (STDIN_FILENO > client_fd) ? STDIN_FILENO : client_fd;
        if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) < 0) 
        {
            perror("select");
            break;
        }

        // Forward stdin → socket
        if (FD_ISSET(STDIN_FILENO, &read_fds)) 
        {
            n = read(STDIN_FILENO, buffer, sizeof(buffer));
            if (n <= 0) break;
            write(client_fd, buffer, n);
        }

        // Forward socket → stdout
        if (FD_ISSET(client_fd, &read_fds)) 
        {
            n = read(client_fd, buffer, sizeof(buffer));
            if (n <= 0) 
            {
                printf("\n[!] Connection closed by remote host.\n");
                break;
            }
            write(STDOUT_FILENO, buffer, n);
        }
    }

    close(client_fd);
    return 0;
}