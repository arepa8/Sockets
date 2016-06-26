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
#include <math.h>


typedef struct
{
  char placa[10];
  int id_ticket;
  time_t hora_entrada;
  time_t hora_salida;
}carro;

/*Estructura usada para los parametros
que le ingresan al hilo*/
/*typedef struct 
{
  int sockfd;
  char op[1];
  char id[20];
  char buf[BUFFER_LEN];
  struct carro *Tick;

}DATOSHILOS;*/

#define SERVER_PORT 20539 //Puerto a usar, esto habra q cambiarlo
#define BUFFER_LEN 1024 //longitud del buffer de lectura
int CAP_EST = 10;//Capacidad del estacionamiento
carro tickets[200];


void init(){
	int i;
	for ( i = 0; i < 200; ++i)
	{
		strcpy(tickets[i].placa,"$");
		tickets[i].id_ticket = i;
		printf("Tiquet: %d Placa: %s \n",tickets[i].id_ticket,tickets[i].placa);
	}
}

void imprimirTickets(){
	int i = 0;
	for ( i = 0; i < 200; ++i)
	{
		printf("Tiquet: %d Placa: %s \n",tickets[i].id_ticket,tickets[i].placa);
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

char* solicitudCliente(char op[1], char id[20], char buf[BUFFER_LEN], int sockfd){
	char idTicket[3];
	char capEst[30];
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
		  		strftime(FechaHoraEntrada,128,"%d/%m/%y %H:%M:%S",tlocalentrada);
		  		strcpy(tickets[i].placa, id);
				  sprintf(idTicket, "%d", tickets[i].id_ticket);
				  tickets[i].hora_entrada = entrada;
				  pos = i;
				  break;
		  	}
		  }
	  	imprimirTickets();
	  	CAP_EST--;

		  
		  sprintf(capEst, "Puestos Disponibles: %d", CAP_EST);

		  //Se carga mensaje a buffer:
			strcat(buf,FechaHoraEntrada);// Si el vehiculo entrara o saldra
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
		if (CAP_EST < 10)
		{
			CAP_EST++;
		}
		  i = 0;
		  for (i = 0; i < 200; i++)
		  {
		  	if (strcmp(tickets[i].placa,id) == 0)
		  	{
		  		salida = time(0);
		  		tlocalsalida = localtime(&salida);	 
		  		//FechaHora contiene String con la fecha y hora actual local 
		  		tickets[i].hora_salida = salida;
		  		strftime(FechaHoraSalida,128,"%d/%m/%y %H:%M:%S",tlocalsalida);
				  sprintf(idTicket, "%d", tickets[i].id_ticket);
				  sprintf(tarifa, "%f", montoAPagar(tickets[i].hora_entrada,tickets[i].hora_salida));
				  pos = i;


				  break;
		  	}
		  }

		//Obtencion de datos del ticket:
		tlocalentrada = localtime(&tickets[pos].hora_entrada);
		strftime(FechaHoraEntrada,128,"%d/%m/%y %H:%M:%S",tlocalentrada);


		
		strcat(buf,"Hora de Entrada: ");
		strcat(buf,FechaHoraEntrada);// Hora Entrada
		strcat(buf," ");//separador
		strcat(buf,"Hora de Salida: ");
		strcat(buf,FechaHoraSalida);// Hora Salida
		strcat(buf,"\n");//separador
		strcat(buf,"ID ticket: ");//separador
		strcat(buf,idTicket);// Id del vehiculo
		strcat(buf, "\n");
		strcat(buf,"Monto a Pagar: ");//separador
		strcat(buf,tarifa);// Id del vehiculo
		strcat(buf, "\n");
		sprintf(capEst, "Puestos Disponibles: %d", CAP_EST);
		strcat(buf, capEst);
		strcat(buf," ");
		strcat(buf,"Hasta luego \n");
		strcat(buf,"\n");//fin del buffer	

		
		tickets[pos].hora_entrada = 0;
		tickets[pos].hora_salida = 0;
		strcpy(tickets[pos].placa,"$");





		imprimirTickets();

	}

	return buf;
}

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
my_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */ 
my_addr.sin_addr.s_addr = INADDR_ANY; /* escuchamos en todas las IPs */ 
bzero(&(my_addr.sin_zero), 8); /* rellena con ceros el resto de la estructura */
/* Se le da un nombre al socket (se lo asocia al puerto e IPs) */ 
printf("Asignado direccion al socket ....\n");
if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0) {
perror("bind");
exit(2); }
///////////////////////////////////////////////////////////////

//listen(sockfd,10);
//Servidor se queda activo esperando que alguien se comunique con el
while(1){
		/* Se reciben los datos (directamente, UDP no necesita conexión) */
	addr_len = sizeof(struct sockaddr);
	printf("Esperando datos ....\n");
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
	memset(buf,'\0', BUFFER_LEN);//Se borra el buffer por si acaso habia algo antes q' fastidie

	strcpy(buf, solicitudCliente(op, id, buf, sockfd));
	printf("%s\n",buf);
	//Envio de respuesta
	if ((numbytes=sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
	perror("sendto");
	exit(2); }

}


/* cerramos descriptor del socket */ 
//imprimirTickets();
close(sockfd);
exit (0); }
