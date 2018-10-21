//TODO: cambiar los argumentos para adaptarlos al tablero con numeros en las filas y letras en las columnas

#ifndef _MINESWEEPER_BOARD_HPP_
#define _MINESWEEPER_BOARD_HPP_

#include <vector>
#include <utility>
#include <map>
#include <iomanip>
#include <string>

#include "minesweeper_box.hpp"

class minesweeper_board{
	private:
		//Attributes
		std::vector< std::vector<minesweeper_box> > board; //Empty game board
		int _player1, _player2;
		std::map <std::string, int> _columnas;

	public:
		//Builder
		minesweeper_board(int player1 = -1, int player2 = -1);

		//Public Methods
		void coordinates(std::string fila, std::string columna, int * x, int * y);

		bool checkCoordinates(std::string fila, std::string columna);

		inline void putMineBox(int x, int y){board[x][y].putMine();};
		inline void set_minesNearBox(int x, int y, int count){board[x][y].set_minesNear(count);};

		inline int get_boardRows() const{return board.size();};
		inline int get_boardCols() const{return board[0].size();};
		inline int get_player1() const{return this->_player1;};
		inline int get_player2() const{return this->_player2;};
		inline int get_minesNearBox(int x, int y) const{return board[x][y].get_minesNear();};

		inline void set_player1(int player1) {this->_player1 = player1;};
		inline void set_player2(int player2) {this->_player2 = player2;};

		inline bool get_flagsBox(int x, int y, int player) const{return board[x][y].get_flags(player);};
		inline bool isSafeBox(int x, int y) const{return board[x][y].isSafe();};
		inline bool isSecretBox(int x, int y) const{return board[x][y].isSecret();};

		void set_flagBox(std::string fila, std::string columna, int player);
		bool revealBox(std::string fila, std::string columna);
		bool revealBox(int x, int y);
		void printBoard() const;
		std::string board2string() const;

		bool endGame(int player) const;
};

#endif
