/*
					***************************************************
					********** Metodos de Gestion de Memoria **********
					***************************************************
					
Esta aplicacion define las estructuras necesarias para implementar los siguientes metodos de gestion de memoria:
	
								Registro base\Limite
								---------------------

Este metodo se basa en dos valores numericos por cada proceso:
	
	Registro Base (RB): Dirección de inicio físico en memoria de la partición dinámica asignada al proceso.
	Registro Limite (RL): Longitud de la partición.

	
	
									Paginacion
									----------
									
Este método se basa en un tamaño de página, que dividirá la memoria principal en un numero de marcos de ese mismo tamaño.
Por otro lado, también se basa en dividir cada proceso en partes llamadas páginas, y asignarle una serie de marcos.
Estas asignaciones, estarán almacenados en una tabla de mapa de paginas (PMT), que puede ser global para todos los procesos, o una por proceso.
Para conocer cada proceso que subconjunto de valores le corresponden de la PMT, estos contienen un registro base y un registro límite de la misma. (RBTP y RLTP)



									Segmentacion
									------------

Este método divide los procesos en regiones que son alojados en particiones (segmentos), creadas en memoria de forma dinámica.
Los datos de cada particion (RB y RL, entre otros) se almacenan en una tabla de descriptores de segmentos.
Para conocer que subconjunto de entradas de la SDT corresponde a cada proceso, estos contienen un registro base y un registro limite de la misma. (RBTS y RLTS)




					**************************************
					********** Entrada de datos **********
					**************************************

El programa recibira por parametros un fichero de texto con los datos correspondientes, divididos mediante un salto de linea (y a su vez cada conjunto de datos divididos por espacios).
En caso de no recibir el fichero de texto por parametros, sera pedido al usuario.
El formato sera el siguiente:

1.- La primera linea recibira el numero de procesos.
2.- La segunda linea recibira los registros base y limite asociados a cada proceso (por tanto si hay dos procesos, tendremos cuatro datos separados por espacios).
3.- La tercera linea recibira los RBTS y RLTS de cada proceso. (idem a lo anterior)
4.- la cuarta linea recibira los datos asociados a cada segmento de cada proceso. Tendremos tantas parejas de datos (RB y RL) como segmentos tengan los procesos que tengamos.
5.- La quinta linea recibira el tamaño de pagina.
6.- La sexta linea recibira el tipo de paginacion: 0 -> PMT global, 1 -> PMT por proceso
7.- La septima linea recibira los RBTP y RLTP de cada proceso. (idem a lo anterior)
8.- La octava linea recibira los datos asociados a cada pagina de cada proceso. Tendremos tantos datos como paginas tengan los procesos que tengamos, un dato por pagina.

----------------------
Ejemplo con PMT Global:
----------------------
2																				// 2 Procesos
27 105 550 1401																	// RB y RL por proceso
4 3 0 3																			// RBTS y RLTS por proceso
1500 408 500 85 746 103 234 40 101 300 599 700 2000 670 3690 210				// RB y RL de cada segmento del proceso
100																				// Tamaño de pagina
0																				// Tipo de paginacion: PMT Global
0 3 4 6																			// RBTP y RLTP por proceso
0 1 3 7 10 2 4 6 9 5 8															// Marcos de pagina asociados a cada pagina del proceso.

	
---------------------------
Ejemplo con PMT por Proceso:
---------------------------
2																				// 2 Procesos
4 1105 1300 4001																// RB y RL por proceso
3 2 0 2																			// RBTS y RLTS por proceso
3690 210 1500 408 500 85 101 300 599 700 2000 670								// RB y RL de cada segmento del proceso
100																				// Tamaño de pagina
1																				// Tipo de paginacion: PMT por proceso
0 3 0 5																			// RBTP y RLTP por proceso
0 1 3 7 0 2 4 7 -1 5															// Marcos de pagina asociados a cada pagina del proceso.




					**************************************
					********** Salida de datos ***********
					**************************************

El sistema mostrara por pantalla los datos del fichero almacenados en las correspondientes
estructuras de datos utilizadas para implementar el gestor de memoria.
	
	
	
					***********************************************
					********** Traduccion de direcciones **********
					***********************************************
					
El sistema pedira por pantalla direcciones logicas en cualquiera de los dos formatos
existentes.
La traduccion a direccion fisica correspondientes, por proceso y metodo, se mostrara
por pantalla, ademas de sacarla a un fichero de texto, cuyo nombre se relaciona
con el fichero de texto donde estaban los datos inicialmente almacenados.

*/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#define max_paginas 100 				// Numero maximo de paginas que tendra cada tabla PMT, ya sea global o de cada proceso
#define max_segmentos 100   			// Numero maximo de segmentos que tendra la tabla SDT del sistema
#define max_procesos 100				// Numero maximo de procesos del sistema.
#define longitudFichero 255 			// Maximo de caracteres de la ruta de un fichero
	
// Estructura que implementa los conceptos de base y limite para cualquier sistema que necesite hacer uso de los mismos
// Lo usamos para los siguientes escenarios:
//		- Implementar el metodo de gestion de memoria Registro Base/Limite
//		- Implementar los RBTP y RLTP de las paginas de cada proceso	
//		- Implementar los RBTS y RLTS de los segmentos de cada proceso
typedef struct registro{
	int base;											// valor entero que representa el registro base
	int limite;											// valor entero que representa el registro limite
}registro;

// Estructura que implementa el metodo de gestion de memoria de la paginación.
typedef struct paginacion{
	int tamano;											// Valor entero que representa el tamano de pagina del sistema
	int tipo;											// Entero que representa el tipo de tabla que usara el sistema:
														// 		0 - PMT global para todos los procesos
														// 		1 - PMT por proceso
	
	int PMT_Global[max_paginas];						// Array Unidimensional. Representa la PMT del sistema para todos los procesos, con un numero maximo de paginas "num_Paginas".
	int PMT[max_procesos][max_paginas]; 				// Array Bidimensional. Representa las PMT de cada proceso. Cada PMT tendra un numero maximo de paginas "num_paginas".
	
	registro RBTP_RLTP[max_procesos];					// Array Bidimensional de tipo registro. Almacena los registros RBTP y RLTP de cada proceso. Equivalen a:
														// En el caso de PMT del sistema: Indice y longitud en el vector PMT_Global.
														// En el caso de PMT por proceso: Indice y longitud de las columnas de cada fila (proceso) en la matriz PMT.
}paginacion;

// Estructura que implementa el metodo de gestion de memoria de la segmentacion.
typedef struct segmentacion{
	registro SDT[max_segmentos]; 		 				// Array de tipo registro. Representa la SDT. Tendra un numero maximo de segmentos "num_segmentos"
	registro RBTS_RLTS[max_procesos];	 				// Array Bidimensional de tipo registro. Lo utilizaremos para almacenar los datos de segmentacion de los procesos del sistema.
}segmentacion;

/* Prototipos de las funciones auxiliares */
int datosEntradaValidos (FILE * archivo);	
void leerDatos(FILE *fp, int *numProcesos, registro t_registros[], paginacion *t_paginacion, segmentacion *t_segmentacion);
void mostrarDatos(char *archivo, int numProcesos, registro t_registros[], paginacion t_paginacion, segmentacion t_segmentacion);
void traducirDirecciones(char *archivo, int numProcesos, registro t_registros[], paginacion t_paginacion, segmentacion t_segmentacion);
void validarDireccion(int opcion, FILE *fp, int *dLogica, int *nPag_Seg, int *nInstruccion);

/* Funcion Principal */
int main(int argc, char *argv[]){
	FILE *fp;												// Puntero a archivo que representa nuestro archivo
	char archivo[longitudFichero];							// Nombre del archivo
	int i, j, numProcesos, numPaginasProceso;				// Variable auxiliares
	
	/* Estructuras utilizadas en el ejercicio */
	registro t_registros[max_procesos];						// Registro Base/Limite
	paginacion t_paginacion;								// Paginacion
	segmentacion t_segmentacion;							// Segmentacion		
	
	
	/* Obtenemos el nombre del fichero de entrada de datos mediante parametros
	o pidiendolo por teclado en caso contrario */
	if (argc == 1){
		printf ("Fichero no introducido por parametros. Introduzca nombre del fichero: ");
		gets(archivo);
	}
	else{
		strcpy(archivo,argv[1]);
	}		
	
	system("cls");
		
	if((fp=fopen(archivo,"r"))==NULL){
		system("cls");
		printf ("Error. Archivo no encontrado\n");
		system("pause");
	}
	else if (!datosEntradaValidos(fp)){		
		printf ("--------------------------------------------------------------------------------\n");
		printf ("La informacion del fichero no es valida. Debe cumplir el siguiente formato: \n\n");
				
		printf ("Fichero de texto con los datos divididos mediante un salto de linea.\n");
		printf ("A su vez cada linea de datos divididos por espacios:\n\n");
	
		printf ("1ª linea -> Numero de procesos.\n");
		printf ("2ª linea -> RB y RL de cada proceso.\n");
		printf ("3ª linea -> RBTS y RLTS de cada proceso.\n");
		printf ("4ª linea -> RB y RL de cada segmento de cada proceso.\n");
		printf ("5ª linea -> Tamaño de pagina.\n");
		printf ("6ª linea -> Tipo de Paginacion: 0 -> PMT Global, 1 -> PMT por proceso.\n");
		printf ("7ª linea -> RBTP y RLTP de cada proceso.\n");
		printf ("8ª linea -> Marco de pagina asociado a cada pagina de cada proceso.\n\n");
		printf ("--------------------------------------------------------------------------------\n");
			
		system("pause");
	}else{	
	
		/* Rebobinamos el puntero a fichero despues de haber verificado sus datos */
		rewind (fp);
		
		/*******************************/
		/* Lectura de datos de fichero */
		/*******************************/
		
		leerDatos(fp, &numProcesos, t_registros, &t_paginacion, &t_segmentacion);    
				
		      
        /**********************************************************/
		/* Salida de la informacion por pantalla - Datos Procesos */
		/**********************************************************/       
	
        mostrarDatos(archivo, numProcesos, t_registros, t_paginacion, t_segmentacion);	
        
        fclose(fp);
        fp = NULL;
        
        
		/*********************************************************************/
		/* Salida de la informacion por pantalla - Traduccion de direcciones */
		/*********************************************************************/
		
		traducirDirecciones(archivo, numProcesos, t_registros, t_paginacion, t_segmentacion);
		
	}	
	         
	return 0;
}


//*************************************************************
//*	Funcion:
//*		Funcion que verifica si los datos del fichero de entrada
//*		son correctos.
//*
//*
//* Algoritmo:
//*		El algoritmo va obteniendo los datos del fichero, comprobando
//*		que detras de cada numero aparece un espacio o un salto de linea, dependiendo
//*		de la situacion.
//*		Ademas, la cantidad de valores debe ser correcta respecto a los valores
//*		que se dan en el fichero.
//*		Por otro lado, no permite que existan tamaños de pagina negativos,
//*		y tampoco un valor de tipo de paginacion distinto de 0 o 1.
//*
//*************************************************************
int datosEntradaValidos (FILE * fp){
	int i, j, k, l, numProcesos;
	char aux;
	
	// Estructuras temporales para verificar la integridad de los datos del fichero
	registro t_registros[max_procesos];		// Registro Base/Limite
	paginacion t_paginacion;				// Paginacion
	segmentacion t_segmentacion;			// Segmentacion	
	
	rewind (fp);
	
	// Lectura del numero de procesos
    fscanf (fp, "%d", &numProcesos);    

    fscanf (fp, "%c", &aux);
	if (aux != '\n'){
		printf ("Numero de procesos incorrecto.\n\n");
		return 0;	
	} 
		
    // Lectura de los valores RB y RL de cada proceso, asociados al metodo Registro Base/Limite
    for(i=0 ; i < numProcesos ; i++){
    	
        fscanf (fp, "%d", &t_registros[i].base);

        fscanf (fp, "%c", &aux);
		if (aux != ' '){
			printf ("Valores de RB/RL incorrectos.\n\n");
			return 0;	
		}
        
    	fscanf (fp, "%d", &t_registros[i].limite);                
    		
		if (i < numProcesos - 1){
			fscanf (fp, "%c", &aux);
			if (aux != ' '){
				printf ("Valores de RB/RL incorrectos.\n\n");
				return 0;	
			}
		}
    }        
    
    fscanf (fp, "%c", &aux);
	if (aux != '\n'){
		printf ("Valores de RB/RL incorrectos.\n\n");
		return 0;	
	}
    
    // Lectura de los registros RBTS y RLTS de cada proceso
    for(i=0 ; i < numProcesos ; i++){
        fscanf (fp, "%d", &t_segmentacion.RBTS_RLTS[i].base);
        
        fscanf (fp, "%c", &aux);
		if (aux != ' '){
			printf ("Valores de RBTS/RLTS incorrectos.\n\n");
			return 0;	
		}
        
        fscanf (fp, "%d", &t_segmentacion.RBTS_RLTS[i].limite);
        
		if (i < numProcesos - 1){
			fscanf (fp, "%c", &aux);
			if (aux != ' '){
				printf ("Valores de RBTS/RLTS incorrectos.\n\n");
				return 0;	
			} 	
		}   
    }
    
    fscanf (fp, "%c", &aux);
	if (aux != '\n'){
		printf ("Valores de RBTS/RLTS incorrectos.\n\n");
		return 0;	
	}
	
	// Solo tenemos una SDT global. Comprobamos que los valores RBTS/RLTS de cada proceso
	// no se sobreescriban
    for(i=0 ; i < numProcesos ; i++){
		for(j=0 ; j < numProcesos ; j++){
			if (i != j){				
		    	for (k = t_segmentacion.RBTS_RLTS[i].base; k <= t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite; k++){
		    		for (l = t_segmentacion.RBTS_RLTS[j].base; l <= t_segmentacion.RBTS_RLTS[j].base + t_segmentacion.RBTS_RLTS[j].limite; l++){
		    			if (k == l){
		    				printf ("Valores de RBTS/RLTS incorrectos.\n");
		    				printf ("Se esta usando una SDT global y se han introducido valores que se sobreescriben.\n\n");
							return 0;	
		    			}
		    		}
		    	}
		    }
	    }
    }  
    
    // Lectura de los registros RB y RL de cada segmento
    for(i=0 ; i < numProcesos ; i++){
    	for (j = t_segmentacion.RBTS_RLTS[i].base; j <= t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite; j++){	        	
            fscanf (fp, "%d", &t_segmentacion.SDT[j].base);
            
            fscanf (fp, "%c", &aux);
			if (aux != ' '){
				printf ("Valores de RB/RL de cada segmento incorrectos.\n\n");
				return 0;	
			}
            
        	fscanf (fp, "%d", &t_segmentacion.SDT[j].limite);
        	
        	if (i < numProcesos - 1 || j < t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite){
				fscanf (fp, "%c", &aux);
				if (aux != ' '){
					printf ("Valores de RB/RL de cada segmento incorrectos.\n\n");
					return 0;	
				}
			} 
    	}
    }
    
    fscanf (fp, "%c", &aux);
	if (aux != '\n'){
		printf ("Valores de RB/RL de cada segmento incorrectos.\n\n");
		return 0;	
	}
    
    // Lectura del tamaño de pagina
    fscanf (fp, "%d", &t_paginacion.tamano);        
    
    fscanf (fp, "%c", &aux);
	if (t_paginacion.tamano <= 0 || aux != '\n'){
		printf ("Valor de tamanyo de pagina incorrecto.\n\n");
		return 0;	
	}
    
    // Lectura del tipo de paginacion
    fscanf (fp, "%d", &t_paginacion.tipo);
    
    fscanf (fp, "%c", &aux);
	if ((t_paginacion.tipo != 0 && t_paginacion.tipo != 1) || aux != '\n'){
		printf ("Valor de tipo de paginacion incorrecto.\n\n");
		return 0;	
	}
    
    // Lectura de los registros RBTP y RLTP de cada proceso
    for(i=0 ; i < numProcesos ; i++){
        fscanf (fp, "%d", &t_paginacion.RBTP_RLTP[i].base);
        
        fscanf (fp, "%c", &aux);
		if (aux != ' '){
			printf ("Valores de RBTP/RLTP incorrectos.\n\n");
			return 0;	
		}
        
        fscanf (fp, "%d", &t_paginacion.RBTP_RLTP[i].limite);
        
		if (i < numProcesos - 1){
			fscanf (fp, "%c", &aux);
			if (aux != ' '){
				printf ("Valores de RBTP/RLTP incorrectos.\n\n");
				return 0;	
			}
		}         
    }
    
    // Si tenemos una PMT Global, los RBTP y RLTP no deben sobreescribirse
    if (t_paginacion.tipo == 0){
    	for(i=0 ; i < numProcesos ; i++){
			for(j=0 ; j < numProcesos ; j++){
				if (i != j){				
			    	for (k = t_paginacion.RBTP_RLTP[i].base; k <= t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite; k++){
			    		for (l = t_paginacion.RBTP_RLTP[j].base; l <= t_paginacion.RBTP_RLTP[j].base + t_paginacion.RBTP_RLTP[j].limite; l++){
			    			if (k == l){
			    				printf ("Valores de RBTP/RLTP incorrectos.\n");
			    				printf ("Se esta usando una PMT global y se han introducido valores para una PMT por proceso.\n\n");
								return 0;	
			    			}
			    		}
			    	}
			    }
		    }
	    } 	    	
    }
    
    fscanf (fp, "%c", &aux);
	if (aux != '\n'){
		printf ("Valores de RBTP/RLTP incorrectos.\n\n");
		return 0;	
	}

	// Lectura del marco de pagina asociado a cada pagina de cada proceso
    for(i=0 ; i < numProcesos ; i++){
        for (j = t_paginacion.RBTP_RLTP[i].base; j <= t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite; j++){	        	
        	if (t_paginacion.tipo == 0){
	        	// PMT Global
	        	fscanf (fp, "%d", &t_paginacion.PMT_Global[j]);
	        }
	        else{
	        	// PMT por proceso
	        	fscanf (fp, "%d", &t_paginacion.PMT[i][j]);
	        }		          
	        
	        if (i < numProcesos - 1 || j < t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite){
				fscanf (fp, "%c", &aux);
				if (aux != ' '){
					printf ("Valores de marcos de pagina asociados a cada pagina de cada proceso incorrectos.\n\n");
					return 0;	
				}
			} 
       	}
    }
    
    // Si no se ha llegado a final de fichero...
    if (!feof(fp)){
    	fscanf (fp, "%c", &aux);
    
		// Si el siguiente caracter no es un espacio o un salto de linea...	
		if (aux != 32 && aux != 10){
			printf ("Final de fichero incorrecto.\n\n");
			return 0;
		}else{
			fscanf (fp, "%c", &aux);
			
			// Si no se ha llegado a final de fichero...
			if (!feof(fp)){
				printf ("Final de fichero incorrecto.\n\n");
				return 0;		
			}
		}
    }
    	
	return 1;
}



//*************************************************************
//*	Funcion:
//*		Funcion que lee los datos de fichero y los almacena
//*			 en las correspondientes variables.
//*
//*
//* Algoritmo:
//*		El algoritmo va obteniendo valores enteros del fichero
//* 	de entrada y asignandolos a las distintas variables y
//*		estructuras utilizadas.
//*		Es necesario que el fichero de entrada este en el formato de entrada
//*		pedido, tanto en sintaxis como en cantidad de informacion, no pudiendo
//*		tener mas o menos informacion de la necesaria.	
//*
//*************************************************************
void leerDatos(FILE *fp, int *numProcesos, registro t_registros[], paginacion *t_paginacion, segmentacion *t_segmentacion){        
	int i, j, aux;
	
	// Lectura del numero de procesos
    fscanf (fp, "%d", numProcesos);

    // Lectura de los valores RB y RL de cada proceso, asociados al metodo Registro Base/Limite
    for(i=0 ; i < *numProcesos ; i++){
        fscanf (fp, "%d", &t_registros[i].base);
    	fscanf (fp, "%d", &t_registros[i].limite);                
    }
    
    // Lectura de los registros RBTS y RLTS de cada proceso
    for(i=0 ; i < *numProcesos ; i++){
        fscanf (fp, "%d", &t_segmentacion->RBTS_RLTS[i].base);
        fscanf (fp, "%d", &t_segmentacion->RBTS_RLTS[i].limite);
    }
    
    // Lectura de los registros RB y RL de cada segmento
    for(i=0 ; i < *numProcesos ; i++){
    	for (j = t_segmentacion->RBTS_RLTS[i].base; j <= t_segmentacion->RBTS_RLTS[i].base + t_segmentacion->RBTS_RLTS[i].limite; j++){	        	
            fscanf (fp, "%d", &t_segmentacion->SDT[j].base);
        	fscanf (fp, "%d", &t_segmentacion->SDT[j].limite);
    	}
    }
    
    // Lectura del tamaño de pagina
    fscanf (fp, "%d", &t_paginacion->tamano);
    
    // Lectura del tipo de paginacion
    fscanf (fp, "%d", &t_paginacion->tipo);
    
    // Lectura de los registros RBTP y RLTP de cada proceso
    for(i=0 ; i < *numProcesos ; i++){
        fscanf (fp, "%d", &t_paginacion->RBTP_RLTP[i].base);
        fscanf (fp, "%d", &t_paginacion->RBTP_RLTP[i].limite);
    }

	// Lectura del marco de pagina asociado a cada pagina de cada proceso
    for(i=0 ; i < *numProcesos ; i++){
        for (j = t_paginacion->RBTP_RLTP[i].base; j <= t_paginacion->RBTP_RLTP[i].base + t_paginacion->RBTP_RLTP[i].limite; j++){	        	
        	if (t_paginacion->tipo == 0){
	        	// PMT Global
	        	fscanf (fp, "%d", &t_paginacion->PMT_Global[j]);
	        }
	        else{
	        	// PMT por proceso
	        	fscanf (fp, "%d", &t_paginacion->PMT[i][j]);
	        }		          
       	}
    }

}



//*************************************************************
//*	Funcion:
//*		Funcion que muestra por pantalla los valores de
//*			 las estructuras de datos pasadas como parametros.
//*
//*
//* Algoritmo:
//*		El algoritmo muestra por pantalla, de forma estructurada
//* 	y ordenada, los datos obtenidos por fichero y almacenados
//* 	previamente en las estructuras de datos utilizados para representar
//* 	los distintos sistemas de gestion de memoria.
//*
//*************************************************************
void mostrarDatos(char *archivo, int numProcesos, registro t_registros[], paginacion t_paginacion, segmentacion t_segmentacion){        
	int i, j;
	
	system("cls");        
	
    printf ("Procesado el archivo \"%s\" con la siguiente informacion:\n\n", archivo);
	        
    printf ("Numero total de procesos: %d\n", numProcesos);
    
    printf ("\n\nValores de cada proceso con el metodo de registro base/limite:\n\n");
    for(i=0 ; i < numProcesos ; i++){
        printf ("\tProceso %d: Base = %d, Limite = %d\n", i, t_registros[i].base, t_registros[i].limite);
    }
           
    printf ("\n\nValores de cada proceso con el metodo de segmentacion:\n\n");
    for(i=0 ; i < numProcesos ; i++){
        printf ("\tProceso %d: RBTS = %d, RLTS = %d\n", i, t_segmentacion.RBTS_RLTS[i].base, t_segmentacion.RBTS_RLTS[i].limite);
    }             		
	
	printf ("\n\nValores de los registros base y limite de cada segmento de cada proceso:\n\n");
	for(i=0 ; i < numProcesos ; i++){
		printf ("Entradas en la SDT del Proceso %d:\n", i);
    	for (j = t_segmentacion.RBTS_RLTS[i].base; j <= t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite; j++){	        	
    		printf ("\t[%d] %d - %d\n", j, t_segmentacion.SDT[j].base, t_segmentacion.SDT[j].limite);
    	}
    } 
    
    printf ("\n\nTamano de pagina: %d\n", t_paginacion.tamano);
    
    printf ("\nValores de cada proceso con el metodo de paginacion:\n\n");
    for(i=0 ; i < numProcesos ; i++){
        printf ("\tProceso %d: RBTP = %d, RLTP = %d\n", i, t_paginacion.RBTP_RLTP[i].base, t_paginacion.RBTP_RLTP[i].limite);
    }		 
    
    printf ("\n\nValores del marco de pagina asociado a cada pagina de cada proceso:\n\n");
	for(i=0 ; i < numProcesos ; i++){
		
		if (t_paginacion.tipo == 0){
			/* PMT Global */
			printf ("Entradas del Proceso %d en la PMT Global:\n", i);
		}else{
			/* PMT por proceso */
			printf ("Entradas en la PMT del Proceso %d:\n", i);
		}
		
    	for (j = t_paginacion.RBTP_RLTP[i].base; j <= t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite; j++){	        
			if (t_paginacion.tipo == 0){
	        	/* PMT Global */
	        	printf ("\t[%d] %d\n", j, t_paginacion.PMT_Global[j]);
	        }
	        else{
	        	/* PMT por proceso */
	        	printf ("\t[%d] %d\n", j, t_paginacion.PMT[i][j]);
	        }		                  		
    	}
    } 		        
    
    printf ("\n\n");
	system("pause");
	system("cls");
}



//*************************************************************
//*	Funcion: 
//* -------
//*		Funcion que traduce direcciones logicas a fisicas
//*			 de cada uno de los procesos almacenados en las
//*			 estructuras.
//*
//*
//*
//*	Algoritmo:
//* ---------
//*		El algoritmo te pide que introduzcas el formato de entrada de la direccion:
//*    	
//*    	 	- Valor Entero. Ej: 58
//*    	 	- Par de valores. Ej: 2.46
//*    	 	
//*     Una vez recibidos el dato, muestra por pantalla y almacena en fichero
//*		las correspondientes direcciones fisicas de cada uno de los procesos
//*     en cada uno de los metodos de gestion de memoria.
//*       
//*     En cada uno de ellos, comprobara que tipo de datos estamos intentando convertir, dandonos tres posibles salidas:
//*
//*     	- 'Direccion fisica correspondiente'.
//*     	- Error. Direccion Incorrecta. (El formato es compatible pero el proceso no tiene esa direccion)
//*       	- Error. Formato Incompatible.
//*
//*************************************************************
void traducirDirecciones(char *archivo, int numProcesos, registro t_registros[], paginacion t_paginacion, segmentacion t_segmentacion){
	FILE *fp;
	int i, opcion, dLogica, marco, nPag_Seg, nInstruccion, nPaginas, nSegmentos;
	registro segmento;	
	char *salida = (char*)malloc(sizeof(char) * 100);		// Nombre del archivo de salida
	
	/* Para obtener el nombre del fichero de salida en funcion del de entrada */
	strcpy (salida, "Salida_");
	strcat (salida, archivo);
	    	
	if((fp=fopen(salida,"w"))==NULL){
		system("cls");
		printf ("Error. No ha sido posible abrir archivo de salida.\n");
		system("pause");
	}
    else{
    	
    	do{
		  	system("cls");

			printf ("Obtener direccion fisica a partir de direccion logica.\n\n");
			printf ("Elige una opcion:\n");
			printf ("\t1.- Introducir un valor (p.ej. 306)\n");
			printf ("\t2.- Introduce un par de valores (p.ej. (2,46))\n\n");
			printf ("\t0.- Salir\n\n");
		
			scanf ("%d", &opcion);
			
			system("cls");
						
			validarDireccion(opcion, fp, &dLogica, &nPag_Seg, &nInstruccion);
			
			switch (opcion){
				case 1:
				case 2:
					for (i = 0 ; i < numProcesos ; i++){
						printf ("\t\t----------------------------------\n");		fprintf (fp, "\t\t----------------------------------\n");
						printf ("\t\tDirecciones Fisicas del Proceso %d\n", i);		fprintf (fp, "\t\tDirecciones Fisicas del Proceso %d\n", i);
						printf ("\t\t----------------------------------\n\n");		fprintf (fp, "\t\t----------------------------------\n\n");
						
						printf ("********************\n");							fprintf (fp, "********************\n");
						printf ("Registro Base/Limite\n");							fprintf (fp, "Registro Base/Limite\n");
						printf ("********************\n\n");						fprintf (fp, "********************\n\n");
						
	
						// Mostramos uns salida en funcion del tipo de dato introducido y su valor
						if (opcion == 1){
							// Valores de registro Base/Limite del proceso.		
							printf ("\tBase: %d\tLimite: %d\t(Ultima direccion: %d)\n\n", t_registros[i].base, t_registros[i].limite, t_registros[i].base + t_registros[i].limite);
							fprintf (fp, "\tBase: %d\tLimite: %d\t(Ultima direccion: %d)\n\n", t_registros[i].base, t_registros[i].limite, t_registros[i].base + t_registros[i].limite);
						
							if (dLogica > t_registros[i].limite){
								printf ("\tError. Direccion no permitida. ");
								printf ("\t(%d > %d)", dLogica, t_registros[i].limite);
								
								fprintf (fp, "\tError. Direccion no permitida. ");
								fprintf (fp, "\t(%d > %d)", dLogica, t_registros[i].limite);
							} 
							else{
								printf ("\tDireccion fisica: %d ", t_registros[i].base + dLogica);
								printf ("(%d + %d)", t_registros[i].base, dLogica);
								
								fprintf (fp,"\tDireccion fisica: %d ", t_registros[i].base + dLogica);
								fprintf (fp,"(%d + %d)", t_registros[i].base, dLogica);
							}
						}
						else{
							printf ("\tIncompatible. Error de formato.");	
							fprintf (fp,"\tIncompatible. Error de formato.");	
						}				
						
						printf ("\n\n\n");											fprintf (fp,"\n\n\n");
						
						printf ("************\n");									fprintf (fp,"************\n");
						printf ("Segmentacion\n");									fprintf (fp,"Segmentacion\n");
						printf ("************\n\n");								fprintf (fp,"************\n\n");
	
						
						// Mostramos uns salida en funcion del tipo de dato introducido y su valor
						if (opcion == 1){
							printf ("\tIncompatible. Error de formato.");	
							fprintf (fp,"\tIncompatible. Error de formato.");
						}
						else{
							// Numero de segmentos del proceso
							nSegmentos = t_segmentacion.RBTS_RLTS[i].limite + 1;
							printf ("\tNumero de segmentos del proceso: %d ", nSegmentos);							
							printf ("(del %d al %d)\n\n", t_segmentacion.RBTS_RLTS[i].base, t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite);
							
							fprintf (fp,"\tNumero de segmentos del proceso: %d ", nSegmentos);
							fprintf (fp,"(del %d al %d)\n\n", t_segmentacion.RBTS_RLTS[i].base, t_segmentacion.RBTS_RLTS[i].base + t_segmentacion.RBTS_RLTS[i].limite);
						
							if (nPag_Seg >= nSegmentos){
								printf ("\tError. Segmento incorrecto.");
								printf ("(%d > %d)", nPag_Seg, nSegmentos - 1);
								
								fprintf (fp,"\tError. Segmento incorrecto.");
								fprintf (fp,"(%d > %d)", nPag_Seg, nSegmentos - 1);
							}else{							
								segmento = t_segmentacion.SDT[t_segmentacion.RBTS_RLTS[i].base + nPag_Seg];
								printf ("\tBase Segmento: %d\tLimite Segmento: %d\n\n", segmento.base, segmento.limite);
								fprintf (fp,"\tBase Segmento: %d\tLimite Segmento: %d\n\n", segmento.base, segmento.limite);
								if (nInstruccion > segmento.limite){
									printf ("\tError. Valor de instruccion incorrecta. ");								
									printf ("(%d > %d)", nInstruccion, segmento.limite);
									
									fprintf (fp,"\tError. Valor de instruccion incorrecta. ");								
									fprintf (fp,"(%d > %d)", nInstruccion, segmento.limite);
								}	
								else{
									printf ("\tDireccion fisica: %d ", segmento.base + nInstruccion);	
									printf ("(%d + %d)", segmento.base, nInstruccion);
									
									fprintf (fp,"\tDireccion fisica: %d ", segmento.base + nInstruccion);	
									fprintf (fp,"(%d + %d)", segmento.base, nInstruccion);
								}
							}
						}
						
						printf ("\n\n\n");											fprintf (fp,"\n\n\n");
						
						printf ("***********\n");									fprintf (fp,"***********\n");
						printf ("Paginacion:\n");									fprintf (fp,"Paginacion:\n");
						printf ("***********\n\n");									fprintf (fp,"***********\n\n");
												
						// Valores generales de paginacion interesantes de mostrar por pantalla.											
						printf ("\tTamanyo de pagina: %d ", t_paginacion.tamano);
						printf ("(Del 0 al %d)\n\n", t_paginacion.tamano - 1);
						
						fprintf (fp,"\tTamanyo de pagina: %d ", t_paginacion.tamano);
						fprintf (fp,"(Del 0 al %d)\n\n", t_paginacion.tamano - 1);
						
						nPaginas = t_paginacion.RBTP_RLTP[i].limite + 1;
						
						printf ("\tNumero de paginas del proceso: %d ", nPaginas);
						printf ("(de la %d a la %d)\n\n", t_paginacion.RBTP_RLTP[i].base, t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite);
						
						fprintf (fp,"\tNumero de paginas del proceso: %d ", nPaginas);
						fprintf (fp,"(de la %d a la %d)\n\n", t_paginacion.RBTP_RLTP[i].base, t_paginacion.RBTP_RLTP[i].base + t_paginacion.RBTP_RLTP[i].limite);
						
						// Si el formato es de un valor, lo convertimos al formato de dos valores.
						if (opcion == 1){
							nPag_Seg = dLogica / t_paginacion.tamano;
							nInstruccion = dLogica % t_paginacion.tamano;						
						}
						
						if (nPag_Seg >= nPaginas){
							printf ("\tError. Pagina incorrecta. (");
							fprintf (fp,"\tError. Pagina incorrecta. (");
							if (opcion == 1){
								printf ("%d/%d = ", dLogica, t_paginacion.tamano);	
								fprintf (fp,"%d/%d = ", dLogica, t_paginacion.tamano);
							}
							printf ("%d > %d)", nPag_Seg, nPaginas - 1);
							fprintf (fp,"%d > %d)", nPag_Seg, nPaginas - 1);
						}else if(nInstruccion >= t_paginacion.tamano){
							printf ("\tError. Valor de instruccion incorrecto. ");
							printf ("(%d > %d)", nInstruccion, t_paginacion.tamano - 1);
							
							fprintf (fp,"\tError. Valor de instruccion incorrecto. ");
							fprintf (fp,"(%d > %d)", nInstruccion, t_paginacion.tamano - 1);
						}else{
						
							if (t_paginacion.tipo == 0){
					        	/* PMT Global */
					        	marco = t_paginacion.PMT_Global[t_paginacion.RBTP_RLTP[i].base + nPag_Seg];	
					        }
					        else{
					        	/* PMT por proceso */
					        	marco = t_paginacion.PMT[i][t_paginacion.RBTP_RLTP[i].base + nPag_Seg];	
					        }
					    	
							printf ("\tMarco de pagina donde esta la pagina ");
							fprintf (fp,"\tMarco de pagina donde esta la pagina ");
							
							if (opcion == 1){
								printf ("(%d/%d = ) ", dLogica, t_paginacion.tamano);	
								fprintf (fp,"(%d/%d = ) ", dLogica, t_paginacion.tamano);
							} 
							printf ("%d: %d\n\n", nPag_Seg, marco);
							fprintf (fp,"%d: %d\n\n", nPag_Seg, marco);
							
							if (marco == -1){
								printf ("\tMarco de pagina no valido.");
								fprintf (fp,"\tMarco de pagina no valido.");
							}else{						
								printf ("\tDireccion fisica: %d ", marco*t_paginacion.tamano + nInstruccion);
								printf ("(%d*%d + %d)", marco, t_paginacion.tamano, nInstruccion);
								
								fprintf (fp,"\tDireccion fisica: %d ", marco*t_paginacion.tamano + nInstruccion);
								fprintf (fp,"(%d*%d + %d)", marco, t_paginacion.tamano, nInstruccion);
							}
							
							
						}
						
						printf ("\n\n\n\n\n\n");
						fprintf (fp,"\n\n\n\n\n\n");
					}
					
					break;
					
				case 0:
					break;
					
				default:
					printf ("Opcion no valida\n\n");	
					break;
			}
			
			system("pause");
	
		}while (opcion != 0);
    	
    	fclose(fp);
    	fp = NULL;
    }           
}



//*************************************************************
//*	Funcion: 
//* -------
//*		Funcion que valida las direcciones, en cualquiera de los dos formatos
//* 	introducidas desde teclado.
//*
//*
//*
//*	Algoritmo:
//* ---------
//*		El algoritmo te pide una direccion logica en un formato u otro en
//*		funcion de la opcion pasada por parametros, que previamente tambien ha sido
//*		introducida por teclado tras mostrar el menu correspondiente.
//*		Tras cada entrada por teclado, se le muestra al usuario por pantalla el valor
//*		que se ha capturado, para que confirme si es correcta esa informacion.
//*		En caso contrario, vuelve a pedir la direccion.
//*
//*************************************************************
void validarDireccion(int opcion, FILE *fp, int *dLogica, int *nPag_Seg, int *nInstruccion){
	char validez;
				
	switch (opcion){
		case 1:			
			
			do{
				printf ("Introduce direccion logica (valor entero): ");
				scanf ("%d", dLogica);
				fflush(stdin);
				
				system("cls");
				
				printf ("Direccion logica introducida: %d\n\n", *dLogica);
				printf ("Es correcto? (s/n)");
				validez = getch();
				
				system("cls");
				
			}while (validez!='s');			
			
			system("cls");
			
			fprintf (fp, "*****************************************************************\n");
			fprintf (fp, "*****************************************************************\n");
				
			printf ("\t\tDireccion Logica: %d\n\n\n", *dLogica);
			fprintf (fp, "\t\tDireccion Logica: %d\n\n\n", *dLogica);
		
			break;
			
		case 2:
			do{
				printf ("Introduce direccion logica (Par de valores enteros separados por comas, sin espacios): ");
				scanf ("%d,%d", nPag_Seg, nInstruccion);
				fflush(stdin);
				
				system("cls");
				
				printf ("Direccion logica introducida: (%d,%d)\n\n", *nPag_Seg, *nInstruccion);
				printf ("Es correcto? (s/n)");
				validez = getch();
				
				system("cls");
				
			}while (validez!='s');
						
			system("cls");
	
			fprintf (fp, "*****************************************************************\n");
			fprintf (fp, "*****************************************************************\n");	
				
			printf ("\t\tDireccion Logica: (%d,%d)\n\n\n", *nPag_Seg, *nInstruccion);
			fprintf (fp, "\t\tDireccion Logica: (%d,%d)\n\n\n", *nPag_Seg, *nInstruccion);	
		
			break;		
	}
	
}

