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
#include "minesweeper_board.hpp"
#include "macros.hpp"

#define MSG_SIZE 250
#define MAX_CLIENTS 30
#define MAX_PARTIDAS 10


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, User arrayClientes[]);
int idPartida(minesweeper_board arrayTableros[], int playersd);
std::string clearString(const std::string & str);


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
    minesweeper_board arrayTableros[MAX_PARTIDAS];
    int numClientes = 0;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    //Para la lectura de comandos
    std::string strBuffer;

    //Numero de cliente
    int client;

    //Numero de partida
    int partida = 0;

    //Crea el fichero useList si no existe
    FILE * usersFile;
    usersFile = fopen("userList", "a");
    fclose(usersFile);
	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
        std::cout << BRED;
		perror("- No se puede abrir el socket cliente\n");
        std::cout << RESET;

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
        std::cout << BRED;
		perror("- Error en la operación bind");
        std::cout << RESET;
		exit(1);
	}


	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el
		tamaño de su estructura, el resto de información (familia, puerto,
		ip), nos la proporcionará el método que recibe las peticiones.
	----------------------------------------------------------------------*/
		from_len = sizeof (from);


		if(listen(sd,1) == -1){
            std::cout << BRED;
			perror("- Error en la operación de listen");
            std::cout << RESET;
			exit(1);
		}

    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);


    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);

    std::cout << CLEAR_SCREEN;
    PLACE(1,25);
    std::cout << BBLUE << "MINESWEEPER SERVER [" << BGREEN << "ONLINE" << BBLUE << "]" << RESET << std::endl << std::endl;
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
                    	//std::cout<<i<<std::endl;
                        if( i == sd){

                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                std::cout << BRED;
                                perror("- Error aceptando peticiones");
                                std::cout << RESET;
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes].setSocket_descriptor(new_sd);
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);

                                    std::cout << BCYAN << "+ Nuevo usuario conectado " << BYELLOW << "[" << numClientes << "/" << MAX_CLIENTS << "]" << RESET << std::endl;
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
                                    std::cout << CLEAR_SCREEN;
                                    PLACE(1,25);
                                    std::cout << BBLUE << "MINESWEEPER SERVER [" << BRED << "OFFLINE" << BBLUE << "]" << RESET << std::endl << std::endl;

                                    exit(-1);

                            }
                        //Mensajes que se quieran mandar a los clientes (implementar)
                        }
                        else{
                        	client = 0;
                        	while(client < numClientes && arrayClientes[client].getSocket_descriptor() != i)
                        		client++;
                            bzero(buffer,sizeof(buffer));

                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            strBuffer = buffer;

                            if(recibidos > 0){

                                if(strcmp(buffer,"SALIR\n") == 0)
                                    salirCliente(i,&readfds,&numClientes,arrayClientes);


                                //Si el usuario no ha iniciado sesion aun se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "not_registered"){
	                        		//Comando USUARIO usuario
	                        		if(strBuffer.substr(0, 8) == "USUARIO " && !(strBuffer = strBuffer.substr(8)).empty()){
	                        			strBuffer = clearString(strBuffer);

	                        			arrayClientes[client].setLogin(strBuffer);
	                        			if(arrayClientes[client].getLogin() != "" && arrayClientes[client].checkUser("userList")){
	                        				bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "+Ok. Introduzca mediante \"PASSWORD password\" su contraseña");
	                        			}
	                        			else{
	                        				arrayClientes[client].setLogin("");
	                        				bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "-Err. Usuario incorrecto, use \"REGISTRO –u usuario –p password\" para registrarse");
	                        			}
	                        		}
	                        		//Comando PASSWORD password
	                        		else if(strBuffer.substr(0, 9) == "PASSWORD " && !(strBuffer = strBuffer.substr(9)).empty()){
	                        			strBuffer = clearString(strBuffer);

	                        			if(arrayClientes[client].getLogin() == ""){
	                        				bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "-Err. Primero indique su usuario con \"USUARIO usuario\"");
	                        			}
	                        			else{
	                        				arrayClientes[client].setPassword(strBuffer);
	                        				if(arrayClientes[client].getPassword() != "" && arrayClientes[client].verifyUser("userList")){
	                        					arrayClientes[client].setState("registered");
	                        					bzero(buffer,sizeof(buffer));
	                        					sprintf(buffer, "+Ok. Usuario validado");
	                        				}
	                        				else{
	                        					arrayClientes[client].setLogin("");
	                        					arrayClientes[client].setPassword("");
	                        					bzero(buffer,sizeof(buffer));
	                        					sprintf(buffer, "-Err. Error en la validacion");
	                        				}
	                        			}
	                        		}
	                        		else if(arrayClientes[client].getLogin() != "") {//Se espera PASSWORD
	                        			arrayClientes[client].setLogin("");
	                        			bzero(buffer,sizeof(buffer));
	                        			sprintf(buffer, "-Err. Error en la validacion");
	                        		}
                                    //Comando REGISTRO -u login -p password (o viceversa)
	                        		else if(strBuffer.substr(0, 9) == "REGISTRO " && !(strBuffer = strBuffer.substr(9)).empty()){
	                        			int pos;
	                        			std::string userLogin = "";
	                        			std::string password = "";

	                        			if(strBuffer.substr(0, 3) == "-u " && !(strBuffer = strBuffer.substr(3)).empty()){
	                        				userLogin = clearString(strBuffer);

	                        				if((pos = strBuffer.find(" -p ")) !=  std::string::npos){
	                        					password = strBuffer.substr(pos+4);
	                        					password = clearString(password);
	                        				}
	                        			}

	                        			else if(strBuffer.substr(0, 3) == "-p " && !(strBuffer = strBuffer.substr(3)).empty()){
	                        				password = clearString(strBuffer);

	                        				if((pos = strBuffer.find(" -u ")) !=  std::string::npos){
	                        					userLogin = strBuffer.substr(pos+4);
	                        					userLogin = clearString(userLogin);
	                        				}
	                        			}

	                        			if(!userLogin.empty() && !password.empty()){
	                        				arrayClientes[client].setLogin(userLogin);
	                        				arrayClientes[client].setPassword(password);
	                        				if(arrayClientes[client].registerUser("userList") == 0){

	                        					arrayClientes[client].setState("registered");
	                        					sprintf(buffer, "+Ok. Usuario registrado, sesion iniciada");

	                        				}
	                        				else{
	                        					arrayClientes[client].setLogin("");
	                        					arrayClientes[client].setPassword("");
	                        					sprintf(buffer, "-Err. Ese nombre de usuario ya existe");
	                        				}
	                        			}
                                        else{
	                        				bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "-Err. Registrese mediante \"REGISTRO -u usuario -p password\"");
	                        			}
	                        		}
	                        		else{
	                            		bzero(buffer,sizeof(buffer));
	                            		sprintf(buffer, "-Err. Inicie sesion con \"USUARIO usuario\" y despues \"PASSWORD password\" o registrese con \"REGISTRO -u usuario -p password\"");
	                        		}

                                    send(arrayClientes[client].getSocket_descriptor(),buffer,strlen(buffer),0);
                                }
                                //Si el usuario ha iniciado sesion se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "registered") {

                                    if(strBuffer.substr(0, 15) == "INICIAR-PARTIDA") {

                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"+Ok. Buscando partida...\n");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,strlen(buffer),0);

                                        strBuffer = clearString(strBuffer);

                                        arrayClientes[client].setState("in_queue");

                                        for(int z = 0; z < MAX_CLIENTS; z++) {

                                            if(z != client && arrayClientes[z].getState() == "in_queue" && partida <= MAX_PARTIDAS) {

                                                arrayClientes[z].setState("in_game");
                                                arrayClientes[client].setState("in_game");

                                                std::cout << BCYAN << "+ Partida iniciada: " << BYELLOW << arrayClientes[client].getLogin() << BRED << " VS " << BYELLOW << arrayClientes[z].getLogin() << RESET << std::endl;

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Empieza la partida contra el jugador %s", arrayClientes[client].getLogin().c_str());

                                                send(arrayClientes[z].getSocket_descriptor(),buffer,strlen(buffer),0);

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Empieza la partida contra el jugador %s", arrayClientes[z].getLogin().c_str());

                                                send(arrayClientes[client].getSocket_descriptor(),buffer,strlen(buffer),0);

                                                //se les asigna un objeto tablero dentro del arrayTableros
                                                arrayTableros[partida].set_player1(arrayClientes[client].getSocket_descriptor());
                                                arrayTableros[partida].set_player2(arrayClientes[z].getSocket_descriptor());

                                                partida++;
                                            }
                                        }
                                    }
                                    else{
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. Busque contrincante con el comando \"INICIAR-PARTIDA\"");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,strlen(buffer),0);
                                    }
                            	}
                                //Si el usuario esta in_game se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "in_game") {
                                    std::string Z, x; //letra y numero del tablero
                                    int match;

                                    match = idPartida(arrayTableros, arrayClientes[client].getSocket_descriptor());

                                    if(strBuffer.substr(0, 10) == "DESCUBRIR " && !(strBuffer = strBuffer.substr(10)).empty()) {

                                        Z = strBuffer.substr(0, 1);
                                        x = strBuffer.substr(2);
                                        x = clearString(x);

                                        //controlar que la letra sea entre A y J y el numero entre 0 y 9, si se salen del rango, enviar un mensaje de error y haber estudiao

                                        //la casilla no tenia mina
                                        /*if(arrayTableros[match].revealBox(x, Z)) {

                                            bzero(buffer,sizeof(buffer));
                                            buffer = arrayTableros[match].board2string();
                                            send(arrayTableros[match].get_player1(),buffer,strlen(buffer),0);
                                            send(arrayTableros[match].get_player2(),buffer,strlen(buffer),0);
                                        }
                                        //la casilla tenia mina
                                        else {

                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer, "+Ok. Jugador %s ha perdido la partida\n", arrayClientes[client].getLogin().c_str());
                                            send(arrayTableros[match].get_player1(),buffer,strlen(buffer),0);
                                            send(arrayTableros[match].get_player2(),buffer,strlen(buffer),0);

                                            bzero(buffer,sizeof(buffer));
                                            buffer = arrayTableros[match].board2string();
                                            send(arrayTableros[match].get_player1(),buffer,strlen(buffer),0);
                                            send(arrayTableros[match].get_player2(),buffer,strlen(buffer),0);

                                            arrayClientes[arrayTableros[match].get_player1()].setState("registered");
                                            arrayClientes[arrayTableros[match].get_player2()].setState("registered");

                                            std::cout << BCYAN << "+ Partida terminada: " << BYELLOW << arrayClientes[arrayTableros[match].get_player1].getLogin() << " " << BRED << " VS " << BYELLOW << arrayClientes[arrayTableros[match].get_player2].getLogin() << RESET << std::endl;

                                            //resetear el tablero de esta partida
                                            borrarPartida(match, &partidas, arrayTableros);
                                        }*/
                                    }
                                }
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos == 0)
                            {
                                std::cout << BCYAN << "+ El socket " << GREEN << arrayClientes[client].getSocket_descriptor() << BCYAN;
                                std::cout << ", ha introducido " << BPURPLE << "ctrl+c" << RESET << std::endl;
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }///////////////////////////////////////////////////////////////////
                    }
                }
            }
		}

	close(sd);

    PLACE(1,25);
    std::cout << std::endl << BBLUE << "MINESWEEPER SERVER [" << BRED << "OFFLINE" << BBLUE << "]" << RESET << std::endl << std::endl;

	return 0;

}

void salirCliente(int socket, fd_set * readfds, int * numClientes, User arrayClientes[]){

    char buffer[MSG_SIZE];
    int j;
    std::string cliente;

    close(socket);
    FD_CLR(socket,readfds);

    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++){
        if (arrayClientes[j].getSocket_descriptor() == socket)
            break;
    }

    cliente = arrayClientes[j].getLogin();

    for (; j < (*numClientes) - 1; j++)
        arrayClientes[j] = arrayClientes[j+1];

    (*numClientes)--;

    //Borra el slot de usuario que queda el ultimo
    arrayClientes[*numClientes] = User();

    bzero(buffer,sizeof(buffer));
    if(!cliente.empty())
        std::cout << BCYAN << "+ Desconexion del cliente: " << GREEN << cliente << " " << BYELLOW << "[" << *numClientes << "/" << MAX_CLIENTS << "]" << RESET << std::endl;
    else
        std::cout << BCYAN << "+ Desconexion del cliente con socket: " << GREEN << socket << " " << BYELLOW << "[" << *numClientes << "/" << MAX_CLIENTS << "]" << RESET << std::endl;
    sprintf(buffer,"+Ok. Usuario desconectado\n");

    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j].getSocket_descriptor() != socket)
            send(arrayClientes[j].getSocket_descriptor(),buffer,strlen(buffer),0);


}

int idPartida(minesweeper_board arrayTableros[], int playersd) {
    int match;

    for(int z = 0; z < MAX_PARTIDAS; z++) {

        if(arrayTableros[z].get_player1() == playersd || arrayTableros[z].get_player2() == playersd) {
            match = z;
            break;
        }
    }

    return match;
}

void borrarPartida(int match, int * partidas, minesweeper_board arrayTableros[]) {

    for(int z = match; z < MAX_PARTIDAS - 1; z++) {

        arrayTableros[z] = arrayTableros[z + 1];
    }

    (*partidas)--;

    arrayTableros[*partidas] = minesweeper_board();
}

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);

    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}

std::string clearString(const std::string & str){ //Limpia una cadena para quitar los \n e ignorar lo que haya mas alla de un espacio
	std::string outStr = str;
	int pos;

	if((pos = outStr.find_first_of(" \n", 0)) != std::string::npos)
		outStr.replace(pos, std::string::npos, "");

	return outStr;
}