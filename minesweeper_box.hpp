#ifndef _MINESWEEPER_BOX_HPP_
#define _MINESWEEPER_BOX_HPP_

class minesweeper_box{
	private:
		//Attributes
		bool haveMine;
		int minesNear;
		bool secretBox;

	public:
		//Builder
		inline minesweeper_box(){
			haveMine = false;
			minesNear = 0;
			secretBox = true;
		};	

		//Public Methods
			//Setters
			inline void putMine(){haveMine = true;};
			//inline void removeMine(){haveMine = false;};
			inline void set_minesNear(int minesCount = 0){minesNear = minesCount;};
			inline void reveal(){secretBox = false;};

			//Getters
			inline bool isSafe(){return !haveMine;};
			inline int get_minesNear(){return minesNear;};
			inline bool isSecret(){return secretBox;};
};

#endif