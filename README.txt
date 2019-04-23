README

Anne Harris
CS 372 - 400
Project 2
Updated: 3/8/2019

This program acts as a file transfer program. It starts a server on one instance of flip and starts the client on the other.
It accepts two commands, -l (list) and -g (get). If the client sends any other commands the server will remain open but respond with an invalid message

-l command sends a list of the current files in the current working directory
-g sends the contents of a file from the current working directory and saves them as <FILENAME>_copy
If the file doesn't exist in the current working directory it will send an error message

The program can send files up to 160,000 bytes

The server will remain open and listening until the supervisor terminates with SIGINT (CTRL+C)
The client will close its connection after every request is complete

*NOTE*
***You must change the data port number for each new client request to avoid an error.***
You must ensure the <SERVER_HOST> matches the flip that the ftserver program is running on

Open two instances of flip (ex. flip2 and flip3 in seperate putty window)

1. make the server program executable by running: make ftserver
2. make the client program executable by running: chmod +x ftclient
3. Start the server on instance 1 of flip by running: ./ftserver <PORTNUM>
4. Start the client on instance 2 of filp by running: 	ftclient <SERVER_HOST> <PORTNUM1> <COMMAND> <DATAPORTNUM2>
							--OR--
							ftclient <SERVER_HOST> <PORTNUM1> <COMMAND> <FILENAME> <DATAPORTNUM3>

TO RUN:
Instance 1 of flip (server):
	make ftserver
On Instance 2 of flip (client):
	chmod +x ftclient
Instance 1 of flip (server):
	./ftsever <PORTNUM1>
On Instance 2 of flip (client):
	ftclient <SERVER_HOST> <PORTNUM1> <COMMAND> <DATAPORTNUM2>
	ftclient <SERVER_HOST> <PORTNUM1> <COMMAND> <FILENAME> <DATAPORTNUM3>
Instance 1 of flipe (server):
	ctrl+c



