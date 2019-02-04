# BuscaMinasOnline

## Instrucciones de compilación

Se usa el programa CMake para realizar la compilación de los archivos fuente, para ello, se recomienda crear un subdirectorio llamado “build/” y ejecutar por terminal el comando “cmake ..” para que se generen todos los archivos necesarios para la compilación y estén separados del código fuente del programa. Para compilar el programa, desde el directorio build/ se debe ejecutar el comando “make”, que generará los ejecutables en la misma carpeta.
Manifiesto de archivos
El entregable consta de los archivos que aquí se enumeran:
- minesweeper_server.cpp: código que contiene toda la lógica del servidor para el manejo de comandos que llegan desde el cliente, mediante el uso de una serie de funciones auxiliares.
- minesweeper_client.cpp: código que contiene toda la lógica del cliente para el envío y recepción de mensajes al y desde el servidor. Cuenta con funciones auxiliares para el manejo de tableros de buscaminas.
- minesweeper_box.hpp: código con la clase minesweeper_box, que representa una casilla del buscaminas mediante sus atributos y módulos.
- minesweeper_board.hpp: código con la clase minesweeper_board, que contiene atributos y módulos útiles para el juego del buscaminas. Utiliza la clase minesweeper_box como uno de sus atributos.
- minesweeper_board.cpp: código con la lógica de la clase minesweeper_board.
- user.hpp: código con la clase User, para la gestión del sistema de cuentas e inicio de sesión en el servidor.
- user.cpp: código con la lógica de la clase User.
- macros.hpp: contiene macros para la visualización del texto.
- CMakeLists.txt: archivo que contiene el conjunto de directivas e instrucciones que describen a los archivos fuente del proyecto y sus objetivos.
- README.pdf: este documento.

## Servidor

Se proporciona el servicio de Buscaminas Online, en el que dos jugadores se enfrentan para resolver un tablero de buscaminas por turnos. Cada jugador cuenta con 10 banderas que podrá colocar en las casillas del tablero. El jugador que primero coloque sus 10 banderas en 10 casillas que contengan una mina, será el ganador. Las acciones que realizan los clientes van apareciendo en la pantalla del servidor como un log.
El servidor es capaz de alojar a un total de 30 jugadores y 10 partidas simultáneas. Cuenta con una serie de comandos que puede interpretar y que recibirá de los clientes. Estos comandos son:
Comandos siempre disponibles:
- SALIR: expulsa al cliente del servidor, reduciendo efectivamente el número de clientes conectados en 1.
- AYUDA: al cliente se le imprime un mensaje con los comandos que se ofrecen y en qué momento puede usarlos.
Comandos para el inicio de sesión:
- REGISTRO -u <login> -p <contraseña>: realiza el registro de un nuevo usuario y lo guarda en la lista de usuarios registrados, de forma que dicho usuario no tendrá que volver a registrarse otra vez.
- USUARIO <login>: comprueba que el login introducido por el cliente existe en la lista de usuarios registrados.
- PASSWORD <contraseña>: comprueba que la contraseña introducida por el cliente se corresponde con el login que ya introdujo en la lista de usuarios registrados.
Comandos una vez iniciada la sesión:
- INICIAR-PARTIDA: pone al cliente en espera de otro jugador para comenzar una partida de buscaminas.
Comandos de partida:
- DESCUBRIR <columna, fila>: el jugador descubre la casilla especificada.
- PONER-BANDERA <columna, fila>: el jugador coloca una de sus banderas en la casilla especificada, esto reduce efectivamente el número de banderas disponibles de ese jugador en 1.

## Cliente

Establece conexión con el servidor para poder empezar a jugar al buscaminas. Recibe los mensajes del servidor y se los muestra al cliente añadiendo ayudas por si el usuario no tiene claro cómo interactuar con el servidor.
Cuando el jugador está en partida, muestra a ambos contrincantes el tablero y la acción que realiza el otro jugador. También se muestra cuándo uno de ellos ha ganado y el otro ha perdido.
Si la conexión con el servidor deja de existir, se les informa y se cierra el cliente.
