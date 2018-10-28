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
		int _player1, _player2; //sockets de los jugadores
		std::map <std::string, int> _columnas; //mapeo de columnas
		bool _turno; //true: le toca a player1, false: le toca a player2
		std::vector <int> _nFlags; //numero de flags que cada jugador ha puesto

		//dado el socket de un jugador, devuelve 0 si es el jugador 1, 1 si es el jugador 2, o -1 si no es ninguno
		inline int get_playerNumber (int sd) const{
			if(this->get_player1() == sd)
				return 0;
			if(this->get_player2() == sd)
				return 1;

			return -1;
		};

		//devuelve el numero de minas alrededor de la casilla de coordenadas (x,y)
		inline int get_minesNearBox(int x, int y) const{ return this->board[x][y].get_minesNear(); };

	public:

		//Builder
		minesweeper_board(int player1 = -1, int player2 = -1);

		//Public Methods

		//comprueba si las coordenadas son validas
		bool checkCoordinates(std::string fila, std::string columna) const;

		//transforma coordenadas de formato (string,string) a (int,int)
		void coordinates(std::string fila, std::string columna, int * x, int * y) const;

		//coloca una mina en la casilla de coordenadas (x,y)
		inline void putMineBox(int x, int y) { this->board[x][y].putMine(); };
		//coloca en la casilla de coordenadas (x,y) el numero de minas que tiene alrededor
		inline void set_minesNearBox(int x, int y, int count) { this->board[x][y].set_minesNear(count); };

		//devuelve el numero de filas del tablero
		inline int get_boardRows() const { return this->board.size(); };
		//devuelve el numero de columnas del tablero
		inline int get_boardCols() const { return this->board[0].size(); };
		//devuelve el socket del jugador 1
		inline int get_player1() const { return this->_player1; };
		//devuelve el socket del jugador 2
		inline int get_player2() const { return this->_player2; };
		//devuelve el turno de jugador
		inline bool get_turno() const { return this->_turno; };
		//devuelve el numero de banderas que ha puesto el jugador
		inline int get_nFlags(int playersd) const { return this->_nFlags[this->get_playerNumber(playersd)]; }

		//establece el socket dado como jugador 1
		inline void set_player1(int player1) { this->_player1 = player1; };
		//establece el socket dado como jugador 2
		inline void set_player2(int player2) { this->_player2 = player2; };

		//comprueba si en la casilla de coordenadas (x,y) el jugador player tiene una bandera
		inline bool get_flagsBox(int x, int y, int player) const { return this->board[x][y].get_flags(this->get_playerNumber(player)); };
		//comprueba si en la casilla de coordenadas (x,y) el jugador player tiene una bandera
		inline bool get_flagsBox(std::string x, std::string y, int player) const {
			int xMod, yMod;
			this->coordinates(x, y, &xMod, &yMod);

			return this->board[xMod][yMod].get_flags(this->get_playerNumber(player));
		};
		//comprueba si la casilla de coordenadas (x,y) tiene mina o no
		inline bool isSafeBox(int x, int y) const { return this->board[x][y].isSafe(); };
		//comprueba si la casilla de coordenadas (x,y) tiene mina o no
		inline bool isSafeBox(std::string x, std::string y) const {
			int xMod, yMod;
			this->coordinates(x, y, &xMod, &yMod);

			return this->board[xMod][yMod].isSafe();
		};
		//comprueba si la casilla de coordenadas (x,y) ha sido descubierta
		inline bool isSecretBox(int x, int y) const { return this->board[x][y].isSecret(); };
		//comprueba si la casilla de coordenadas (x,y) ha sido descubierta
		inline bool isSecretBox(std::string x, std::string y) const {
			int xMod, yMod;
			this->coordinates(x, y, &xMod, &yMod);

			return this->board[xMod][yMod].isSecret();
		};

		//comprueba si es el turno del jugador con socket palyersd
		bool myTurn(int playersd);
		//cambia el turno de un jugador a otro
		inline void changeTurn(){
			if(this->_turno)
				this->_turno = false;
			else
				this->_turno = true;
		};

		//pone una bandera del jugador player en la casilla con coordenadas (x,y)
		void set_flagBox(std::string fila, std::string columna, int player);
		//descubre la casilla con coordenadas (fila, columna)
		bool revealBox(std::string fila, std::string columna);
		//descubre la casilla con coordenadas (x,y)
		bool revealBox(int x, int y);
		//imprime el tablero
		void printBoard() const;
		//convierte el tablero a una string
		std::string board2string() const;

		//hace recuento de minas debajo de banderas y devuelve true si el jugador ha ganado, false en caso contrario
		bool GameOver(int player) const;
};

#endif
