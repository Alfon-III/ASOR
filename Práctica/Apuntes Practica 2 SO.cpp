/*

Ejercicio 1. La herramienta principal para consultar el contenido y atributos 
básicos de un fichero es ls. Consultar la página de manual y estudiar el uso de 
las opciones:
-a -l -d -h -i -R -1 -F y --color. Estudiar el significado de la 
salida en cada caso.


-a: all
-l: Lista larga
-d: Muestra directorios
-h: Legible para humanos
-i: inode
-R: Recursive
-1: One file per line
-F: classify
--color: Colorea los tipos de archivos

Para ver los permisos usamos ls -l

Ejercicio 4

Crear un directorio 
y quitar los permisos de ejecución para usuario, 
grupo y otros. Intentar cambiar al directorio.

Ejercicio 2. El modo de un fichero es <tipo><rwx_propietario><rwx_grupo><rwx_resto>:
rwx: r lectura (4); w escritura (2); x ejecución (1)
Comprobar los permisos de algunos directorios (con ls -ld). 

chmod u+rx,g+r-wx,o-wxr fichero

con esta notación, añadimos o quitamos permisos a partir de lo por defecto

Ejercicio 5. Escribir un programa que, usando la llamada open(2), 
cree un fichero con los permisos rw-r--r-x. 
Comprobar el resultado y las características del fichero con la orden ls.

645
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//!!!No olvidar los dos 0 al principios
int main(){
	open("hola.txt", O_CREAT, 00645);
	return 1;
}

/*
Ejercicio 6. Cuando se crea un fichero, los permisos por defecto se derivan de la máscara de usuario (umask). 
El comando interno de la shell umask permite consultar y fijar esta máscara. Usando este comando, fijar la máscara de forma que los nuevos 
ficheros no tengan permiso de escritura para el grupo y ningún permiso para otros. Comprobar el funcionamiento con los comandos touch, mkdir y ls.

umask 0027 para aplicar un amáscara a lo establecido
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(){
	//Cambiamos la umask, creamos el archivo y lo dejamos tal y como estaba
	mode_t past = umask(027);

	//No olvidar poner los permisos del archivo
	open("chema.txt", O_CREAT, 00645);
	umask(past);

	return 1;
}

/*Ej 9*/

//Encontrado al final de man 2 stat, simplemente hay que cambiar un par de cosas (quitar arg)
//y mover las cosas de sitio

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

int
main()
{
   struct stat sb;

   if (stat("chema.txt", &sb) == -1) {
       perror("stat");
       exit(EXIT_FAILURE);
   }

   printf("ID of containing device:  [%lx,%lx]\n",
          (long) major(sb.st_dev), (long) minor(sb.st_dev));

   printf("I-node number:            %ld\n", (long) sb.st_ino);

   printf("File type:                ");

   switch (sb.st_mode & S_IFMT) {
   case S_IFBLK:  printf("block device\n");            break;
   case S_IFCHR:  printf("character device\n");        break;
   case S_IFDIR:  printf("directory\n");               break;
   case S_IFIFO:  printf("FIFO/pipe\n");               break;
   case S_IFLNK:  printf("symlink\n");                 break;
   case S_IFREG:  printf("regular file\n");            break;
   case S_IFSOCK: printf("socket\n");                  break;
   default:       printf("unknown?\n");                break;
   }

   printf("Last status change:       %s", ctime(&sb.st_ctime));
   printf("Last file access:         %s", ctime(&sb.st_atime));
   printf("Last file modification:   %s", ctime(&sb.st_mtime));

   exit(EXIT_SUCCESS);
}

/*Ejercicio 11:

una ruta

Si es un fichero regular-> Enlace simbólico y rígido

*/


#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

int
main(int argc, char *argv[]){

	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}




	if (S_ISREG(sb.st_mode)){

		/*Creamos los nombres de los nuevos archivos pero soy gilipollas 
		char hard = strcat(argv[1], ".hard");
		char sym = strcat(argv[1], ".syn");

		printf("HARD: %s\n", hard);
		printf("SYM: %s\n", sym);

		//Creamos enlace simbólico*/

		if(symlink(argv[1], "nuevo.sym") == -1){
			perror("Error alcrear enlace simbolico");
		}
		else{
			printf("Enlace simbolico creado\n");
		}


		//Creamos enlace rígido
		if(link(argv[1], "nuevo.hard") == -1){
			perror("Error alcrear enlace rígido");
		}
		else{
			printf("Enlace rígido creado\n");
		}

	}
	else{
		printf("No es un fichero regular\n");
	}

	exit(EXIT_SUCCESS);
}

/*Ejercicio 12*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

int
main(int argc, char *argv[]){

	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_CREAT | O_RDWR);

	if(fd == -1){
		perror("Error al crear el archivo");
	}

	/*
		STDIN_FILENO = 0
		STDOUT_FILENO = 1
		STDERR_FILENO = 2
	*/

	dup2(fd, 1);
	dup2(fd, 2);
	perror("JAjA ha fallado")
	printf("Uwu, estamos en %s")

	return 1;
}

/*Ejercicio 16*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]){

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[1]);
		return 1;
	}

	int fd = open(argv[1], O_CREAT | O_RDWR, 00777);

	if(fd == -1){
		perror("Error al abrir/crear el archivo");
	}

	//Cerrojo

	struct flock lock;

	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();

	//Obtenemos cerrojo que estuviese o lo ponemos
	int s = fcntl(fd, F_GETLK, &lock);

	if(lock.l_type == F_UNLCK){
		printf("[Cerrojo desbloqueado] \n Bloqueo en procso... \n");
		//Si está desbloqueado, lo bloqueo + Control de errores

		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();

		//Control de errores

		if(fcntl(fd, F_SETLK, &lock) == -1){
			perror("Error al crear cerrojo bloqueado");
		}
		else{
			printf("[Cerrojo Bloqueado]: Procedemos a la escritura\n");

			//Creamos la hora
			time_t aux_time = time(NULL);
			struct tm *aux_tm = localtime(&aux_time);
			char *buff[500];
			strftime(buff, sizeof(buff), "%A, %d de %B de %Y, %H:%M", aux_tm);

			write(fd, &buff, strlen(buff));
			

			sleep(30);

			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = 0;
			lock.l_len = 0;
			lock.l_pid = getpid();

			printf("Desbloqueando cerrojo...\n");

			if(fcntl(fd, F_SETLK, &lock) == -1){
				perror("Error al crear cerrojo desbloqueado");
				close(fd)S
				return 1;
			}
			else{
				printf("BRO SOS LIBRE\n");
			}
			close(fd);
		}

	}
	else{
		printf("Cerrojo Bloqueado \n");
	}

	return 1;
}
