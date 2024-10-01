# datafusiondb_jjosnayo

## Para ejecutar el programa ejecuta .\datafusion.exe

El repositorio está compuesto por los siguientes archivos:

-anime-dataset-2023.csv: El dataset original de kaggle.

-datos.csv: El archivo csv filtrado con los datos con los que se van a trabajar.

-filtro.py: Para filtrar el dataset original.

-main.cpp: Contiene la interfaz del programa.

-parser.h: Contiene la implementación del parser que transforma las consultas en lenguaje natural en las funciones de las clases.

-inicialización.h: Contiene la generación de los índices con los que se van a trabajar, está aislado del programa.

-auxiliar.cpp: Contiene las llamadas a la generación de los índices, además de pruebas de las funciones de cada estructura, está hecho para inicializar, está aislado del programa.

-datafusion.h: Contiene la clase virtual de la que se derivan los métodos de indexación.

-seqfile.h: Contiene la implementación del archivo secuencial.

-avl.h: Contiene la implementación del archivo AVL.

-ext_hash: Contiene la implementación del extendable hashing además de la estructura del bloque.

-archivo_secuencial.bin: Índice del archivo secuencial.

-archivo_secuencial_aux.bin: Datos de configuración del archivo secuencial.

-arbol_binario_balanceado.bin: Índice del AVL.

-tabla_hash_extensible.bin: Índice del extendible hashing.

-----------------------------------------------------------------------------------------------------------------
El funcionamiento es el siguiente: el usuario eligirá el método de indexación e ingresará sus consultas las cuales pasarán del main al parser para ejecutar las funciones del método de indexación elegido. Los archivos indexados necesarios para cada método ya estarán creados previamente mediante auxiliar.cpp e inicialización.h. El filtro funciona solo mediante la llave primaria, donde no deberían haber elementos repetidos, aunque puede manejar cualquier tipo de llave para trabajar con tablas con otros tipos de llave. El programa está configurado para trabajar con una tabla en específico, la cual es la del archivo csv, por eso ya tiene configurados los atributos y tipos de la tabla, que se encuentran en la especificación del registro llamado Anime.
