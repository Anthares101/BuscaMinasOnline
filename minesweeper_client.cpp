#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <iostream>

#include "macros.hpp"

void print_board(char* board);

int main (int argc, char * argv[])
{

	if(argc < 2) {

		std::cout << "Error de sintaxis: " << argv[0] << " <ip_servidor>\n";
		exit(EXIT_FAILURE);

	}

	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;


    std::string lastMSG;

	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
		exit (1);
	}



	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  inet_addr(argv[1]);

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);

	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}

    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);

    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do{

        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);

        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){

            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);

            if(strstr(buffer, "+Ok.") != NULL) {
				std::cout << CLEAR_SCREEN;
			    PLACE(1,30);
			    std::cout << BBLUE << "MINESWEEPER [" << BGREEN << "ONLINE" << BBLUE << "]" << RESET << std::endl << std::endl;

				std::cout << std::endl  << BCYAN << buffer << RESET << std::endl;

				lastMSG = buffer;

            }
            else if(strstr(buffer, "-Err.") != NULL) {

				std::cout << std::endl  << BRED << buffer << RESET << std::endl;

				if(strcmp(buffer,"-Err. Demasiados clientes conectados\n") == 0 || strcmp(buffer,"-Err. Desconexion servidor\n") == 0) {
					fin = 1;
				}

            }
            else{

				std::cout << CLEAR_SCREEN;
			    PLACE(1,30);
			    std::cout << BBLUE << "MINESWEEPER [" << BGREEN << "ONLINE" << BBLUE << "]" << RESET << std::endl << std::endl;

			    std::cout << BCYAN << lastMSG << RESET << std::endl;

				PLACE(5, 1);
            	print_board(buffer);
            	std::cout << std::endl << std::endl;
            }

        }
        else if(FD_ISSET(0,&auxfds)){
	        //He introducido información por teclado

			bzero(buffer,sizeof(buffer));
	        fgets(buffer,sizeof(buffer),stdin);

			if(strcmp(buffer, "AYUDA\n") == 0) {

			    std::cout << std::endl << BBLUE << "LISTA DE COMANDOS:" << std::endl << std::endl << RESET;

			    std::cout << "AYUDA" << BCYAN << " | Imprime este mensaje\n" << RESET;
			    std::cout << "SALIR" << BCYAN << " | Salir del sistema\n\n" << RESET;
			    std::cout << BYELLOW << "[ANTES DE INICIAR SESION]\n" << RESET;
			    std::cout << "REGISTRO -u <login> -p <password>" << BCYAN << " | Resgistrarse en el sistema\n" << RESET;
			    std::cout << "USUARIO <login>" << BCYAN << " | Introducir login de usuario\n" << RESET;
			    std::cout << "PASSWORD <password>" << BCYAN << " | Introducir la conntraseña de usuario\n\n" << RESET;
			    std::cout << BYELLOW << "[SESION INICIADA]\n" << RESET;
			    std::cout << "INICIAR-PARTIDA" << BCYAN << " | Entrar en cola en busca de partida\n\n" << RESET;
			    std::cout << BYELLOW << "[EN PARTIDA]\n" << RESET;
			    std::cout << "DESCUBRIR <columna,fila>" << BCYAN << " | Descubre la casilla con las coordenadas especificadas\n" << RESET;
			    std::cout << "PONER-BANDERA <columna,fila>" << BCYAN << " | Pone una bandera en la casilla con las coordenadas especificadas\n" << RESET << std::endl;

			}
			else {

	            if(strcmp(buffer,"SALIR\n") == 0) {
	                fin = 1;
	            }

	            send(sd,buffer,sizeof(buffer),0);
	        }
        }

    }while(fin == 0);

    close(sd);

    std::cout << CLEAR_SCREEN;
    PLACE(1,30);
    std::cout << BBLUE << "MINESWEEPER [" << BRED << "OFFLINE" << BBLUE << "]" << RESET << std::endl << std::endl;

    return 0;

}


void print_board(char* board) {

	std::string c = "";
	int j = 1;
	int i = 0;

	std::cout << "     ";

	std::cout << BYELLOW << " [A]  [B]  [C]  [D]  [E]  [F]  [G]  [H]  [I]  [J] " << RESET;

	std::cout << std::endl;

	while(i < strlen(board)) {

		if(i == 0) {
			std::cout << BYELLOW << " [0] " << RESET;
		}

		if(c == ";") {

			std::cout << BYELLOW << " [" << j << "] " << RESET;
			j++;

		}

		if(board[i] == 'A' && board[i + 1] == 'B') {

			c = "AB";

			i += 2;
		}
		else {
			c = board[i];

			i++;
		}

		if(c != "," && c != ";" && c != "AB") {
			if(c == "0") {
				std::cout << BWHITE << "  " << c << "  " << RESET;
			}
			else if(c == "1") {
				std::cout << BBLUE << "  " << c << "  " << RESET;
			}
			else if(c == "2") {
				std::cout << BGREEN << "  " << c << "  " << RESET;
			}
			else if(c == "-") {
				std::cout << BWHITE << "  " << c << "  " << RESET;
			}
			else if(c == "A") {
				std::cout << BYELLOW << "  " << c << "  " << RESET;
			}
			else if(c == "B") {
				std::cout << BPURPLE << "  " << c << "  " << RESET;
			}
			else if(c == "*") {
				std::cout << BCYAN << "  " << c << "  " << RESET;
			}
			else {
				std::cout << BRED << "  " << c << "  " << RESET;
			}

		}
		else if(c == "AB") {
			std::cout << BYELLOW << "  " << c[0] << BPURPLE << c[1] << " " << RESET;
		}

		if(c == ";") {
			std::cout << std::endl;
		}

	}

}


















