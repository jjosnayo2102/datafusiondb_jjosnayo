#include "avl.h"
#include <iomanip>
#include <functional>
#include <map>

#define MAX_B 5
#define D 3
#define L 16
using namespace std;

struct Bucket{
    Anime records[MAX_B];
    int size;
    long next_bucket;
    int d; // profundidad local

    Bucket(int _d=D){ // inicialmente cada entrada del índice apunta a un bucket distinto
        size = 0;
        next_bucket = -1;
        d = _d;
    }

    void showData(){
        cout<<"["<<endl;
        for(int i=0;i<size;i++) records[i].show();
        cout<<"]"<<endl;
    }
};

template<typename TK>
class ExtendibleHashFile: public DataFusion<TK> {
private:
    string filename;
    string indexfile; // archivo con índices y posiciones de buckets
    int P; // profundidad global
    int N; // número de entradas del índice
    map<int,int> indice; // tabla de módulos y direcciones de buckets
public:
    ExtendibleHashFile(string _filename, int _N=D){
        filename = _filename + ".bin";
        indexfile = _filename + "_aux.bin";
        ifstream archivo(filename, ios::binary);
        if(archivo.is_open()){
            archivo.close();
            ifstream archivo1(indexfile, ios::binary);
            int key;
            int pos;
            archivo1.read((char*) &P, sizeof(int)); // leer profundidad global
            N = 1 << P; // número de entradas del índice
            // recuperar índice en RAM
            while(archivo1.read((char*) &key, sizeof(int))){
                archivo1.read((char*) &pos, sizeof(int));
                indice[key] = pos;
            }
            return;
        }
        P = _N;
        N = 1 << P; // número inicial de buckets = 8
        archivo.close();
        ofstream file(filename, ios::binary);
        for(int i = 0; i < N; i++){
            Bucket bloque;
            file.write((char*) &bloque, sizeof(Bucket));
            // crear índice inicial
            indice[i] = i;
        }
        file.close();
    }

    bool insertar(Anime record) override{
        //1- usando una funcion hashing ubicar la pagina de datos
        hash<TK> hf;
        long key = hf(record.id);
        int index = labs(key) % N;
        //2- leer la pagina de datos,
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(indice[index]*sizeof(Bucket), ios::beg); // se usa la entrada del índice
        Bucket bloque;
        file.read((char*) &bloque, sizeof(Bucket));
        //3- verificar si size < MAX_B, si es asi, se inserta en esa pagina y se re-escribe al archivo
        if(bloque.size < MAX_B){
            for(int i = 0; i < bloque.size; i++){
                if(bloque.records[i].id == record.id){
                    return false;
                }
            }
            bloque.records[bloque.size] = record;
            bloque.size += 1;
            file.seekg(indice[index]*sizeof(Bucket), ios::beg);
            file.write((char*) &bloque, sizeof(Bucket));
            file.close();
            return true;
        }
        if(bloque.d == L){ // si la profundidad del bucket supera el umbral
            //aplicar encadenamiento
            int actpos = index;
            while(bloque.next_bucket != -1){
                actpos = bloque.next_bucket;
                file.seekg(bloque.next_bucket*sizeof(Bucket), ios::beg);
                file.read((char*) &bloque, sizeof(Bucket));
                if(bloque.size < MAX_B){
                    for(int i = 0; i < bloque.size; i++){
                        if(bloque.records[i].id == record.id){
                            return false;
                        }
                    }
                    bloque.records[bloque.size] = record;
                    bloque.size += 1;
                    file.seekg(actpos*sizeof(Bucket), ios::beg);
                    file.write((char*) &bloque, sizeof(Bucket));
                }
            }
            Bucket nuevo;
            nuevo.records[nuevo.size] = record;
            nuevo.size += 1;
            file.seekg(0, ios::end);
            long npos = file.tellp() / sizeof(Bucket);
            bloque.next_bucket = npos;
            file.seekg(actpos*sizeof(Bucket), ios::beg);
            file.write((char*) &bloque, sizeof(Bucket));
            file.seekg(npos * sizeof(Bucket), ios::beg);
            file.write((char*) &nuevo, sizeof(Bucket));
            file.close();
            return true;
            // si hay espacio libre en vez de crear el nuevo bucket al final se crea en el espacio libre
            // leer el archivo de posiciones libres hasta encontrar la primera posición que no sea -1
            // luego meter -1 a esa posición
        }else{
            if(bloque.d == P){
                // duplicar índice
                for(int i = N; i < 2*N; i++){ // los nuevos índices inicialmente apuntan a los mismos buckets
                    indice[i] = i-N;
                }
                P += 1; // aumentar profundidad
                N = 1 << P; // duplicar número de entradas
                Bucket b1(bloque.d + 1);
                Bucket b2(bloque.d + 1);
                int nind = index;
                for(int j = 0; j < bloque.size; j++){
                    long llave = hf(bloque.records[j].id);
                    int ind = labs(llave) % (1 << (bloque.d+1));
                    if(ind == index){ // pertenece a la primera mitad
                        b1.records[b1.size] = bloque.records[j];
                        b1.size += 1;
                    }
                    else{
                        nind = ind;
                        b2.records[b2.size] = bloque.records[j];
                        b2.size += 1;
                    }
                }
                // elemento nuevo
                long llave = hf(record.id);
                int ind = labs(llave) % (1 << (bloque.d+1));
                if(ind == index){ // pertenece a la primera mitad
                    b1.records[b1.size] = record;
                    b1.size += 1;
                }
                else{
                    nind = ind;
                    b2.records[b2.size] = record;
                    b2.size += 1;
                }
                file.seekg(indice[index]*sizeof(Bucket), ios::beg); // posición del bucket
                file.write((char*) &b1, sizeof(Bucket)); // modificar bucket
                file.seekg(0, ios::end);
                int ntam = file.tellg()/sizeof(Bucket);
                file.write((char*) &b2, sizeof(Bucket)); // insertar nuevo bucket
                indice[nind] = ntam; // una de las entradas apunta al nuevo bucket
                return true;
            }else{
                Bucket b1(bloque.d + 1);
                Bucket b2(bloque.d + 1);
                int ind1 = index;
                int ind2 = index;
                for(int j = 0; j < bloque.size; j++){
                    long llave = hf(bloque.records[j].id);
                    int ind = labs(llave) % (1 << (bloque.d+1));
                    if(ind < (N/2)){ // pertenece a la primera mitad
                        ind1 = ind;
                        b1.records[b1.size] = bloque.records[j];
                        b1.size += 1;
                    }
                    else{
                        ind2 = ind;
                        b2.records[b2.size] = bloque.records[j];
                        b2.size += 1;
                    }
                }
                // elemento nuevo
                long llave = hf(record.id);
                int ind = labs(llave) % N;
                if(ind < (N/2)){ // pertenece a la primera mitad
                    b1.records[b1.size] = record;
                    b1.size += 1;
                }
                else{
                    b2.records[b2.size] = record;
                    b2.size += 1;
                }
                file.seekg(indice[ind1]*sizeof(Bucket), ios::beg); // posición de la primera mitad
                file.write((char*) &b1, sizeof(Bucket)); // modificar bucket
                file.seekg(0, ios::end);
                int ntam = file.tellg()/sizeof(Bucket);
                file.write((char*) &b2, sizeof(Bucket)); // insertar nuevo bucket
                indice[ind2] = ntam; // una de las entradas apunta al nuevo bucket
                return true;
            }
        }
    }

    Anime buscar(TK key) override{
        hash<TK> hf;
        long llave = hf(key);
        int index = labs(llave) % N;
        ifstream file(filename, ios::binary);
        file.seekg(indice[index]*sizeof(Bucket), ios::beg);
        Bucket bloque;
        Anime ret;
        file.read((char*) &bloque, sizeof(Bucket));
        for(int i = 0; i < bloque.size; i++){
            if(bloque.records[i].id == key){
                ret = bloque.records[i];
                file.close();
                return ret;
            }
        }
        while(bloque.next_bucket != -1){
            file.seekg(bloque.next_bucket*sizeof(Bucket), ios::beg);
            file.read((char*) &bloque, sizeof(Bucket));
            for(int i = 0; i < bloque.size; i++){
                if(bloque.records[i].id == key){
                    ret = bloque.records[i];
                    file.close();
                    return ret;
                }
            }
        }
        file.close();
        return ret;
    }

    vector<Anime> buscar_por_rango(TK key1, TK key2) override{
        cout << "Busqueda por rango no soportada" << endl;
        cout << endl;
        vector<Anime> animes;
        return animes;
    }

    bool remover(TK key) override{
        hash<TK> hf;
        long llave = hf(key);
        int index = labs(llave) % N;
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(index*sizeof(Bucket), ios::beg);
        Bucket bloque;
        Anime del;
        file.read((char*) &bloque, sizeof(Bucket));
        for(int i = 0; i < bloque.size; i++){
            if(bloque.records[i].id == key){
                for(int j = i; j < bloque.size - 1; j++){
                    bloque.records[j] = bloque.records[j+1];
                }
                bloque.size -= 1;
                file.seekg(indice[index]*sizeof(Bucket), ios::beg);
                file.write((char*) &bloque, sizeof(Bucket));
                // juntar entradas si el bloque se queda sin elementos
                if(bloque.size == 0 && bloque.next_bucket == -1){
                    int hermano = index ^ (1 << (bloque.d - 1));
                    indice[index] = indice[hermano]; // entrada apunta a bucket con elementos
                    // si todos los bloques tienen profundidad local menor a la global, reducir profundidad global y modificar índice
                    bool menor = true;
                    Bucket temp;
                    file.seekg(0, ios::beg);
                    while(file.read((char*) &temp, sizeof(Bucket))){
                        if(temp.d >= P){
                            menor = false;
                            break;
                        }
                    }
                    if(menor){
                        map<int,int> nindice;
                        P--;
                        N = 1 << P;
                        for(int i = 0; i < N; i++){
                            nindice[i] = indice[i]; // va a apuntar al bucket con elementos
                        }
                        this->indice = nindice;
                    }
                }
                // caso donde el bloque está encadenado y vacío
                file.close();
                return true;
            }
        }
        while(bloque.next_bucket != -1){
            long actpos = bloque.next_bucket;
            file.seekg(bloque.next_bucket*sizeof(Bucket), ios::beg);
            file.read((char*) &bloque, sizeof(Bucket));
            for(int i = 0; i < bloque.size; i++){
                if(bloque.records[i].id == key){
                    for(int j = i; j < bloque.size - 1; j++){
                        bloque.records[j] = bloque.records[j+1];
                    }
                    bloque.size -= 1;
                    // cuando un bloque se quede sin elementos reemplazar con el bloque siguiente y apuntar al siguiente del siguiente
                    if(bloque.size == 0){
                        //modificar punteros
                    }
                    file.seekg(actpos*sizeof(Bucket), ios::beg);
                    file.write((char*) &bloque, sizeof(Bucket));
                    file.close();
                    return true;
                    // el antiguo siguiente forma parte del espacio libre
                    // si el bloque no tiene siguiente, forma parte del espacio libre directamente
                    // crear un archivo auxiliar de posiciones de buckets libres y meter la posición libre
                }
            }
        }
        // último bloque
        return false;      
    }

    ~ExtendibleHashFile(){
        //guardar el P actual y el índice
        ofstream file(indexfile, ios::binary);
        file.write((char*) &P, sizeof(int));
        for(auto it: indice){
            int key = it.first;
            int pos = it.second;
            file.write((char*) &key, sizeof(int));
            file.write((char*) &pos, sizeof(int));
        }
        file.close();
    }
};

