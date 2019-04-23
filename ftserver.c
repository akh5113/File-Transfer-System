/***********************************************************************
* Anne Harris harranne@oregonstate.edu
* CS 372 - 400
* Projct 2 
* Last Modified: 3/8/2019
* Description: This program implements a simple file transfer system. 
* This file acts as the server and opens to recieve requests from client
* Client must send -l for a list of the files in the current directory
* or -g to get a files contents
************************************************************************/

/************************************************************************
* SERVER
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

//Source: Beej's Guide to Network Programming - Using Internet Sockets
//Source: "Computer Networking - A top-down Approach" 7 ed. Kurose & Ross,  pgs 164 -170
// Source: CS 344 Program 4 by Anne Harris - Course by Benjamin Brewster
// Source: www.tutorialspoint.com/cprogramming
//Source: Linux manual pages - man7.org


//get address information
int getaddrinfor(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

//main function
int main(int argc, char *argv[]) {

	//check for right number of command line arguments
	if (argc < 2) {
		printf("Error, too few arguments\n");
		exit(1);
	}

	//variables for connection
	int listeningSocket;
	int clientConnection; //where data is read from and written to
	int serverPort;
	socklen_t clientAddrSize;
	struct sockaddr_in serverAddress, clientAddress;

	//variables for files
	char messageBuffer[1000]; //message sent by client
	int charsRead;
	char fileName[256]; //file name sent by client
	char command[256]; //comand sent by client
	char dataPortStr[10]; //second port for data transfer, string
	int dataPort;  //second port for data transfer

	//set up server address struct
	// set address struct to null terminators
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	// get port num and convert from string to int
	serverPort = atoi(argv[1]);
	// create socket
	serverAddress.sin_family = AF_INET;
	//assign port number
	serverAddress.sin_port = htons(serverPort);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//create socket
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket < 0) {
		printf("Sever - Error opening socket\n");
		exit(1);
	}

	//bind port to socket
	if (bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		printf("Sever - Error binding the socket");
		exit(1);
	}
	//begin listening
	listen(listeningSocket, 5); //can recieve up to 5 connections???
	printf("Server open on %d\n", serverPort);


	//enter while loop to keep server open until ctrl + c is received
	while(1){
		//accept connection
		//get size of address for the client
		clientAddrSize = sizeof(clientAddress);
		//accept the connections
		clientConnection = accept(listeningSocket, (struct sockaddr *) &clientAddress, &clientAddrSize);
		if (clientConnection < 0) {
			printf("Server - Error accepting connection\n");
			exit(1);
		}

		//get name of client and print
  		char clientName[clientAddrSize];
		memset(clientName, '\0', sizeof(clientName));
		inet_ntop(AF_INET, &clientAddress.sin_addr, clientName, sizeof(clientName)); //convert client name to readable string
		printf("Connection from %s\n", clientName);

		//get message from client
		//clear out buffer
		memset(messageBuffer, '\0', sizeof(messageBuffer));
		charsRead = recv(clientConnection, messageBuffer, (sizeof(messageBuffer) - 1), 0);
		if (charsRead < 0) {
			printf("Server - Error reading from socket");
			exit(1);
		}
		//printf("Recieved from clinet: %s \n", messageBuffer);
	
		//Variables for second data connection
		int listeningSocket2;
		int clientConnection2; 
		socklen_t clientAddrSize2;
		struct sockaddr_in serverAddress2, clientAddress2;
		char infoSent[1000]; //for list of files

		//clear out data server address struct
		memset((char*)&serverAddress2, '\0', sizeof(serverAddress2));

		/* Data is sent in the following format:
		 *[COMMAND]%[PORTNUM]
		 * or
		 *[COMMAND]%[PORTNUM]@[FILENAME]
		This is used to parse out the data easily by looking for the seperators
		*/ 

		//parse out information
		//search for positions of symbols
		char percentSym = '%';
		char atSym = '@';
		char dollarSym = '$';

		//get pointer to posision of char
		char *percentPtr = strchr(messageBuffer, percentSym);
		char *atPtr = strchr(messageBuffer, atSym);
		char *dollarPtr = strchr(messageBuffer, dollarSym);

		//set up positions
		//percent sign is where command ends and port begins
		int percentPos = percentPtr - messageBuffer; //should be 3rd char
		//@ is where port ends and file name begins (if there is a file name)
		int atPos = atPtr - messageBuffer;
		//$ is where file name ends
		int dollarPos = dollarPtr - messageBuffer;

		//variabls to compare commands
		char listCommand[2] = "-l";
		char getCommand[2] = "-g";

		//check for list command
		if(strncmp(messageBuffer, listCommand, 2) == 0){
			//printf("List command\n");
			memset(dataPortStr, '\0', sizeof(dataPortStr));
			//get length of port
			int portLen = atPos - percentPos - 1;
			//copy into a string
			strncpy(dataPortStr, messageBuffer + percentPos + 1, portLen);
			//convert to int
			dataPort = atoi(dataPortStr);
			printf("List directory requested on port %d\n", dataPort);
	
			//set up server address struct
			serverAddress2.sin_family = AF_INET;
			serverAddress2.sin_port = htons(dataPort);
			serverAddress2.sin_addr.s_addr = INADDR_ANY;
	
			//create socket
			listeningSocket2 = socket(AF_INET, SOCK_STREAM, 0);
			if(listeningSocket2 < 0){
				printf("Listneing socket 2 error\n");
				exit(1);
			}

			//bind
			if(bind(listeningSocket2, (struct sockaddr *) &serverAddress2, sizeof(serverAddress2)) < 0){
				printf("error on second bind\n");
				exit(1);
				
			}
			//begining second listening
			listen(listeningSocket2, 5);
			//printf("Server open on %d\n", dataPort);
	
			//accept data connection
			clientAddrSize2 = sizeof(clientAddress2);
			clientConnection2 = accept(listeningSocket2, (struct sockaddr *) &clientAddress2, &clientAddrSize2);
			if(clientConnection2 < 0){
				printf("Error accepting second connection\n");
				exit(1);
			}

			//get client name a second time
			char clientName2[clientAddrSize2];
			memset(clientName2, '\0', sizeof(clientName2));
			inet_ntop(AF_INET, &clientAddress.sin_addr, clientName2, sizeof(clientName2));
			//printf("Connection from %s\n", clientName2);

			//get current working directory
			char currDir[1000];
			memset(currDir, '\0', sizeof(currDir));
			getcwd(currDir, sizeof(currDir));
			//printf("current directory: %s\n", currDir);

			//get contents of directory
			DIR *directory;
			struct dirent *ent;
			char contents[1000]; //contents of the directory
			memset(contents, '\0', sizeof(contents));
			int numFiles = 0;	//number of files in the directory	
	
			//open directory
			if((directory = opendir(currDir)) != NULL){
				//while there are files to read
				while((ent = readdir(directory)) != NULL){
					//printf("%s\n", ent->d_name);
					strcat(contents, ent->d_name); //concatinate to contents
					strcat(contents, ","); //seperate by comman
					numFiles++; //increase number of files
				}
				//close directory
				closedir(directory);
			}	

			//send contents of directory
			printf("Sending directory contents to %s:%d\n", clientName2, dataPort);
			//printf("printing contents\n");
			//printf("%s\n", contents);

			int charsSent = send(clientConnection2, contents, strlen(contents),0);
			//printf("charsSent = %d\n", charsSent);
			if(charsSent < 0){
				printf("error in sending\n");
			}
	
			//close data socket
			close(listeningSocket2);
	
		}  	
		//check for get command
		else if(strncmp(messageBuffer, getCommand, 2) == 0){
			//get data port number
			memset(dataPortStr, '\0', sizeof(dataPortStr));
			//get length of port
			int portLen = atPos - percentPos -1;
			//copy into a string
			strncpy(dataPortStr, messageBuffer + percentPos + 1, portLen);
			//convert string to int
			dataPort = atoi(dataPortStr);
			
			//get text file name
			memset(fileName, '\0', sizeof(fileName));
			//get length of file name
			int fileNameLen = dollarPos - atPos - 1;
			//copy into a string
			strncpy(fileName, messageBuffer + atPos +1, fileNameLen);

			//print message
			printf("File \"%s\" requested on port %d\n", fileName, dataPort);

			//set up server address struct
			serverAddress2.sin_family = AF_INET;
			serverAddress2.sin_port = htons(dataPort);
			serverAddress2.sin_addr.s_addr = INADDR_ANY;
	
			//create socket
			listeningSocket2 = socket(AF_INET, SOCK_STREAM, 0);
			if(listeningSocket2 < 0){
				printf("Listneing socket 2 error\n");
				exit(1);
			}

			//bind
			if(bind(listeningSocket2, (struct sockaddr *) &serverAddress2, sizeof(serverAddress2)) < 0){
				printf("error on second bind\n");
				exit(1);
			
			}
			//begining second listening
			listen(listeningSocket2, 5);
			//printf("Server open on %d\n", dataPort);

			//accept connection
			clientAddrSize2 = sizeof(clientAddress2);
			clientConnection2 = accept(listeningSocket2, (struct sockaddr *) &clientAddress2, &clientAddrSize2);
			if(clientConnection2 < 0){
				printf("Error accepting second connection\n");
				exit(1);
			}

			//get client name a second time
			char clientName2[clientAddrSize2];
			memset(clientName2, '\0', sizeof(clientName2));
			inet_ntop(AF_INET, &clientAddress.sin_addr, clientName2, sizeof(clientName2));
			//printf("Connection from %s\n", clientName2);

			//check for file in directory
			DIR *directory;
			struct dirent *ent;
			int numFiles = 0;	//number of files in directory	
			int cmpResult;		//for comparing file name to files in directory
			int fileFound = 0;	//if file is in directory
			FILE *fptr;		//file pointer
			char fileContents[1024]; //file contents to be sent in 1024 byte chunks
		
			//get current working directory
			char currDir[1000];
			memset(currDir, '\0', sizeof(currDir));
			getcwd(currDir, sizeof(currDir));
	
			//open current working directory
			if((directory = opendir(currDir)) != NULL){
				//while there are files to read in the directory
				while((ent = readdir(directory)) != NULL){
					//printf("%s\n", ent->d_name);
					//compare file name to file in directory
					cmpResult = strcmp(ent->d_name, fileName);
					if(cmpResult == 0){
						//if file was found , set variable
						fileFound = 1;
					}
					//numFiles++;
				}
				//close the directory
				closedir(directory);
			}

			//if the file does not exist
			if(fileFound == 0){
				//print message on server
				printf("File not found. Sending Error message to %s:%d\n", clientName, dataPort);
				
				//create message to send
				char errormesg[100];
				memset(errormesg, '\0', sizeof(errormesg));
				strcat(errormesg, clientName);
				strcat(errormesg, ":");
				strcat(errormesg, dataPortStr);
				strcat(errormesg, " says FILE NOT FOUND");
	
				//send message
				int errorSnt = send(clientConnection2, errormesg, strlen(errormesg), 0);
				if(errorSnt < 0){
					printf("Error sending error message\n");
				}
			}
			//if file is in directory
			else if(fileFound == 1){
				//clear out file conents buffer
				memset(fileContents, '\0', sizeof(fileContents));
	
				//open file
				fptr = fopen(fileName, "r");
				if(fptr == NULL){
					printf("Cannot open file\n");
					exit(1);
				}
				//get size of file
				fseek(fptr, 0, SEEK_END); //put pointer at end of file
				int fileSize = ftell(fptr); 
				//printf("fileSize = %d\n", fileSize);
				//set back to beginning of file
				fseek(fptr, 0, SEEK_SET);
				
				//set total amount of bytes sent to 0
				int totalSent = 0;
			
				//send message to screen
				printf("Sending \"%s\" to %s:%d\n", fileName, clientName, dataPort);
	
				//loop through send() to send files in 1024b chunks
				//while the total amount of data sent is less than the file size
				while(totalSent < fileSize){
					//clear out file conents buffer
					memset(fileContents, '\0', sizeof(fileContents));	
		
					//read the file to the contents
					fread(fileContents, sizeof(fileContents), 1, fptr);
				
					//printf("%s\n", fileContents);
	
					//send data
					int charsSent = send(clientConnection2, fileContents, strlen(fileContents),0);
					//printf("strlen(fileContents) = %d\n", strlen(fileContents));
					//printf("charsSent = %d\n", charsSent);
					if(charsSent <= 0){
						printf("error in sending\n");
						exit(1);
					}
					if(charsSent < strlen(fileContents)){
						printf("not all data in file was sent\n");
					}
	
					//add bytes sent to running total
					totalSent += charsSent;
				}
				//close file
				fclose(fptr);
	
				//send closing byte @
				memset(fileContents, '\0', sizeof(fileContents)); //clear buffer
				char closing[1]; //create closing char
				memset(closing, '\0', sizeof(closing));
				strcpy(closing, "@");
				int closeSent = send(clientConnection2, closing, strlen(closing), 0); //send closing char
				
				//printf("closing charsSent: %d\n", closeSent);
				if(closeSent < 0){
					printf("error in closing send message\n");
				}
			}
		}
		//if an invalid command was sent
		else{
			//printf("invalid command\n");
			//create message to send to client
			char invalid[100];
			memset(invalid, '\0', sizeof(invalid));
			strcpy(invalid, "From Server - Invalid Command\n");
			int invalidSend = send(clientConnection, invalid, strlen(invalid), 0);
			if(invalidSend < 0){
				printf("Error sending invalid command message\n");
			}
		}
	}

	//close the listening socket
	close(listeningSocket);

	return 0;
}
