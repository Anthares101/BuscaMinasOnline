#include <vector>
#include <iostream>

#include "minesweeper_board.hpp"
#include "minesweeper_box.hpp"

void minesweeper_board::printBoard(){

	for(std::vector< std::vector<minesweeper_box> >::iterator rowIt = board.begin(); rowIt<board.end(); rowIt++){
		std::cout << std::endl << std::endl;
		for(std::vector<minesweeper_box>::iterator colIt = rowIt->begin(); colIt<rowIt->end(); colIt++)
			if(!colIt->isSecret())
				std::cout << colIt->get_minesNear() << "    ";
			else
				std::cout << "X    ";
	}

	std::cout << std::endl;

	return;
}