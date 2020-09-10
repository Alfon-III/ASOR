/*
Un esquema que me ayudó a tener de un vistazo todos los concepptos de la parte teórica de redes, yo que tu me haría uno bien hecho.
Éste tiene muchas tonterías, no sé ni por que lo subo.
*/

Tema 1:
 -Mensajes DHCP

 Tema 2:
-Venatana de Envío y Recepción
-Fases de conexión, desconexión...
	-3Way -Transferencia -Fin4Way -Fin3Way
-Mecanismos de retransmisión:
	-Temporiz de retransmisión y Retransmisón rápida (3 ACK duplicados)

-Temporizadores TCP: 
	-Retransmisón, Keepalive, TIMEWAIT, Tempo Persistencia (Ventana = 0, fuerza ACK)
		-Temp. Retransmisión elije dichos tiempo (RTO y RTT) de manera dinámica

-Sindrome de ventana trivial: Cuando se generan/consumen datos de manera muy lenta.
	-EMISOR: los datagramas se envían de manera más lenta
	-RECEPTOR: Se reduce el tamaño de la ventana a 0. 

-Control de congestión:
	-Ventana de congestión (CW <= RW)
	-Fase Arranque lento (n^2)
	-Fase de evitación de congestión (n)
	-Fase constante

		- ACK duplicados
		-Expiración del temporizador de retransmisón (c jodió)

Tema 3.1 Filtrado de paquetes

- Reglas/Cadenas y Tablas
- Tabla Filter/NAT/Mangle

 -NAT: Network Address translation
 	-Trad estática y dinámica
 	-Masquerading: n privada a 1 pública
 		-Traducción dinámica
 	-Vitual servers: 1 pública a N privada

Tema 3.2 DNS
	-Dominio Raíz(13)/Top Level Domain/Generic (com/gov) / Country code
	-Caching
	-Tipos de servidores
	-Base de datos DNS: Directivas y Registro de Recursos
	-SOA
	-BIND: Mierda libre

Tema 4 IPv6
-Tipos de Direccionamiento: Unicast/Multicast/Anycast
-Ámbito: Enlace local (una lan), sitio local (1 o varias redes), global (internet)
-Zona: Región de una red con un ámbito determinado


-Enlace Local: 
	-Direccionamiento: Unicast, Se asigna a un enlace 
	-Zona: 1 enlace y todos los interfaces conectados a este enlace 
	-Sirve para autoconfig y descubrimiento de vecinos
	-FE80

-ULA Unique Local Address
	-Direccionamiento: unicast, pero no para internet majo
	-Autoconfiguracion
	-FC00

-Unicast Global:
	-Direccionamiento Unicast global
	-Autoconfiguración
	-2000::
-MAC -> IPv6

-Multicast:
	-Direcc : Multicast
	-Grupo de interfaces en un ámbito determinado
	-FFXX:
		[COMPUTADORES]
		-FF01 Interfaz Local: Datagrama enviado a la interfaz de dicho nodo
		-FF02: Link Local : Todo slos interfaces del enalce
		
		[ENCAMINADORES]
		- FF05 Site Local: Un datagrma enviado a esta direccion se envía a 
			todos los ENCAMINADORES del sitio.
		-FF02 Link Local: SE envía todos los encaminadores del enlace local (RIP).


	Multicast de nodo solicitado: Protoclo de descubrimiento de descubrimiento de vecinos
	-Otras direcciones: Sin espec, loopback e IPv4 mapeada

-Formato cabecera Ipv6:
	-Fragmentación + cabecera de extensión

-ICMPv6
	+Descubrimiento de vecinos
		-Descubrimiento de vecinos
			*Dir Duplicada
			*Vecino inalcanzable
			*Cambio en la direccion de enlace
			*Resolucion de direcciones (El ARP de IPv6)
		-Descubrimiento de encaminadores
		-Redireccion

	----------------------

	+Solicitud de vecino: Averiguar la direccion física asociada a una IPv6
		-Detecta: Si un nodo es alcanzable o si está duplicada

	+Anuncio de vecino: Responde un mensaje de solicitud de vecino, dir unicast destino
		-Uso: Responder y Cambio en la direcc física

	+Solicitud de encaminador: Tras activar una interfaz
		-Detecta encaminadores y autoconfigura la interfaz (FF02 a todos los encaminadores)

	+Anuncio de encaminador: Lo envían los encaminadores para anunciar su presencia en la red
		-Lo anuncian de manera periódica
		-Respuesta a solicitud de encaminador con destini ununcas o multicast

	+Autoconfiguración:
		*Incluye: 
			-ID interfaz
			-Prefijo anunciado encaminador
			-Problemas de proivacidad
			-DHCPvs para IPv6

Tema 5
[MUCHO TEXTO]

+Reenvío de paquetes.
	-Tabla de encaminamiento: 
	-Etiquetas: Cada paquete IP se Etiquetas y conmuta según la Etiqueta
		* Reduce complejidad a l atabla de encaminamiento y siempre usa el mismo circuito

+Tablas de encaminamiento
	-Por siguiente salto:
		*Principio optimo de Bellman: El camino más corto
	-Incluye la siguiente información: [Destino], máscara, interfaz y métrica
		-Destino: Host/red/defecto
	-Internet: Una movida, mucha red

+ Técnica de encaminamiento
	-Encaminamiento local: Se la suda la topología de la red
	-Encaminamiento estático: Considera la topología, las tabals se construyen a mano 
	 por un maestro artesano y no se adaptan los cambios de red.

	-Encaminamiento dinámico: LAs tablas se construyen automáticamente.
		-Intercambian información periódicamente
			*Vector de distancias [RIP] / Estado de enlaces OSPF


+Vector de distancias:
	-Cada encaminador tiene un atabla con: [DESTINO / SIG.SALTO/ DISTANCIA]
	-Se intercambia info periódicamente para actualizar las tablas 
	PROBLEMAS!!!
		*Problema de convergencia: Cuenta a infinito
			-Los cambios no se propagan correctamente a todos los encaminadores.
			-Si uno tiene info erónea, ésta se propaga (no convergen)
			
			[SOLUCION]
			-Infinito = 16 saltos
			-Horizonte dividido: B po pasa a A info de X
			-Horizonte dividido con ruta invers turbo-envenada ultramax: 
			B anuncia a A que X está MUERTO = distancia infinito
			-Actualizaciones forzadas: Cada vez que se modifica algo, se comunica (Mucho ping)

		*Problema de convergencia: Bucles
			[PROBLEMA]
			-No convergen, horizonte dividido no funciona por que si no te lo pasa A, te lo pasa C

			[SOLUCION]
			-Con actualiz forzada acelera dicha convergencia

+Estado de enlaces:
	-Cada router tiene su BD 
	-Identifica a sus vecinos y auncia esa información a todos los nodos
	-Inundación
	-Con toda esta información genera un gafo (OSPF)

+Encaminamiento en Internet:
	-Internet se organiza por sistemas Autónomos 
		*Conjunto de redes y encaminadores gestionados por una misma autoridad

	-Encaminadores internos: interconectan redes dentro del AS
	-Encaminadores externos: interconectan redes fuera del AS / Border. 
	Ni idea q hay dentro del AS


+Protocolos Internos: IGP usado para encaminamientos internos [RIP Y OSPF]
+Protocolos Externo: EGP para la frontera, BGP


+Vector de Rutas: Es mejor que vector de distancias que es inestable con pocos saltos y problema de convergencia
			Por estados converge rápido pero genera una barbaridad de información.
			No podemos usarlso Inter-AS

	-Vector de rutas:
		*Basado en vector de disatncas pero arreglando problemas de convergencia
		*A partir de los destinos alcanzables enel AS, el router obtiene:
			-Lista de destinos alcanzables
			-Ruta completa con los AS que se atraviesan, así se Detectan los bucles rápidamente

---[RIP]---
+Vector de distancias: Tabla con lista de destinos y la distancia
+Mensajes RIP: 
	-REQUEST: Cuando se conecta a la red o caduca entrada en la tabla
	-RESPONSE: Se envía periódicamente, respuesta a una solicitud o actualización forzada

+Temporizadores
	-Temporizador Periódico: Intervalo de mensaje response "Estoy aquí vivo"
	-Temporizador de expiración: Cuando pierde el período de validaez de una entrada "Tarjeta amarilla"
	-Temporizador de recolección de basura: En vez de eleiminarlo inmediatamente, le dejamos 120s para que vuelva
	Si no hay respuesta, se elimina definitivamente "A tu puta casa crack"

+Limitaciones
	-Gran cantidad de tráfico broadcast
	-No admite otra médica distinta a numero de saltos
	-No admite caminos alternativos
	-Tarde mucho en propagarse
	-Problema en redes grandes, pues el máximo de saltos en 16

+RIP_2
	-Soporte: Dir sin clase, multicast y autenticación	
+RIPng = RIP para IPv6 WOW!
	
---[OSPF]---

+Into:
	-Protocolo alternativo a RIP
	-Reduce la cantida de info, convergencia rápida, másk de long variable. 

+Áreas: Agrupación lógica de encaminadores y redes, los encaminadores solo tien info de su area.
+Encaminadores y redes:
	-Cada encaminador tiene un ID único y pueden ser de 3 tipos (lo de los rombitos)
		*Internal Router: Encaminadores INTERNOS de un área
		*Area Border Router: Interconecytan 2 o más áreas EXTERNAS dentro del AS. una de ellas es siempre la 0.
		*Autonomous System Boundary Router: Transmiten a redes EXTERNAS al AS

+Vecindades y adyacencias:
	-Relación de vecindad: Entre encaminadores con enlace común, misma área OSPF y mismo mec. Autent
	-Relación de adyacencia: Entre encaminadores vecinos, 2 adyacentes intercambia info de los enlaces
		*Las relaciones de adyacencia se desarrollan según el tipo de red:
			-Punto a punto: 2 vecinos
			-Multi acceso: Encaminador designado (DR) y el de RESPALDO (BDR) adyacentes al resto de encam de la red 
	Para distribuir los enlaces usamos una técnica de inundación OPTIMIZADA. Cuando falla el DR, actúa el BDR
	El DR lo envía poco a poco para evitar solapamientos, los mensajes se confirman.

+Funcionamiento

	-Descubrimiento de vecinos: "OSPF HELLO"
		*Envío periódico de mensajes Hello a través de los interfaces. Estos mensajes van con una lista
		con los Router ID que han recibido Hello. Si un router recibe un Hello con su RID, crea una relación de vecindad

		* En redes multiacceso los encaminadores con mayor prioridad, se convierten en los DR y BDR.
	
	-Sincronización de la BD de estado de enlace:
		*Intercambio de BD: Patrón Maestro-Esclavo: Maestro inicia el intercambio de mensajes y el esclavo devueve el mensaje de 
		confirmación que además incluye información de este. 	
		
		*Carga de la base de datos: LA información obsoleta o no presente en el DR, se pide una copia ([link state] request) y ésta debe ser confirmada
		con [lik state] update/ack	

	-Contrucción de las tablas de encaminamiento
		*Cuando tiene stoda la info te montas tu árbol de rutas
		*Incluye encaminamiento de redes como de rutas*Contine destino/sig salto / metrica total

---[BGP]---

Protocolo de encaminamiento Inter-AS exterior
Informa sobre las redes alcanzables por otros sistemas BGP. 

+Tipos de AS:
	-Stub: Conectado a 1 (una) AS = origen o destino
	-Tránsito: Conectado a varios AS perimite tránsito
	-MultiHomed:  Conectado a varios AS NO perimite tránsito

+Políticas:
	-Cada AS puede aplicar políticas para limitar el flujo de tráfico de datos en la red
	-Cada AS puede: Configurarse como multihomed, no actuar de tránsito para X AS...

+Funcionamiento
	-Mensajes: Mediante TCP
		*OPEN: Establece sesión BGP
		*UPDATE: Actualización incremental de la información
		*NOTIFICATION:	Envía a los vecinos cuandfo se detecta un error
		*KEEPALIVE: Asegura que la sesión permanece activa.

+Atributos
	-Mensajes UPDATE:
		*Evalúan caminos alternativos
		*Generados por cada el encaminador

	-Tipos:
		*Bien conocidos:
			-Admitidos por todas las implementaciones BGP
			-Pueden ser obligatorios (se deben incluir en cada actualizacoión) o discrecionales
		*Opcionales:
			-Específicos de dcada implementación
			-Pueden ser Transitivos (se incluyen en las actus aunque no se implementen) o no

	-BIEN CONOCIDOS && OBLIGATORIOS
		*ORIGIN: Origen de la información de la ruta
		*AS_PATH: Ruta como secuencia de AS
		*NEXT_HOP: Ip del siguiente salto
