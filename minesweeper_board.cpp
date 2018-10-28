#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <map>
#include <iomanip>
#include <cctype>
#include <sstream>

#include "minesweeper_board.hpp"
#include "minesweeper_box.hpp"

#define MAX_BANDERAS 10

typedef std::pair<std::string, int> columna;

minesweeper_board::minesweeper_board(int player1, int player2){
	this->board = std::vector < std::vector<minesweeper_box> >(10, std::vector< minesweeper_box >(10)); //10x10 minesweeper board
	this->_columnas = std::map <std::string, int> ();
	this->_player1 = player1;
	this->_player2 = player2;
	this->_turno = true;
	this->_nFlags = std::vector <int> (2, 0);

	this->_columnas.insert(columna("A", 0));
	this->_columnas.insert(columna("B", 1));
	this->_columnas.insert(columna("C", 2));
	this->_columnas.insert(columna("D", 3));
	this->_columnas.insert(columna("E", 4));
	this->_columnas.insert(columna("F", 5));
	this->_columnas.insert(columna("G", 6));
	this->_columnas.insert(columna("H", 7));
	this->_columnas.insert(columna("I", 8));
	this->_columnas.insert(columna("J", 9));

	srand(time(NULL)); //Random number seed initialized

	for(int i = 0; i<20; i++){
		int x = rand()%10;
		int y = rand()%10;

		if(this->isSafeBox(x, y))
			this->putMineBox(x,y);
		else
			i--;
	}
}

void minesweeper_board::coordinates(std::string fila, std::string columna, int * x, int * y) const{

	*x = atoi(fila.c_str());
	std::map <std::string, int>::const_iterator it = this->_columnas.find(columna);
	if(it != this->_columnas.end()) {
		*y = it->second;
	}
}

bool minesweeper_board::checkCoordinates(std::string fila, std::string columna) const{

	int x;
	bool checked = false;

	if(!fila.empty() && !columna.empty() && isdigit(fila.c_str()[0])) {

		x = atoi(fila.c_str());

		if(x >= 0 && x <= 9 && (columna.find("A") != std::string::npos ||
		   						columna.find("B") != std::string::npos ||
		  					 	columna.find("C") != std::string::npos ||
		   						columna.find("D") != std::string::npos ||
								columna.find("E") != std::string::npos ||
								columna.find("F") != std::string::npos ||
								columna.find("G") != std::string::npos ||
								columna.find("H") != std::string::npos ||
								columna.find("I") != std::string::npos ||
								columna.find("J") != std::string::npos)){
			checked = true;
		}
	}

	return checked;
}

void minesweeper_board::printBoard() const{

	std::cout << "     ";

	std::cout << " [A]  [B]  [C]  [D]  [E]  [F]  [G]  [H]  [I]  [J] ";

	for(int i=0; i < this->get_boardRows(); i++){
		std::cout << std::endl << std::endl;
		std::cout << " [" << i << "] ";
		for(int j=0; j < this->get_boardCols(); j++)

			if(this->isSecretBox(i,j)){
				if(this->get_flagsBox(i, j, this->get_player1()) && get_flagsBox(i, j, this->get_player2()))
					std::cout << " AB  ";
				else if(this->get_flagsBox(i, j, this->get_player1()))
					std::cout << "  A  ";
				else if(this->get_flagsBox(i, j, this->get_player2()))
					std::cout << "  B  ";
				else
					std::cout << "  -  ";
			}
			else if(!this->isSafeBox(i, j))
				std::cout << "  *  ";
			else
				std::cout << "  " << this->get_minesNearBox(i, j) << "  ";
	}

	std::cout << std::endl << std::endl;

	return;
}

bool minesweeper_board::revealBox(std::string fila, std::string columna){
	int cont = 0, x, y;

	this->coordinates(fila, columna, &x, &y);

	this->board[x][y].reveal();

	if(!this->isSafeBox(x, y))
		return false;
	else{

		for(int i=x-1; i<(x-1)+3; i++){
			for(int j=y-1; j<(y-1)+3; j++){
				if(i>=0 && j>=0 && i < this->get_boardRows() && j < this->get_boardCols() && !this->isSafeBox(i,j))
					cont++;
			}
		}

		this->set_minesNearBox(x, y, cont);

		if(this->get_minesNearBox(x, y) ==0){ //Recursive call to reveal the others boxes if the number of mines around the current box is 0

			for(int i=x-1; i<(x-1)+3; i++){
				for(int j=y-1; j<(y-1)+3; j++){
					if(i>=0 && j>=0 && i < this->get_boardRows() && j < this->get_boardCols() && this->isSecretBox(i, j))
						this->revealBox(i, j);
				}
			}

		}

	}

	return true;
}

bool minesweeper_board::revealBox(int x, int y){
	int cont = 0;

	this->board[x][y].reveal();

	if(!this->isSafeBox(x, y))
		return false;
	else{

		for(int i=x-1; i<(x-1)+3; i++){
			for(int j=y-1; j<(y-1)+3; j++){
				if(i>=0 && j>=0 && i < this->get_boardRows() && j < this->get_boardCols() && !this->isSafeBox(i,j))
					cont++;
			}
		}

		this->set_minesNearBox(x, y, cont);

		if(this->get_minesNearBox(x, y) == 0){ //Recursive call to reveal the others boxes if the number of mines around the current box is 0

			for(int i=x-1; i<(x-1)+3; i++){
				for(int j=y-1; j<(y-1)+3; j++){
					if(i>=0 && j>=0 && i < this->get_boardRows() && j < this->get_boardCols() && this->isSecretBox(i, j))
						this->revealBox(i, j);
				}
			}

		}

	}

	return true;
}

bool minesweeper_board::myTurn(int playersd) {

	if(playersd == this->get_player1() && this->get_turno() == true) {
		return true;
	}

	if(playersd == this->get_player2() && this->get_turno() == false) {
		return true;
	}

	return false;
}

void minesweeper_board::set_flagBox(std::string fila, std::string columna, int player){

	int x, y;
	int changedPlayer;

	this->coordinates(fila, columna, &x, &y);

	changedPlayer = this->get_playerNumber(player);

	this->board[x][y].setFlag(changedPlayer);

	this->_nFlags[changedPlayer]++; //el jugador ha puesto una bandera mas
}

std::string minesweeper_board::board2string() const{
	std::string strBoard = "";

	for(int i=0; i < this->get_boardRows(); i++){
		if(!strBoard.empty()){
			strBoard.pop_back();
			strBoard.push_back(';');
		}
		for(int j=0; j < this->get_boardCols(); j++)

			if(this->isSecretBox(i,j)){
				if(this->get_flagsBox(i, j, this->get_player1()) && this->get_flagsBox(i, j, this->get_player2()))
					strBoard += "AB,";
				else if(this->get_flagsBox(i, j, this->get_player1()))
					strBoard += "A,";
				else if(this->get_flagsBox(i, j, this->get_player2()))
					strBoard += "B,";
				else
					strBoard += "-,";
			}
			else if(!this->isSafeBox(i, j))
				strBoard +="*,";
			else{
				std::stringstream ss;
				ss << this->get_minesNearBox(i, j);
				strBoard += ss.str() + ",";
			}
	}

	strBoard.pop_back();
	strBoard.push_back(';');

	return strBoard;
}

bool minesweeper_board::GameOver(int player) const {
	int cont = 0;

	for(int i=0; i < this->get_boardRows(); i++){

		for(int j=0; j < this->get_boardCols(); j++){

			if(!this->isSafeBox(i, j) && this->get_flagsBox(i, j, player)) {
				cont++;
			}
		}
	}

	if(cont==MAX_BANDERAS) {
		return true;
	}
	else {
		return false;
	}
}