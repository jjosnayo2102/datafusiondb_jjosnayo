#include "seqfile.h"
#include <sstream>

void leerCSV_Seqfile(const string& filename) {
  Seqfile<int>* sf= new Seqfile<int>("archivo_secuencial");
  ifstream file(filename);
  string line;
  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
  }
  getline(file, line); // primero lee los nombres de los atributos
  int i = 0;
  while (i < 100000) {
    getline(file, line);
    stringstream ss(line);
    string token;
    Anime record;
    getline(ss, token, '|');
    record.id = stoi(token);
    getline(ss, token, '|');
    strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
    record.nombre[sizeof(record.nombre) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, '|');
    if(token != "UNKNOWN"){
      record.puntaje = stof(token);
    }else{
      record.puntaje = -1;
    }
    getline(ss, token, '|');
    if(token != "UNKNOWN"){
      strncpy(record.genero, token.c_str(), sizeof(record.genero));
      record.genero[sizeof(record.genero) - 1] = '\0';  // Asegurar el null-termination
    }else{
      strncpy(record.genero, "", sizeof(record.genero));
    }
    getline(ss, token, '|');
    strncpy(record.tipo, token.c_str(), sizeof(record.tipo));
    record.tipo[sizeof(record.tipo) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, '|');
    if(!token.empty()){
      record.temporada = stof(token);
    }else{
      record.temporada = -1;
    }
    getline(ss, token, '|');
    strncpy(record.estado, token.c_str(), sizeof(record.estado));
    record.estado[sizeof(record.estado) - 1] = '\0';  // Asegurar el null-termination
    getline(ss, token, '|');
    if(token != "UNKNOWN"){
      strncpy(record.estudio, token.c_str(), sizeof(record.estudio));
      record.estudio[sizeof(record.estudio) - 1] = '\0';  // Asegurar el null-termination
    }else{
      strncpy(record.estudio, "", sizeof(record.estudio));
    }
    sf->insertar(record);
    i++;
  }
  file.close();
  delete sf;
}

void mostrar(){
  ifstream file("prueba_secuencial.bin", ios::binary);
  Anime anime;
  while(file.read((char*) &anime, sizeof(Anime))){
    cout << anime.id << endl;
    cout << anime.next << endl;
    cout << "eliminado?: " << anime.eliminado << endl;
  }
}

int main() {
    // prueba de funciones auxiliares
    //string nombreArchivo = "datos.csv";
    // inserción
    //leerCSV_Seqfile(nombreArchivo);
    //mostrar();
    //cout << endl;
    // búsqueda
    //auto sf1 = new Seqfile<int>("prueba_secuencial");
    //Anime res = sf1->buscar(6682); // puede no estar, he metido random
    //res.show();
    //cout << endl;
    // búsqueda por rango
    //auto rango = sf1->buscar_por_rango(18677, 38016);
    //cout << rango.size() << endl;
    //for(int i = 0; i < rango.size(); i++){
    //  rango[i].show();
    //}
    //cout << endl;
    // eliminación
    //sf1->remover(37393);
    //mostrar();
    // delete sf1;
    // funciona
    // prueba general
    string nombreArchivo = "datos.csv";
    // inserción
    //leerCSV_Seqfile(nombreArchivo);
    auto sf1 = new Seqfile<int>("archivo_secuencial");
    Anime res = sf1->buscar(170);
    res.show();
    return 0;
}