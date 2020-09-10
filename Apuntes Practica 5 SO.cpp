/*Configuramos los routers

VM1: 
sudo su
ip link set dev eth0 up
ip address add 192.168.0.1/24 dev eth0
ip -6 address add fd00::a:0:0:0:1/64 dev eth0

VM2:
sudo su
ip link set dev eth0 up
ip address add 192.168.0.100/24 dev eth0
ip -6 address add fd00::a:0:0:0:100/64 dev eth0

Hacer ping y comprobar conexión.
Si no has sido capaz, agárrate que se vienen curvas

*/

/*Ejercicio 1

La mayoría se ha sacado de man 3 getaddrinfo
que al final del todo, en "Server" viene un ejemplo con código
que he fusilado aquí. 
*/


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[])
{
	//Creamos muchas variables
   struct addrinfo hints;
   struct addrinfo *result, *rp;
   int sfd, s;
   struct sockaddr_storage peer_addr;
   socklen_t peer_addr_len;
   ssize_t nread;
   char buf[BUF_SIZE];

   //Comprobamos que introducimos 1 (una) unidad de argumento
   //Más te vale que sea localhost o ::1 o una ip bien chida
   if (argc != 2) {
       fprintf(stderr, "Usage: %s port\n", argv[0]);
       exit(EXIT_FAILURE);
   }

   //He borrado las que estaban a NULL, nadie las iba a echar en falta
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
   hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   hints.ai_protocol = 0;          /* Any protocol */

   //Ahora atención!
   //getaddrinfo recibe la direcion q le hemos pasado por argv[1]
   //luego recibe el puetro. En este caso nos la repanplinfa
   //La pasamos unas hints y nos devuelve en result, el resultado
   s = getaddrinfo(argv[1], NULL, &hints, &result);
   if (s != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
   }
/* getaddrinfo() returns a list of address structures.
      Try each address until we successfully bind(2).
      If socket(2) (or bind(2)) fails, we (close the socket
      and) try the next address. */

    //Aquí recorreremos todos los resultado obtenidos, 
    //Iremos pasando de uno a otro tal y como se ve en la diapositiva 16 del tema 2.5
    //Cada struct addrinfo de result tiene info de la dir actual y la direccion
    //de la siguiente en ai_next, y así avanzamos a ella.
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        //No hace falta el bind y socket por que este ejercico no requiere conexiones ni nada
        //Solo explorar los resultados ofrecidos por getaddrinfo

        //En estas variables guardamos informacion sobre el host y el servivio. Se usan al final del
        //codigo del servidor (el del man)
        char host[NI_MAXHOST], service[NI_MAXSERV];
        int num;

        s = getnameinfo(rp->ai_addr, rp->ai_addrlen,
          host, NI_MAXHOST,
          service, NI_MAXSERV ,NI_NUMERICHOST);
        //Ponemos la flag que os pide el enunciado

        //Imprimimos el host, la familia que está en rp / result y sel socktpe 
        printf("%s %d %d \n", host, rp->ai_family
          , rp->ai_socktype);

    }

	return 0;
}


/*Ejercico 2*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//Importar biblioteca time
#include <time.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    //Pasamos 2 argumentos, dir y host
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    //Hacemos get addr info pasándole la direccion y el host
    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
         break;                  /* Success */

            close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */
    for (;;) {
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
        (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;               /* Ignore failed request */

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *) &peer_addr,
        peer_addr_len, host, NI_MAXHOST,
        service, NI_MAXSERV, NI_NUMERICSERV);

        //Aquí podemos ver el valor del buffer, que es lo recibido

        if (s == 0)
            printf("Received %ld bytes from %s:%s = %s\n",
            (long) nread, host, service, buf);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));


        //TRABAJAMOS EL TIEMPO
        //man 3 strftime
        char outstr[200];
        time_t t;
        struct tm *tmp;
        t = time(NULL);
        tmp = localtime(&t);


        //Buf[0] es lo que he recibido del cliente del recvfrom
        if(buf[0] == 't'){
            strftime(outstr, sizeof(outstr), "%T", tmp);

            //outstr va ser lo que enviemos al cliente
            //y gracias a sizeof(outstr) sabemos los bytes que tenemso que enviar
            //el resto de parámetros de entrada de la función no han sido alterados
            sendto(sfd, outstr, sizeof(outstr), 0,
            (struct sockaddr *) &peer_addr,
            peer_addr_len);
        }
        else if(buf[0] == 'd'){
        
            strftime(outstr, sizeof(outstr), "%F", tmp);

            sendto(sfd, outstr, sizeof(outstr), 0,
            (struct sockaddr *) &peer_addr,
            peer_addr_len);
        }
        //Apagamos el servidor
        else if(buf[0] == 'q'){
            printf("SALIENDO\n");
            return 1;
        }
        //Se muestra que el comando no es valido (todo lo qu eno sea ni q, t o d)
        else{
            printf("NO VALIDO\n");
        }


        //Comentado por que no merece la pena ver si se ha enviado mal
        /*
        if (sendto(sfd, buf, nread, 0,
        (struct sockaddr *) &peer_addr,
        peer_addr_len) != nread);
            fprintf(stderr, "Error sending response\n");*/
    }
}


/*Ejercicio 3: Cliente UDP*/

/*No hemos tocado absolutamente nada, solo que le pasamos 4 argumentos
1. Programa (impepinable)
2. Direccion Ip
3. Puerto
4. Comando (t, d, q, x...) 

Es muy similar al servidor, pero la parte del final es distinta, la he comentao
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 4) {
        fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully connect(2).
    If socket(2) (or connect(2)) fails, we (close the socket
    and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Send remaining command-line arguments as separate
    datagrams, and read responses from server */

    /*Manda al servidor todos lo que venga a partir de argv[3] inclusive
        1. Se conecta
        2. Si se ha conectado entra en este for
        3. Write en el socket
        4. read del socket para leer lo que le haya mandado el servidor
        5- Repite hast aagotar los argumentos
    */

    for (j = 3; j < argc; j++) {
    len = strlen(argv[j]) + 1;
    /* +1 for terminating null byte */

        if (len + 1 > BUF_SIZE) {
            fprintf(stderr,
               "Ignoring long message in argument %d\n", j);
            continue;
        }

        if (write(sfd, argv[j], len) != len) {
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        nread = read(sfd, buf, BUF_SIZE);
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Received %ld bytes: %s\n", (long) nread, buf);
    }

    exit(EXIT_SUCCESS);
}


//Ejercicio 4


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//Importar biblioteca time
#include <time.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    //Pasamos 2 argumentos, dir y host
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    //Hacemos get addr info pasándole la direccion y el host
    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
         break;                  /* Success */

            close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */

    fd_set rfds;
    int retval;

    for (;;) {


        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sfd, &rfds);


        //Quitamos el time por respeto
        retval = select(sfd + 1, &rfds, NULL, NULL, NULL);

        if (retval == -1){
            perror("ERROR en SELECT");
            exit(EXIT_FAILURE);
        }

        //Si recibimos una entrada por Socket, lo dejamos igual q antes
        if(FD_ISSET(sfd,&rfds)){
            printf("[Socket]: ");

            //Cosas de sockets
            peer_addr_len = sizeof(struct sockaddr_storage);
            nread = recvfrom(sfd, buf, BUF_SIZE, 0,
            (struct sockaddr *) &peer_addr, &peer_addr_len);
            if (nread == -1)
                continue;               /* Ignore failed request */

            char host[NI_MAXHOST], service[NI_MAXSERV];

            s = getnameinfo((struct sockaddr *) &peer_addr,
            peer_addr_len, host, NI_MAXHOST,
            service, NI_MAXSERV, NI_NUMERICSERV);

            //Aquí podemos ver el valor del buffer, que es lo recibido

            if (s == 0)
                printf("Received %ld bytes from %s:%s = %s\n",
                (long) nread, host, service, buf);
            else
                fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));


            //TRABAJAMOS EL TIEMPO
            //man 3 strftime
            char outstr[200];
            time_t t;
            struct tm *tmp;
            t = time(NULL);
            tmp = localtime(&t);


            //Buf[0] es lo que he recibido del cliente del recvfrom
            if(buf[0] == 't'){
                strftime(outstr, sizeof(outstr), "%T", tmp);

                //outstr va ser lo que enviemos al cliente
                //y gracias a sizeof(outstr) sabemos los bytes que tenemso que enviar
                //el resto de parámetros de entrada de la función no han sido alterados
                sendto(sfd, outstr, sizeof(outstr), 0,
                (struct sockaddr *) &peer_addr,
                peer_addr_len);
            }
            else if(buf[0] == 'd'){
            
                strftime(outstr, sizeof(outstr), "%F", tmp);

                sendto(sfd, outstr, sizeof(outstr), 0,
                (struct sockaddr *) &peer_addr,
                peer_addr_len);
            }
            //Apagamos el servidor
            else if(buf[0] == 'q'){
                printf("SALIENDO\n");
                return 1;
            }
            //Se muestra que el comando no es valido (todo lo qu eno sea ni q, t o d)
            else{
                printf("NO VALIDO\n");
            }

        }//Fin socket

        //Si lo recibo por la entrada estandar (1) hacemos otra movida
        else if(FD_ISSET(0, &rfds)){
            printf("[Consola]\n");

            read(0, buf, 2);
            buf[1] = '\0';

            //TRABAJAMOS EL TIEMPO
            //man a3 strftime
            char outstr[200];
            time_t t;
            struct tm *tmp;
            t = time(NULL);
            tmp = localtime(&t);


            //Buf[0] es lo que he recibido del cliente del recvfrom
            if(buf[0] == 't'){
                strftime(outstr, sizeof(outstr), "%T", tmp);
                printf("%s\n", outstr);

            }
            else if(buf[0] == 'd'){
            
                strftime(outstr, sizeof(outstr), "%F", tmp);
                printf("%s\n", outstr);
            }
            //Apagamos el servidor
            else if(buf[0] == 'q'){
                printf("SALIENDO\n");
                return 1;
            }
            //Se muestra que el comando no es valido (todo lo qu eno sea ni q, t o d)
            else{
                printf("NO VALIDO\n");
            }
        }
    }
}

/*Ejercico 5 UDP

Ahora el servidor es multiproceso, lo que nos va a causar multierrores.
Partimos de la base del ejercicio 2, que el 4 ocupa una barbaridad.
*/


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//Importar biblioteca time
#include <time.h>

//Biblioteca del fork
#include <unistd.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    //Pasamos 2 argumentos, dir y host
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    //Hacemos get addr info pasándole la direccion y el host
    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
         break;                  /* Success */

            close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */

    //Ya hemos hecho el bind al socket, lanzamos varios proceso

    int i;
    int status;
    pid_t pid;
    for(i  =0; i < 2; i++){
        pid = fork();

        //Si es hijo...
        if(pid==0){


            for (;;) {
                peer_addr_len = sizeof(struct sockaddr_storage);
                nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
                if (nread == -1)
                    continue;               /* Ignore failed request */

                char host[NI_MAXHOST], service[NI_MAXSERV];

                s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

                //Aquí podemos ver el valor del buffer, que es lo recibido

                if (s == 0)
                    printf("[PID: %d]Received %ld bytes from %s:%s = %s\n", getpid(),
                    (long) nread, host, service, buf);
                else
                    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

                char outstr[200];
                time_t t;
                struct tm *tmp;
                t = time(NULL);
                tmp = localtime(&t);

                if(buf[0] == 't'){
                    strftime(outstr, sizeof(outstr), "%T", tmp);
                    sendto(sfd, outstr, sizeof(outstr), 0, (struct sockaddr *) &peer_addr, peer_addr_len);
                }
                else if(buf[0] == 'd'){
                    strftime(outstr, sizeof(outstr), "%F", tmp);
                    sendto(sfd, outstr, sizeof(outstr), 0, (struct sockaddr *) &peer_addr, peer_addr_len);
                }
                //Apagamos el servidor
                else if(buf[0] == 'q'){
                    printf("SALIENDO\n");
                    return 1;
                }
                //Se muestra que el comando no es valido (todo lo qu eno sea ni q, t o d)
                else{
                    printf("NO VALIDO\n");
                }
            }
        }
        //Padre
        else{ 
            pid = wait(&status);
        }
    }
}

/*PROTOCOLO TCP - SERVIDOR DE ECO*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* CAMBIAMOS A STREAM = TCP */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
         break;                  /* Success */

            close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    //Después del bind, hay que hacer el lisen con el sfd

    listen(sfd, 10);

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */

    for (;;) {


        //Elimino la otra cosa q había y lo sustituyo por accept
        int clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);

    	printf("Post Accept\n");
        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *) &peer_addr,
        peer_addr_len, host, NI_MAXHOST,
        service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0)
            printf("Received %ld bytes from %s:%s\n",
            (long) nread, host, service);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
        ssize_t c;

        //Cambio el if por este while que va a ser el bucle de recivido y enviado
     	//recives del socket de cliente q has pillado en accept
     	//Vas a leer los 80 primeros caracteres de lo q q te pase, 0 flags

        while(c = recv(clisd, buf, 80, 0)){
        	buf[c] = '\0';

        	if(buf[0] == 'Q'){
        		printf("CERRANDO SERVIDOR\n");
        		return 0;
        	}

        	printf("[Mensaje]: %s\n", buf);
        	send(clisd, buf, c, 0);
        }
    }
}

/*Cliente*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s, j;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc < 3) {
		fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}
	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
		    rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Send remaining command-line arguments as separate
	datagrams, and read responses from server */

	while(buf[0] != 'Q'){
		read(0, &buf, 80);
		buf[strlen(buf)] = '\0';

		send(sfd, &buf, strlen(buf), 0);
		recv(sfd, buf,80, 0);

		printf("%s\n",buf);
	}
	

	exit(EXIT_SUCCESS);
}

/*Ejercicio 8 El servidor no se cierra*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#define BUF_SIZE 500

volatile int nChild = 0;

void hler(int signal){
	pid_t pid;
	nChild--;
	pid = wait(NULL);
	printf("PID exited\n");

}

int
main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* CAMBIAMOS A STREAM = TCP */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
         break;                  /* Success */

            close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    //Después del bind, hay que hacer el lisen con el sfd

    listen(sfd, 10);

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */

    int status;
    pid_t pid;
	
	//Cuando capturemos la señal de proceso hijo terminado, lo ponemos en espera
    signal(SIGCHLD, hler);

    for (;;) {


        //Elimino la otra cosa q había y lo sustituyo por accept
        int clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);


        pid = fork();
        nChild++;

        if(pid == 0){//hijo

        	printf("Post Accept\n");
	        char host[NI_MAXHOST], service[NI_MAXSERV];

	        s = getnameinfo((struct sockaddr *) &peer_addr,
	        peer_addr_len, host, NI_MAXHOST,
	        service, NI_MAXSERV, NI_NUMERICSERV);
	        if (s == 0)
	            printf("Received %ld bytes from %s:%s\n",
	            (long) nread, host, service);
	        else
	            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
	        ssize_t c;

	        //Cambio el if por este while que va a ser el bucle de recivido y enviado
	     	//recives del socket de cliente q has pillado en accept
	     	//Vas a leer los 80 primeros caracteres de lo q q te pase, 0 flags

	        while(c = recv(clisd, buf, 80, 0)){
	        	buf[c] = '\0';

	        	if(buf[0] == 'q'){
	        		printf("CERRANDO SERVIDOR\n");
	        		exit(0);
	        	}
	        	printf("[Nº Hijos = %d][PID = %d] ",nChild ,getpid());
	        	printf("[Mensaje]: %s\n", buf);
	        	send(clisd, buf, c, 0);
        	}
        }
        else if(pid == 1){//Padre
        	pid = wait(&status);
        	close(clisd);
        	exit(0);
        }
        //Cerramos el último hijo
    	close(clisd);
    }
}