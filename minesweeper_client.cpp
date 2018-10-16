#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

void print_board(std::string board);

int main (int argc, char** argv) {

	if(argc < 2) {

		std::cout << "Error de sintaxis: " << argv[0] << " <ip_servidor>\n";
		exit(EXIT_FAILURE);

	}

	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	std::string buffer;
	char buffer2[256];
	socklen_t len_sockname;
   fd_set readfds, auxfds;
   int salida;
   int fin = 0;

	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd == -1) {

		std::cout << "No se puede abrir el socket cliente\n";
		exit (EXIT_FAILURE);

	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  inet_addr(argv[1]);

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);

	if(connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1) {

		std::cout << "Error de conexión\n";
		exit(EXIT_FAILURE);

	}
	else
		std::cout << "+Ok. Usuario conectado\n";

	//Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);

    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do {

        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);

        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)) {

            bzero(buffer2,sizeof(buffer2));
            recv(sd,buffer2,sizeof(buffer2),0);

				buffer = buffer2;

				//mensajes desde el servidor
            if((buffer.find("+Ok.") != std::string::npos) || (buffer.find("-Err.") != std::string::npos)) {

					std::cout << buffer << std::endl;

					if(buffer.find("-Err. Demasiados clientes conectados\n") != std::string::npos)
						fin = 1;

				}
				else {

					print_board(buffer);

					std::cout << std::endl << std::endl;

				}

			}
			//He introducido información por teclado
			else if(FD_ISSET(0,&auxfds)) {

				bzero(buffer2,sizeof(buffer2));
				fgets(buffer2,sizeof(buffer2), stdin);

				buffer = buffer2;

				if(strcmp(buffer.c_str(),"SALIR\n") == 0)
					fin = 1;

				send(sd,buffer2,sizeof(buffer2),0);

        }

    }while(fin == 0);

	close(sd);

	return 0;
}


void print_board(std::string board) {

	char c = '\0';
	int j = 1;

	std::cout << "      [0] ";

	for(int i = 1; i < 3; i++)
		std::cout << " [" << i << "] ";

	std::cout << std::endl;

	for(int i = 0; i < board.size(); i++) {

		if(i == 0) std::cout << " [0] ";

		if(c == ';') {

			std::cout << " [" << j << "] ";
			j++;

		}

		c = board[i];

		if(c != ',' && c != ';') std::cout << "  " << c << "  ";

		if(c == ';') std::cout << std::endl;

	}

}
