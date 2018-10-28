CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Practica2)
#Required for uco thinstation. May not be required in your own pc
SET(CMAKE_CXX_FLAGS " -D_GLIBCXX_USE_CXX11_ABI=0 -m32")

add_executable(Server.out "minesweeper_server.cpp" "user.cpp" "minesweeper_board.cpp")
add_executable(Client.out "minesweeper_client.cpp")
