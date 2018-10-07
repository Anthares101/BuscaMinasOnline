#ifndef _MINESWEEPER_BOARD_HPP_
#define _MINESWEEPER_BOARD_HPP_

#include <cstdlib>
#include <ctime>
#include <vector>

#include "minesweeper_box.hpp"

class minesweeper_board{
	private:
		//Attributes
		std::vector< std::vector<minesweeper_box> > board; //Empty game board

		//Private Methods
		inline int get_boardRows(){return board.size();};
		inline int get_boardCols(){return board[0].size();};

	public:
		//Builder
		inline minesweeper_board(){
			board = std::vector< std::vector<minesweeper_box> >(10, std::vector< minesweeper_box >(10)); //10x10 minesweeper board
			srand(time(NULL)); //Random number seed initialized
		};	

		//Public Methods
		void printBoard();
};

#endif
