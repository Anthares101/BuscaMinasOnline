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

            if(strstr(buffer, "+Ok.") != NULL || strstr(buffer, "-Err.") != NULL) {
            	std::cout << std::endl << buffer << std::endl;

	            if(strcmp(buffer,"-Err. Demasiados clientes conectados\n") == 0 || strcmp(buffer,"-Err. Desconexion servidor\n") == 0)
	                fin =1;
            }
            else{
            	print_board(buffer);
            	std::cout << std::endl << std::endl;
            }

        }
        else if(FD_ISSET(0,&auxfds)){
	        //He introducido información por teclado

            bzero(buffer,sizeof(buffer));
            fgets(buffer,sizeof(buffer),stdin);

            if(strcmp(buffer,"SALIR\n") == 0)
                    fin = 1;

            send(sd,buffer,sizeof(buffer),0);
        }

    }while(fin == 0);

    close(sd);

    return 0;

}


void print_board(char* board) {

	std::string c = "";
	int j = 1;
	int i = 0;

	std::cout << "     ";

	std::cout << " [A]  [B]  [C]  [D]  [E]  [F]  [G]  [H]  [I]  [J] ";

	std::cout << std::endl;

	while(i < strlen(board)) {

		if(i == 0) std::cout << " [0] ";

		if(c == ";") {

			std::cout << " [" << j << "] ";
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

		if(c != "," && c != ";" && c != "AB") std::cout << "  " << c << "  ";
		else if(c == "AB") std::cout << "  " << c << " ";

		if(c == ";") std::cout << std::endl;

	}

}


















