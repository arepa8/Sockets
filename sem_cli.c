/*
* PROYECTO SOCKETS REDES
* Neylin Belisario  09-10093
* Andres Hernandez  10-10353
* Daniel Pelayo         10-10539
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
#include <math.h>


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
char oper[1];
char placa[20];

/*********************Verificacion de entrada*****************************/

//Verificacion de numero de argumentos
if (argc != 9) {
	printf("Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
	exit(1); 
}
//Verificacion de Banderas en cualquier orden
else if ( ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&
          ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[1],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&

          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) &&
          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[3],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) &&

          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&
          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[7],"-i") != 0)) &&
          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[7],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) &&
          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&
          ((strcmp(argv[5],"-d") != 0) || (strcmp(argv[7],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) &&

          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[1],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&
          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[5],"-i") != 0)) &&
          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[5],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) &&
          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[1],"-c") != 0) || (strcmp(argv[3],"-i") != 0)) &&
          ((strcmp(argv[7],"-d") != 0) || (strcmp(argv[5],"-p") != 0) || (strcmp(argv[3],"-c") != 0) || (strcmp(argv[1],"-i") != 0)) ){
	fprintf(stderr,"Alguna bandera esta colocada incorrectamente\n");
	fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
	exit(1); 
}
//Verificacion del flag -d
if ( (strcmp(argv[1],"-d") == 0) )
{
    if ((strcmp(argv[3],"-d") == 0) || (strcmp(argv[5],"-d") == 0) || (strcmp(argv[7],"-d") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((host_name = gethostbyname(argv[2])) == NULL) {
            perror("gethostbyname");
            exit(1);
        }
    }
}
if ( (strcmp(argv[3],"-d") == 0) )
{
    if ((strcmp(argv[1],"-d") == 0) || (strcmp(argv[5],"-d") == 0) || (strcmp(argv[7],"-d") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((host_name = gethostbyname(argv[4])) == NULL) {
            perror("gethostbyname");
            exit(1);
        }
    }
}
if ( (strcmp(argv[5],"-d") == 0) )
{
    if ((strcmp(argv[1],"-d") == 0) || (strcmp(argv[3],"-d") == 0) || (strcmp(argv[7],"-d") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((host_name = gethostbyname(argv[6])) == NULL) {
            perror("gethostbyname");
            exit(1);
        }
    }
}
if ( (strcmp(argv[7],"-d") == 0) )
{
    if ((strcmp(argv[1],"-d") == 0) || (strcmp(argv[3],"-d") == 0) || (strcmp(argv[5],"-d") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((host_name = gethostbyname(argv[8])) == NULL) {
            perror("gethostbyname");
            exit(1);
        }
    }
}

//Verificacion del flag -p
if ( (strcmp(argv[1],"-p") == 0) )
{
    if ((strcmp(argv[3],"-p") == 0) || (strcmp(argv[5],"-p") == 0) || (strcmp(argv[7],"-p") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((atoi(argv[2]) != 20539) && (atoi(argv[2]) != 20353)  && (atoi(argv[2]) != 20093)) {
            fprintf(stderr,"ERROR, El puerto no es compatible\n");
            exit(1);
        }
        else if ((strlen(argv[2]) != 5))
        {
            fprintf(stderr,"ERROR, Numero de puerto no valido\n");
            exit(1);
        }
        else if ((portnum = atoi(argv[2])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
    }
}
else if ( (strcmp(argv[3],"-p") == 0) )
{
    if ((strcmp(argv[1],"-p") == 0) || (strcmp(argv[5],"-p") == 0) || (strcmp(argv[7],"-p") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((atoi(argv[4]) != 20539) && (atoi(argv[4]) != 20353)  && (atoi(argv[4]) != 20093)) {
            fprintf(stderr,"ERROR, El puerto no es compatible\n");
            exit(1);
        }
        else if ((strlen(argv[4]) != 5))
        {
            fprintf(stderr,"ERROR, Numero de puerto no valido\n");
            exit(1);
        }
        else if ((portnum = atoi(argv[4])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
    }
}
else if ( (strcmp(argv[5],"-p") == 0) )
{
    if ((strcmp(argv[1],"-p") == 0) || (strcmp(argv[3],"-p") == 0) || (strcmp(argv[7],"-p") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((atoi(argv[6]) != 20539) && (atoi(argv[6]) != 20353)  && (atoi(argv[6]) != 20093)) {
            fprintf(stderr,"ERROR, El puerto no es compatible\n");
            exit(1);
        }
        else if ((strlen(argv[6]) != 5))
        {
            fprintf(stderr,"ERROR, Numero de puerto no valido\n");
            exit(1);
        }
        else if ((portnum = atoi(argv[6])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
    }
}
else if ( (strcmp(argv[7],"-p") == 0) )
{
    if ((strcmp(argv[1],"-p") == 0) || (strcmp(argv[3],"-p") == 0) || (strcmp(argv[5],"-p") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((atoi(argv[8]) != 20539) && (atoi(argv[8]) != 20353)  && (atoi(argv[8]) != 20093)) {
            fprintf(stderr,"ERROR, El puerto no es compatible\n");
            exit(1);
        }
        else if ((strlen(argv[8]) != 5))
        {
            fprintf(stderr,"ERROR, Numero de puerto no valido\n");
            exit(1);
        }
        else if ((portnum = atoi(argv[8])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
    }
}

//Verificacion del flag -c
if ( (strcmp(argv[1],"-c") == 0) )
{
    if ((strcmp(argv[3],"-c") == 0) || (strcmp(argv[5],"-c") == 0) || (strcmp(argv[7],"-c") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((strcmp(argv[2],"e") != 0) && (strcmp(argv[2],"s") != 0)) {
            strcpy(oper,argv[2]);
            fprintf(stderr,"ERROR, Indicacion '%s' no valida:\n", oper);
            fprintf(stderr,"Usar s para indicar salida de vehiculo\n");
            fprintf(stderr,"Usar e para indicar entrada de vehiculo\n");
            exit(1);
        }
        else if ( strcpy(oper,argv[2]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando operacion de entrada o salida\n");
        }
    }
}
else if ( (strcmp(argv[3],"-c") == 0) )
{
    if ((strcmp(argv[1],"-c") == 0) || (strcmp(argv[5],"-c") == 0) || (strcmp(argv[7],"-c") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((strcmp(argv[4],"e") != 0) && (strcmp(argv[4],"s") != 0)) {
            strcpy(oper,argv[4]);
            fprintf(stderr,"ERROR, Indicacion '%s' no valida:\n", oper);
            fprintf(stderr,"Usar s para indicar salida de vehiculo\n");
            fprintf(stderr,"Usar e para indicar entrada de vehiculo\n");
            exit(1);
        }
        else if ( strcpy(oper,argv[4]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando operacion de entrada o salida\n");
        }
    }
}
else if ( (strcmp(argv[5],"-c") == 0) )
{
    if ((strcmp(argv[1],"-c") == 0) || (strcmp(argv[3],"-c") == 0) || (strcmp(argv[7],"-c") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((strcmp(argv[6],"e") != 0) && (strcmp(argv[6],"s") != 0)) {
            strcpy(oper,argv[6]);
            fprintf(stderr,"ERROR, Indicacion '%s' no valida:\n", oper);
            fprintf(stderr,"Usar s para indicar salida de vehiculo\n");
            fprintf(stderr,"Usar e para indicar entrada de vehiculo\n");
            exit(1);
        }
        else if ( strcpy(oper,argv[6]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando operacion de entrada o salida\n");
        }
    }
}
else if ( (strcmp(argv[7],"-c") == 0) )
{
    if ((strcmp(argv[1],"-c") == 0) || (strcmp(argv[3],"-c") == 0) || (strcmp(argv[5],"-c") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ((strcmp(argv[8],"e") != 0) && (strcmp(argv[8],"s") != 0)) {
            strcpy(oper,argv[8]);
            fprintf(stderr,"ERROR, Indicacion '%s' no valida:\n", oper);
            fprintf(stderr,"Usar s para indicar salida de vehiculo\n");
            fprintf(stderr,"Usar e para indicar entrada de vehiculo\n");
            exit(1);
        }
        else if ( strcpy(oper,argv[8]) == "\0" )
        {
            fprintf(stderr, "ERROR, almacenando operacion de entrada o salida\n");
        }
    }
}

//Verificacion del flag -i
if ( (strcmp(argv[1],"-i") == 0) )
{
    if ((strcmp(argv[3],"-i") == 0) || (strcmp(argv[5],"-i") == 0) || (strcmp(argv[7],"-i") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ( strcpy(placa,argv[2]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando id del vehiculo\n");
        }
    }
}
else if ( (strcmp(argv[3],"-i") == 0) )
{
    if ((strcmp(argv[1],"-i") == 0) || (strcmp(argv[5],"-i") == 0) || (strcmp(argv[7],"-i") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ( strcpy(placa,argv[4]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando id del vehiculo\n");
        }
    }
}
else if ( (strcmp(argv[5],"-i") == 0) )
{
    if ((strcmp(argv[1],"-i") == 0) || (strcmp(argv[3],"-i") == 0) || (strcmp(argv[7],"-i") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ( strcpy(placa,argv[6]) ==  "\0" )
        {
            fprintf(stderr, "ERROR, almacenando id del vehiculo\n");
        }
    }
}
else if ( (strcmp(argv[7],"-i") == 0) )
{
    if ((strcmp(argv[1],"-i") == 0) || (strcmp(argv[3],"-i") == 0) || (strcmp(argv[5],"-i") == 0))
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -d <nombre_módulo_atención> -p <puerto_sem_svr> -c <op> -i <identificación_vehiculo>\n", argv[0]);
        exit(1);
    }
    else 
    {
        if ( strcpy(placa,argv[8]) == "\0" )
        {
            fprintf(stderr, "ERROR, almacenando id del vehiculo\n");
        }
    }
}

/**************************Fin revision entrada*************************/

/* convertimos el hostname a su direccion IP */ 
/*if (host_name == NULL) {
perror("gethostbyname");
exit(1); }*/
//portnum = atoi(argv[4]);
/* Creamos el socket */
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
perror("socket");
exit(1); }
/* a donde mandar */
svr_addr.sin_family = AF_INET; /* usa host byte order */ 
svr_addr.sin_port = htons(portnum); /* usa network byte order */ 
svr_addr.sin_addr = *((struct in_addr *)host_name->h_addr); 
/*bcopy((char *)host_name->h_addr, 
    (char *)&svr_addr.sin_addr.s_addr,  
    host_name->h_length);*/
bzero(&(svr_addr.sin_zero), 8); /* pone en cero el resto */



/* enviamos info al servidor en el buffer*/
memset(buf,'\0', BUFFER_LEN);
sprintf(buf, "%s %s\n", oper, placa);
printf("MANDARE ESTO AL SERV: %s\n",buf);
if ((numbytes = sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&svr_addr, sizeof(struct sockaddr))) < 0) {
perror("sendto");
exit(1); }


memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

/* recibimos respuesta del servidor */
addr_len = sizeof(struct sockaddr);

if ((numbytes = recvfrom(sockfd, buf,1024, 0, (struct sockaddr *)&svr_addr,(socklen_t *)&addr_len)) == -1) { 
printf("%s\n", buf);
perror("recvfrom");
exit(1);}
printf("Respuesta del servidor:\n %s", buf);


/* cierro socket */ 

close(sockfd); 
exit (0);
}
