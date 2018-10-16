#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

#include "user.hpp"

std::istream & operator>>(std::istream & stream, User & user) {

	std::string login, password, state;

	stream >> state;

	if(state.compare("not_registered") ||
		state.compare("registered") ||
		state.compare("in_queue") ||
		state.compare("in_game")) {

		stream >> login;
		stream >> password;

		user.setLogin(login);
		user.setPassword(password);
		user.setState(state);

	}
	else
		exit(-1);

	return stream;

}

std::ostream & operator<<(std::ostream & stream, const User & user) {

	stream << user.getLogin() << "\t" << user.getPassword() << "\n";

	return stream;

}

bool checkUser(std::string filename, const User & user) {

	bool isRegistered = false;
	std::ifstream file(filename.c_str());

	std::string loginf, passwordf;

	while(file >> loginf >> passwordf) {

		if(loginf.compare(user.getLogin())) {

			isRegistered = true;
			break;

		}

	}

	file.close();

	return isRegistered;

}

int registerUser(std::string filename, const User & user) {

	int retCode;
	std::ofstream file;

	if(!checkUser(filename, user)) {

		file.open(filename.c_str());

		file << user;

		retCode = 0;

	}
	else
		retCode = -1;

	file.close();

	return retCode;

}