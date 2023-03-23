#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <sys/time.h>
#include "anssock_intf.h"
#include "ans_errno.h"

#include "list.c"
// #include "msg.c"

#define BUFFER_SIZE 203
#define MAX_EVENTS 10

struct mq_message_t{
    int port;
	char type;
    char topic[100];
    char text[100];
};

typedef struct mq_message_t mq_message;

list subscribers = NULL;
struct mq_message m;

int dpdk_handle_event(struct epoll_event ev){
    char recv_buf[BUFFER_SIZE];
    int len, sin_size, send_len = 0;
    char send_buf[BUFFER_SIZE];
    struct sockaddr_in remote_addr;
    if (ev.events & EPOLLIN)
    /*The field events is an input parameter, a bit mask specifying the events the application is interested in for the file descriptor fd.*/
    /*The associated file is available for read(2) operations.*/
    {
        while(1)
        {
            int client_sockfd = anssock_accept(ev.data.fd, (struct sockaddr *)&remote_addr,&sin_size);
            len = anssock_recvfrom(ev.data.fd, recv_buf, BUFFER_SIZE, 0, NULL, NULL);
            /*ssize_t anssock_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)*/
            /*Receive user data from socket. This function is designed as nonblocking function, so shall not set socket as nonblocking and work with epoll.*/
            /*These calls return the number of bytes received, or -1 if an error occurred. In the event of an error, errno is set to indicate the error.*/
            /*If errno is EAGAIN, no data are present to be received.*/
            if(len > 0)
            {
                m.port = (recv_buf[1] << 8) | recv_buf[0];
                m.type = recv_buf[2];
                for (int i = 3; i < 103; i++) m.topic[i-3] = recv_buf[i];
                for (int i = 103; i < 203; i++) m.text[i-103] = recv_buf[i];
                if (m.type == 'p'){
                    printf("Message on topic \" %s \" recieved. text: \"%s\"\n", m.topic, m.text);
                    node* iter = subscribers;
                    while (iter != NULL){
                        if (strcmp(m.topic, iter->topic) == 0){
                            send_buf[0] = m.port & 0xff;
                            send_buf[1] = (unsigned char) (m.port >> 8);
                            send_buf[2] = m.type;
                            for (int i = 3; i < 103; i++) send_buf[i] = m.topic[i-3];
                            for (int i = 103; i < 203; i++) send_buf[i] = m.text[i-103];
                            send_len = anssock_send(ev.data.fd, send_buf, BUFFER_SIZE, 0);
                            if(send_len < 0){
                                printf("send data failed, send_len %d \n", send_len);
                            }                            
                            printf("Message sent to subscriber %s:%d!\n", iter->ip, iter->port);
                        }
                        iter = iter->next;
                    }
                }
                else if (m.type == 's')
                {
                    printf("Client %s:%d has subscribed to topic %s\n", inet_ntoa(remote_addr.sin_addr), m.port, m.topic);
                    list_add(&subscribers, m.topic, inet_ntoa(remote_addr.sin_addr), m.port);
                }
                printf("receive from client(%d) , data len:%d \n", ev.data.fd, len);
            }
            else if(len < 0)
            {
                if (errno == ANS_EAGAIN)                                        /* Resource temporarily unavailable.*/
                {
                    break;
                }
                else
                {
                    printf("remote close the socket, errno %d \n", errno);
                    /*The value in errno is significant only when the return value of the call indicated an error.*/
                    anssock_close(ev.data.fd);                                  /*Close a socket.*/
                    break;
                }
            }
            else
            {
                printf("remote close the socket, len %d \n", len);
                anssock_close(ev.data.fd);                                      /*Close a socket.*/
                break;
            }
        }
    }
    if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
    /*Error condition (only returned in revents; ignored in events). This bit is also set for a file descriptor referring to the write end of a
      pipe when the read end has been closed.*/
    /*Hang up happened on the associated file descriptor.*/
    {
        printf("remote close the socket, event %x \n", ev.events);              /*(%x) outputs a hexadecimal number.*/
        anssock_close(ev.data.fd);                                              /*Close a socket.*/
    }
    return 0;
}

int main(){
    printf("Congratulations! \n");
    int ret, epoll_fd, server_sockfd, client_sockfd, sin_size, nfds;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;                                             /*Structures for handling internet addresses.*/
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];

    ret = anssock_init(NULL);
    if(ret != 0)
        printf("init sock failed \n");
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;                                               /*Internet Protocol v4 addresses.*/

    /**************To be modified**************/
    my_addr.sin_addr.s_addr = INADDR_ANY;                                       /*Binds the socket to all available interfaces*/
    my_addr.sin_port = htons(8883);                                             /*8000*/
    /******************************************/

    /**************To be replaced**************/
    // int soc = socket(AF_INET, SOCK_STREAM, 0);
    /******************************************/
    // int new_socket, sub_socket;
    // struct sockaddr_in srv;
    // struct sockaddr_in cli;
    // int cli_len = sizeof(cli);
    if((server_sockfd = anssock_socket(AF_INET,SOCK_STREAM, 0)) < 0){
        printf("socket error \n");
        return 1;
    }

    // int i, j;
    // char buff[100];
    // char topic[100];
    // char msg[100];
    printf("Server Starting...\n");

    /**************To be replaced**************/
    // if (soc < 0) {
    //     perror("Socket creating failed");
    //     exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    // }
    /******************************************/

    // srv.sin_family = AF_INET;                                   /*Internet Protocol v4 addresses.*/
    
    /**************To be replaced**************/
    // srv.sin_addr.s_addr = htonl(INADDR_ANY);
    // srv.sin_port = htons(8883);

    // if (bind(soc, (struct sockaddr *)&srv, sizeof(srv)) < 0){
    //     perror("Bind failed");
    //     exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    // } 
    // if (listen(soc, 5) < 0){
    //     perror("listening on port failed");
    //     exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    // }
    /******************************************/

    if (anssock_bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0){
        printf("bind error \n");
        return 1;
    }
    if (anssock_listen(server_sockfd, 5) < 0){
        printf("listen error \n");
        return 1;
    }

    printf("Listening socket created successfully!\n");
    printf("Server waiting for connections...\n");

    sin_size = sizeof(struct sockaddr_in);
    epoll_fd = anssock_epoll_create(MAX_EVENTS);
    if(epoll_fd == -1){
        printf("epoll_create failed \n");
        anssock_close(server_sockfd);                                       /*Close a socket.*/
        return 1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = server_sockfd;
    if(anssock_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev) == -1){
        printf("epll_ctl: server_sockfd register failed");
        anssock_close(server_sockfd);                                       /*Close a socket.*/
        anssock_close(epoll_fd);                                            /*Close a socket.*/
        return 1;
    }
    printf("dpdk tcp server is running \n");
    
    while (1){
        // Getting message from Publisher

        nfds = anssock_epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if(nfds == -1){
            printf("start epoll_wait failed \n");
            anssock_close(server_sockfd);                                   /*Close a socket.*/
            anssock_close(epoll_fd);                                        /*Close a socket.*/
            return 1;
        }
        else if(nfds == 0){
            printf("epoll timeout \n");
            continue;
        }
        for(int i = 0; i < nfds; i++){
            if(events[i].data.fd == server_sockfd){
                if((client_sockfd = anssock_accept(server_sockfd, (struct sockaddr *)&remote_addr,&sin_size)) < 0){
                    printf("accept client_sockfd failed \n");
                    anssock_close(server_sockfd);                           /*Close a socket.*/
                    anssock_close(epoll_fd);                                /*Close a socket.*/
                    return 1;
                }
                ev.events = EPOLLIN;
                ev.data.fd = client_sockfd;
                if(anssock_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev) == -1){
                    printf("epoll_ctl: client_sockfd register failed \n");
                    anssock_close(server_sockfd);                           /*Close a socket.*/
                    anssock_close(epoll_fd);                                /*Close a socket.*/
                    return 1;
                }
                printf("Accept client %s,  family: %d, port %d \n", inet_ntoa(remote_addr.sin_addr), remote_addr.sin_family, remote_addr.sin_port);
                anssock_send(client_sockfd, "I have received your message.", 20, 0);
            }
            else{
                ret = dpdk_handle_event(events[i]);
            }
        }
        /**************To be replaced**************/
        // if ((new_socket = accept(soc, (struct sockaddr *)&cli, &cli_len)) < 0){
        //     perror("accept");
        //     exit(1);                                            /*indicates unsucessful termination. However, it's usage is non-portable.*/
        // }
        // recv_message(&m, new_socket);
		// close(new_socket);
        /******************************************/
    }
    return 0;
}