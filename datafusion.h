#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>
using namespace std;

// en este caso la llave es int, en la creación de la tabla se describen los tipos y nombres para crear la clase registro
struct Anime {
  int id; // llave primaria
  char nombre[100]; // nombre original
  float puntaje; // del 0 al 10
  char genero[50]; // a qué tipos de tramas pertenece
  char tipo[10]; // serie, película o especial
  float temporada; //estación y año en el que salió (año.(1,2,3,4) 1:verano, 2:otoño, 3:invierno, 4:primavera)
  char estado[20]; // si ha terminado, está en emisión o aún no se emite
  char estudio[30]; // empresa que la desarrolló
  int next = -1; // para el archivo secuencial
  bool eliminado = false; // para el archivo secuencial
  int padre = -1; // para el AVL
  int left = -1; // para el AVL
  int right = -1; // para el AVL
  Anime(){
    id = -1;
    strcpy(nombre, "UNKNOWN");
    puntaje = -1;
    strcpy(genero, "UNKNOWN");
    strcpy(tipo, "UNKNOWN");
    temporada = -1;
    strcpy(estado, "UNKNOWN");
    strcpy(estudio, "UNKNOWN");
  }
  void show() {
    cout << id << endl;
    cout << nombre << endl;
    cout << puntaje << endl;
    cout << genero << endl;
    cout << tipo << endl;
    cout << temporada << endl;
    cout << estado << endl;
    cout << estudio << endl;
  }
};

template<typename TK>
class DataFusion {
public:
    virtual bool insertar(Anime registro) = 0;
    virtual Anime buscar(TK key) = 0;
    virtual vector<Anime> buscar_por_rango(TK key1, TK key2) = 0;
    virtual bool remover(TK key) = 0;
    // Destructor virtual para asegurar la correcta destrucción de objetos derivados.
    virtual ~DataFusion() {}
};