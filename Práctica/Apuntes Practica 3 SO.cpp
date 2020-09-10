/*Ejercicio 1
Mirar el PID con TOP y consultar los manuales:

Para poner valores negativos hemso de ser ROOT
Para sacae el ID de shell /7 bash ponemos "echo $$"

chrt -f -p 12 1840
renice -10 -p 1360
*/


/*Ejercicio 2
Politica de planificacion
Prioridad del proceso actual
Valores max y min de prioridad para la politica de planificacion
*/

#include <sched.h>


int main(){

	int planificador = sched_getscheduler(0);

	switch(planificador){
		case SCHED_FIFO: printf("FIFO \n"); break;
		case SCHED_RR: printf("Round Robin \n"); break;
		case SCHED_OTHER: printf("OTHER \n"); break;
		default: perror("Fallo al encontrar planificación"); break;
	}


	//Prioridad del proceso actual

	struct sched_param param;
	if(sched_getparam(0, &param) == -1){
		perror("Error al obtener prioridad del proceso");
	}

	printf("Prioridad del proceso: %d", param.sched_priority);

	//Prioridad maxima y minima de la politica de planificacion

	printf("Min: %d \n Max: %d \n", sched_get_priority_min(planificador),
		sched_get_priority_max(planificador));

	return 1;
}


/*Ejercicio 5

Id procso, Ip Proceso padre, grupo y IDSesion
Max archivos que puede abrir el proceso & el cirectorio de trabajo actual

*/

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>


int main(){

	printf("PID: %d \n", getpid());
	printf("PPID: %d \n", getppid());
	printf("Process GROUP ID: %d \n", getpgid(getpid()));
	printf("SID: %d \n", getsid(getpid()));

	struct rlimit aux;
	
	if(getrlimit(RLIMIT_NOFILE, &aux) == -1){
		perror("Error al obtener Rlimit");
	}

	printf("MAX ARCHIVOS: %d \n", aux.rlim_max);

	//En el chuletario no dice como cnsultar el directorio de trabajo sí que XD
	printf("XD \n");

	return 1;
}




/*Ejercicio 6*/

#include <unistd.h> //Fork
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

void muestraInfo(char *user){

	printf("------ %s", user);
	printf("------\n");

	printf("PID: %d \n", getpid());
	printf("PPID: %d \n", getppid());
	printf("Process GROUP ID: %d \n", getpgid(getpid()));
	printf("SID: %d \n", getsid(getpid()));
}

int main(){
	pid_t pid = fork();

	if(pid == -1){
		perror("Error en el fork");
	}
	//Proceso hijo
	else if(pid == 0){
		//Creamos nueva sesión
		pid_t sid_hijo = setsid();

		//Cambiamos el directorio
		if(chdir("/tmp") == -1){
			perror("Error al cambiar de directorio");
		}

		muestraInfo("Hijo");
	}
	else/*Proceso padre = 1*/{
		muestraInfo("Padre");
	}

	return 0;
}

/*Ejercicio 7*/
//Program dummy, muestra numeros cada 0.1 segundo.
int main(){
	int i = 0;
	for (i = 0; i <10 ; i++){
		printf("--- %d",i); printf(" ---\n");		
		usleep(100000);
	}
	return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){

	if (argc != 2) {
		perror("Introduzca el archivo a ejecutar");
		return 1;
	}

	char command[200] = "./";
	strcat(command, argv[1]);

	if(system(command)){
		perror("Error al ejecutar el programa");
	}

	printf("El comando terminó de ejecutarse\n");
	return 1;
}

/*
En terminal  scribir ./ej7 dummy y va DPM*/


/*Ejercicio 7b*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	if (argc != 2) {
		perror("Introduzca el archivo a ejecutar");
		return 1;
	}

	//int execvp(const char *file, char *const argv[]);
	char *arg[2];//donde pongo info adicional ["dir" y NULL AL FINAL!]
	arg[0] = "/"; 
	arg[1] = NULL;

	char *arg[] = {"/", NULL};

	char command[200] = "./";
	strcat(command, argv[1]);

	if(execvp(command, arg)){
		perror("Error al ejecutar el programa");
	}

	printf("El comando terminó de ejecutarse\n");
	return 1;
}

//Con execvp no nontinua la execucuin del programa principal
//Para continuarlo, dbería hecer un fork.
//justo lo que pide el ejercicio siguiente

/*Ejercicio 8*/

#include <unistd.h> //Fork
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



int main(int argc, char *argv[]){


	if (argc != 2) {
		perror("Introduzca el archivo a ejecutar");
		return 1;
	}

	char *arg[] = {"/", NULL};


	pid_t pid = fork();

	if(pid == -1){
		perror("Error en el fork");
	}
	//Proceso hijo
	else if(pid == 0){
		//Creamos nueva sesión
		pid_t sid_hijo = setsid();

		//Creamos los archivos/directorios
		int fd_err = open("/tmp/daemon.err", O_CREAT | O_RDWR, 00777);
		int fd_out = open("/tmp/daemon.out", O_CREAT | O_RDWR, 00777);
		int fd_in = open("/dev/null", O_CREAT | O_RDWR, 00777);

		/*STDIN_FILENO = 0
		STDOUT_FILENO = 1
		STDERR_FILENO = 2
		in, out, err*/

		//Duplicamos descriptor de fichero y redirigimos la salida ahí
		if(dup2(fd_in, 0) == -1){
			perror("Error al abrir el fichero IN");
		}
		if(dup2(fd_out, 1) == -1){
			perror("Error al abrir el fichero OUT");
		}
		if(dup2(fd_err, 2) == -1){
			perror("Error al abrir el fichero ERROR");
		}

		//Ejecutamos el programa
		char command[200] = "./";
		strcat(command, argv[1]);

		if(execvp(command, arg)){
			perror("Error al ejecutar el programa");
		}

	}
	else/*Proceso padre = 1*/{
		return 1;
	}

	return 0;
}

/*Ejercicio 11*/

#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>

int main() {
	sigset_t blk_set;

	int SLEEP_SECS = 60;

	//SET VACÍO
	sigemptyset(&blk_set);

	//SEÑALES CDE LAS QUE PROTEJEREMOS EL CÓDIGO
	sigaddset(&blk_set, SIGINT);
	sigaddset(&blk_set, SIGTSTP);



	printf("Captura de señales...\n");
	//SHIELDS UP!
	sigprocmask(SIG_BLOCK, &blk_set, NULL);

	printf("A mimir\n");
	sleep(SLEEP_SECS);

	//OTRO SET DONDE CAPTURAMOS LAS SEÑALS ENTRANTES
	sigset_t set;
	sigpending(&set);

	//SI SE CAPTURA ALGUNA, SE VA A LA VERGA EL ESCUDO
	if(sigismember(&set, SIGINT) == 1){
		printf("Obtenido SIGINT\n");
		sigdelset(&blk_set, SIGINT);
	}

	if(sigismember(&set, SIGTSTP) == 1){
		printf("Obtenido SIGTSTP\n");
		sigdelset(&blk_set, SIGTSTP);
	}
	printf("FIN captura de señales...\n");
	sigprocmask(SIG_UNBLOCK, &blk_set, NULL);

	return 1;

}



/*Ejercicio 12*/

#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>

int sINT = 0;
int stop = 0;

void handlerFunc(int signal){
	if(signal == SIGINT) sINT++;
	if(signal == SIGSTOP) stop++;
}

int main() {

	//HAndler para cada nueva señal de estos dos tipos, llamamos a esta funcion
	struct sigaction hand;
	hand.sa_handler = handlerFunc;

	sigaction(SIGINT, &hand, NULL);
	sigaction(SIGSTP, &hand, NULL);

	
	sigset_t set;
	sigemptyset(&set);

	//Con un sig vacío, por cada nueva señal, llamamos al handler
	while(sINT + stop < 10)
		sigsuspend(&set);

	printf("SIGINT: %d\n", sINT);
	printf("SIGSTP: %d\n", stop);
	return 1;

}

/*Ejercicio 12*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


volatile int boom = 0;

void handlerFunc(int signal){
	boom = 1;
}

//kill -s 10 PID
int main(int argc, char *argv[]){


	if (argc != 2) {
		perror("Introduzca el nº de segundos \n");
		return 1;
	}

	struct sigaction hand;
	hand.sa_handler = handlerFunc;

	sigaction(SIGUSR1, &hand, NULL);

	int secs = strtol(argv[1], NULL, 10);

	sleep(secs);

	if(boom == 0){
		unlink(argv[0]);
		printf("BOOOM! \n");
	}
		
	else{
		printf("Te salvaste wey\n");
	}

	return 1;
}