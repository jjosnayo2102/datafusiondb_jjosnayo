#include "seqfile.h"
#include <sstream>
#include <cstring>

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
    Seqfile<int> sf("archivo_secuencial");
    for(auto it: registros){
      sf.insertar(it);
    }
    ifstream archivo("archivo_secuencial.bin", ios::binary);
    Anime anime;
    while(archivo.read((char*) &anime, sizeof(anime))){
      anime.show();
    }
    return 0;
}