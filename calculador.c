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

int canal, numero;
int fd[2], pid0, pid1, estat;

void acabar() {
	close(fd[0]);
	exit(1);
}

void agafarhexa() {
	read(fd[0], &numero, sizeof(int));
	printf ("El valor %d en hexadecimal es %x.\n", numero, numero);
}

void agafaroctal() {
	read(fd[0], &numero, sizeof(int));
	printf ("El valor %d en octal es %o.\n", numero, numero);
}

int main() {
	signal(SIGUSR2, acabar);
	if(mkfifo("./canal",00660)!=0) {
		perror("ERROR! No s'ha pogut crear la named pipe\n");
		exit(1);
	}
	
	canal = open("./canal", O_RDONLY); 	// obrim pipe amb nom per connectar amb entrada
	if (canal<0) {
		perror("ERROR! No s'ha pogut obrir la named pipe\n");
		exit(1);
	}
	// Fill 1
	pipe(fd);
	pid0 = fork();
	if(pid0 == 0) {
		signal(SIGUSR1, agafarhexa);
		signal(SIGUSR2, acabar);
		close(fd[1]);
		
		while(1) {
			pause();
		}
	}
	else if (pid0 < 0) {
		perror("ERROR! No s'ha pogut mostrar el valor en hexadecimal\n");
		exit(1);
	}
	
	// Fill 2
	pid1 = fork();
	if (pid1 == 0) {
		signal(SIGUSR1, agafaroctal);
		signal(SIGUSR2, acabar);
		close(fd[1]);
		
		while(1) {
			pause();
		}
	}
	else if (pid1 < 0) {
		perror("ERROR! No s'ha pogut mostrar el valor en octal\n");
		exit(1);
	}

	// Pare
	char opcio;
	while(read(canal, &opcio, sizeof(char)) > 0) {
		if (read(canal, &numero, sizeof(int)) < 0) {
			perror("ERROR! No s'ha pogut llegir el numero del canal\n");
			exit(1);
		}
		if (write(fd[1], &numero, sizeof(int)) < 0) {
			perror("ERROR! No s'ha pogut escriure el numero a la pipe\n");
			exit(1);
		}
				
		if (opcio == 'o') {
			kill(pid1,SIGUSR1);
		}
		else { 
			kill(pid0,SIGUSR1);
		}
	}
	kill(pid0,SIGUSR2);
	kill(pid1,SIGUSR2);
	wait(&estat); wait(&estat);
	
	close(canal);
	close(fd[1]); close(fd[0]);
	unlink("./canal");
	exit(0);
}