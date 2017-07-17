#include <sys/types.h>          /* some systems still require this */
#include <sys/stat.h>
#include <stdio.h>              /* for convenience */
#include <stdlib.h>             /* for convenience */
#include <stddef.h>             /* for offsetof */
#include <string.h>             /* for convenience */
#include <unistd.h>             /* for convenience */
#include <signal.h>             /* for SIG_ERR */ 
#include <netdb.h> 
#include <errno.h> 
#include <syslog.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <fcntl.h>

#define BUFLEN 128 
#define QLEN 10 

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 256 
#endif

int main(int argc,char *argv[]){
	if(argc == 1){
		printf("Uso: ./servidor <ip> <numero de puerto>\n");
		exit(-1);
	}

	if(argc != 3){
		printf( "por favor especificar un numero de puerto\n");
		exit(-1);
	}

	struct sockaddr_in direccionServer;
	char *ip = argv[1];
	int puerto = atoi(argv[2]);
	memset(&direccionServer, 0, sizeof(direccionServer));
	
	direccionServer.sin_family = AF_INET;
	direccionServer.sin_port = htons(puerto);
	direccionServer.sin_addr.s_addr = inet_addr(ip);
	
	int socketServer;
	socketServer = socket(((struct sockaddr *)&direccionServer)->sa_family, SOCK_STREAM, 0);
	
	if(bind(socketServer,(struct sockaddr *)&direccionServer,sizeof(direccionServer)) < 0)
		goto errout;

	if(listen(socketServer, 1000) < 0)
		goto errout;
	
	struct sockaddr_in direccionClient;
	int clfd; FILE *fp; char buf[BUFLEN];
 	int sizeCliente;
	int hue;

	while(1){
		sizeCliente = sizeof(direccionClient);
		
		if((clfd = accept(socketServer,(struct sockaddr*)&direccionClient,&sizeCliente)) < 0)
			return(-1);

		direccionClient.sin_family = AF_INET;
		direccionClient.sin_port = htons(puerto);
		
		fp = fopen("huehue","wb");
		char msg[15] = "Huehuehuehuehue";
		if(write(socketServer,msg,sizeof(msg)) == -1)
			return(-1);
		while((hue = recv(socketServer,buf,BUFLEN,0))>0){
			printf("%s\n",buf);
			fwrite(buf,sizeof(char),1,fp);
		}
		fclose(fp);
		close(clfd);
	}

errout:
	printf("Hubo un error.\n");
	return (-1);	
}
