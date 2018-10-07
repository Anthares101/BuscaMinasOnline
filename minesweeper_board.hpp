#ifndef _MINESWEEPER_BOARD_HPP_
#define _MINESWEEPER_BOARD_HPP_

#include <vector>

#include "minesweeper_box.hpp"

class minesweeper_board{
	private:
		//Attributes
		std::vector< std::vector<minesweeper_box> > board; //Empty game board

	public:
		//Builder
		minesweeper_board();	

		//Public Methods
		inline void putMineBox(int x, int y){board[x][y].putMine();};
		inline void set_minesNearBox(int x, int y, int count){board[x][y].set_minesNear(count);};

		inline int get_boardRows() const{return board.size();};
		inline int get_boardCols() const{return board[0].size();};
		inline int get_minesNearBox(int x, int y) const{return board[x][y].get_minesNear();};
		inline bool get_flagsBox(int x, int y, int player) const{return board[x][y].get_flags(player);};
		inline bool isSafeBox(int x, int y) const{return board[x][y].isSafe();};
		inline bool isSecretBox(int x, int y) const{return board[x][y].isSecret();};
		
		inline void set_flagBox(int x, int y, int player){board[x][y].setFlag(player);};
		bool revealBox(int x, int y);
		void printBoard() const;

		bool endGame(int player) const;
};

#endif
