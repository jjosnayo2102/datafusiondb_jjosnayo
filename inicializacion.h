#include "ext_hash.h"
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

void leerCSV_AVL(string filename){
  AVLFile<int>* avl= new AVLFile<int>("arbol_binario_balanceado");
  ifstream file(filename);
  string line;
  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
  }
  getline(file, line); // primero lee los nombres de los atributos
  int i  = 0;
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
    avl->insertar(record);
    i++;
  }
  file.close();
  delete avl;
}

void leerCSV_ExtHash(string filename){
  ExtendibleHashFile<int>* eh= new ExtendibleHashFile<int>("archivo_hash_extensible");
  ifstream file(filename);
  string line;
  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo.\n";
  }
  getline(file, line); // primero lee los nombres de los atributos
  int i  = 0;
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
    eh->insertar(record);
    i++;
  }
  file.close();
  delete eh;
  return;
}