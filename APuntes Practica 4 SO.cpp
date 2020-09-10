#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int
main(int argc, char *argv[]){

  int pipefd[2];
  pid_t cpid;
  char buf;

  if (argc != 5) {
  printf("Faltan argumentos");
  exit(EXIT_FAILURE);
  }
  //Creamos la tubería
  if (pipe(pipefd) == -1) {
	 perror("pipe");
	 exit(EXIT_FAILURE);
  }

  //hacemos Fork
  cpid = fork();
  if (cpid == -1) {
	 perror("fork");
	 exit(EXIT_FAILURE);
  }

  if (cpid == 0) {//HIJO
	//Cerramos escritura
	
	close(pipefd[1]);  
	//Leemos mensaje
	int fd2 = dup2(pipefd[0], 0);
	close(pipefd[0]);

	//Procesamiento
	execlp(argv[3], argv[3], argv[4], NULL);
	_exit(EXIT_SUCCESS);

  } 
  else { //PADRE
	//Cerramos lectura
	close(pipefd[0]);          
	//Procesamiento
	int fd1 = dup2(pipefd[1], 1);
	close(pipefd[1]);

	//Ya hemos cerrado todos los descriptores no necesarios
	execlp(argv[1], argv[1], argv[2], NULL);

	wait(NULL);                /* Wait for child */
	exit(EXIT_SUCCESS);

  }

}

//Tuberías sin nombre bidireccional

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int
main(){

  int p_h[2];
  int h_p[2];

  pid_t cpid;

  //Creamos la tubería
  if (pipe(p_h) == -1) {
	 perror("pipe");
	 exit(EXIT_FAILURE);
  }

  if (pipe(h_p) == -1) {
	 perror("pipe");
	 exit(EXIT_FAILURE);
  }

  //hacemos Fork
  cpid = fork();
  if (cpid == -1) {
	 perror("fork");
	 exit(EXIT_FAILURE);
  }
  else if(cpid == 0){ //HIJO
	//Cerramos lectura hijo y escritura padre
	close(p_h[1]);
	close(h_p[0]);
	char aux, response;
	char sms[255];
	int count = 0;

	while(read(p_h[0], &aux, 1) > 0 && aux != '\n')
		write(STDOUT_FILENO, &aux, 1);

	write(STDOUT_FILENO, "\n", 1);

	count++;

	if(count == 10){
		aux = 'q';
		write(h_p[1], &aux, 1);
		close(p_h[0]);
		close(h_p[1]);
		exit(EXIT_SUCCESS);
	}
	else{
		aux = '1';
		write(h_p[1], &aux, 1);
	}


  } 
  else { //PADRE
	//Cerramos lectura padre y escritura hijo
	close(p_h[0]);
	close(h_p[1]);
	int i;
	char aux, response;
	char sms[255];
	while(1){

		//Leer de la entrada estandar es una putisimam mierda y se hace así
		int dummy = read(STDIN_FILENO, &aux, 1);
		while(dummy > 0 && aux != '\n'){
			sms[i] = aux;
			i++;
			dummy = read(STDIN_FILENO, &aux, 1);
		}

		sms[i] = '\n';
		sms[i + 1] = '\0';

		write(p_h[1], &sms, strlen(sms));

		//esperamos lectura hijo
		read(h_p[0], &response, 1);

		if(response = '1'){
			printf("RECIVIDO!\n");
		}
		else{//Response = 'q'
			close(p_h[1]);
			close(h_p[0]);

			exit(EXIT_SUCCESS);
		}
	}
  }
}

//Ejercicio 4 


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



int
main(int argc, char *argv[]){

  if (argc != 2) {
  printf("Faltan argumentos");
  exit(EXIT_FAILURE);
  }

  //char *HOME = getenv("HOME");
  char *tuberia = "tuberia";

  //printf("HOME: %s\n", tuberia);

  mkfifo(tuberia, 00777);

  int fd = open(tuberia, O_WRONLY);
  if(fd == -1){
  	perror("Error al abrr la tuberia");
  }

  write(fd, argv[1], strlen(argv[1]));
  close(fd);
  return 1;


}

/*
Para ejecutarlo:
./exe loququieras

En otro terminal
cat tuberia

Apreciaciones mínimas: Por el amor de Cristo, en la direccion de la 
tubería pon la dirección correcta. No hay más misterio

El orden de hacer las cosas es:

1. Mkfifo para crear tuberia con nombre, no olvides poner la mask
si pones 00777 no te equivocarás casi nunca (depende del enunciado)
2. Hacer el open de dicha tubería, con la flag de solo escritura! 
3. Escribir con buena letra en la tubería
EXTRA: Si cierras el fichero, el profe te da un abrazo. Bueno eso no, 
pero no te restará 0,5 puntos lo cual está batante bien. 


Haz un buen control de errores y te queda Niquelodeon!
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>


int main() {
	fd_set rfds;
	struct timeval tv;
	int retval;

	//1. Abro las tuberías + control de errores

	int fd1, fd2;
	char buff[256];

	fd1 = open("tuberia", O_NONBLOCK | O_RDONLY);
	if(fd1 == -1){
		perror("Error al abrir la tubería 1");
		//Error dado por que no se ha encontrado el archivo con la tubería
		//mkfifo tuberia o está en otro directorio
	}


	fd2 = open("tuberia2", O_NONBLOCK | O_RDONLY);
	if(fd2 == -1){
		perror("Error al abrir la tubería 2");
	}

	while(1){
		printf("Entramos al while\n");
		//Creamos un conjunto vacío al que le añadimos las 2 pipes
		FD_ZERO(&rfds);
		FD_SET(fd1, &rfds);
		FD_SET(fd2, &rfds);//XD, gracias darío por ver q no es FD_SET(fd1, &rfds);
		//Ajustamos el temporizador para establecer la conexióne entre las dos tuberías

		tv.tv_sec = 20;
		tv.tv_usec = 0;

		//Calculamos el máximo de las dos tuberías y le añadimos + 1, acorde al manual
		//Yo no pongo las normas eh...

		int max_fd;

		//Con las 2 pipes abiertas entramos en este bucle infinito
		//Donde haremos que ambas pipes se comuniquen


		if(fd1 > fd2){
			max_fd = fd1 + 1;
		}
		else{
			max_fd = fd2 + 1;
		}
		printf("PRE SELECT\n");
		retval = select(max_fd, &rfds, NULL, NULL, &tv);
		printf("POST SELECT\n");
		/* Don't rely on the value of tv now! */

		if (retval == -1){
			perror("ERROR en SELECT");
			exit(EXIT_FAILURE);
		}

		else if (retval == 0){
			printf("TIMEOUT");
			exit(EXIT_SUCCESS);
		}

		printf("Accedimos a la pipe, vamos bien\n"); 
		if(FD_ISSET(fd1, &rfds)){
			printf("TUBERIA 1: ");
			while(read(fd1, &buff, 256) > 0){
				printf("%s", buff);
			}
			printf("\n");
			close(fd1);
			fd1 = open("tuberia", O_NONBLOCK | O_RDONLY);
			if(fd1 == -1){
				perror("Error al abrir la tubería 1");
			}
		}
		if(FD_ISSET(fd2,&rfds)){
			printf("TUBERIA 2: ");
			while(read(fd2, &buff, 256) > 0){
				printf("%s", buff);
			}
			printf("\n");
			close(fd2);
			fd2 = open("tuberia2", O_NONBLOCK | O_RDONLY);
			if(fd2 == -1){
				perror("Error al abrir la tubería 2");
			}
		}
	}

	

	exit(EXIT_SUCCESS);
}

/*

1. open pipes
while(1){
	2. SET + tv
	3. select(selector)// no olvides el  + 1 el max
	4. Conrol de errores
	//we re in bois
	if(FD_ISSET(fd1, selector))
		1. We are here
		2. while read > 0
			write(stdout buf)
		3. close
		4. open + control de errores
	if(fd2)...
}

*/