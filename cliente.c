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


#define BUFLEN 128 

int main(int argc, char *args[]){	
	if(argc !=5)
		return(-1);

	int client;
	int sizeCliente;
	int puerto;
	int hue;
	char msg[15];
	char buf[BUFLEN];
	FILE *archivo;
	struct sockaddr_in direccionServer;
	
	puerto = atoi(args[2]);
	archivo = fopen(args[3],"rb");
	if(!archivo) return (-1);

	client = socket(PF_INET, SOCK_STREAM,0);
	memset(&direccionServer,0,sizeof(direccionServer));

	direccionServer.sin_family = AF_INET;
	direccionServer.sin_port = htons(puerto);
	hue = inet_pton(AF_INET,args[1],&direccionServer.sin_addr);
	sizeCliente = sizeof(direccionServer);

	if(hue==0){
		close(client);
		return(-1);
	}

	if (connect(client, (struct sockaddr *)&direccionServer, sizeof(direccionServer)) == -1) {
		close(client);
		return(-1);
	}

	while(!feof(archivo)){
		fread(buf,sizeof(char),BUFLEN,archivo);
		if(send(client,buf,BUFLEN,0)==-1)
			return(-1);
	}

	read(client,msg,sizeof(msg));
	printf("%s\n",msg);
	
	return 0;
}
