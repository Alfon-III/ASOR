VM1
[cursoredes@localhost ~]$ sudo su
[root@localhost cursoredes]# ip link set dev eth0 up
[root@localhost cursoredes]# ip address add 192.168.0.1/24 dev eth0
[root@localhost cursoredes]# ip route add default via 192.168.0.10

Router
ip link set dev eth0 up
ip link set dev eth1 up
ip address add 192.168.0.10/24 dev eth0
ip address add 172.16.0.10/24 dev eth1
sysctl net.ipv4.ip_forward=1

VM2
ip link set dev eth0 up
ip address add 172.16.0.1/24 dev eth0
ip route add default via 172.16.0.10


En router poner una de estas reglas:

iptables -t nat -A POSTROUTING -o eth1 -j SNAT --to 172.16.0.10
iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o eth1 -j MASQUERADE

//Ejercicio 2

/*Al ser un ejercicio con fork, te recomiendo que mires man 2 pipes donde viene
un ejemplo de fork y lo puedews usar como plantilla ;)*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int main(int argc, char *argv[]) {
	pid_t pid;

	//Necesitamos como mínimo 1 argumento, el comando y arg1
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1) {
		perror("Error al hacer fork\n");
		exit(EXIT_FAILURE);
	}

	//Proceso hijo
	if (pid == 0) {
		//Creamos un anueva sesión
		if(setsid() == -1){
			perror("Error al crear nueva sesión\n");
			exit(EXIT_FAILURE);
		}
		printf("Sesión creada\n");

		//Fijamos directorio de trabajo a /tmp
		if (chdir("/tmp") == -1){
			perror("Error al cambiar el directorio de trabajo\n");
			exit(EXIT_FAILURE);
		}

		//mostrar pid, parent pid, gid y sid.

		printf("[PID]: %li\n", getpid());
		printf("[PPID]: %li\n", getppid());
		printf("[GID]: %li\n", getgid());
		printf("[SID]: %li\n", getsid(getpid()));


		char *newargv[argc];

		//argv = {./ ,tcp, ::, 80, NULL}
		int j;
		for(j = 1; j < argc; j++){
			newargv[j-1] = argv[j];
		}

		newargv[argc - 1] = NULL;
		//newargv {tcp, ::, 80, NULL}
		if(execvp(argv[1], newargv) == -1){
			perror("Error al ejecutar el comando");
			exit(EXIT_FAILURE);
		}

		_exit(EXIT_SUCCESS);
	}

	else {//Proceso padre
		wait(NULL);
		exit(EXIT_SUCCESS);
	}
}



#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

//Time
#include <time.h>

//Select
#include <sys/select.h>


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

	char outstr[9];

	//Variables de select
	fd_set rfds;
	struct timeval tv;
	int retval;

	char aux[500];

    for (;;) {

    	//Guardamos en outstr la hora actual
		time_t t;
		struct tm *tmp;

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


		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);//Entrada estandar
		FD_SET(sfd, &rfds);//Socket

		tv.tv_sec = 20;
		tv.tv_usec = 0;

		int max;
		if(sfd > 0){
			max = sfd;
		}
		else{
			max = 0;
		}

		retval = select(max + 1, &rfds, NULL, NULL, &tv);

		if(FD_ISSET(sfd, &rfds)){
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

	        printf("[Socket]\n");

	        if (sendto(sfd, outstr, sizeof(outstr), 0,
	        (struct sockaddr *) &peer_addr,
	        peer_addr_len) !=  sizeof(outstr))
	            fprintf(stderr, "Error sending response\n");
		}

		if(FD_ISSET(0, &rfds)){
			read(0, aux, 2);
			aux[1] = '\0';
			printf("[Terminal]\n");
			printf("%s\n", outstr);
		}


    }
}