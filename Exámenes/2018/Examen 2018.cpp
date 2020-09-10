//Ejerercicio 1: RIP

VM1: 
ip link set dev eth0 up
ip address add 192.168.0.129/25 dev eth0
ip route add default via 192.168.0.130

VM2
ip link set dev eth0 up
ip link set dev eth1 up
ip address add 192.168.0.130/25 dev eth0
ip address add 10.0.0.10/24 dev eth1
ip route add default via 10.0.0.20
sysctl net.ipv4.ip_forward=1

VM3
ip link set dev eth0 up
ip link set dev eth1 up
ip address add 10.0.0.20/24 dev eth0
ip address add 172.16.0.10/24 dev eth1
ip route add default via 10.0.0.10
sysctl net.ipv4.ip_forward=1

//Revisa que puedes hacer ping a VM1 desde VM3 y Niquelao!

En VM2 y VM3
gedit /etc/quagga/ripd.conf

router rip
	version 2
	network eth0
	network 

service ripd start

Abrir wireshark para ver se intercambian paquetes RIP+
No olvidar hacer sudo ip -s -s neigh flush all 
para tener las iptables vacías, 
/*Ejercicio 2*/


#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>



int main(int argc, char *argv[]){

	//Argv > 2
	if (argc != 2) {
	   perror("Faltan argumentos.\n");
	   exit(EXIT_FAILURE);
	}


	//Open dir
	DIR *directory = opendir(argv[1]);

	if(directory == NULL){
		perror("Error al abrir el directorio. \n");
		exit(EXIT_FAILURE);
	}


	//Si dir esiste, lo recorremos
	struct dirent *dir = readdir(directory);
	
	char *auxDir;

	while(dir != NULL){
		struct stat buf;

		printf("[DIR]%s\n", dir->d_name);

		//./.

		auxDir = (char*)malloc(sizeof(char)*(strlen(argv[1])+strlen(dir->d_name)+strlen("/")));
		strcpy(auxDir, argv[1]);
		strcat(auxDir, "/");
		strcat(auxDir, dir->d_name);

		printf("%s\n", auxDir );

		if (stat(auxDir, &buf) == -1){
			perror("Error al obtener el estado de un fichero.\n");
			exit(EXIT_FAILURE);
		}

		printf("[UID]%li\n",buf.st_uid);
		printf("[i-node]%li\n",buf.st_ino);
		printf("[size]%li\n",buf.st_size);

		dir = readdir(directory);
	}

	return 0;

}


//Otra versión de Darío más simplificada

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>


int main(int argc, char **argv){
    DIR *d;
    d = opendir(argv[1]);
    struct dirent *file;
    if(d == NULL){
        printf("error al encontrar el directorio\n");
        exit(EXIT_FAILURE);
    }
    char *path;
    for(file = readdir(d); file != NULL; file = readdir(d)){
        struct stat fileStat;

        //Guardamso el tamaño de directorio, la barra y el nombre del file
        path = (char*)malloc(sizeof(char)*(strlen(argv[1])+strlen(file->d_name)+strlen("/")));
        strcpy(path,argv[1]);
        strcat(path,"/");
        strcat(path,file->d_name);
            
        stat(path, &fileStat);
        printf("Nombre: %s\n",file->d_name);
        printf("UID: %li\n",fileStat.st_uid);
        printf("i-nodo: %li\n",file->d_ino);
        printf("Tamaño: %li\n",fileStat.st_size);
        printf("----------------------\n");
    }
    
    return 0;
}


//Ejercicio 3, Servidor TCP

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


    if (argc != 3) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
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

    //Listen

    listen(sfd, 10);
    freeaddrinfo(result);  
    
    int c;
    int clisd;
    char buf[80];
    char host[NI_MAXHOST], service[NI_MAXSERV];

    while(1){
    	//Ayúdate de recvfrom del udp ;)
    	clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    	
    	int pid = fork();

    	if(pid == 0){
	    	s = getnameinfo((struct sockaddr *) &peer_addr,
	        peer_addr_len, host, NI_MAXHOST,
	        service, NI_MAXSERV, NI_NUMERICSERV);
	        if (s == 0){

	        	printf("[PID]%s\n",getpid());
	            printf("Received %ld bytes from %s:%s\n",
	            (long) nread, host, service);
	        }
	        else
	            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

	    	while(c = recv(clisd, buf, 80, 0)){
	    		buf[c] = '\0';
	    		printf("%s\n", buf);
	    		send(clisd, buf,  80, 0);
	    	}
    	}
    	else{//Padre
   			close(clisd);
    	}


    }

}
