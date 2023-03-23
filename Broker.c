#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "list.c"
#include "msg.c"

int main(){
    list subscribers = NULL;
    mq_message m;
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    int new_socket, sub_socket;
    struct sockaddr_in srv;
    struct sockaddr_in cli;
    int cli_len = sizeof(cli);
    int i, j;
    char buff[100];
    char topic[100];
    char msg[100];
    printf("Server Starting...\n");
    if (soc < 0){
        perror("Socket creating failed");
        exit(1);
    }
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(8883);
    if (bind(soc, (struct sockaddr *)&srv, sizeof(srv)) < 0){
        perror("Bind failed");
        exit(1);
    }
    if (listen(soc, 5) < 0){
        perror("listening on port failed");
        exit(1);
    }
    printf("Listening socket created successfully!\n");
    printf("Server waiting for connections...\n");
    while (1){
        // Getting message from Publisher
        if ((new_socket = accept(soc, (struct sockaddr *)&cli, &cli_len)) < 0){
            perror("accept");
            exit(1);
        }
        recv_message(&m, new_socket);
		close(new_socket);
        if (m.type == 'p'){
        	printf("Message on topic \" %s \" recieved. text: \"%s\"\n", m.topic, m.text);
		    node* iter = subscribers;
		    while (iter != NULL){
		        if (strcmp(m.topic, iter->topic) == 0){
		        	send_message(&m, iter->ip, iter->port);
		            printf("Message sent to subscriber %s:%d!\n", iter->ip, iter->port);
		        }
		        iter = iter->next;
		    }
        }
        else if (m.type == 's'){
        	printf("Client %s:%d has subscribed to topic %s\n", inet_ntoa(cli.sin_addr), m.port, m.topic);
    		list_add(&subscribers, m.topic, inet_ntoa(cli.sin_addr), m.port);
        }
    }
    return 0;
}