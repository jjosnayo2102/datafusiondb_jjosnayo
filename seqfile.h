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

template<typename TK> // para indexar cualquier tipo de llave
  class Seqfile {
    private: string filename;
    string aux_file;
    int header; //inicio de los punteros
    int limite; // cantidad máxima de registros auxiliares
    int ord_size; // número de registros ordenados
    int tam; // tamaño del archivo
    void reconstruir() {
      rename(filename.c_str(), "delete.bin");
      ifstream file("delete.bin", ios::binary);
      ofstream archivo(filename, ios::binary);
      Anime registro;
      file.seekg(sizeof(Anime) * header, ios::beg);
      file.read((char * ) & registro, sizeof(registro));
      int i = 1;
      header = 0; // archivo ordenado
      int siguiente_pos = header;
      while (registro.next != -1) {
        // buscar siguiente posición antes de modificar el puntero next
        siguiente_pos = registro.next;
        if(!registro.eliminado){ // si no está eliminado
          // actualizar el puntero next y escribir el registro
          registro.next = i;
          archivo.write((char*) &registro, sizeof(registro));
          i++;
        }
        // leer el siguiente registro desde la posición correcta
        file.seekg(sizeof(Anime)*siguiente_pos, ios::beg);
        file.read((char*) &registro, sizeof(registro));
      }
      // último registro (next == -1)
      if(!registro.eliminado){
        archivo.write((char*) &registro, sizeof(registro));
      }
      file.close();
      archivo.close();
      // eliminar viejo
      remove("delete.bin");
      ord_size = tam;
      if(tam != 0){
        limite = max(5, static_cast<int> (log2(tam)));
      }else{ // para la eliminación
        limite = 5; 
      }
    }

    bool insertar_aux(Anime registro) { //tomar en cuenta los eliminados: -2
      if (tam == 0) {
        // El archivo no tiene registros aún
        ofstream file(filename, ios::binary);
        file.write((char * ) & registro, sizeof(registro));
        file.close();
        tam += 1;
        return true;
      }
      fstream file(filename, ios::in | ios::out | ios::binary);
      Anime anime, nanime;
      int pos = header;
      int npos = -1;
      // Leer el primer registro
      file.seekg(sizeof(Anime) * header, ios::beg);
      file.read((char * ) & anime, sizeof(anime));
      // Caso 1: si el registro es menor que el primero
      if (anime.id > registro.id) {
        // Obtener nueva posición al final del archivo
        file.seekg(0, ios::end);
        long p = file.tellg();
        int ntam = p / sizeof(Anime);
        // El nuevo registro apunta al antiguo header
        registro.next = header;
        header = ntam; // Actualizar el header al nuevo registro
        // Insertar el nuevo registro al final
        file.write((char * ) & registro, sizeof(registro));
        file.close();
        tam += 1;
        // Verificar si se debe reconstruir
        if ((tam - ord_size) >= limite) {
          reconstruir();
        }
        return true;
      }
      // Caso 2: si el registro ya existe, no hacer nada
      if (anime.id == registro.id) {
        file.close();
        return false;
      }
      // Iterar sobre los registros hasta encontrar la posición correcta
      while (anime.next != -1) {
        npos = anime.next;
        file.seekg(sizeof(Anime) * npos, ios::beg);
        file.read((char * ) & nanime, sizeof(nanime));
        if (anime.id < registro.id && registro.id < nanime.id) {
          // Obtener nueva posición al final del archivo
          file.seekg(0, ios::end);
          long p = file.tellg();
          int ntam = p / sizeof(Anime);
          // El nuevo registro apunta al siguiente
          registro.next = npos;
          file.write((char * ) & registro, sizeof(registro));
          // El registro actual debe apuntar al nuevo registro
          anime.next = ntam;
          file.seekg(sizeof(Anime) * pos, ios::beg);
          file.write((char * ) & anime, sizeof(anime));
          file.close();
          tam += 1;
          // Verificar si se debe reconstruir
          if ((tam - ord_size) >= limite) {
            reconstruir();
          }
          return true;
        }
        // Caso 2: si el registro ya existe, no hacer nada
        if (anime.id == registro.id) {
          file.close();
          return false;
        }
        // Avanzar al siguiente registro
        anime = nanime;
        pos = npos;
      }
      // Caso 3: si el registro es mayor que el último
      if (anime.id < registro.id) {
        // Obtener nueva posición al final del archivo
        file.seekg(0, ios::end);
        long p = file.tellg();
        int ntam = p / sizeof(Anime);
        // Actualizar el último registro para que apunte al nuevo
        anime.next = ntam;
        file.seekg(sizeof(Anime) * pos, ios::beg);
        file.write((char * ) & anime, sizeof(anime));
        // Insertar el nuevo registro al final
        file.write((char * ) & registro, sizeof(registro));
        file.close();
      }
      // Caso 2: si el registro ya existe, no hacer nada
      if (anime.id == registro.id) {
        file.close();
        return false;
      }
      tam += 1;
      // Verificar si se debe reconstruir
      if ((tam - ord_size) >= limite) {
        reconstruir();
      }
      return true;
    }

    Anime buscar_aux(TK key){
      Anime defa;
      ifstream file(filename, ios::binary);
      file.seekg(sizeof(Anime)*header, ios::beg);
      Anime anime;
      file.read((char*) &anime, sizeof(anime));
      while(anime.next != -1  && !anime.eliminado){
        if(anime.id == key){
          file.close();
          return anime;
        }
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) &anime, sizeof(anime));
      }
      if(anime.id == key && !anime.eliminado){
        file.close();
        return anime;
      }
      file.close();
      return defa;
    }

    vector<Anime> buscar_por_rango_aux(TK key1, TK key2){
      vector<Anime> animes;
      ifstream file(filename, ios::binary);
      file.seekg(sizeof(Anime)*header, ios::beg);
      Anime anime;
      file.read((char*) &anime, sizeof(anime));
      while(anime.next != -1){
        if((key1 <= anime.id) && (anime.id <= key2)){
          animes.push_back(anime);
        }
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) &anime, sizeof(anime));
      }
      if((key1 <= anime.id) && (anime.id <= key2)){
        animes.push_back(anime);
      }
      return animes;
    }

    bool remover_aux(TK key){
      fstream file(filename, ios::in | ios::out | ios::binary);
      file.seekg(sizeof(Anime)*header, ios::beg);
      Anime anime;
      file.read((char*) &anime, sizeof(anime));
      int pos = header;
      while(anime.next != -1  && !anime.eliminado){
        if(anime.id == key){
          anime.eliminado = true;
          file.seekg(sizeof(Anime)*pos, ios::beg);
          file.write((char*) &anime, sizeof(anime));
          file.close();
          tam--;
          if(tam == 0){
            reconstruir();
          }
          return true;
        }
        pos = anime.next;
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) &anime, sizeof(anime));
      }
      if(anime.id == key && !anime.eliminado){
        file.close();
        tam--;
        if(tam == 0){
          reconstruir();
        }
        return true;
      }
      file.close();
      return false;
    }

    int buscar_posicion(TK key) {
      int inicio = 0;
      int fin = ord_size - 1;
      int m = -1; // Si no se encuentra nada, devuelve -1
      int menor_cercano = -1; // Variable para almacenar el menor cercano
      ifstream file(filename, ios::binary);
      while (inicio <= fin) {
        m = inicio + (fin - inicio) / 2;
        Anime reg;
        file.seekg(sizeof(Anime)*m, ios::beg);
        file.read((char * ) &reg, sizeof(Anime));
        if(reg.id == key){
          return m;
        }
        if (reg.id < key) {
          menor_cercano = m; // Actualiza menor_cercano
          inicio = m + 1; // Busca en la parte derecha
        } else {
          if(m == 0) return m;
          fin = m - 1; // Busca en la parte izquierda
        }
      }
      return menor_cercano; // Devuelve la posición del menor cercano
    }
public: 
    Seqfile(string _filename) {
      filename = _filename + ".bin";
      aux_file = _filename + "_aux.bin";
      ifstream file(filename, ios::binary);
      if (!file.is_open()) {
        header = 0;
        limite = 5;
        ord_size = 0;
        tam = 0;
      }
      file.close();
      ifstream archivo(aux_file, ios::binary);
      archivo.read((char*) & header, sizeof(header));
      archivo.read((char*) & limite, sizeof(limite));
      archivo.read((char*) & ord_size, sizeof(ord_size));
      archivo.read((char*) & tam, sizeof(tam));
      archivo.close();
    }

    bool insertar(Anime registro){
      if(ord_size > 0){
        int pos = buscar_posicion(registro.id);
        Anime anime;
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(sizeof(Anime)*pos, ios::beg);
        file.read((char*) & anime, sizeof(anime));
        if (pos == 0 && registro.id < anime.id && !anime.eliminado) {
          // leer el header
          Anime actual; // actual
          file.seekg(sizeof(Anime)*header, ios::beg);
          file.read((char*) & actual, sizeof(actual));
          // Modificar el header para que apunte al nuevo registro
          if(actual.id > registro.id){
            // Asignar el nuevo registro como header
            file.seekg(0, ios::end);
            int ntam = file.tellg()/sizeof(Anime);
            registro.next = header;
            file.write((char*) & registro, sizeof(registro));
            header = ntam;
            tam += 1;
            file.close();
            if ((tam - ord_size) >= limite) {
              reconstruir();
            }
            return true;
          }
          int apos = header;
          int npos = actual.next;
          Anime siguiente;
          file.seekg(sizeof(Anime)*actual.next, ios::beg);
          file.read((char*) & siguiente, sizeof(siguiente));
          while(actual.next > ord_size) {
            if(actual.id == registro.id && !actual.eliminado){ // si está eliminado se puede volver a poner en la misma posición
              file.close();
              return false;
            }
            if((actual.id < registro.id) && (registro.id < siguiente.id) && !actual.eliminado){
              file.seekg(0, ios::end);
              int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
              registro.next = actual.next;
              file.write((char*)&registro, sizeof(registro));
              actual.next = ntam;
              file.seekg(sizeof(Anime)*apos, ios::beg);
              file.write((char*)&actual, sizeof(actual));
              tam += 1;
              file.close();
              if ((tam - ord_size) >= limite) {
                reconstruir();
              }
              return true;
            }
            // Mover al siguiente registro
            actual = siguiente;
            file.seekg(sizeof(Anime)*siguiente.next, ios::beg);
            file.read((char*)&siguiente, sizeof(siguiente));
          }
          if(actual.id == registro.id && !actual.eliminado){ // si está eliminado se puede volver a poner en la misma posición
            file.close();
            return false;
          }
          // si es menor que el primero pero es mayor que la lista de menores se pone al final de esta
          file.seekg(0, ios::end);
          int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
          registro.next = actual.next;
          file.write((char*)&registro, sizeof(registro));
          actual.next = ntam;
          file.seekg(sizeof(Anime)*apos, ios::beg);
          file.write((char*)&actual, sizeof(actual));
          tam += 1;
          file.close();
          if ((tam - ord_size) >= limite) {
              reconstruir();
          }
          return true;
        }
        // apunta a ordenado: modificar puntero y agregarlo al nuevo elemento al final
        if(anime.next < ord_size){ // también toma en cuenta el -1
          file.seekg(0, ios::end);
          int ntam = file.tellg()/sizeof(Anime); 
          registro.next = anime.next;
          file.write((char*)&registro, sizeof(registro));
          anime.next = ntam;
          file.seekg(sizeof(Anime)*pos, ios::beg);
          file.write((char*)&anime, sizeof(anime));
          tam += 1;
          file.close();
          if ((tam - ord_size) >= limite) {
            reconstruir();
          }
          return true;
        }
        // apunta a auxiliar: ubicar en la lista de auxiliares
        else{
          Anime actual = anime;
          int apos = pos;
          int npos = actual.next;
          Anime siguiente;
          file.seekg(sizeof(Anime)*actual.next, ios::beg);
          file.read((char*) & siguiente, sizeof(siguiente));
          while(actual.next > ord_size) {
            if(actual.id == registro.id && !actual.eliminado){ // si está eliminado se puede volver a poner en la misma posición
              file.close();
              return false;
            }
            if((actual.id < registro.id) && (registro.id < siguiente.id) && !actual.eliminado){
              file.seekg(0, ios::end);
              int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
              registro.next = actual.next;
              file.write((char*)&registro, sizeof(registro));
              actual.next = ntam;
              file.seekg(sizeof(Anime)*apos, ios::beg);
              file.write((char*)&actual, sizeof(actual));
              tam += 1;
              file.close();
              if ((tam - ord_size) >= limite) {
                reconstruir();
              }
              return true;
            }
            // Mover al siguiente registro
            actual = siguiente;
            file.seekg(sizeof(Anime)*siguiente.next, ios::beg);
            file.read((char*)&siguiente, sizeof(siguiente));
          }
          if(actual.id == registro.id && !actual.eliminado){ // si está eliminado se puede volver a poner en la misma posición
            file.close();
            return false;
          }
          // si es menor que el elemento ordenado pero es mayor que la lista se pone al final de esta
          file.seekg(0, ios::end);
          int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
          registro.next = actual.next;
          file.write((char*)&registro, sizeof(registro));
          actual.next = ntam;
          file.seekg(sizeof(Anime)*apos, ios::beg);
          file.write((char*)&actual, sizeof(actual));
          tam += 1;
          file.close();
          if ((tam - ord_size) >= limite) {
              reconstruir();
          }
          return true;
        }
      }else{
        return insertar_aux(registro);
      }
    }

    Anime buscar(TK key){
      // tomar en cuenta cuando no hay ordenado
      if(ord_size == 0){
        return buscar_aux(key);
      }
      int pos = buscar_posicion(key);
      Anime defa; 
      Anime anime;
      ifstream file(filename, ios::binary);
      file.seekg(sizeof(Anime)*pos, ios::beg);
      file.read((char*) & anime, sizeof(anime));
      // tomar en cuenta caso donde el elemento esté antes del primer ordenado

      if(anime.id == key && !anime.eliminado){
        file.close();
        return anime;
      }else{
        // buscar en auxiliar hasta el final o el siguiente ordenado
        if(anime.next < ord_size || anime.next == -1){
          return defa;
        }
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) & anime, sizeof(anime));
        while(anime.next >= ord_size || anime.next != -1){
          if(anime.id == key  && !anime.eliminado){
            file.close();
            return anime;
          }
          file.seekg(sizeof(Anime)*anime.next, ios::beg);
          file.read((char*) & anime, sizeof(anime));
        }
        file.close();
        return defa;
      }
    }
    
    vector<Anime> buscar_por_rango(TK key1, TK key2){
      // si no hay ordenados
      if(ord_size == 0){
        return buscar_por_rango_aux(key1, key2);
      }
      vector<Anime> animes;
      int inicio = buscar_posicion(key1);
      // tomar en cuenta caso donde el inicio del rango esté antes del primer ordenado

      ifstream file(filename, ios::binary);
      Anime anime;
      file.seekg(sizeof(Anime)*inicio, ios::beg);
      file.read((char*) & anime, sizeof(anime));
      while(anime.next != -1  && !anime.eliminado){
        if(anime.id > key2){
          return animes;
        }
        animes.push_back(anime);
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) & anime, sizeof(anime));
      }
      if(anime.id < key2  && !anime.eliminado){
        animes.push_back(anime);
      } 
      file.close();
      return animes;
    }
    
    // los efectos se muestran en la reconstrucción
    bool remover(TK key){
      // tomar en cuenta cuando no hay ordenado
      if(ord_size == 0){
        return remover_aux(key);
      }
      int pos = buscar_posicion(key);
      Anime anime;
      fstream file(filename, ios::in | ios::out | ios::binary);
      file.seekg(sizeof(Anime)*pos, ios::beg);
      file.read((char*) & anime, sizeof(anime));
      // tomar en cuenta caso donde el elemento esté antes del primer ordenado

      if(anime.id == key){
        anime.eliminado = true; // eliminación lógica
        file.seekg(sizeof(Anime)*pos, ios::beg);
        file.write((char*) & anime, sizeof(anime));
        file.close();
        tam--;
        if(tam - ord_size < 0){ // reconstruir en un archivo más pequeño
          reconstruir(); 
        }
        return true;
      }else{
        // buscar en auxiliar hasta el final o el siguiente ordenado
        if(anime.next < ord_size || anime.next == -1){
          return false; // no hay elemento con esa llave a eliminar
        }
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) & anime, sizeof(anime));
        while(anime.next >= ord_size || anime.next != -1){
          if(anime.id == key){
            anime.eliminado = true; // eliminación lógica
            file.seekg(sizeof(Anime)*pos, ios::beg);
            file.write((char*) & anime, sizeof(anime));
            file.close();
            tam--;
            if(tam - ord_size < 0){ // reconstruir en un archivo más pequeño
              reconstruir(); 
            }
            return true;
          }
          file.seekg(sizeof(Anime)*anime.next, ios::beg);
          file.read((char*) & anime, sizeof(anime));
          pos = anime.next;
        }
        return false; // no hay elemento con esa llave a eliminar
      }
    } // el booleano indica si se eliminó o no
    
    ~Seqfile() {
        // header,limite,ord_size,tam
        ofstream file(aux_file, ios::binary);
        file.write((char * ) & header, sizeof(header));
        file.write((char * ) & limite, sizeof(limite));
        file.write((char * ) & ord_size, sizeof(ord_size));
        file.write((char * ) & tam, sizeof(tam));
        file.close();
    }
};
