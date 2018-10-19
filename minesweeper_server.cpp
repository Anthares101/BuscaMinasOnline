#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "user.hpp"

#define MSG_SIZE 250
#define MAX_CLIENTS 30


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, User arrayClientes[]);



int main ( )
{

	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    User arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    //Para la lectura de comandos
    char * information;

    //Crea el fichero useList si no existe
    FILE * usersFile;
    usersFile = fopen("userList", "r");
    if(usersFile == NULL)
    	fopen("userList", "w");
    fclose(usersFile);
	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
			exit (1);
	}

    // Activaremos una propiedad del socket que permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permitir· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}


	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el
		tamaño de su estructura, el resto de información (familia, puerto,
		ip), nos la proporcionará el método que recibe las peticiones.
	----------------------------------------------------------------------*/
		from_len = sizeof (from);


		if(listen(sd,1) == -1){
			perror("Error en la operación de listen");
			exit(1);
		}

    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);


    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);

	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
		while(1){

            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)

            auxfds = readfds;

            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);

            if(salida > 0){


                for(i=0; i<FD_SETSIZE; i++){

                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {

                        if( i == sd){

                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes].setSocket_descriptor(new_sd);
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);

                                    printf("Nuevo usuario conectado %d/%d\n", numClientes, MAX_CLIENTS);
                                    strcpy(buffer, "+Ok. Usuario conectado\n");

                                    send(new_sd,buffer,strlen(buffer),0);
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                                    send(new_sd,buffer,strlen(buffer),0);
                                    close(new_sd);
                                }

                            }


                        }
                        else if (i == 0){
                            //Se ha introducido información de teclado
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);

                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){

                                for (j = 0; j < numClientes; j++){
                                    send(arrayClientes[j].getSocket_descriptor(), "-Err. Desconexion servidor\n", strlen("-Err. Desconexion servidor\n"),0);
                                    close(arrayClientes[j].getSocket_descriptor());
                                    FD_CLR(arrayClientes[j].getSocket_descriptor(),&readfds);
                                }
                                    close(sd);
                                    exit(-1);


                            }
                        //Mensajes que se quieran mandar a los clientes (implementar)
                        }
                        else{
                            bzero(buffer,sizeof(buffer));

                            recibidos = recv(i,buffer,sizeof(buffer),0);

                            if(recibidos > 0){

                                if(strcmp(buffer,"SALIR\n") == 0){

                                    salirCliente(i,&readfds,&numClientes,arrayClientes);

                                }
                                else{
                                	//Si el usuario no ha iniciado sesion aun se tendran en cuenta estos comandos
                                	if(arrayClientes[i].getState() == "not_registered"){
                                		//Comando USUARIO usuario
                                		if(strcmp(strtok(buffer, " \n"), "USUARIO") == 0 && (information = strtok(NULL, " \n")) != NULL && arrayClientes[i].getLogin() == ""){

                                			arrayClientes[i].setLogin(std::string (information));
                                			if(arrayClientes[i].checkUser("userList")){
                                				bzero(buffer,sizeof(buffer));
                                				sprintf(buffer, "+Ok. Introduzca mediante \"PASSWORD password\" su contraseña");
                                			}
                                			else{
                                				arrayClientes[i].setLogin("");
                                				bzero(buffer,sizeof(buffer));
                                				sprintf(buffer, "-Err. Usuario incorrecto, use \"REGISTRO –u usuario –p password\" para registrarse");
                                			}
                                		}
                                		//Comando PASSWORD password
                                		else if(strcmp(strtok(buffer, " \n"), "PASSWORD") == 0 && (information = strtok(NULL, " \n")) != NULL){
                                			if(arrayClientes[i].getLogin() == ""){
                                				bzero(buffer,sizeof(buffer));
                                				sprintf(buffer, "-Err. Primero indique su usuario con \"USUARIO usuario\"");
                                			}
                                			else{//TODO verifica
                                				arrayClientes[i].setPassword(std::string (information));
                                			}
                                		}
                                		else if(arrayClientes[i].getLogin() != "") {
                                			arrayClientes[i].setLogin("");
                                			bzero(buffer,sizeof(buffer));
                                			sprintf(buffer, "-Err. Error en la validacion");
                                		}
                                		else{
	                                		bzero(buffer,sizeof(buffer));
	                                		sprintf(buffer, "-Err. Inicie sesion con \"USUARIO usuario\" y despues \"PASSWORD password\"");
                                		}

                                		send(arrayClientes[i].getSocket_descriptor(),buffer,strlen(buffer),0);
                                	}
                                	else{//Si el usuario ha iniciado sesion se tendran en cuenta estos comandos
                                    //sprintf(identificador,"%d: %s",i,buffer);
                                    sprintf(identificador,"+Ok. Eres el usuario %d",i);
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,identificador);
                                    send(arrayClientes[i].getSocket_descriptor(),buffer,strlen(buffer),0);
                                    /*for(j=0; j<numClientes; j++)
                                        if(arrayClientes[j] != i)
                                            send(arrayClientes[j],buffer,strlen(buffer),0);*/
                                	}

                                }


                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos == 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;

}

void salirCliente(int socket, fd_set * readfds, int * numClientes, User arrayClientes[]){

    char buffer[250];
    int j;

    close(socket);
    FD_CLR(socket,readfds);

    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j].getSocket_descriptor() == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j].setSocket_descriptor(arrayClientes[j+1].getSocket_descriptor()));

    (*numClientes)--;

    bzero(buffer,sizeof(buffer));
    printf("Desconexión del cliente: %d\n", socket);
    sprintf(buffer,"+Ok. Usuario desconectado\n");

    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j].getSocket_descriptor() != socket)
            send(arrayClientes[j].getSocket_descriptor(),buffer,strlen(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);

    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
