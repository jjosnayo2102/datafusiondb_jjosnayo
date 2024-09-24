#include "seqfile.h"
#include <sstream>
#include <cstring>

void leerCSV_Seqfile(const string& filename) {
  Seqfile<int>* sf= new Seqfile<int>("archivo_secuencial");
  ifstream file(filename);
  string line;
  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
  }
  getline(file, line); // primero lee los nombres de los atributos
  int i = 0;
  while (i < 7) {
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

int main() {
    string nombreArchivo = "datos.csv";
    leerCSV_Seqfile(nombreArchivo);
    //auto sf1 = new Seqfile<int>("archivo_secuencial");
    //Anime res = sf1->buscar(984); // puede no estar, he metido random
    //res.show();
    //delete sf1;
    ifstream file("archivo_secuencial.bin", ios::binary);
    Anime registro;
    while(file.read((char*) &registro, sizeof(Anime))){
      cout << registro.id << endl;
      cout << registro.next << endl;
    }
    return 0;
}