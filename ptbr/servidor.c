#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <pthread.h>
 
void* Servidor(void* arg)
{
    char buffer_do_cliente[256];
    char *buffer, *pollution;

    int sockEntrada = *(int *) arg;
    int n, payload_size;
    for(n = 1; n <= 1000; n++){
        pollution = malloc(128);
        strcpy(pollution,"GETMEMRMALICIOUS");
        free(pollution);   
    }
    printf("\n\n\nAguardando uma requisição de heartbeat...\n");


    read(sockEntrada, buffer_do_cliente, sizeof (buffer_do_cliente));
    
    printf("Type: %s\n",buffer_do_cliente);

    bzero(buffer_do_cliente, sizeof (buffer_do_cliente));

    strcpy(buffer_do_cliente,"Send your payload size");
    write(sockEntrada, buffer_do_cliente, sizeof(buffer_do_cliente));
    bzero(buffer_do_cliente, sizeof (buffer_do_cliente));

    read(sockEntrada, buffer_do_cliente, sizeof (buffer_do_cliente));
    payload_size = atoi(buffer_do_cliente);
    
    buffer = malloc(sizeof(char[payload_size]));

    printf("Payload size: %d\n",payload_size);
    printf("buffer size : %d\n",sizeof(buffer));
    bzero(buffer_do_cliente, sizeof (buffer_do_cliente));

    strcpy(buffer_do_cliente,"Send your payload");
    write(sockEntrada, buffer_do_cliente, sizeof(buffer_do_cliente));
    bzero(buffer_do_cliente, sizeof (buffer_do_cliente));

    read(sockEntrada, buffer_do_cliente, sizeof (buffer_do_cliente));
    printf("Payload: %s\n",buffer_do_cliente);
    memcpy(buffer,buffer_do_cliente,payload_size);

    printf("Sending heartbeat response ");
    write(sockEntrada, buffer, payload_size);

/*      n = 0;
    while ((buffer_do_cliente[n++] = getchar()) != '\n') 
        ; */
    close(sockEntrada);
    pthread_exit((void*) 0);

}
 
int configuracaoServidor()
{
    int sockfd;
    struct sockaddr_in serverAddr;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      printf("Erro no Socket\n");
      exit(1);
    }
    memset(&serverAddr, 0, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(6881);
    if (bind(sockfd, (struct sockaddr *) & serverAddr, sizeof (serverAddr)) < 0)
    {
      printf("Erro no Socket\n");
      exit(1);
     }
    if (listen(sockfd, 5) < 0)
    {
      printf("Erro no Socket\n");
      exit(1);
    }
    return sockfd;
}
 
int main()
{
    struct sockaddr_in serverAddr;
    int sockfd = configuracaoServidor();
 
    while (1)
    {
        int clienteSockfd;
        struct sockaddr_in clienteAddr;
        unsigned int clntLen;
        clntLen = sizeof (clienteAddr);
        pthread_t thread;
    
        if ((clienteSockfd = accept(sockfd, (struct sockaddr *) & clienteAddr, &clntLen)) < 0)
        {
            printf("Erro no Socket\n");
            exit(1);
        }
        if (pthread_create(&thread, NULL, Servidor, &clienteSockfd) != 0)
        {
            printf("Erro na Thread\n");
            exit(1);
        }
        pthread_detach(thread);
    }
    exit(0);
}
