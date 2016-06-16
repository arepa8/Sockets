/*
* Ejemplo de cliente de chat simple con datagramas (UDP). *
* Leandro Lucarella - Copyleft 2004
* Basado en diversos ejemplos públicos. *
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#define SERVER_PORT 4321 
#define BUFFER_LEN 1024

int main(int argc, char *argv[]) {

/* Variables asociadas al uso de Sockets */
char buf[BUFFER_LEN]; /* Buffer de recepción */
char buf_copy[BUFFER_LEN]; /* Copia Buffer de recepción que se modificara para leer la respuesta*/
int sockfd, portnum; /* descriptor a usar con el socket */
struct sockaddr_in svr_addr; /* almacenara la direccion IP y numero de puerto del servidor */
struct hostent * host_name; /* para obtener nombre del host */
int addr_len, numbytes; /* conteo de bytes a escribir */


/*********************Verificacion de entrada*****************************/

//Verificacion de numero de argumentos
if (argc != 9) {
	printf("Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
	exit(1); 
}
//Verificacion de Banderas
else if ( (strcmp(argv[1],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[7],"-i") != 0)   ){
	fprintf(stderr,"Alguna bandera esta incorrecta\n");
	fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
	exit(1); 
}
//Verificacion del Puerto, (Num de cifras del puerto)
else if( (strlen(argv[4]) != 5) ){
    fprintf(stderr,"ERROR, el puerto no es compatible\n");
    exit(1);
}
//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
else if( (atoi(argv[4]) != 20539) && (atoi(argv[4]) != 20353)  && (atoi(argv[4]) != 20093) ){
    fprintf(stderr,"ERROR, numero de puerto no valido\n");
    exit(1);
}
else if( (strcmp(argv[6],"e") != 0) && (strcmp(argv[6],"s") != 0) ){
    fprintf(stderr,"ERROR, Indicacion no valida:\n");
    fprintf(stderr,"usar s para indicar salida de vehiculo\n");
 		fprintf(stderr,"usar e para indicar entrada de vehiculo\n");
    exit(1);
}
/**************************Fin revision entrada*************************/

/* convertimos el hostname a su direccion IP */ 
if ((host_name=gethostbyname(argv[2])) == NULL) {
perror("gethostbyname");
exit(1); }

portnum = atoi(argv[4]);
/* Creamos el socket */
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
perror("socket");
exit(2); }
/* a donde mandar */
svr_addr.sin_family = AF_INET; /* usa host byte order */ 
svr_addr.sin_port = htons(portnum); /* usa network byte order */ 
svr_addr.sin_addr = *((struct in_addr *)host_name->h_addr); bzero(&(svr_addr.sin_zero), 8); /* pone en cero el resto */


/* enviamos info al servidor en el buffer*/
memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
strcat(buf,(argv[6]));// Si el vehiculo entrara o saldra
strcat(buf," ");//separador
strcat(buf,(argv[8]));// Id del vehiculo
strcat(buf,"\n");//fin del buffer

if ((numbytes = sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&svr_addr, sizeof(struct sockaddr))) == -1) {
perror("sendto");
exit(2); }

memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

/* recibimos respuesta del servidor*/
if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&svr_addr,(socklen_t *)&addr_len)) == -1) { 
perror("recvfrom");
exit(2);}

printf("enviados %d bytes hacia %s\n",numbytes,inet_ntoa(svr_addr.sin_addr));
printf("Respuesta del servidor: %s\n", buf);

/* cierro socket */ 
close(sockfd); 
exit (0);
}