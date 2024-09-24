#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

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
  int left = -1; // para el AVL
  int right = -1; // para el AVL
  void show() {
    cout << id << endl;
    cout << nombre << endl;
    cout << puntaje << endl;
    cout << genero << endl;
    cout << tipo << endl;
    cout << temporada << endl;
    cout << estado << endl;
    cout << estudio << endl;
    cout << next << endl;
  }
};

template < typename TK >
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
      while (registro.next != -1) {
        // buscar siguiente posición antes de modificar el puntero next
        int siguiente_pos = registro.next;
        // actualizar el puntero next y escribir el registro
        registro.next = i;
        archivo.write((char*) &registro, sizeof(registro));
        // leer el siguiente registro desde la posición correcta
        file.seekg(sizeof(Anime)*siguiente_pos, ios::beg);
        file.read((char*) &registro, sizeof(registro));
        i++;
      }
      // último registro (next == -1)
      archivo.write((char*) &registro, sizeof(registro));
      file.close();
      archivo.close();
      // eliminar viejo
      remove("delete.bin");
      ord_size = tam;
      limite = max(5, static_cast<int> (log2(tam)));
    }

    bool insertar_aux(Anime registro) {
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

    int buscar_posicion(int key) {
      int inicio = 0;
      int fin = ord_size - 1;
      int m = -1; // Si no se encuentra nada, devuelve -1
      int menor_cercano = -1; // Variable para almacenar el menor cercano
      ifstream file(filename, ios::binary);
      while (inicio <= fin) {
        m = inicio + (fin - inicio) / 2;
        Anime reg;
        file.seekg(sizeof(Anime) * m, ios::beg);
        file.read((char * ) & reg, sizeof(Anime));
        if (reg.id < key) {
          menor_cercano = m; // Actualiza menor_cercano
          inicio = m + 1; // Busca en la parte derecha
        } else {
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
        if (pos == 0 && registro.id < anime.id) {
            // Modificar el header para que apunte al nuevo registro
            if (header == 0) {
                // Asignar el nuevo registro como header
                file.seekg(0, ios::end);
                int ntam = file.tellg()/sizeof(Anime);
                anime.next = header;
                file.write((char*) & anime, sizeof(anime));
                header = ntam;
                tam += 1;
                file.close();
                if ((tam - ord_size) >= limite) {
                  reconstruir();
                }
                return true;
            } else {
                // si el header no es 0 ya ha sido modificado: ubicarlo en la lista de menores al primer ordenado
                Anime menor;
                file.seekg(sizeof(Anime)*header, ios::beg);
                file.read((char*)&menor, sizeof(menor));
                anime = menor; // anime apuntará al previo
                int ppos = header; // posición del previo
                while(registro.id > menor.id) {
                  // Mover al siguiente registro
                  anime = menor;
                  ppos = file.tellg()/sizeof(Anime);
                  file.seekg(sizeof(Anime)*menor.next, ios::beg);
                  file.read((char*)&menor, sizeof(menor));
                }
                if(menor.id == registro.id){
                  file.close();
                  return false;
                }
                file.seekg(0, ios::end);
                int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
                registro.next = anime.next;
                file.write((char*)&registro, sizeof(registro));
                anime.next= ntam;
                file.seekg(sizeof(Anime)*ppos, ios::end); // posición del previo
                file.write((char*)&anime, sizeof(anime));
                tam += 1;
                file.close();
                if ((tam - ord_size) >= limite) {
                  reconstruir();
                }
                return true;
            }
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
          Anime panime = anime; // panime apuntará al previo
          int ppos = pos; // posición del previo
          file.seekg(sizeof(Anime)*anime.next, ios::beg);
          file.read((char*)&anime, sizeof(anime)); // primer auxiliar
          // Mover al siguiente registro
          while(registro.id > anime.id) {
            // último de la lista
            if(anime.next == -1){
              file.seekg(0, ios::end);
              int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
              registro.next = anime.next;
              file.write((char*)&registro, sizeof(registro));
              anime.next= ntam;
              file.seekg(sizeof(Anime)*pos, ios::end); // posición actual
              file.write((char*)&panime, sizeof(panime));
              tam += 1;
              file.close();
              if ((tam - ord_size) >= limite) {
                reconstruir();
              }
              return true;
            }
            panime = anime;
            ppos = file.tellg()/sizeof(Anime);
            pos = anime.next;
            file.seekg(sizeof(Anime)*anime.next, ios::beg);
            file.read((char*)&anime, sizeof(anime));
          }
          if(anime.id == registro.id){
            file.close();
            return false;
          }
          file.seekg(0, ios::end);
          int ntam = file.tellg()/sizeof(Anime); // Nueva posición del registro
          registro.next = panime.next;
          file.write((char*)&registro, sizeof(registro));
          panime.next= ntam;
          file.seekg(sizeof(Anime)*ppos, ios::end); // posición del previo
          file.write((char*)&panime, sizeof(panime));
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
      int pos = buscar_posicion(key);
      Anime del;
      del.id = -1;
      Anime anime;
      ifstream file(filename, ios::binary);
      file.seekg(sizeof(Anime)*pos, ios::beg);
      file.read((char*) & anime, sizeof(anime));
      if(anime.id == key){
       return anime;
      }else{
        return del;
      }
    }
    
    vector<Anime> buscar_por_rango(TK key1, TK key2){
      vector<Anime> animes;
      int inicio = buscar_posicion(key1);
      ifstream file(filename, ios::binary);
      Anime anime;
      file.seekg(sizeof(Anime)*inicio, ios::beg);
      file.read((char*) & anime, sizeof(anime));
      while(anime.next != -1){
        if(anime.id > key2){
          return animes;
        }
        animes.push_back(anime);
        file.seekg(sizeof(Anime)*anime.next, ios::beg);
        file.read((char*) & anime, sizeof(anime));
      }
      if(anime.id < key2){
        animes.push_back(anime);
      } 
      return animes;
    }
    
    bool remover(TK key); // el booleano indica si se eliminó o no
    
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
