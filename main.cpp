#include <iostream>
#include <string>

#include "minesweeper_board.hpp"

int main(){
	int option;
	std::string fila, columna;
	int flagsNum = 10;
	bool status = true;
	bool win = false;

	std::cout << "Prueba buscaminas 1.0" << std::endl << std::endl;

	minesweeper_board buscaminas(0, 1);
	buscaminas.printBoard();


		do{
			std::cout << "1. Revelar" << std::endl;
			std::cout << "2. Bandera (" << flagsNum << "/10)" << std::endl << std::endl;
			std::cout << "Opcion: ";

			std::cout<<std::endl;
			std::cout<<buscaminas.board2string();
			std::cout<<std::endl;

			std::cin >> option;
			std::cin.ignore();

			switch(option){
				case 1:
					do{
						std::cout << "Introduzca una casilla: ";
						std::cin >> fila >> columna;
						std::cin.ignore();

						//suponemos que el usuario mete bien las coordenadas
						if(!buscaminas.checkCoordinates(fila, columna)){
							std::cout << "Elija una casilla valida";
							std::cin.ignore();
							std::cout << std::endl;
						}
						else{
							if(!buscaminas.revealBox(fila, columna))
								option = 0;
						}

					}while(!buscaminas.checkCoordinates(fila, columna));

					break;

				case 2:
					if(flagsNum==0){
						std::cout << "Limite de banderas acanzado";
						std::cin.ignore();
						std::cout << std::endl;
					}
					do{
						std::cout << "Introduzca una casilla: ";
						std::cin >> fila >> columna;
						std::cin.ignore();

						if(!buscaminas.checkCoordinates(fila, columna)){
							std::cout << "Elija una casilla valida";
							std::cin.ignore();
							std::cout << std::endl;
						}
						else{
							buscaminas.set_flagBox(fila, columna, 0);
							flagsNum--;
						}

					}while(!buscaminas.checkCoordinates(fila, columna));

					break;

				case 0:
					std::cout << "Hasta pronto";
					std::cin.ignore();
					std::cout << std::endl;

					break;

				default:
					std::cout << "Elija una opcion valida";
					std::cin.ignore();
					std::cout << std::endl;
			}

			buscaminas.printBoard();
			if(flagsNum == 0){
				win = buscaminas.endGame(0);
				option = 0;
			}

		}while(option!=0);

		if(win)
			std::cout<<"TU GANAS"<<std::endl;
		else
			std::cout<<"Buena suerte la proxima vez"<<std::endl;

	return 0;
}
