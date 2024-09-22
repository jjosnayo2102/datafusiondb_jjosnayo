#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
using namespace std;

struct Anime {
    int id; //llave primaria
    char nombre[100]; // nombre original
    float puntaje; // del 0 al 10
    char genero[50]; // a qué tipos de tramas pertenece
    float temporada; //estación y año en el que salió
    char tipo[10]; // serie, película o especial
    char estado[20]; // si ha terminado, está en emisión o aún no se emite
    char estudio[30]; // empresa que la desarrolló
    void show(){
        cout << id << endl;
        cout << nombre << endl;
        cout << puntaje << endl;
        cout << genero << endl;
        cout << temporada << endl;
        cout << tipo << endl;
        cout << estado << endl;
        cout << estudio << endl;
    }
};

vector<Anime> leerCSV(const string& filename) {
  ifstream file(filename);
  vector<Anime> records;
  string line;
  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
    return records;
  }
  getline(file, line); // primero lee los nombres de los atributos
  int i = 0;
  while (i < 5) {
    getline(file, line);
    stringstream ss(line);
    string token;
    Anime record;
    getline(ss, token, ';');
    record.id = stoi(token);
    getline(ss, token, ';');
    strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
    record.nombre[sizeof(record.nombre) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, ';');
    if(token != "UNKNOWN"){
      record.puntaje = stof(token);
    }else{
      record.puntaje = -1;
    }
    getline(ss, token, ';');
    if(token != "UNKNOWN"){
      strncpy(record.genero, token.c_str(), sizeof(record.genero));
      record.genero[sizeof(record.genero) - 1] = '\0';  // Asegurar el null-termination
    }else{
      strncpy(record.genero, "", sizeof(record.genero));
    }
    getline(ss, token, ';');
    strncpy(record.tipo, token.c_str(), sizeof(record.tipo));
    record.tipo[sizeof(record.tipo) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, ';');
    if(!token.empty()){
      record.temporada = stof(token);
    }else{
      record.temporada = -1;
    }
    getline(ss, token, ';');
    strncpy(record.estado, token.c_str(), sizeof(record.estado));
    record.estado[sizeof(record.estado) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, ';');
    if(token != "UNKNOWN"){
      strncpy(record.estudio, token.c_str(), sizeof(record.estudio));
      record.estudio[sizeof(record.estudio) - 1] = '\0';  // Asegurar el null-termination
    }else{
      strncpy(record.estudio, "", sizeof(record.estudio));
    }
    records.push_back(record);
    i++;
  }
  file.close();
  return records;
}

int main() {
    string nombreArchivo = "datos.csv";
    vector<Anime> registros = leerCSV(nombreArchivo);
    for(auto it: registros){
        it.show();
    }
    return 0;
}
