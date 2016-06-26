/*
* Ejemplo de server de chat simple con datagramas (UDP). *
* Leandro Lucarella - Copyleft 2004
* Basado en otros ejemplos públicos. *
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

#define SERVER_PORT 20539 //Puerto a usar, esto habra q cambiarlo
#define BUFFER_LEN 1024 //longitud del buffer de lectura
int CAP_EST = 10;//Capacidad del estacionamiento
int ids_vehiculos[200];


int main(int argc, char *argv[]) {

int sockfd; /* descriptor para el socket */
struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
int addr_len, numbytes;/* addr_len contendra el tamano de la estructura sockadd_in y numbytes el * numero de bytes recibidos */
char buf[BUFFER_LEN]; /* Buffer de recepción */
char buf_copy[BUFFER_LEN]; /* Copia Buffer de recepción que se modificara para leer la respuesta*/
char op[1];
char id[20];
char idTicket[4];
char capEst[4];
time_t tiempo;
struct tm *tlocal;
char FechaHora[128];

/*********************Verificacion de entrada*****************************/

//Verificacion de numero de argumentos
if (argc != 7) {
	printf("Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
	exit(1); 
}

//Verificacion de Banderas en cualquier orden
else if ( (strcmp(argv[1],"-l") != 0) || (strcmp(argv[3],"-i") != 0) || (strcmp(argv[5],"-o") != 0) ||
		  (strcmp(argv[1],"-l") != 0) || (strcmp(argv[5],"-i") != 0) || (strcmp(argv[3],"-o") != 0)	||
		  (strcmp(argv[3],"-l") != 0) || (strcmp(argv[5],"-i") != 0) || (strcmp(argv[1],"-o") != 0)	|| 
		  (strcmp(argv[3],"-l") != 0) || (strcmp(argv[1],"-i") != 0) || (strcmp(argv[5],"-o") != 0)	|| 
		  (strcmp(argv[5],"-l") != 0) || (strcmp(argv[1],"-i") != 0) || (strcmp(argv[3],"-o") != 0)	|| 
		  (strcmp(argv[5],"-l") != 0) || (strcmp(argv[3],"-i") != 0) || (strcmp(argv[1],"-o") != 0) ){
	fprintf(stderr,"Alguna bandera esta incorrecta\n");
	fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
	exit(1); 
}

// Verificacion del flag -l
else if( strcmp(argv[1],"-l") = 0 ){
	//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
	if ( (atoi(argv[2]) != 20539) && (atoi(argv[2]) != 20353) && (atoi(argv[2]) != 20093) )
	{
		fprintf(stderr,"ERROR, numero de puerto no valido\n");
	}
	//Verificacion del Puerto, (Num de cifras del puerto)
	if ( (strlen(argv[2]) != 5) )
	{
		fprintf(stderr,"ERROR, el puerto no es compatible\n");
	}
	exit(1);
}
else if( strcmp(argv[3],"-l") = 0 ){
	//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
	if ( (atoi(argv[4]) != 20539) && (atoi(argv[4]) != 20353) && (atoi(argv[4]) != 20093) )
	{
		fprintf(stderr,"ERROR, numero de puerto no valido\n");
	}
	//Verificacion del Puerto, (Num de cifras del puerto)
	if ( (strlen(argv[4]) != 5) )
	{
		fprintf(stderr,"ERROR, el puerto no es compatible\n");
	}
	exit(1);
}
else if( strcmp(argv[5],"-l") = 0 ){
	//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
	if ( (atoi(argv[6]) != 20539) && (atoi(argv[6]) != 20353) && (atoi(argv[6]) != 20093) )
	{
		fprintf(stderr,"ERROR, numero de puerto no valido\n");
	}
	//Verificacion del Puerto, (Num de cifras del puerto)
	if ( (strlen(argv[6]) != 5) )
	{
		fprintf(stderr,"ERROR, el puerto no es compatible\n");
	}
	exit(1);
}

// Verificacion del flag -i
else if( strcmp(argv[1],"-i") = 0 ){
	if ( strcmp(argv[3],"-i") = 0  || strcmp(argv[5],"-i") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}
else if( strcmp(argv[3],"-i") = 0 ){
	if ( strcmp(argv[1],"-i") = 0  || strcmp(argv[5],"-i") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}
else if( strcmp(argv[5],"-i") = 0 ){
	if ( strcmp(argv[3],"-i") = 0  || strcmp(argv[1],"-i") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}

// Verificacion del flag -o
else if( strcmp(argv[1],"-o") = 0 ){
	if ( strcmp(argv[3],"-o") = 0  || strcmp(argv[5],"-o") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}
else if( strcmp(argv[3],"-o") = 0 ){
	if ( strcmp(argv[1],"-o") = 0  || strcmp(argv[5],"-o") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}
else if( strcmp(argv[5],"-o") = 0 ){
	if ( strcmp(argv[3],"-o") = 0  || strcmp(argv[1],"-o") = 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
}

/**************************Fin revision entrada*************************/

//Inicializacion de arreglo de ids de tickets de vehiculos
int i;
for (i = 0; i < 200; ++i)
{
	ids_vehiculos[i] = i+1;
}

/////////////////////////////////////////////////////////
/* se crea el socket */
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
perror("socket");
exit(1); }
/* Se establece la estructura my_addr para luego llamar a bind() */ 
my_addr.sin_family = AF_INET; /* usa host byte order */
my_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */ 
my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */ 
bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */
/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 
printf("Asignado direccion al socket ....\n");
if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0) {
perror("bind");
exit(2); }
///////////////////////////////////////////////////////////////
listen(sockfd,10);
//Servidor se queda activo esperando que alguien se comunique con el
while(1){
	/* Se reciben los datos (directamente, UDP no necesita conexión) */
addr_len = sizeof(struct sockaddr);
printf("Esperando datos ....\n");
memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
(socklen_t *)&addr_len)) == -1) { perror("recvfrom");
exit(3); }
/* Se visualiza lo recibido */

printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr)); 
printf("longitud del paquete en bytes: %d\n",numbytes);
printf("el paquete contiene: %s\n", buf);
strcpy(buf_copy, buf);
strcpy(op, strtok(buf_copy, " "));//op contiene el caracter que nos dice 
strcpy(id, strtok(buf_copy, "\n"));


/* enviamos respuesta al cliente */
memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

if (strcmp(op,"e") == 0)
{
	//Verificamos si hay puestos
	if (CAP_EST > 0)
	{

	  tiempo = time(0);
	  tlocal = localtime(&tiempo);	  
	  strftime(FechaHora,128,"%d/%m/%y %H:%M:%S",tlocal);//FechaHora contiene String con la fecha y hora actual local
	  i = 0;
	  //positivos indican que el id no ha sido asignado, negativo, ya fue asignado
	  while(ids_vehiculos[i] < 0){ //Consigue el primer id que no ha sido asignado, esta en la pos i
	  	i++;
	  }
	  sprintf(idTicket, "%d", ids_vehiculos[i]);
	  ids_vehiculos[i] = (i+1)*-1;
	  CAP_EST--;
	  sprintf(capEst, "Puestos Disponibles: %d", CAP_EST);

	  //Se carga mensaje a buffer:
		strcat(buf,FechaHora);// Si el vehiculo entrara o saldra
		strcat(buf," ");//separador
		strcat(buf,idTicket);// Id del vehiculo
		strcat(buf, "\n");
		strcat(buf, capEst);
		strcat(buf,"\n");//fin del buffer	  		

	}
	else{
		strcat(buf,"leveee, el est esta full compai");
	}
}
else{// hay q Verificar si alguien quiere salir pero hay full cap
	CAP_EST++;
	sprintf(capEst, "Puestos Disponibles: %d", CAP_EST);
	strcat(buf, capEst);
	strcat(buf," ");
	strcat(buf,"Hasta luego \n");
}

if ((numbytes=sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
perror("sendto");
exit(2); }

}


/* cerramos descriptor del socket */ 
close(sockfd);
exit (0); }
