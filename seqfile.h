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
    void show(){
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
class Seqfile{
private:
    string filename; 
    string aux_file;
    int header; //inicio de los punteros
    int limite; // cantidad máxima de registros auxiliares
    int ord_size; // número de registros ordenados
    int tam; // tamaño del archivo
    void reconstruir(){
        rename(filename.c_str(),"delete.bin");
        ifstream file("delete.bin", ios::binary);
        ofstream archivo(filename, ios::binary);
        file.seekg(sizeof(Anime)*header, ios::beg);
        Anime registro;
        int i = 1;
        header = 0; // archivo ordenado
        file.read((char*) &registro, sizeof(registro));
        while(registro.next != -1){
            registro.next = i;
            archivo.write((char*) &registro, sizeof(registro));
            // buscar siguiente posición
            file.seekg(sizeof(Anime)*registro.next, ios::beg);
            file.read((char*) &registro, sizeof(registro));
            i++;
        }
        // último registro
        registro.next = -1;
        archivo.write((char*) &registro, sizeof(registro));
        file.close();
        archivo.close();
        // eliminar viejo
        remove("delete.bin");
        ord_size = tam;
        limite = max(5,static_cast<int>(log2(tam)));
    }
    bool insertar_aux(Anime registro){
        // para el primer elemento, fstream requiere que el archivo exista
        if(tam == 0){
            ofstream file(filename, ios::binary);
            file.write((char*) &registro, sizeof(registro));
            file.close();
            tam += 1;
            return true;
        }
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(sizeof(Anime)*header, ios::beg);
        Anime anime;
        Anime nanime;
        int pos = header;
        int npos = -1;
        file.read((char*) &anime, sizeof(anime));
        // si es menor que el primero
        if(anime.id > registro.id){
            // obtener nueva posición
            file.seekg(0, ios::end);
            long p = file.tellg();
            int ntam = p/sizeof(Anime);
            // el nuevo apunta al antiguo header
            registro.next = header;
            // crear nuevo header
            header = ntam;
            // insertar nuevo al final
            file.seekg(0, ios::end);
            file.write((char*) &registro, sizeof(registro));
            file.close();
            tam += 1;
            // si supera el límite reconstruir
            if((tam - ord_size) >= limite){
                reconstruir();
            }
            return true;
        }
        // si es igual no hace nada
        if(anime.id == registro.id){
            file.close();
            return false;
        }
        while(anime.next != -1){
            npos = anime.next;
            file.seekg(sizeof(Anime)*npos, ios::beg);
            file.read((char*) &nanime, sizeof(nanime));
            if(anime.id < registro.id && registro.id < nanime.id){
                // obtener nueva posición
                file.seekg(0, ios::end);
                long p = file.tellg();
                int ntam = p/sizeof(Anime);
                // que la posición actual apunte al nuevo
                anime.next = ntam;
                file.seekg(sizeof(Anime)*pos, ios::beg);
                file.write((char*) &anime, sizeof(anime));
                // que el nuevo apunte al siguiente
                registro.next = npos;
                file.seekg(0, ios::end);
                file.write((char*) &registro, sizeof(registro));
                file.close();
                tam += 1;
                // si supera el límite reconstruir
                if((tam - ord_size) >= limite){
                    reconstruir();
                }
                return true;
            }
            // si es igual no hace nada
            if(anime.id == registro.id){
                file.close();
                return false;
            }
            anime = nanime;
            pos = npos;
        }
        // si es mayor que el último
        if(anime.id < registro.id){
            // obtener nueva posición
            file.seekg(0, ios::end);
            long p = file.tellg();
            int ntam = p/sizeof(Anime);
            // reemplazar puntero actual por nuevo
            anime.next = ntam;
            file.seekg(sizeof(Anime)*pos, ios::beg);
            file.write((char*) &anime, sizeof(anime));
            // insertar nuevo al final
            file.seekg(0, ios::end);
            file.write((char*) &registro, sizeof(registro));
            file.close();
        }
        // si es igual no hace nada
        if(anime.id == registro.id){
            file.close();
            return false;
        }
        tam += 1;
        // si supera el límite reconstruir
        if((tam - ord_size) >= limite){
            reconstruir();
        }
        return true;
    }
    int buscar_posicion(int key){ // los registros usan el id para ordenarse
        int inicio = 0;
        int fin = ord_size-1;
        int m = -1; // si el inicio no es menor al final devuelve -1
        ifstream file(filename, ios::binary);
        while(inicio <= fin){
            m = inicio + (fin - inicio)/2;
            file.seekg(sizeof(Anime)*m, ios::beg);
            Anime reg;
            file.read((char*) &reg, sizeof(Anime));
            if(reg.id > key){
                fin = m - 1;
            }else if(reg.id < key){
                inicio = m + 1;
            }else{
                return m;
            }
        }
        file.close();
        return m; // si no está devuelve el menor más cercano
    }
public:
    Seqfile(string _filename) {
        filename = _filename + ".bin";
        aux_file = _filename + "_aux.bin";
        ifstream file(filename, ios::binary);
        if(!file.is_open()){
            header = 0;
            limite = 5;
            ord_size = 0;
            tam = 0;
        }
        file.close();
        ifstream archivo(aux_file, ios::binary);
        archivo.read((char*) &header, sizeof(header));
        archivo.read((char*) &limite, sizeof(limite));
        archivo.read((char*) &ord_size, sizeof(ord_size));
        archivo.read((char*) &tam, sizeof(tam));
        archivo.close();
    }
    bool insertar(Anime registro){ // el booleano indica si se insertó o no
        if(ord_size > 0){
            int pos = buscar_posicion(registro.id);
            fstream file(filename, ios::in | ios::out | ios::binary);
            file.seekg(sizeof(Anime)*pos, ios::beg);
            Anime anime;
            file.read((char*) &anime, sizeof(anime));
            // caso donde es menor al primero
            if(anime.id > registro.id){
                file.seekg(sizeof(Anime)*header, ios::beg);
                if(header == 0){
                    // obtener nueva posición
                    file.seekg(0, ios::end);
                    long p = file.tellg();
                    int ntam = p/sizeof(Anime);
                    // reemplazar header por nuevo
                    int npos = header;
                    header = ntam;
                    // modificar puntero del nuevo por el antiguo header
                    registro.next = npos;
                    file.seekg(0, ios::end);
                    file.write((char*) &registro, sizeof(registro));
                    file.close();
                    tam += 1;
                    if((tam - ord_size) >= limite){
                        reconstruir();
                    }
                    return true;
                }else{ // si el header no es 0 significa que una inserción ya modificó el orden
                    Anime anime;
                    Anime panime = anime;
                    file.seekg(sizeof(Anime)*header, ios::beg);
                    file.read((char*) &anime, sizeof(anime));
                    while(anime.id < registro.id){ // ubica la posición por los punteros
                        panime = anime;
                        file.seekg(sizeof(Anime)*anime.next, ios::beg);
                        file.read((char*) &anime, sizeof(anime));
                    }
                    if(panime.id == registro.id){
                        file.close();
                        return false;
                    }
                    // se modifica el anterior: anime
                    // obtener nueva posición
                    file.seekg(0, ios::end);
                    long p = file.tellg();
                    int ntam = p/sizeof(Anime);
                    // modificar puntero del nuevo por el siguiente de la posición comparada
                    registro.next = panime.next;
                    file.seekg(0, ios::end);
                    file.write((char*) &registro, sizeof(registro));
                    // reemplazar puntero actual por nuevo
                    panime.next = ntam;
                    file.seekg(sizeof(Anime)*pos, ios::beg);
                    file.write((char*) &panime, sizeof(panime));
                    file.close();
                    tam += 1;
                    if((tam - ord_size) >= limite){
                        reconstruir();
                    }
                    return true;
                }
            }
            else if(anime.id < registro.id){
                // la posición puede apuntar a un ordenado o auxiliar
                if(anime.next < ord_size){ // si el siguiente es ordenado o es -1
                    // obtener nueva posición
                    file.seekg(0, ios::end);
                    long p = file.tellg();
                    int ntam = p/sizeof(Anime);
                    // modificar puntero del nuevo por el siguiente de la posición comparada
                    registro.next = anime.next;
                    file.seekg(0, ios::end);
                    file.write((char*) &registro, sizeof(registro));
                    // reemplazar puntero actual por nuevo
                    anime.next = ntam;
                    file.seekg(sizeof(Anime)*pos, ios::beg);
                    file.write((char*) &anime, sizeof(anime));
                    file.close();
                    tam += 1;
                    if((tam - ord_size) >= limite){
                        reconstruir();
                    }
                    return true;
                }else{ // si apunta a auxiliar
                    Anime nanime;
                    file.seekg(sizeof(Anime)*anime.next, ios::beg);
                    file.read((char*) &nanime, sizeof(nanime));
                    while(nanime.id < registro.id){ // ubica la posición por los punteros
                        // caso especial donde es el final
                        if(nanime.next == -1){
                            // obtener nueva posición
                            file.seekg(0, ios::end);
                            long p = file.tellg();
                            int ntam = p/sizeof(Anime);
                            // modificar puntero del nuevo por el siguiente de la posición comparada
                            registro.next = nanime.next;
                            file.seekg(0, ios::end);
                            file.write((char*) &registro, sizeof(registro));
                            // reemplazar puntero actual por nuevo
                            nanime.next = ntam;
                            file.seekg(sizeof(Anime)*anime.next, ios::beg);
                            file.write((char*) &nanime, sizeof(nanime));
                            file.close();
                            tam += 1;
                            if((tam - ord_size) >= limite){
                                reconstruir();
                            }
                            return true;
                        }
                        pos = anime.next; 
                        anime = nanime;
                        file.seekg(sizeof(Anime)*nanime.next, ios::beg);
                        file.read((char*) &nanime, sizeof(nanime));
                    }
                    if(nanime.id == registro.id){
                        file.close();
                        return false;
                    }
                    // se modifica el anterior: anime
                    // obtener nueva posición
                    file.seekg(0, ios::end);
                    long p = file.tellg();
                    int ntam = p/sizeof(Anime);
                    // modificar puntero del nuevo por el siguiente de la posición comparada
                    registro.next = anime.next;
                    file.seekg(0, ios::end);
                    file.write((char*) &registro, sizeof(registro));
                    // reemplazar puntero actual por nuevo
                    anime.next = ntam;
                    file.seekg(sizeof(Anime)*pos, ios::beg);
                    file.write((char*) &anime, sizeof(anime));
                    file.close();
                    tam += 1;
                    if((tam - ord_size) >= limite){
                        reconstruir();
                    }
                    return true;
                }
            }
            else{
                file.close();
                return false;
            }
        }else{
            return insertar_aux(registro);
        }
    }
    Anime buscar(TK key){
        Anime def;
        def.id = -1; // si el id es -1 es que no está
        int pos = buscar_posicion(key);
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(sizeof(Anime)*pos, ios::beg);
        Anime anime;
        file.read((char*) &anime, sizeof(anime));
        if(anime.id == key){
            return anime;
        }else{
            while(anime.next >= ord_size){ // con esto también se revisa que no sea -1
                file.seekg(sizeof(Anime)*anime.next, ios::beg);
                file.read((char*) &anime, sizeof(anime));
                if(anime.id == key){
                    return anime;
                }
            }
            return def;
        }
    }
    vector<Anime> buscar_por_rango(TK key1, TK key2);
    bool remover(TK key); // el booleano indica si se eliminó o no
    ~Seqfile(){
        // header,limite,ord_size,tam
        ofstream file(aux_file, ios::binary);
        file.write((char*) &header, sizeof(header));
        file.write((char*) &limite, sizeof(limite));
        file.write((char*) &ord_size, sizeof(ord_size));
        file.write((char*) &tam, sizeof(tam));
        file.close();
    }
};
