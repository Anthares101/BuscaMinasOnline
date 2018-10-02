#ifndef _BUSCAMINAS_HPP_
#define _BUSCAMINAS_HPP_

#include<cstdlib>
#include<ctime>
#include <vector>

class buscaMinas{
	private:
		//Attributes
		std::vector< std::vector<int> > board; //Empty game board

		//Private Methods

	public:
		//Builder
		inline buscaMinas(){
			board = std::vector< std::vector<int> >(10, std::vector< int >(10)); //10x10 game Board
			srand(time(NULL)); //Random number seed initialized
		};	

		//Public Methods

};

#endif
