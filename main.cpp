#include "inicializacion.h"

int main() {
    string nombreArchivo = "datos.csv";
    cout << "Este es datafusionDB: Una base de datos con técnicas combinadas" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "Esta base de datos maneja información de animes: Tiene creada la tabla 'Anime'" << endl;
    cout << endl;
    cout << "Puedes consultar los siguientes atributos: " << endl;
    cout << "id, nombre, puntaje, genero, tipo, temporada, estado y estudio" << endl;
    cout << "id es la clave identificadora del anime" << endl;
    cout << "nombre es el nombre original del anime" << endl;
    cout << "puntaje es la nota del 1 al 10 que indica su calidad, es un int" << endl;
    cout << "genero es el tipo de trama del anime, es un string" << endl;
    cout << "tipo es el formato del anime: serie, película, etc; es un string" << endl;
    cout << "temporada es la fecha de estreno del anime codificado como año.estación_del_año, es un float" << endl;
    cout << "estado indica si el anime está en emisión, finalizado o aún no ha sido emitido, es un string" << endl;
    cout << "estudio es la empresa que emitió el anime, es un string" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "Ingrese 1 si desea indexar de forma secuencial" << endl;
    cout << "Ingrese 2 si desea indexar a través de un árbol AVL" << endl;
    cout << "Ingrese 3 si desdea indexar por extendible hashing" << endl;
    int opcion;
    while(cin >> opcion){
        switch(opcion){
        case 1: cout << "procesando..." << endl; leerCSV_Seqfile(nombreArchivo); break;
        case 2: cout << "procesando..." << endl; leerCSV_AVL(nombreArchivo); break;
        case 3: cout << "procesando..." << endl; leerCSV_ExtHash(nombreArchivo); break;
        default: cout << "Ingrese un número válido" << endl;
        }
    }
    cout << "------------------------------------------------------------------" << endl;
    cout << "Bienvenido" << endl;
    cout << "Ingrese sus consultas" << endl;
    cout << "Si quiere terminar escriba 'termine'" << endl;
    string consulta;
    cin >> consulta;
    while(consulta != "termine"){
        cout << "Consulta ejecutada" << endl;
        // hacer el parser aquí
        cin >> consulta;
    }
    cout << "Sesión finalizada" << endl;
    return 0;
}