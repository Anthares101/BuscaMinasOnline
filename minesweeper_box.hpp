#ifndef _MINESWEEPER_BOX_HPP_
#define _MINESWEEPER_BOX_HPP_

#include <vector>

class minesweeper_box{
	private:
		//Attributes
		bool haveMine;
		int minesNear;
		bool secretBox;
		std::vector<bool> flags;

		//Private Methods
		inline void set_haveMine(bool mineStatus){haveMine = mineStatus;};
		inline void set_secretBox(bool status){secretBox = status;};

	public:
		//Builder
		inline minesweeper_box(){
			flags = std::vector<bool> (2, false);
			set_haveMine(false);
			set_minesNear();
			set_secretBox(true);
		};

		//Public Methods
			//Setters
			inline void putMine(){set_haveMine(true);};
			inline void set_minesNear(int minesCount = 0){minesNear = minesCount;};
			inline void reveal(){set_secretBox(false);};
			inline void setFlag(int player){flags[player] = true;};

			//Getters
			inline bool isSafe() const{return !haveMine;};
			inline int get_minesNear() const{return minesNear;};
			inline bool isSecret() const{return secretBox;};
			inline bool get_flags(int player) const{return flags[player];};
};

#endif