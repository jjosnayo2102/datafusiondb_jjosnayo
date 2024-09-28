#include "avl.h"
#include <iomanip>
#include <functional>

#define MAX_B 4
using namespace std;

struct Bucket{
    Anime records[MAX_B];
    int size;
    long next_bucket;

    Bucket(){
        size = 0;
        next_bucket = -1;
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
    int N; // profundidad global
public:
    ExtendibleHashFile(string _filename, int _N){
        filename = _filename;
        N = _N;
        ifstream archivo(filename, ios::binary);
        if(archivo.is_open()){
            archivo.close();
            return;
        }
        ofstream file(filename, ios::app | ios::binary);
        for(int i = 0; i < N; i++){
            Bucket bloque;
            file.write((char*) &bloque, sizeof(Bucket));
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
        file.seekg(index*sizeof(Bucket), ios::beg);
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
            file.seekg(index*sizeof(Bucket), ios::beg);
            file.write((char*) &bloque, sizeof(Bucket));
            file.close();
            return true;
        }
        //4- caso contrario, crear nuevo bucket, insertar ahi el nuevo registro, y enlazar
        else{
            long actpos = index;
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
                    file.close();
                    return true;
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
        }
        // si hay espacio libre en vez de crear el nuevo bucket al final se crea en el espacio libre
        // leer el archivo de posiciones libres hasta encontrar la primera posición que no sea -1
        // luego meter -1 a esa posición
    }

    Anime buscar(TK key) override{
        hash<TK> hf;
        long llave = hf(key);
        int index = labs(llave) % N;
        ifstream file(filename, ios::binary);
        file.seekg(index*sizeof(Bucket), ios::beg);
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

    // modificar para búsqueda por rango
    void scanAll(){
        ifstream file(filename, ios::binary);
        Bucket bloque;
        while(file.read((char*) &bloque, sizeof(Bucket))){
            bloque.showData();
        }
    }

    vector<Anime> buscar_por_rango(TK key1, TK key2) override{
        vector<Anime> animes;
        return animes;
    }

    bool remover(TK key) override{
        hash<string> hf;
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
                file.seekg(index*sizeof(Bucket), ios::beg);
                file.write((char*) &bloque, sizeof(Bucket));
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
                    file.seekg(actpos*sizeof(Bucket), ios::beg);
                    file.write((char*) &bloque, sizeof(Bucket));
                    file.close();
                    return true;
                }
            }
        }
        // cuando un bloque se quede sin elementos reemplazar con el bloque siguiente y apuntar al siguiente del siguiente
        // el antiguo siguiente forma parte del espacio libre
        // si el bloque no tiene siguiente, forma parte del espacio libre directamente
        // crear un archivo auxiliar de posiciones de buckets libres y meter la posición libre
        return false;      
    }
};

