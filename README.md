# datafusiondb_jjosnayo
El repositorio está compuesto por los siguientes archivos:

-anime-dataset-2023.csv: El dataset original de kaggle.

-datos.csv: El archivo csv filtrado con los datos con los que se van a trabajar.

-filtro.py: Para filtrar el dataset original.

-main.cpp: Que contiene la función para leer los datos a utilizar y convertirlos a la estructura de registros.

-seqfile.h: Para implementar el archivo secuencial.

-avl.h: Para implementar el archivo AVL.

-ext_hash: Para implementar el extendable hashing.

-----------------------------------------------------------------------------------------------------------------
La idea es que mediante el main.cpp se creen los archivos e índices necesarios para cada método de organización, para que mediante una interfaz en python se pueda seleccionar una técnica para hacer las consultas y así trabajar con sus archivos asociados. Para hacer las consultas se dispondrá de un parser que ejecutará las funciones correspondientes a las consultas SQL.
TODO:
-archivos asociados a cada método de organización
-archivo de python con el parser
-archivo de python con la interfaz
