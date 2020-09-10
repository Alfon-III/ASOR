
/*
Parctica 1 SO

Ejercicio 1

Consultar:
man 3 perror
man 2 setuid 

Para coger las liberrías y ver que si uid == -1 es que se ha dado un error,
el cual debemos retornar.

*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


int main(){

	char *s;

	if(setuid(0) == -1){
		perror(s);
	}
	return 1;
}

/*Debería mostrar: Operation no permited
Pues estamos con un usuario normal y si lo ejecutamos como "Root" no sale nada

Para compilar un programa:

sudo gcc -o solucion ej1.c
(el archivo a ejecutar sería "solucion")

Para ejecutar
./solucion
*/
//-------------------------------------------------------
/*Ejercicio 2:

Ahora queremos el número de error y el texto asociado a éste

Gracias a la biblioteca errno.h crea la variable errno donde se guarda el número 
de error cometido, en este caso como falla el setuid se guarda el número de error

La diferencia respecto a perror es que perror imprime directamente el error.
Es lo que vamos a usar por regla general´.

*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>



int main(){

	if(setuid(0) == -1){
		printf("Error:  %s %d \n", strerror(errno), errno);
	}
	return 1;
}

//-------------------------------------------------------

/*Ejercicio 3*/

/*Hay como 133 tipos de errorers y por suerte nosotros no somos ninguno de ellos :D
*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(){

	int i;
	for(i = 1;i < 255; i++){
		printf("Error:  %s\n", strerror(i));
	}

	return 1;
}

/*
Ejercicio 4

Muestra información del sistema:

Linux localhost.localdomain 3.10.0-862.11.6.el7.x86_64 #1 SMP 
Tue Aug 14 21:49:04 UTC 2018 
x86_64 x86_64 x86_64 GNU/Linux

Ejercicio 5
Si consultamos el man 2 unmae

#include <sys/utsname.h>

       int uname(struct utsname *buf);

DESCRIPTION
       uname()  returns  system information in the structure pointed to by buf.  The
       utsname struct is defined in <sys/utsname.h>:

           struct utsname {
               char sysname[];    /* Operating system name (e.g., "Linux") 
               char nodename[];   /* Name within "some implementation-defined
                                     network" *
               char release[];    /* Operating system release (e.g., "2.6.28") *
               char version[];    /* Operating system version *
               char machine[];    /* Hardware identifier *
           #ifdef _GNU_SOURCE
               char domainname[]; /* NIS or YP domain name *
           #endif
           };

       The length of the arrays in a struct utsname is unspecified (see NOTES);  the
       fields are terminated by a null byte ('\0').


*/


#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>


int main(){

	struct utsname buf;

	if(uname(&buf)== -1){
		perror("La jodimos en UNAME");
	}
	else{
		printf("OS: %s \n", buf.sysname);
		printf("Nodename: %s \n", buf.nodename);
		printf("Release: %s \n", buf.release);
		printf("Version: %s \n", buf.version);
		printf("Mechine: %s \n", buf.machine);
	}

	return 1;
}

/*Obtenemos:

[cursoredes@localhost p0]$ ./ej5
OS: Linux 
Nodename: localhost.localdomain 
Release: 3.10.0-862.11.6.el7.x86_64 
Version: #1 SMP Tue Aug 14 21:49:04 UTC 2018 
Que cabrón, no ha cambiado la máquina desde hace 2 años 

Mechine: x86_64 
*/

/*Ejercicio 6

Cuidado, hay que usar la parte de la derecha de lo que salga en man 3 sysconf
es decir que tenga el "_SC_" al principio del parámetro que quieras

*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>

int main(){
	printf("Max argumentos: %d \n", sysconf(_SC_ARG_MAX));
	printf("Max hijos: %d \n", sysconf(_SC_CHILD_MAX));
	printf("Max ficheros: %d \n", sysconf(_SC_OPEN_MAX));
	return 1;
}

/*Ejercicio 7*/


#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>

int main(){

	printf("Max enlaces: %d \n", pathconf("/",_PC_LINK_MAX));
	printf("Max tam ruta: %d \n", pathconf("/",_PC_PATH_MAX));
	printf("Max nombre de fichero: %d \n", pathconf("/",_PC_NAME_MAX));

	return 1;
}

/*
Tiene sentido que el máximo tamaño de nombre sea 255 caracteres.
Osea ya me jodería que el nombre de mi archivo no quea ni en un tweet.
Hazte una carpeta gañán!

Max enlaces: 2147483647 
Max tam ruta: 4096 
Max nombre de fichero: 255 


Ejercicio 8

Ejercicio 9

El setuid se utiliza para dar privilegios elevados en determinados archivos para 
usuarios que no lo poseen, como por ejemplo en el archivo 
/etc/bin/passwd para que todos puedan cambiar la contraseña pero que no puedan 
leer ni escribir las otras. Por lo tanto, cuando el uid es distinto del 
euid tiene el bit setuid activado.

*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>

int main(){

	printf("User ID: %d \n", getuid());
	printf("Effective ID: %d \n", geteuid());

	return 1;
}

/*Ejercicio 10*/


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>


int main(){

	struct passwd *pw = getpwuid(getuid());
 	
 	printf("Nombre Usuario: %s \n", pw->pw_name);
 	printf("Home Dir: %s \n", pw->pw_dir);
 	printf("Descrip Usuario: %s \n", pw->pw_gecos);

	return 1;
}

/*Ejercicio 12*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>

int main(){
	time_t t;
	t =  time(NULL);

	printf("Tiempo desde Epoch: %d\n", t);

	return 1;
}

/*Ejercicio 13*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <sys/time.h>


int main(){

	struct timeval tv1, tv2;
	int ini, fin;

	ini = gettimeofday(&tv1, NULL);
	if(ini == -1)
		perror("Fallo obteniendo TV 1");

	int i = 0;

	for(i = 0; i < 1000000; i++ ){
		//Nada
	}

	fin = gettimeofday(&tv2, NULL);
	if(fin == -1)
		perror("Fallo obteniendo TV 1");

	printf("Tiempo transcurrido: %d\n", tv2.tv_usec - tv1.tv_usec );

	return 1;
}


/*Ejercicio 14*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>

int main(){

	struct tm *tiempo;
	time_t t;
	t =  time(NULL);

	tiempo = localtime(t);

	size_t strftime(char *s, size_t max, const char *format, &tiempo);


	printf("Year: %d", tiempo->tm_year + 1900);

	return 1;
}



#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int
main()
{
   char outstr[200];
   time_t t;
   struct tm *tmp;

   t = time(NULL);
   tmp = localtime(&t);
   if (tmp == NULL) {
       perror("localtime");
       exit(EXIT_FAILURE);
   }
   //Para ponerlo en Español... ------------------!!!--------------------------
   setlocale(LC_ALL,"es_ES");
   if (strftime(outstr, sizeof(outstr), "%A, %d de %B de %Y, %H:%M", tmp) == 0) {
       fprintf(stderr, "strftime returned 0");
       exit(EXIT_FAILURE);
   }
   printf("Result string is \"%s\"\n", outstr);
   exit(EXIT_SUCCESS);
}
