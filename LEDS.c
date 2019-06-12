/* Einfacher Mirrorserver in C mit Threads*/
/*von Franz Pfleger 
 * steuerung der LEDs mit einer Zahl zwischen 0 und 15
 * zum beenden q drücken
 * zum kompelieren 'gcc LEDS.c -o LEDS -Wall -lpthread -lwiringPi' in die Konsole eingeben
 * zum ausführen './LEDS' in die konsole eingeben
 * 
 * um sich mit dem server zu verbinden in einer anderen Konsole 'nc localhost 4000' eingeben
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>

#define LED1 18
#define LED2 23
#define LED3 24
#define LED4 25

#define BUFSIZE 1000
#define PORT 4000

//Warteschlange fuer Verbindungen
#define QUEUE 3

//Mirror-Funktion (Prototyp für pthread_create)
void *mirror(void* arg)
{
	wiringPiSetupGpio();
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	
	digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED3, 0);
    digitalWrite(LED4, 0);
    
	int clientfd = *(int *)arg; //typecast
	char inbuffer[BUFSIZE];
	int decz = 0;
	
	//Begrüßung
	write(clientfd, "Welcome to LED Server 2!\n\r", 27);
	write(clientfd, "\nUm eine dez. Zahl in eine in bin. auf den LEDs anzeigen zu lassen gib eine Zahl zwischen 0 und 15 ein.\n\r", 107);
	write(clientfd, "Um das Programm zu beenden drücke 'q'\n\n\r", 43);
	while(1){
		read(clientfd, inbuffer, sizeof(inbuffer));  
		
		//q zum beenden des Programms mit verabschiedung und reset der LEDs
		if(inbuffer[0] == 'q'){
			write(clientfd, "Bye Bye\n\r", 12);
			digitalWrite(LED1, 0);
			digitalWrite(LED2, 0);
			digitalWrite(LED3, 0);
			digitalWrite(LED4, 0);
			break;
		};
			
		decz = atoi(inbuffer);
		
		if(decz >= 0 && decz <= 15){	//findet heraus ob Zahl gültig ist gegenstück dazu ist in Zeile 168 :)
			if(decz % 2 == 1){			//Albtraum eines jeden Software Technikers EINE IF VERSCHATELUNG
				digitalWrite(LED1, 1);
				decz = (decz-1)/2;
				if(decz % 2 == 1){
					digitalWrite(LED2, 1);
					decz = (decz-1)/2;
					if(decz % 2 == 1){
						digitalWrite(LED3, 1);
						decz = (decz-1)/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					}else{
						digitalWrite(LED3, 0);
						decz = decz/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					};
				}else{
					digitalWrite(LED2, 0);
					decz = decz/2;
					if(decz % 2 == 1){
						digitalWrite(LED3, 1);
						decz = (decz-1)/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					}else{
						digitalWrite(LED3, 0);
						decz = decz/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					};
				};
			}else{
				digitalWrite(LED1, 0);
				decz = decz/2;
				if(decz % 2 == 1){
					digitalWrite(LED2, 1);
					decz = (decz-1)/2;
					if(decz % 2 == 1){
						digitalWrite(LED3, 1);
						decz = (decz-1)/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					}else{
						digitalWrite(LED3, 0);
						decz = decz/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					};
				}else{
					digitalWrite(LED2, 0);
					decz = decz/2;
					if(decz % 2 == 1){
						digitalWrite(LED3, 1);
						decz = (decz-1)/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					}else{
						digitalWrite(LED3, 0);
						decz = decz/2;
						if(decz % 2 == 1){
							digitalWrite(LED4, 1);
							//break;
						}else{
							digitalWrite(LED4, 0);
							//break;
						};
					};
				};
			};
		}else{
			write(clientfd, "\n\nDeine Zahl liegt nicht im gültigen Werte Bereich bitte versuche es noch einmal!\n\r",88);
			write(clientfd, "Gültiger Werte Bereich: 0 bis 15\n\n\r",38);
		};			
		};

	
	close(clientfd);
	return NULL;
}
//arg: 
int main()
{
	//Socket
	int server_socket, rec_socket;
	unsigned int len;
	struct sockaddr_in serverinfo, clientinfo;
	
	// Serversocket konfigurieren
	server_socket = socket(AF_INET, SOCK_STREAM, 0 );	//TCP
	serverinfo.sin_family = AF_INET;
	//Hoert auf allen Iterfaces: 0.0.0.0
	serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverinfo.sin_port = htons(PORT);
	
	//Verbindet Socket mit IP-Adresse und Port
	if (bind(server_socket, (struct sockaddr *) &serverinfo, sizeof(serverinfo)) != 0){
		printf("Fehler Socket\n");
		return 1; //Rueckgabe Fehlercode
	}
	
	
	listen(server_socket, QUEUE); //Server wartet auf connect von Client
	
	
	// Endlosschleife Server zur Abarbeitung der Client Anfragen
	while(1){
		printf("Server wartet......\n");
		//Verbinung von Client
		rec_socket = accept(server_socket, (struct sockaddr *)&clientinfo, &len);
		printf("Verbindung von %s:%d\n", inet_ntoa(clientinfo.sin_addr), ntohs(clientinfo.sin_port));
		
		pthread_t child;
		if (pthread_create(&child, NULL, mirror, &rec_socket) != 0) {
			perror("child error"); // Fehlerfall : Abbruch
			return 1;
		}
		else { //Kind erzeugt:
			printf("Abgekoppelt!\n"); 
			pthread_detach(child); // abkoppeln vom Hauptprozess
		}
		
	}


	return 0;
}
