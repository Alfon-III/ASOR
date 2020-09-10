
//Ejercicio 1 

Primero eliminar los interfaces de red en VM1 y VM2:
ip addr del 192.168.0.129 dev eth0
ip addr del 10.0.2.1 dev eth0

En router modificar el fichero: gedit /etc/dhcp/dhcpd.conf 
subnet 10.0.2.0 netmask 255.255.255.0 {
    range 10.0.2.30 10.0.2.50;
    option routers 10.0.2.10;
    option broadcast-address 10.0.2.255;
}

para iniciarlo, lo que viene en la hoja. Cópialo BIEN!
service dhcpd start 

En el cliente VM2:
dhclient eth0 


//Ejercicio 2

Copio lo que hay en man 3 getaddrinfo...

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
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
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

    freeaddrinfo(result);           /* No longer needed */

    /* Read datagrams and echo them back to sender */

    //Variables para obtener el nombre

	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];


    for (;;) {
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
        (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;               /* Ignore failed request */

        char host[NI_MAXHOST], service[NI_MAXSERV];

    	//Fusilado de: getnameinfo. 
        s = getnameinfo((struct sockaddr *) &peer_addr,
        peer_addr_len, host, NI_MAXHOST,
        service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0)
            printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));


        //Obtenemos la dirección en hbuf cuando le pasamos la info del socket obtenida en getnameinfo
        if (getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, hbuf, sizeof(hbuf), sbuf,
                       sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
               printf("host=%s, serv=%s\n", hbuf, sbuf);


        //Paso 15 caracteres, pero es un pocoi chapuza. XD
        if (sendto(sfd, hbuf, 15, 0,
        (struct sockaddr *) &peer_addr,
        peer_addr_len) != nread)
            fprintf(stderr, "Error sending response\n");
    }
}
//Ejercicio 3 el de las pipes
/*
1. open pipes
while(1){
	//man 2 select nos da un buen esquema general para multiplexar pipes
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int
main(void)
{

	int fd1, fd2;

	//buscar en man 2 open: "fifo" verás que necesitas este flag
	fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
	if(fd1 == -1){
		perror("Error al abrir la tuberia1");
	}
	
	fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
	if(fd2 == -1){
		perror("Error al abrir la tuberia2");
	}

	int fd_max;
	char buf[500];
	while(1){
		fd_set rfds;
		struct timeval tv;
		int retval;

		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(fd1, &rfds);
		FD_SET(fd2, &rfds);

		/* Wait up to five seconds. */
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		//Fichero maximo + 1
		if(fd1 > fd2){
			fd_max = fd1;
		}
		else{
			fd_max = fd2;
		}
		//NO OLVIDARSE DEL "+ 1" CENUTRI@!!!
		retval = select(fd_max + 1, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */

		if (retval == -1)
			perror("select()");
		else if (retval == 0){
			printf("TIMEOUT!!!\n");
				exit(EXIT_SUCCESS);
		}
		//Pipe seleccionada
		else{
			if(FD_ISSET(fd1 ,&rfds)){
				printf("[PIPE 1]:\n");
				while(read(fd1, buf, 500) > 0){
					printf("%s\n",buf);
				}
				printf("\n");

				close(fd1);
				fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
				if(fd1 == -1){
					perror("Error al abrir la tuberia1");
				}
			}
			if(FD_ISSET(fd2 ,&rfds)){
				printf("[PIPE 2]:\n");
				while(read(fd2, buf, 500) > 0){
					printf("%s\n",buf);
				}
				printf("\n");

				close(fd2);
				fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
				if(fd2 == -1){
					perror("Error al abrir la tuberia2");
				}
			}
		}
	}

	

	exit(EXIT_SUCCESS);
}


