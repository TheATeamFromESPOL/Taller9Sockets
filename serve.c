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
#define QLEN 10 

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 256 
#endif	


//Funcion de ayuda para setear la bandera close on exec
void set_cloexec(int fd){
	if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) < 0){
		printf("error al establecer la bandera FD_CLOEXEC\n");	
	}
}


//Funcion para inicializar el servidor
int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen){

	int fd;
	int err = 0;
	
	if((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
		
	if(bind(fd, addr, alen) < 0)
		goto errout;
		
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		
			if(listen(fd, qlen) < 0)
				goto errout;
	}
	return fd;
errout:
	err = errno;
	close(fd);
	errno = err;
	return (-1);
}


//Damos el servicio
void serve(int sockfd) { 
	int clfd; FILE *fp; 
	char buf[ BUFLEN]; 

	set_cloexec( sockfd); 

	for (;;) { 
		if (( clfd = accept( sockfd, NULL, NULL)) < 0) { 		//Aceptamos una conexion
			syslog( LOG_ERR, "ruptimed: accept error: %s", strerror( errno)); 	//si hay error la ponemos en la bitacora			
			exit( 1); 
		} 

		set_cloexec(clfd); 

		if (( fp = popen("/usr/bin/uptime", "r")) == NULL) { 		//llamamos al programa uptime con un pipe
			sprintf( buf, "error: %s\n", strerror( errno)); 
			send( clfd, buf, strlen( buf), 0); 
		} 
		else { 
			while (fgets( buf, BUFLEN, fp) != NULL) 		//lo que nos haya devuelto uptime, lo mandamos al cliente
				send( clfd, buf, strlen(buf), 0); 
			pclose( fp); 						//cerramos el pipe
		}
		close(clfd); 							//cerramos la conexion con el cliente.
	} 
}



//Main

int main( int argc, char *argv[]) { 
	int sockfd;
	char *ip = argv[1];
	int puerto = atoi(argv[2]);

	struct sockaddr_in direccion_servidor;

	memset(&direccion_servidor, 0, sizeof(direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(puerto);
	direccion_servidor.sin_addr.s_addr = inet_addr(ip) ;
	sockfd = socket(((struct sockaddr *)&direccion_servidor)->sa_family,SOCK_STREAM,0);
	bind(sockfd,(struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor));
	listen(sockfd,100);
	int clfd;FILE *fp;
	char buf[BUFLEN];
	while(1){
		if (( clfd = accept( sockfd, NULL, NULL)) < 0) { 
			syslog( LOG_ERR, "ruptimed: accept error: %s", strerror( errno));
			exit( 1); 
		} 

		set_cloexec(clfd); 

		if (( fp = popen("/usr/bin/uptime", "r")) == NULL) { 	
			sprintf( buf, "error: %s\n", strerror( errno)); 
			send( clfd, buf, strlen( buf), 0); 
		} 
		else { 
			while (fgets( buf, BUFLEN, fp) != NULL) 
				send( clfd, buf, strlen(buf), 0); 
			pclose( fp);
		}
		close(clfd); 
		
	}
	

	
	exit( 1); 
}

