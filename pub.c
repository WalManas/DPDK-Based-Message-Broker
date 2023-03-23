#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "msg.c"

int main(){
    int port = 555;
    char type = 'p';
    char topic[100];
    char text[100];
    int n = 0;
    printf("Connected to server succesfully\n");
    printf("Enter the topic: ");
    while ((topic[n++] = getchar()) != '\n');
    topic[n-1] = '\0';
    n = 0;
    printf("Enter the message: ");
    while ((text[n++] = getchar()) != '\n');
    text[n-1] = '\0';
    
    /**************To be modified**************/
    send_message(&m, "127.0.0.1", 8883);
    /******************************************/

    printf("Message sent to server!\n");
    return 0;
}