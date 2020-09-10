//Ejercicio 1

VM1:

sudo su
ip link set dev eth0 up
ip address add 192.168.0.129/25 dev eth0
ip route add default via 192.168.0.130


RouterA
sudo su
ip link set dev eth0 up
ip link set dev eth1 up
ip address add 192.168.0.130/25 dev eth0
ip address add 10.0.0.10/24 dev eth1
ip route add default via 10.0.0.20
sysctl net.ipv4.ip_forward=1

(Configuré mal virtual box, eth0 y eth1 las puse al revés)
RouterB
sudo su
ip link set dev eth0 up
ip link set dev eth1 up
ip address add 10.0.0.20/24 dev eth0
ip address add 172.16.0.1/24 dev eth1
ip route add default via 10.0.0.10
sysctl net.ipv4.ip_forward=1

En router A y B 

gedit /etc/quagga/ripd.conf

router rip
	version 2
	network eth0
	network eth1


//Ejercicio 2

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//time
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

    	char outstr[200];
		time_t t;
		struct tm *tmp;

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
        if (s == 0)
            printf("Received %ld bytes from %s:%s\n",
            (long) nread, host, service);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));




		t = time(NULL);
		tmp = localtime(&t);
		if (tmp == NULL) {
			perror("localtime");
			exit(EXIT_FAILURE);
		}

		if (strftime(outstr, sizeof(outstr), "%T", tmp) == 0) {
			fprintf(stderr, "strftime returned 0");
			exit(EXIT_FAILURE);
		}

        sendto(sfd, outstr, 10, 0, (struct sockaddr *) &peer_addr, peer_addr_len);
            
    }
}


//Ejercicio 3

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(){


	int fd1, fd2;

	fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
	if(fd1 == -1){
		perror("Error al abrir la tuberia 1\n");
		return 0;
	}

	fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
	if(fd1 == -1){
		perror("Error al abrir la tuberia 2\n");
		return 0;
	}

	fd_set rfds;
	struct timeval tv;
	int retval, maxFD, i;
	char buff[50];
	char aux;
	while(1){
		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(fd1, &rfds);
		FD_SET(fd2, &rfds);

		if(fd1 > fd2){
			maxFD = fd1;
		}
		else{
			maxFD = fd2;
		}

		/* Wait up to five seconds. */
		tv.tv_sec = 20;
		tv.tv_usec = 0;



		retval = select(maxFD + 1, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */

		if (retval == -1){

			perror("select()");
			return 0;
		}

		else if (retval == 0){
			printf("TIMEOUT!\n");
			return 0;
		}
		
		else if(FD_ISSET(fd1, &rfds)){

			//leemos
			i = 0;
			while(read(fd1, &aux, 1) != 0){
				buff[i] = aux;
				i++;
			}

			buff[i] = '\0';
			printf("[PIPE 1]: %s\n", buff);

			close(fd1);
			fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
			if(fd1 == -1){
				perror("Error al abrir la tuberia 1\n");
				return 0;
			}
		}
		else if(FD_ISSET(fd2, &rfds)){

			//leemos
			i = 0;
			while(read(fd2, &aux, 1) != 0){
				buff[i] = aux;
				i++;
			}

			buff[i] = '\0';
			printf("[PIPE 2]: %s\n", buff);

			close(fd2);
			fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
			if(fd2 == -1){
				perror("Error al abrir la tuberia 2\n");
				return 0;
			}
		}
	}
	exit(EXIT_SUCCESS);
}
