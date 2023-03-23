#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "msg.c"

int main(){
    /**************To be replaced**************/
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    /******************************************/

    int new_socket;
    struct sockaddr_in srv;
    struct sockaddr_in cli;
    int cli_len = sizeof(cli);
    int i, j;
    mq_message m, sub_m;
    printf("Enter the port to listen on for messages: ");
    scanf("%d", &sub_m.port);
    getchar();
    // First, send a message to the broker with the topic you want to subscribe to
    sub_m.type = 's';
    printf("Enter a topic to subscribe to: ");
    i=0;
    while ((sub_m.topic[i++] = getchar()) != '\n');
    sub_m.topic[i-1] = '\0';
    
    /**************To be modified**************/
    send_message(&sub_m, "127.0.0.1", 8883);
    /******************************************/

    // Secondly, listen for messages on your topic
    printf("Subscriber Starting...\n");

    /**************To be replaced**************/
    if (soc < 0){
        perror("Socket creating failed");
        exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    }
    /******************************************/
    srv.sin_family = AF_INET;                                   /*Internet Protocol v4 addresses.*/
    
    /**************To be replaced**************/
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(sub_m.port);
    if (bind(soc, (struct sockaddr *)&srv, sizeof(srv)) < 0){
        perror("Bind failed");
        exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    }
    if (listen(soc, 5) < 0){
        perror("listening on port failed");
        exit(1);                                                /*indicates unsucessful termination. However, it's usage is non-portable.*/
    }
    /******************************************/

    printf("Listening socket created successfully!\n");
    printf("Subscriber waiting for messages...\n");
    while (1){
        /**************To be replaced**************/
        if ((new_socket = accept(soc, (struct sockaddr *)&cli, &cli_len)) < 0){
            perror("accept");
            exit(1);                                            /*indicates unsucessful termination. However, it's usage is non-portable.*/
        }
        /******************************************/

        recv_message(&m, new_socket);
        printf("received message on topic %s: %s\n", m.topic, m.text);

        /**************To be replaced**************/
        close(new_socket);
        /******************************************/
    }
    return 0;
}