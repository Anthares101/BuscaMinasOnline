#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "minesweeper_board.hpp"
#include "minesweeper_box.hpp"

minesweeper_board::minesweeper_board(){
	board = std::vector< std::vector<minesweeper_box> >(10, std::vector< minesweeper_box >(10)); //10x10 minesweeper board
	srand(time(NULL)); //Random number seed initialized

	for(int i = 0; i<20; i++){
		int x = rand()%10;
		int y = rand()%10;

		if(isSafeBox(x, y))
			putMineBox(x,y);
		else
			i--;
	}
}

void minesweeper_board::printBoard() const{

	std::cout << "     ";
	for(int i=0; i<get_boardCols(); i++)
		std::cout << " [" << i << "] ";

	for(int i=0; i<get_boardRows(); i++){
		std::cout << std::endl << std::endl;
		std::cout << " [" << i << "] ";
		for(int j=0; j<get_boardCols(); j++)

			if(isSecretBox(i,j)){
				if(get_flagsBox(i, j, 0) && get_flagsBox(i, j, 1))
					std::cout << " AB  ";
				else if(get_flagsBox(i, j, 0))
					std::cout << "  A  ";
				else if(get_flagsBox(i, j, 1))
					std::cout << "  B  ";
				else
					std::cout << "  -  ";
			}
			else if(!isSafeBox(i, j))
				std::cout << "  *  ";
			else
				std::cout << "  " << get_minesNearBox(i, j) << "  ";
	}

	std::cout << std::endl << std::endl;

	return;
}

bool minesweeper_board::revealBox(int x, int y){
	int cont = 0;

	board[x][y].reveal();

	if(!isSafeBox(x, y))
		return false;
	else{

		for(int i=x-1; i<(x-1)+3; i++){
			for(int j=y-1; j<(y-1)+3; j++){
				if(i>=0 && j>=0 && i<get_boardRows() && j<get_boardCols() && !isSafeBox(i,j))
					cont++;
			}
		}

		set_minesNearBox(x, y, cont);

		if(get_minesNearBox(x, y) ==0){ //Recursive call to reveal the others boxes if the number of mines around the current box is 0

			for(int i=x-1; i<(x-1)+3; i++){
				for(int j=y-1; j<(y-1)+3; j++){
					if(i>=0 && j>=0 && i<get_boardRows() && j<get_boardCols() && isSecretBox(i, j))
						revealBox(i, j);
				}
			}

		}

	}

	return true;
}

bool minesweeper_board::endGame(int player) const{
	int cont = 0;

	for(int i=0; i<get_boardRows(); i++){
		for(int j=0; j<get_boardCols(); j++){
			if(!isSafeBox(i, j) && get_flagsBox(i, j, 0))
				cont++;
		}
	}

	if(cont==10)
		return true;
	else
		return false;
}