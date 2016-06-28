/*
* PROYECTO SOCKETS REDES
* Neylin Belisario	09-10093
* Andres Hernandez 	10-10353
* Daniel Pelayo		10-10539
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
#include <pthread.h>

#define SERVER_PORT 20539 //Puerto a usar, esto habra q cambiarlo
#define BUFFER_LEN 1024 //longitud del buffer de lectura
typedef struct
{
  char placa[10];
  int id_ticket;
  time_t hora_entrada;
  time_t hora_salida;
}carro;

/*Estructura usada para los parametros
que le ingresan al hilo*/
typedef struct 
{
  int sockfd;
  char op[1];
  char id[20];
  char buf[BUFFER_LEN];
  FILE* entradas;
  FILE* salidas;
  struct sockaddr_in their_addr; 

}DATOSHILOS;


int CAP_EST = 200;//Capacidad del estacionamiento
carro tickets[200];
int k = 0;


void init(){
	int i;
	for ( i = 0; i < 200; ++i)
	{
		strcpy(tickets[i].placa,"$");
		tickets[i].id_ticket = i;
		//printf("Ticket: %d Placa: %s \n",tickets[i].id_ticket,tickets[i].placa);
	}
}

void imprimirTickets(){
	int i = 0;
	system("clear");
	printf("Carros dentro del Estacionamiento: \n");
	for ( i = 0; i < 200; ++i)
	{
		if (strcmp(tickets[i].placa,"$") != 0)
		{
			printf("Ticket: %d Placa: %s \n",tickets[i].id_ticket,tickets[i].placa);
		}
	}
}


double montoAPagar(time_t hora_entrada, time_t hora_salida){
	double monto = 0;
	double horas = (difftime(hora_salida, hora_entrada))/3600;
	horas = ceil(horas);
	printf("HORAS: %f\n", horas);
	if (horas == 1)
	{
		monto = 80;
	}
	else{
		horas = horas -1;
		monto = 80 + 30*horas;
	}
	return(monto);
}

void *solicitudClienteHilos(void* estructura){
	DATOSHILOS datos = *((DATOSHILOS*) estructura);
	int sockfd = datos.sockfd;
	char op[1];
	strcpy(op,datos.op);
	char id[20];
	strcpy(id, datos.id);
	char buf[BUFFER_LEN];
	strcpy(buf, datos.buf);
	FILE* entradas = datos.entradas;
	FILE* salidas = datos.salidas;
	struct sockaddr_in their_addr = datos.their_addr; 

	int numbytes;
//=============================
	char idTicket[3];
	time_t entrada;
	time_t salida;
	struct tm *tlocalentrada;
	struct tm *tlocalsalida;
	char FechaHoraEntrada[128];
	char FechaHoraSalida[128];
	char tarifa[20];
	int i = 0;
	int pos = 0;


	strcpy(op, strtok(buf, " "));//op contiene el caracter que nos dice 
	strcpy(id, strtok(NULL, "\n"));//id contiene el id del vehiculo q entrara o saldra
	memset(buf,'\0', BUFFER_LEN);
	if (strcmp(op,"e") == 0)
	{
		//Verificamos si hay puestos
		if (CAP_EST > 0)
		{  		  
		  //Correspondencia Ticket-Vehiculo
		  i = 0;
		  for (i = 0; i < 200; ++i)
		  {
		  	if (strcmp(tickets[i].placa,"$") == 0)
		  	{
		  		time(&entrada);
		  		tlocalentrada = localtime(&entrada);	 
		  		//FechaHora contiene String con la fecha y hora actual local 
		  		strftime(FechaHoraEntrada,128,"Fecha Entrada: %d/%m/%y | Hora Entrada: %H:%M:%S",tlocalentrada);
		  		strcpy(tickets[i].placa, id);
				  sprintf(idTicket, "%d", tickets[i].id_ticket);
				  tickets[i].hora_entrada = entrada;
				  //Se carga mensaje a buffer que es lo que se imprimira en el log:
				  strcat(buf, "==================================================================\n");
				  strcat(buf,"Id Ticket: ");
					strcat(buf,idTicket);// Id del ticket
					strcat(buf," | ");
					strcat(buf, "Id Vehiculo: ");
					strcat(buf, id);
					strcat(buf,"\n");
					strcat(buf,FechaHoraEntrada);// Si el vehiculo entrara o saldra
					//fin del buffer	
					fprintf(entradas, "%s\n", buf);
					fclose(entradas);
					imprimirTickets();//imprime el estado actual del estacionamiento en la pantalla del servidor
				  CAP_EST--;

				  //memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
				  strcat(buf,"\nPUEDE PASAR, BIENVENIDOS\n");

				  break;
		  	}
		  	else if(strcmp(tickets[i].placa,id) == 0){
		  		strcat(buf,"EL VEHICULO QUE INTENTA INGRESAR YA ESTA DENTRO DEL ESTACIONAMIENTO\n");
		  		break;
		  	}
		  }
		}
		else{

		  memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
		  strcat(buf,"NO PUEDE PASAR. ESTACIONAMIENTO LLENO\n");
			imprimirTickets();

		}
	}
	else{

		  i = 0;
		  for (i = 0; i < 200; i++)
		  {
		  	if (strcmp(tickets[i].placa,id) == 0)
		  	{
		  		salida = time(0);
		  		tlocalsalida = localtime(&salida);	 
		  		//FechaHora contiene String con la fecha y hora actual local 
		  		tickets[i].hora_salida = salida;
		  		strftime(FechaHoraSalida,128,"Fecha Salida: %d/%m/%y | Hora Salida: %H:%M:%S",tlocalsalida);

					//Obtencion de datos del ticket:
					tlocalentrada = localtime(&tickets[i].hora_entrada);
		  		strftime(FechaHoraEntrada,128,"Fecha Entrada: %d/%m/%y | Hora Entrada: %H:%M:%S",tlocalentrada);
		  		//Id Ticket
		  		sprintf(idTicket, "%d", tickets[i].id_ticket);
		  		//Calculo del monto a pagar:
				  sprintf(tarifa, "%f", montoAPagar(tickets[i].hora_entrada,tickets[i].hora_salida));

					strcat(buf, "==================================================================\n");
				  strcat(buf,"Id Ticket: ");
					strcat(buf,idTicket);// Id del ticket
					strcat(buf," | ");
					strcat(buf, "Id Vehiculo: ");
					strcat(buf, id);
					strcat(buf,"\n");
					strcat(buf,FechaHoraEntrada);// Si el vehiculo entrara o saldra
					strcat(buf,"\n");
					strcat(buf,FechaHoraSalida);// Si el vehiculo entrara o saldra
					strcat(buf,"\n");
					strcat(buf,"Monto a Pagar: ");//separador
					strcat(buf,tarifa);// Id del vehiculo

					fprintf(salidas, "%s\n", buf);
					fclose(salidas);

					tickets[i].hora_entrada = 0;
					tickets[i].hora_salida = 0;
					strcpy(tickets[i].placa,"$");
					imprimirTickets();
					if (CAP_EST < 200)
					{
						CAP_EST++;
					}

				  //memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
				  strcat(buf,"\nHASTA LUEGO. CONDUZCA CON CUIDADO\n");
				  break;
		  	}

		  }
		  if (i == 200)//el i llego a 200 sin encontrar a alguien con esta placa
		  	{
		  		strcat(buf,"EL VEHICULO NO SE ENCUENTRA DENTRO DEL ESTACIONAMIENTO\n");
		  	}	




	}

	k--;
	if ((numbytes=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
	perror("sendto");
	exit(2); }



}

/*char* solicitudCliente(char op[1], char id[20], char buf[BUFFER_LEN], int sockfd, FILE* entradas, FILE* salidas){
	char idTicket[3];
	time_t entrada;
	time_t salida;
	struct tm *tlocalentrada;
	struct tm *tlocalsalida;
	char FechaHoraEntrada[128];
	char FechaHoraSalida[128];
	char tarifa[20];
	int i = 0;
	int pos = 0;
	if (strcmp(op,"e") == 0)
	{
		//Verificamos si hay puestos
		if (CAP_EST > 0)
		{  		  
		  //Correspondencia Ticket-Vehiculo
		  i = 0;
		  for (i = 0; i < 200; ++i)
		  {
		  	if (strcmp(tickets[i].placa,"$") == 0)
		  	{
		  		time(&entrada);
		  		tlocalentrada = localtime(&entrada);	 
		  		//FechaHora contiene String con la fecha y hora actual local 
		  		strftime(FechaHoraEntrada,128,"Fecha Entrada: %d/%m/%y | Hora Entrada: %H:%M:%S",tlocalentrada);
		  		strcpy(tickets[i].placa, id);
				  sprintf(idTicket, "%d", tickets[i].id_ticket);
				  tickets[i].hora_entrada = entrada;
				  //Se carga mensaje a buffer que es lo que se imprimira en el log:
				  strcat(buf, "==================================================================\n");
				  strcat(buf,"Id Ticket: ");
					strcat(buf,idTicket);// Id del ticket
					strcat(buf," | ");
					strcat(buf, "Id Vehiculo: ");
					strcat(buf, id);
					strcat(buf,"\n");
					strcat(buf,FechaHoraEntrada);// Si el vehiculo entrara o saldra
					//fin del buffer	
					fprintf(entradas, "%s\n", buf);
					fclose(entradas);
					imprimirTickets();//imprime el estado actual del estacionamiento en la pantalla del servidor
				  CAP_EST--;

				  //memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
				  strcat(buf,"\nPUEDE PASAR, BIENVENIDOS\n");

				  break;
		  	}
		  	else if(strcmp(tickets[i].placa,id) == 0){
		  		strcat(buf,"EL VEHICULO QUE INTENTA INGRESAR YA ESTA DENTRO DEL ESTACIONAMIENTO\n");
		  		break;
		  	}
		  }
		}
		else{

		  memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
		  strcat(buf,"NO PUEDE PASAR. ESTACIONAMIENTO LLENO\n");
			imprimirTickets();

		}
	}
	else{

		  i = 0;
		  for (i = 0; i < 200; i++)
		  {
		  	if (strcmp(tickets[i].placa,id) == 0)
		  	{
		  		salida = time(0);
		  		tlocalsalida = localtime(&salida);	 
		  		//FechaHora contiene String con la fecha y hora actual local 
		  		tickets[i].hora_salida = salida;
		  		strftime(FechaHoraSalida,128,"Fecha Salida: %d/%m/%y | Hora Salida: %H:%M:%S",tlocalsalida);

					//Obtencion de datos del ticket:
					tlocalentrada = localtime(&tickets[i].hora_entrada);
		  		strftime(FechaHoraEntrada,128,"Fecha Entrada: %d/%m/%y | Hora Entrada: %H:%M:%S",tlocalentrada);
		  		//Id Ticket
		  		sprintf(idTicket, "%d", tickets[i].id_ticket);
		  		//Calculo del monto a pagar:
				  sprintf(tarifa, "%f", montoAPagar(tickets[i].hora_entrada,tickets[i].hora_salida));

					strcat(buf, "==================================================================\n");
				  strcat(buf,"Id Ticket: ");
					strcat(buf,idTicket);// Id del ticket
					strcat(buf," | ");
					strcat(buf, "Id Vehiculo: ");
					strcat(buf, id);
					strcat(buf,"\n");
					strcat(buf,FechaHoraEntrada);// Si el vehiculo entrara o saldra
					strcat(buf,"\n");
					strcat(buf,FechaHoraSalida);// Si el vehiculo entrara o saldra
					strcat(buf,"\n");
					strcat(buf,"Monto a Pagar: ");//separador
					strcat(buf,tarifa);// Id del vehiculo

					fprintf(salidas, "%s\n", buf);
					fclose(salidas);

					tickets[i].hora_entrada = 0;
					tickets[i].hora_salida = 0;
					strcpy(tickets[i].placa,"$");
					imprimirTickets();
					if (CAP_EST < 200)
					{
						CAP_EST++;
					}

				  //memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
				  strcat(buf,"\nHASTA LUEGO. CONDUZCA CON CUIDADO\n");
				  break;
		  	}

		  }
		  if (i == 200)//el i llego a 200 sin encontrar a alguien con esta placa
		  	{
		  		strcat(buf,"EL VEHICULO NO SE ENCUENTRA DENTRO DEL ESTACIONAMIENTO\n");
		  	
		  	}	




	}

	return buf;
}*/

int main(int argc, char *argv[]) {

int sockfd; /* descriptor para el socket */
struct sockaddr_in my_addr; /* direccion IP y numero de puerto local */
struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
int addr_len, numbytes;/* addr_len contendra el tamano de la estructura sockadd_in y numbytes el * numero de bytes recibidos */
char buf[BUFFER_LEN]; /* Buffer de recepción */
char op[1];
char id[20];
char buf_copy[BUFFER_LEN]; /* Copia Buffer de recepción que se modificara para leer la respuesta*/
FILE *archivoEntradas;
FILE *archivoSalidas;
char capEst[30];

int portnum;
char bitacora_entrada[20], bitacora_salida[20];

/*********************Verificacion de entrada*****************************/

//Verificacion de numero de argumentos
if (argc != 7) {
	printf("Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
	exit(1); 
}

//Verificacion de Banderas en cualquier orden
else if ( ((strcmp(argv[1],"-l") != 0) || (strcmp(argv[3],"-i") != 0) || (strcmp(argv[5],"-o") != 0)) &&

		  ((strcmp(argv[1],"-l") != 0) || (strcmp(argv[5],"-i") != 0) || (strcmp(argv[3],"-o") != 0)) &&
		  ((strcmp(argv[3],"-l") != 0) || (strcmp(argv[5],"-i") != 0) || (strcmp(argv[1],"-o") != 0)) && 
		  ((strcmp(argv[3],"-l") != 0) || (strcmp(argv[1],"-i") != 0) || (strcmp(argv[5],"-o") != 0)) && 
		  ((strcmp(argv[5],"-l") != 0) || (strcmp(argv[1],"-i") != 0) || (strcmp(argv[3],"-o") != 0)) && 
		  ((strcmp(argv[5],"-l") != 0) || (strcmp(argv[3],"-i") != 0) || (strcmp(argv[1],"-o") != 0)) ){
	fprintf(stderr,"Alguna bandera esta incorrecta\n");
	fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
	exit(1); 
}

// Verificacion del flag -l
if( strcmp(argv[1],"-l") == 0 )
{
	if ((strcmp(argv[3],"-l") == 0) || (strcmp(argv[5],"-l") == 0) )
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
        exit(1);
    }
	else 
	{
		//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
		if ( (atoi(argv[2]) != 20539) && (atoi(argv[2]) != 20353) && (atoi(argv[2]) != 20093) )
		{
			fprintf(stderr,"ERROR, numero de puerto no valido\n");
			exit(1);
		}
		//Verificacion del Puerto, (Num de cifras del puerto)
		else if ( (strlen(argv[2]) != 5) )
		{
			fprintf(stderr,"ERROR, el puerto no es compatible\n");
			exit(1);
		}
		else if ((portnum = atoi(argv[2])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
	}
}
else if( strcmp(argv[3],"-l") == 0 )
{
	if ((strcmp(argv[1],"-l") == 0) || (strcmp(argv[5],"-l") == 0) )
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
        exit(1);
    }
	else 
	{
		//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
		if ( (atoi(argv[4]) != 20539) && (atoi(argv[4]) != 20353) && (atoi(argv[4]) != 20093) )
		{
			fprintf(stderr,"ERROR, numero de puerto no valido\n");
			exit(1);
		}
		//Verificacion del Puerto, (Num de cifras del puerto)
		else if ( (strlen(argv[4]) != 5) )
		{
			fprintf(stderr,"ERROR, el puerto no es compatible\n");
			exit(1);
		}
		else if ((portnum = atoi(argv[4])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
	}
}
else if( strcmp(argv[5],"-l") == 0 )
{
	if ((strcmp(argv[1],"-l") == 0) || (strcmp(argv[3],"-l") == 0) )
    {
        fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
        fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
        exit(1);
    }
	else 
	{
		//Verificacion del Puerto, (Numeros de puertos validos: 20539, 20353, 20093)
		if ( (atoi(argv[6]) != 20539) && (atoi(argv[6]) != 20353) && (atoi(argv[6]) != 20093) )
		{
			fprintf(stderr,"ERROR, numero de puerto no valido\n");
			exit(1);
		}
		//Verificacion del Puerto, (Num de cifras del puerto)
		else if ( (strlen(argv[6]) != 5) )
		{
			fprintf(stderr,"ERROR, el puerto no es compatible\n");
			exit(1);
		}
		else if ((portnum = atoi(argv[6])) == 0) {
            fprintf(stderr,"ERROR, almacenando el puerto\n");
            exit(1);
        }
	}
}

// Verificacion del flag -i
if( strcmp(argv[1],"-i") == 0 )
{
	if ( strcmp(argv[3],"-i") == 0  || strcmp(argv[5],"-i") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_entrada,argv[2]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de entrada\n");
    }
}
else if( strcmp(argv[3],"-i") == 0 )
{
	if ( strcmp(argv[1],"-i") == 0  || strcmp(argv[5],"-i") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_entrada,argv[4]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de entrada\n");
    }
}
else if( strcmp(argv[5],"-i") == 0 )
{
	if ( strcmp(argv[3],"-i") == 0  || strcmp(argv[1],"-i") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_entrada,argv[6]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de entrada\n");
    }
}

// Verificacion del flag -o
if( strcmp(argv[1],"-o") == 0 )
{
	if ( strcmp(argv[3],"-o") == 0  || strcmp(argv[5],"-o") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_salida,argv[2]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de salida\n");
    }
}
else if( strcmp(argv[3],"-o") == 0 )
{
	if ( strcmp(argv[1],"-o") == 0  || strcmp(argv[5],"-o") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_salida,argv[4]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de salida\n");
    }
}
else if( strcmp(argv[5],"-o") == 0 )
{
	if ( strcmp(argv[3],"-o") == 0  || strcmp(argv[1],"-o") == 0 )
	{
		fprintf(stderr,"ERROR, Uso incorrecto de las banderas\n");
		fprintf(stderr,"Uso: %s -l <puerto_sem_svr> -i <bitacora_entrada> -o <bitacora_salida>\n", argv[0]);
		exit(1);
	}
	else if ( strcpy(bitacora_salida,argv[6]) ==  "\0" )
    {
        fprintf(stderr, "ERROR, almacenando la bitacora de salida\n");
    }
}

/**************************Fin revision entrada*************************/

//Inicializacion de arreglo de ids de tickets de vehiculos
int i =0;
init();



/////////////////////////////////////////////////////////
/* se crea el socket */
if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
perror("socket");
exit(1); }
/* Se establece la estructura my_addr para luego llamar a bind() */ 
my_addr.sin_family = AF_INET; /* usa host byte order */
my_addr.sin_port = htons(portnum); /* usa network byte order */ 
my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */ 
bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */
/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 
printf("Asignado direccion al socket ....\n");
imprimirTickets();
if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0) {
perror("bind");
exit(2); }
///////////////////////////////////////////////////////////////
pthread_t thread_id;//Arreglo de hilos

//listen(sockfd,10);
//Servidor se queda activo esperando que alguien se comunique con el
DATOSHILOS argumentos;
while(1){
		/* Se reciben los datos (directamente, UDP no necesita conexión) */
	addr_len = sizeof(struct sockaddr);
  sprintf(capEst, "Puestos Disponibles: %d", CAP_EST);
  printf("%s",capEst);
	memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

	//Recibiendo del cliente:
	if ((numbytes=recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr,
	(socklen_t *)&addr_len)) == -1) { perror("recvfrom");
	exit(3); }
	/* Se visualiza lo recibido s*/

	printf("paquete proveniente de : %s\n",inet_ntoa(their_addr.sin_addr)); 
	printf("longitud del paquete en bytes: %d\n",numbytes);
	printf("el paquete contiene: %s\n", buf);
	strcpy(buf_copy, buf);
	strcpy(op, strtok(buf_copy, " "));//op contiene el caracter que nos dice 
	strcpy(id, strtok(NULL, "\n"));//id contiene el id del vehiculo q entrara o saldra


	/* Se procesa la peticion del cliente */
	//memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie
	archivoEntradas = fopen(bitacora_entrada,"a");
	archivoSalidas = fopen(bitacora_salida,"a");

  argumentos.sockfd = sockfd;
  strcpy(argumentos.buf,buf);
  argumentos.entradas = archivoEntradas;
  argumentos.salidas = archivoSalidas;
  argumentos.their_addr = their_addr;
  if (k < 3)
  {
		if (pthread_create(&thread_id,NULL,solicitudClienteHilos,(void *)&argumentos) < 0) {
			perror("Fallo el enlace");
			return 1;
		}
		pthread_detach(thread_id); 
		k++;
  }
  else{
  	strcat(buf,"alcanzado limite de procesos concurrentos");
		if ((numbytes=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
		perror("sendto");
		exit(2); }
	  }


}


/* cerramos descriptor del socket */ 
//imprimirTickets();
close(sockfd);
exit (0); }
