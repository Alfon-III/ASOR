
//Ejercicio 1

VM1
ip link set dev eth0 up
ip address add FD00:1:1:A::1/64 dev eth0
ip route add default via FD00:1:1:A::10

Router: 
ip link set dev eth0 up
ip link set dev eth1 up
ip address add FD00:1:1:A::10/64 dev eth0
ip address add FD00:1:1:B::10/64 dev eth1
sysctl -w net.ipv6.conf.all.forwarding=1

VM2

ip link set dev eth0 up
ip address add FD00:1:1:B::1/64 dev eth0
ip route add default via FD00:1:1:B::10


ip -s -s neigh flush all


//En Router...
gedit /etc/quagga/zebra.conf 
interface eth0
  no ipv6 nd suppress-ra
  ipv6 nd prefix fd00:1:1:A::/64

  //Antes de iniciar el servicio zebra, hemos de hacer ip link set dev eth0 down en VM1
  //Después de iniciar zebra activamos la interfaz. No olvides usar wireshark para ver que se pasan


//Ejercicio 2

      
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>

int sig_int;
int sig_stp;

void handler(int sig)
{
	if(sig == SIGINT)
		sig_int++;
	
	if(sig == SIGTSTP)
		sig_stp++;
}

int main(int argc, char *argv[])
{

	struct sigaction aux, sa;
	sig_int = 0;
	sig_stp = 0;

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = handler;


	if (sigaction(SIGINT, &sa, NULL) == -1)
		printf("ERROR\n");

	if (sigaction(SIGTSTP, &sa, NULL) == -1)
		printf("ERROR\n");



	while(sig_int + sig_stp < 10){
		sigpending(&sa);
	}

	printf("[INT]: %li\n", sig_int );
	printf("[STP]: %li\n", sig_stp );
	exit(EXIT_SUCCESS);
	
}


//Ejercicio 3
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
        fprintf(stderr, "Introduzca Host y Puerto\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
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

    listen(sfd, 5);

    int c, clisd;
    char mensaje[40];
    int pid;        
    char host[NI_MAXHOST], service[NI_MAXSERV];

    for (;;) {
    	
    	clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
		pid = fork();

		if(pid == 0){
				peer_addr_len = sizeof(struct sockaddr_storage);
				s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

		        printf("Received from %s:%s\n", host, service);
		        printf("[PID]: %li\n", getpid());

			while(c = recv(clisd, mensaje, 80, 0)){
	        	mensaje[c] = '\0';
	        	//Mostramos información de la conexión + PID
	        	if(mensaje[0] == 'q'){
	        		printf("ADIOS!\n");
	        		close(clisd);
	        		exit(EXIT_SUCCESS);
	        	}
	        	send(clisd, mensaje, c, 0);
        	}
        	
		}
		if(pid == 1){
			wait();
			close(clisd);
			exit(EXIT_SUCCESS);
		}
		if(pid == -1){
			perror("Error al hacer Fork\n");
		}
        //Inicio comunicación TCP
    }
    return 0;
}