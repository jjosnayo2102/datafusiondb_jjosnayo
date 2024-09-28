#include "parser.h"

int main() {
    string nombreArchivo = "datos.csv";
    cout << "Este es datafusionDB: Una base de datos con tecnicas combinadas" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "Esta base de datos maneja informacion de animes: Tiene creada la tabla 'Anime'" << endl;
    cout << endl;
    cout << "Puedes consultar los siguientes atributos: " << endl;
    cout << "id, nombre, puntaje, genero, tipo, temporada, estado y estudio" << endl;
    cout << "id es la clave identificadora del anime" << endl;
    cout << "nombre es el nombre original del anime" << endl;
    cout << "puntaje es la nota del 1 al 10 que indica su calidad, es un int" << endl;
    cout << "genero es el tipo de trama del anime, es un string" << endl;
    cout << "tipo es el formato del anime: serie, película, etc; es un string" << endl;
    cout << "temporada es la fecha de estreno del anime codificado como año.estacion_del_año, es un float" << endl;
    cout << "estado indica si el anime esta en emision, finalizado o aún no ha sido emitido, es un string" << endl;
    cout << "estudio es la empresa que emitió el anime, es un string" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "Ingrese 1 si desea indexar de forma secuencial" << endl;
    cout << "Ingrese 2 si desea indexar a través de un árbol AVL" << endl;
    cout << "Ingrese 3 si desea indexar por extendible hashing" << endl;
    int opcion;
    while(cin >> opcion){
        switch(opcion){
        case 1: cout << "Se eligio la indexación por archivo secuencial" << endl; break;
        case 2: cout << "Se eligio la indexación por árbol AVL" << endl; break;
        case 3: cout << "Se eligio la indexación por extendible hashing" << endl; break;
        default: cout << "Ingrese un número válido" << endl; continue;
        }
        break;
    }
    cin.ignore();
    cout << "------------------------------------------------------------------" << endl;
    cout << "Bienvenido" << endl;
    cout << "Ingrese sus consultas" << endl;
    cout << "Sintaxis de consulta: Lenguaje sql en minusculas con espacios entre palabras y ; al final" << endl;
    cout << "Si quiere terminar escriba 'termine'" << endl;
    string consulta;
    getline(cin, consulta);
    DataFusion<int>* indice;
    if(opcion == 1){
        indice = new Seqfile<int>("archivo_secuencial");
    }
    if(opcion == 2){
        indice = new AVLFile<int>("arbol_binario_balanceado");
    }
    if(opcion == 3){
        indice = new ExtendibleHashFile<int>("arbol_binario_balanceado");
    }
    SQLParser<int> parser_sf(indice);
    while(consulta != "termine"){
        // llamar al parser aquí
        parser_sf.ejecutar(consulta);
        getline(cin, consulta);
    }
    cout << "Sesion finalizada" << endl;
    return 0;
}