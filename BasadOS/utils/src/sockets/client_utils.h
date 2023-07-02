#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>


// Estrucutras para paquetes, que capaz usamos despues
typedef enum
{
	INTERRUPCION_A_READY,
	FINALIZACION,
	INTERRUPCION_BLOQUEANTE,
	PETICION_RECURSO,
	LIBERACION_RECURSO,
	CREAR_SEGMENTO,
	ELIMINAR_SEGMENTO, 
	ABRIR_ARCHIVO,
	CERRAR_ARCHIVO,
	ACTUALIZAR_PUNTERO,
	TRUNCAR_ARCHIVO,
	PETICION_LECTURA,
	PETICION_ESCRITURA,
	COMPACTACION_NECESARIA,
	SEGMENTO_CREADO,
	OUT_OF_MEMORY,
	
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;



int crear_conexion_al_server(t_log* , char* , char* );
void liberar_conexion(int);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete();
t_paquete* crear_super_paquete(void);
void crear_buffer(t_paquete*);

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

#endif /* CLIENT_UTILS_H_ */