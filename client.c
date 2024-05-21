#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int canal;
char opcio;

void mostrar_menu() { 
	printf("\n---------------------\n");
	printf("|      OPCIONS      |\n");
	printf("|-------------------|\n");
	printf("| o -> Octal        |\n");
	printf("| h -> Hexadecimal  |\n");
	printf("| x -> Exit         |\n");
	printf("---------------------\n");
}	

int main() {	
	canal = open("./canal", O_WRONLY); 	// obrim pipe amb nom per connectar amb entrada
	if (canal<0) {
		perror("ERROR! No s'ha pogut obrir la pipe amb nom\n");
		exit(1);
	}
	
	mostrar_menu();
	// Llegir opció 
	printf("Opció: ");
	scanf(" %c", &opcio);
	
	while (opcio != 'x') {
		if (opcio == 'h' || opcio == 'o') {
			// Llegir número ----------------------------
			int numero;
			printf("Entra un valor de màxim 3 xifres: ");
			scanf(" %d", &numero);
			
			if(write(canal, &opcio, sizeof(char)) == -1) {
				perror("ERROR! No s'ha pogut escriure a la named pipe\n");
				exit(1);
			}
			if(write(canal, &numero, sizeof(int)) == -1) {
				perror("ERROR! No s'ha pogut escriure a la named pipe\n");
				exit(1);
			}
		}
		else printf("Opcio incorrecte\n");
		
		usleep(2000);
		// Llegir opció 
		printf("\nOpció: ");
		scanf(" %c", &opcio);
	}
	close(canal);
	unlink("./canal");
	printf("FI :)\n");
	exit(0);
}