#include <stdio.h>
#include <string.h>

struct mq_message_t{
    int port;
	char type;
    char topic[100];
    char text[100];
};

typedef struct mq_message_t mq_message;

void initialization(){//12 function
    ret = anssock_init(NULL);
    if(ret != 0)
        printf("init sock failed \n");
    if((soc=anssock_socket(AF_INET,SOCK_STREAM, 0)) < 0){
        printf("socket error \n");
        return 1;
    }
    if (anssock_bind(soc,(struct sockaddr *)&srv,sizeof(struct sockaddr)) < 0){
        printf("bind error \n");
        return 1;
    }
    if (anssock_listen(soc, 5) < 0){
        printf("listen error \n");
        return 1;
    }
    if (epoll_fd=anssock_epoll_create(MAX_EVENTS) == -1)
    {
        printf("epoll_create failed \n");
        anssock_close(soc);                                       /*Close a socket.*/
        return 1;
    }
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    ev.events=EPOLLIN;
    /*The associated file is available for read(2) operations.*/
    ev.data.fd=soc;
    if(anssock_epoll_ctl(epoll_fd,EPOLL_CTL_ADD,soc,&ev)==-1){
        printf("epll_ctl:server_sockfd register failed");
        anssock_close(server_sockfd);                                       /*Close a socket.*/
        anssock_close(epoll_fd);                                            /*Close a socket.*/
        return 1;
    }
    while(1){
        if(nfds=anssock_epoll_wait(epoll_fd, events, MAX_EVENTS, -1) == -1){
            printf("start epoll_wait failed \n");
            anssock_close(server_sockfd);                                   /*Close a socket.*/
            anssock_close(epoll_fd);                                        /*Close a socket.*/
            return 1;
        }
        else if(nfds == 0){
            printf("epoll timeout \n");
            continue;
        }
        int i;
        for(i = 0; i < nfds; i++)
        {
            if(events[i].data.fd==server_sockfd)
            {
                if((client_sockfd = anssock_accept(server_sockfd, (struct sockaddr *)&remote_addr,&sin_size)) < 0){
                    printf("accept client_sockfd failed \n");
                    anssock_close(server_sockfd);                           /*Close a socket.*/
                    anssock_close(epoll_fd);                                /*Close a socket.*/
                    return 1;
                }
                ev.events=EPOLLIN;
                /*The associated file is available for read(2) operations.*/
                ev.data.fd=client_sockfd;
                if(anssock_epoll_ctl(epoll_fd, EPOLL_CTL_ADD,client_sockfd,&ev)==-1){
                    printf("epoll_ctl:client_sockfd register failed \n");
                    anssock_close(server_sockfd);                           /*Close a socket.*/
                    anssock_close(epoll_fd);                                /*Close a socket.*/
                    return 1;
                }
                printf("accept client %s,  family: %d, port %d \n",inet_ntoa(remote_addr.sin_addr), remote_addr.sin_family, remote_addr.sin_port);
                anssock_send(client_sockfd, "I have received your message.", 20, 0);
            }
            else{
                ret = dpdk_handle_event(events[i]);
            }
        }
    }
}

void send_message(mq_message *msg, char* ip, int port){
	unsigned char buff[203];
    
    /**************To be replaced**************/
	int soc = socket(AF_INET, SOCK_STREAM, 0);
    /******************************************/

    int i;
    struct sockaddr_in srv;
    srv.sin_family = AF_INET;                                   /*Internet Protocol v4 addresses.*/
    
    /**************To be replaced**************/
    srv.sin_addr.s_addr = inet_addr(ip);
    srv.sin_port = htons(port);
    connect(soc, (struct sockaddr*) &srv, sizeof(srv));
    /******************************************/

    buff[0] = msg->port & 0xff;
    buff[1] = (unsigned char) (msg->port >> 8);
    buff[2] = msg->type;
    for (i = 3; i < 103; i++) buff[i] = msg->topic[i-3];
    for (i = 103; i < 203; i++) buff[i] = msg->text[i-103];

    /**************To be replaced**************/
    write(soc, buff, sizeof(buff));
    close(soc);
    /******************************************/
}

void recv_message(mq_message* msg_buff, int socket){
	unsigned char buff[203];
	int i, temp;

    /**************To be replaced**************/
	read(socket, buff, sizeof(buff));
    /******************************************/

	msg_buff->port = (buff[1] << 8) | buff[0];
    msg_buff->type = buff[2];
    for (i = 3; i < 103; i++) msg_buff->topic[i-3] = buff[i];
    for (i = 103; i < 203; i++) msg_buff->text[i-103] = buff[i];

    /**************To be replaced**************/
    fflush(0);
    /******************************************/
}