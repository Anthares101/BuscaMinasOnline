#ifndef _MINESWEEPER_BOX_HPP_
#define _MINESWEEPER_BOX_HPP_

#include <vector>

class minesweeper_box{
	private:
		//Attributes
		bool haveMine; //si tiene mina o no
		int minesNear; //numero de minas alrededor
		bool secretBox; //si ha sido descubierta o no
		std::vector <bool> flags; //si tiene banderas de los jugadores

		//Private Methods
		//establece si tiene mina o no
		inline void set_haveMine(bool mineStatus) { this->haveMine = mineStatus; };
		//establece si ha sido descubierta o no
		inline void set_secretBox(bool status) { this->secretBox = status; };

	public:
		//Builder
		inline minesweeper_box(){
			this->flags = std::vector<bool> (2, false);
			this->set_haveMine(false);
			this->set_minesNear();
			this->set_secretBox(true);
		};

		//Public Methods

		//Setters

		//pone una mina
		inline void putMine() { this->set_haveMine(true); };
		//guarda el numero de minas de alrededor
		inline void set_minesNear(int minesCount = 0) { this->minesNear = minesCount; };
		//descubre la casilla
		inline void reveal() { this->set_secretBox(false); };
		//pone la bandera de un jugador
		inline void setFlag(int player) { this->flags[player] = true; };

		//Getters

		//comprueba si tiene mina o no
		inline bool isSafe() const { return !this->haveMine; };
		//devuelve el numero de minas de alrededor
		inline int get_minesNear() const { return this->minesNear; };
		//comprueba si ha sido descubierta
		inline bool isSecret() const { return this->secretBox; };
		//comprueba si un jugador ha puesto una bandera
		inline bool get_flags(int player) const { return this->flags[player]; };
};

#endif