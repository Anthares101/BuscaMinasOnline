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
#define MAX_BANDERAS 10


/*
 * El servidor ofrece el servicio de un buscaminas online
 */

//maneja la recepcion de la señal SIGINT
void manejador(int signum);
//gestiona la salida de un cliente del servidor, sin que afecte a los demas clientes
void salirCliente(int socket, fd_set * readfds, std::vector <User> & arrayClientes);
//dado el socket de un cliente, busca la partida en la que se encuentra
int idPartida(const std::vector <minesweeper_board> & arrayTableros, int playersd);
//dado un cliente y una partida, busca el contrincante de dicho cliente en esa partida
int otherPlayer(const std::vector <User> & arrayClientes, const std::vector <minesweeper_board> & arrayTableros, int match, int client);
//Limpia una cadena para quitar los \n y espacios e ignorar lo que haya despues de ellos
std::string clearString(const std::string & str);
//Comprueba si un login tiene una sesion iniciada. client: Index del usuario que quiere conectar, user: nombre del login que se quiere usar
bool userAlreadyConnected(int client, const std::vector <User> & arrayClientes);

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
    std::vector<User> arrayClientes;
    std::vector<minesweeper_board> arrayTableros;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];

    int on, ret;

    //Para la lectura de comandos
    std::string strBuffer;

    //Numero de cliente
    int client, enemyClient;

    //partida
    int match;

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
                                if(arrayClientes.size() < MAX_CLIENTS){
                                	arrayClientes.push_back(User());
                                    arrayClientes[arrayClientes.size()-1].setSocket_descriptor(new_sd);
                                    FD_SET(new_sd,&readfds);

                                    std::cout << BCYAN << "+ Nuevo usuario conectado " << BYELLOW << "[" << arrayClientes.size() << "/" << MAX_CLIENTS << "]" << RESET << std::endl;
                                    strcpy(buffer, "+Ok. Usuario conectado\n");

                                    send(new_sd,buffer,sizeof(buffer),0);
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"-Err. Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
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

                                for (j = 0; j < arrayClientes.size(); j++){
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
                        //Mensajes que se quieran mandar a los clientes
                        }
                        else{
                        	client = 0;
                        	while(client < arrayClientes.size() && arrayClientes[client].getSocket_descriptor() != i)
                        		client++;
                            bzero(buffer,sizeof(buffer));

                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            strBuffer = buffer;

                            if(recibidos > 0){

                                if(strcmp(buffer,"SALIR\n") == 0) {

                                    if(arrayClientes[client].getState() == "in_game") {

                                        match = idPartida(arrayTableros, arrayClientes[client].getSocket_descriptor());
                                        enemyClient = otherPlayer(arrayClientes, arrayTableros, match, client);

                                        arrayClientes[enemyClient].setState("registered");

                                        std::cout << BCYAN << "+ Partida terminada: " << BYELLOW << arrayClientes[client].getLogin() << " " << BRED << " VS " << BYELLOW << arrayClientes[enemyClient].getLogin() << RESET << std::endl;

                                        //borrar el tablero de esta partida
                                        arrayTableros.erase(arrayTableros.begin()+match);

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "+Ok. El otro jugador ha abandonado la partida");
                                        send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                    }

                                    salirCliente(i, &readfds, arrayClientes);

                                }

                                //Si el usuario no ha iniciado sesion aun se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "not_registered"){
	                        		//Comando USUARIO usuario
	                        		if(strBuffer.substr(0, 8) == "USUARIO " && !(strBuffer = strBuffer.substr(8)).empty()){
	                        			strBuffer = clearString(strBuffer);

	                        			arrayClientes[client].setLogin(strBuffer);
	                        			if(arrayClientes[client].getLogin() != "" && arrayClientes[client].checkUser("userList")){//Usuario Correcto
                                            bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "+Ok. Introduzca mediante \"PASSWORD password\" su contraseña");
	                        			}
	                        			else{//El usuario no existe
                                            arrayClientes[client].setLogin("");
	                        				bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "-Err. Usuario incorrecto, use \"REGISTRO –u usuario –p password\" para registrarse");
	                        			}
	                        		}
	                        		//Comando PASSWORD password
	                        		else if(strBuffer.substr(0, 9) == "PASSWORD " && !(strBuffer = strBuffer.substr(9)).empty()){//Se ha puesto PASSWORD antes de USUARIO
	                        			strBuffer = clearString(strBuffer);

	                        			if(arrayClientes[client].getLogin() == ""){
                                            bzero(buffer,sizeof(buffer));
	                        				sprintf(buffer, "-Err. Primero indique su usuario con \"USUARIO usuario\"");
	                        			}
	                        			else{
	                        				arrayClientes[client].setPassword(strBuffer);

	                        				if(arrayClientes[client].getPassword() != "" && arrayClientes[client].verifyUser("userList")){//Contraseña y usuario correctos
                                                if(!userAlreadyConnected(client, arrayClientes)){//El login que inicia sesion no tiene una sesion iniciada
		                        					arrayClientes[client].setState("registered");
		                        					bzero(buffer,sizeof(buffer));
													sprintf(buffer, "+Ok. Usuario validado. Escriba \"INICIAR-PARTIDA\" para entrar en cola");
	                        					}
	                        					else{
	                        						arrayClientes[client].setLogin("");
	                        						arrayClientes[client].setPassword("");
	                        						bzero(buffer,sizeof(buffer));
		                        					sprintf(buffer, "-Err. La cuenta de usuario a la que intenta acceder tiene una sesion activa");
	                        					}
                                            }
                                            else{//Contraseña incorrecta
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
												sprintf(buffer, "+Ok. Usuario registrado, sesion iniciada. Escriba \"INICIAR-PARTIDA\" para entrar en cola");

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

                                    send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                }
                                //Si el usuario ha iniciado sesion se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "registered") {

                                    if(strBuffer.substr(0, 15) == "INICIAR-PARTIDA" && arrayTableros.size() <= MAX_PARTIDAS) {

                                        bzero(buffer,sizeof(buffer));
                                        strcpy(buffer,"+Ok. Buscando partida...\n");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                        strBuffer = clearString(strBuffer);

                                        arrayClientes[client].setState("in_queue");

                                        for(int z = 0; z < arrayClientes.size(); z++) {

                                            if(z != client && arrayClientes[z].getState() == "in_queue") {

                                                arrayClientes[z].setState("in_game");
                                                arrayClientes[client].setState("in_game");

                                                std::cout << BCYAN << "+ Partida iniciada: " << BYELLOW << arrayClientes[client].getLogin() << BRED << " VS " << BYELLOW << arrayClientes[z].getLogin() << RESET << std::endl;

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Empieza la partida contra el jugador %s. Eres el jugador 2", (arrayClientes[client].getLogin()).c_str());

                                                send(arrayClientes[z].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Empieza la partida contra el jugador %s. Eres el jugador 1", (arrayClientes[z].getLogin()).c_str());

                                                send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                arrayTableros.push_back(minesweeper_board());

                                                //se les asigna un objeto tablero dentro del arrayTableros
                                                arrayTableros[arrayTableros.size()-1].set_player1(arrayClientes[client].getSocket_descriptor());
                                                arrayTableros[arrayTableros.size()-1].set_player2(arrayClientes[z].getSocket_descriptor());

                                                bzero(buffer,sizeof(buffer));
                                                strcpy(buffer, arrayTableros[arrayTableros.size()-1].board2string().c_str());
                                                send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                                send(arrayClientes[z].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                break;
                                            }
                                        }
                                    }
                                    else if(strBuffer.substr(0, 15) == "INICIAR-PARTIDA" && arrayTableros.size() > MAX_PARTIDAS) {

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. El servidor no puede alojar mas partidas, debe esperar");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                    }
                                    else{
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. Busque contrincante con el comando \"INICIAR-PARTIDA\"");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                    }
                            	}
                                //Si el usuario esta in_game se tendran en cuenta estos comandos
                                else if(arrayClientes[client].getState() == "in_game") {
                                    std::string Z, x; //letra y numero del tablero

                                    match = idPartida(arrayTableros, arrayClientes[client].getSocket_descriptor());

                                    if(!strBuffer.empty() && !arrayTableros[match].myTurn(arrayClientes[client].getSocket_descriptor())) {

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. Es el turno del otro jugador\n");
                                        send(arrayClientes[client].getSocket_descriptor(), buffer, sizeof(buffer), 0);
                                    }
                                    //comando DESCUBRIR
                                    else if(strBuffer.substr(0, 10) == "DESCUBRIR " && arrayTableros[match].myTurn(arrayClientes[client].getSocket_descriptor()) && !(strBuffer = strBuffer.substr(10)).empty()) {

                                        Z = strBuffer.substr(0, 1);//Primer caracter como parametro
                                        if(strBuffer.substr(1, 1) == ","){//Se comprueba si hay una coma justo despues del caracter
                                        	x = strBuffer.substr(2);
                                        	x = clearString(x);
                                        }
                                        else {
                                        	x = "";
                                        }

                                        //Casilla introducida posible y no tiene bandera del jugador ni esta descubierta
                                        if(arrayTableros[match].checkCoordinates(x, Z) && arrayTableros[match].isSecretBox(x, Z) && !arrayTableros[match].get_flagsBox(x, Z, arrayClientes[client].getSocket_descriptor())){
                                        	//la casilla no tenia mina
	                                        if(arrayTableros[match].revealBox(x, Z)) {

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. El jugador %s ha descubierto la casilla %s,%s\n", (arrayClientes[client].getLogin()).c_str(), Z.c_str(), x.c_str());
                                                send(arrayTableros[match].get_player1(),buffer,sizeof(buffer),0);
                                                send(arrayTableros[match].get_player2(),buffer,sizeof(buffer),0);

	                                            bzero(buffer,sizeof(buffer));
	                                            strcpy(buffer, arrayTableros[match].board2string().c_str());
	                                            send(arrayTableros[match].get_player1(),buffer,sizeof(buffer),0);
	                                            send(arrayTableros[match].get_player2(),buffer,sizeof(buffer),0);

	                                        }
	                                        //la casilla tenia mina
	                                        else {

                                                enemyClient = otherPlayer(arrayClientes, arrayTableros, match, client);

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Enhorabuena! Has ganado la partida contra %s\n", (arrayClientes[client].getLogin()).c_str());
                                                send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer, "+Ok. Has perdido la partida contra %s, pero bien jugado!\n", (arrayClientes[enemyClient].getLogin()).c_str());
                                                send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

	                                            bzero(buffer,sizeof(buffer));
	                                            strcpy(buffer, arrayTableros[match].board2string().c_str());
	                                            send(arrayTableros[match].get_player1(),buffer,sizeof(buffer),0);
	                                            send(arrayTableros[match].get_player2(),buffer,sizeof(buffer),0);

	                                            arrayClientes[client].setState("registered");
	                                            arrayClientes[enemyClient].setState("registered");

	                                            std::cout << BCYAN << "+ Partida terminada: " << BYELLOW << arrayClientes[client].getLogin() << " " << BRED << " VS " << BGREEN << arrayClientes[enemyClient].getLogin() << RESET << std::endl;

	                                            //borrar el tablero de esta partida
	                                            arrayTableros.erase(arrayTableros.begin()+match);
	                                        }

                                            arrayTableros[match].changeTurn();
	                                    }
	                                    else{//Casilla invalida
	                                    	bzero(buffer,sizeof(buffer));
											sprintf(buffer, "-Err. Casilla introducida invalida, escoja otra\n");
	                                    	send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
	                                    }

                                    }
                                    //comando PONER-BANDERA
                                    else if(strBuffer.substr(0, 14) == "PONER-BANDERA " &&
                                            arrayTableros[match].myTurn(arrayClientes[client].getSocket_descriptor()) &&
                                            arrayTableros[match].get_nFlags(arrayClientes[client].getSocket_descriptor()) < MAX_BANDERAS &&
                                            !(strBuffer = strBuffer.substr(14)).empty()) {

                                        Z = strBuffer.substr(0, 1);//Primer caracter como parametro
                                        if(strBuffer.substr(1, 1) == ","){//Se comprueba si hay una coma justo despues del caracter
                                            x = strBuffer.substr(2);
                                            x = clearString(x);
                                        }
                                        else {
                                            x = "";
                                        }

                                        //Casilla introducida posible y no tiene bandera del jugador ni esta descubierta
                                        if(arrayTableros[match].checkCoordinates(x, Z) && arrayTableros[match].isSecretBox(x, Z) && !arrayTableros[match].get_flagsBox(x, Z, arrayClientes[client].getSocket_descriptor())) {

                                            arrayTableros[match].set_flagBox(x, Z, arrayClientes[client].getSocket_descriptor());

                                            enemyClient = otherPlayer(arrayClientes, arrayTableros, match, client);

                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer, "+Ok. El jugador %s ha puesto bandera en la casilla %s,%s\n", (arrayClientes[client].getLogin()).c_str(), Z.c_str(), x.c_str());
                                            send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer, "+Ok. Bandera colocada en la casilla %s,%s. Quedan [%d/%d] banderas\n", Z.c_str(), x.c_str(), MAX_BANDERAS - arrayTableros[match].get_nFlags(arrayClientes[client].getSocket_descriptor()), MAX_BANDERAS);
                                            send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                            bzero(buffer,sizeof(buffer));
                                            strcpy(buffer, arrayTableros[match].board2string().c_str());
                                            send(arrayTableros[match].get_player1(),buffer,sizeof(buffer),0);
                                            send(arrayTableros[match].get_player2(),buffer,sizeof(buffer),0);

                                            arrayTableros[match].changeTurn();

                                            if(arrayTableros[match].get_nFlags(arrayClientes[client].getSocket_descriptor()) == MAX_BANDERAS) {

                                                //este cliente ha ganado
                                                if(arrayTableros[match].GameOver(arrayClientes[client].getSocket_descriptor())) {

                                                    bzero(buffer,sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Enhorabuena! Has ganado la partida contra %s\n", (arrayClientes[enemyClient].getLogin()).c_str());
                                                    send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                    bzero(buffer,sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Has perdido la partida contra %s, pero bien jugado!\n", (arrayClientes[client].getLogin()).c_str());
                                                    send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                    std::cout << BCYAN << "+ Partida terminada: " << BGREEN << arrayClientes[client].getLogin() << " " << BRED << " VS " << BYELLOW << arrayClientes[enemyClient].getLogin() << RESET << std::endl;
                                                }
                                                //este cliente ha perdido
                                                else {

                                                    bzero(buffer,sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Enhorabuena! Has ganado la partida contra %s\n", (arrayClientes[client].getLogin()).c_str());
                                                    send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                    bzero(buffer,sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Has perdido la partida contra %s, pero bien jugado!\n", (arrayClientes[enemyClient].getLogin()).c_str());
                                                    send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);

                                                    std::cout << BCYAN << "+ Partida terminada: " << BYELLOW << arrayClientes[client].getLogin() << " " << BRED << " VS " << BGREEN << arrayClientes[enemyClient].getLogin() << RESET << std::endl;
                                                }

                                                arrayClientes[client].setState("registered");
                                                arrayClientes[enemyClient].setState("registered");

                                                //borrar el tablero de esta partida
                                                arrayTableros.erase(arrayTableros.begin()+match);

                                            }
                                        }
                                        else{//Casilla invalida
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer, "-Err. Casilla introducida invalida, escoja otra\n");
                                            send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                        }
                                    }
                                    //no se pueden poner mas banderas
                                    else if(strBuffer.substr(0, 14) == "PONER-BANDERA " &&
                                            arrayTableros[match].myTurn(arrayClientes[client].getSocket_descriptor()) &&
                                            arrayTableros[match].get_nFlags(arrayClientes[client].getSocket_descriptor()) < MAX_PARTIDAS) {

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. No puede poner mas banderas\n");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                    }
                                    //comando invalido
                                    else {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "-Err. Utilice los comandos \"DESCUBRIR columna,fila\" o \"PONER-BANDERA columna,fila\" para jugar\n");
                                        send(arrayClientes[client].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                    }
                                }
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos == 0)
                            {
                                std::cout << BCYAN << "+ El socket " << GREEN << arrayClientes[client].getSocket_descriptor() << BCYAN;
                                std::cout << ", ha introducido " << BPURPLE << "ctrl+c" << RESET << std::endl;

                                if(arrayClientes[client].getState() == "in_game") {

                                    match = idPartida(arrayTableros, arrayClientes[client].getSocket_descriptor());
                                    enemyClient = otherPlayer(arrayClientes, arrayTableros, match, client);

                                    arrayClientes[enemyClient].setState("registered");

                                    std::cout << BCYAN << "+ Partida terminada: " << BYELLOW << arrayClientes[client].getLogin() << " " << BRED << " VS " << BYELLOW << arrayClientes[enemyClient].getLogin() << RESET << std::endl;

                                    //borrar el tablero de esta partida
                                    arrayTableros.erase(arrayTableros.begin()+match);

                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "+Ok. El otro jugador ha abandonado la partida");
                                    send(arrayClientes[enemyClient].getSocket_descriptor(),buffer,sizeof(buffer),0);
                                }
                                //Eliminar ese socket
                                salirCliente(i, &readfds, arrayClientes);
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

void salirCliente(int socket, fd_set * readfds, std::vector<User> & arrayClientes){

    char buffer[MSG_SIZE];
    int j;
    std::string cliente;

    close(socket);
    FD_CLR(socket,readfds);

    //Re-estructurar el array de clientes
    for (j = 0; j < arrayClientes.size(); j++){
        if (arrayClientes[j].getSocket_descriptor() == socket)
            break;
    }

    cliente = arrayClientes[j].getLogin();

    arrayClientes.erase(arrayClientes.begin()+j);

    bzero(buffer,sizeof(buffer));
    if(!cliente.empty())
        std::cout << BCYAN << "+ Desconexion del cliente: " << GREEN << cliente << " " << BYELLOW << "[" << arrayClientes.size() << "/" << MAX_CLIENTS << "]" << RESET << std::endl;
    else
        std::cout << BCYAN << "+ Desconexion del cliente con socket: " << GREEN << socket << " " << BYELLOW << "[" << arrayClientes.size() << "/" << MAX_CLIENTS << "]" << RESET << std::endl;

}

int idPartida(const std::vector <minesweeper_board> & arrayTableros, int playersd) {
    int match;

    for(int i = 0; i < arrayTableros.size(); i++) {

        if(arrayTableros[i].get_player1() == playersd || arrayTableros[i].get_player2() == playersd) {
            match = i;
            break;
        }
    }

    return match;
}

int otherPlayer(const std::vector <User> & arrayClientes, const std::vector <minesweeper_board> & arrayTableros, int match, int client) {
    int other;

    for(int cont = 0; cont < arrayClientes.size(); cont++) {
        if((arrayClientes[cont].getSocket_descriptor() == arrayTableros[match].get_player2() && cont != client) ||
           (arrayClientes[cont].getSocket_descriptor() == arrayTableros[match].get_player1() && cont != client)) {
            other = cont;
        }
    }

    return other;
}

void manejador (int signum){
    std::cout << std::endl << BRED << "Se ha recibido la señal sigint al introducir " << BPURPLE << "ctrl+c" << RESET << std::endl;
    std::cout << BRED << "Introduzca el comando \"SALIR\" para cerrar el servidor correctamente" << RESET << std::endl;
}

bool userAlreadyConnected(int client, const std::vector <User> & arrayClientes){
	for(int i = 0; i < arrayClientes.size(); i++){
		if(client != i && arrayClientes[i].getLogin() == arrayClientes[client].getLogin())
			return true;
	}
 	return false;
}

std::string clearString(const std::string & str) {
	std::string outStr = str;
	int pos;

	if((pos = outStr.find_first_of(" \n", 0)) != std::string::npos)
		outStr.replace(pos, std::string::npos, "");

	return outStr;
}