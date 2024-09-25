#include "avl.h"
#include <iomanip>
#include <functional>

#define MAX_B 4
using namespace std;

struct Record {
    char codigo[10];
    char nombre[20];
    char apellido[20];
    int ciclo;

    Record(){ciclo = -1;}
   
    void setData(ifstream &file) {
        file.getline(codigo, 10, ',');
        file.getline(nombre, 20, ',');
        file.getline(apellido, 20, ',');
        file>>ciclo; file.get();
    }

    void showData() {
        cout<<setw(10)<<left<<codigo;
        cout<<setw(10)<<left<<nombre;
        cout<<setw(10)<<left<<apellido;
        cout<<setw(10)<<left<<ciclo<<endl;
    }
};

struct Bucket{
    Record records[MAX_B];
    int size;
    long next_bucket;

    Bucket(){
        size = 0;
        next_bucket = -1;
    }

    void showData(){
        cout<<"["<<endl;
        for(int i=0;i<size;i++) records[i].showData();
        cout<<"]"<<endl;
    }
};

class StaticHashFile {
private:
    string filename;
    int N;
public:
    StaticHashFile(string _filename, int _N){
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

    void insert(Record record){
        //1- usando una funcion hashing ubicar la pagina de datos
        string nombre = string(record.nombre);
        hash<string> hf;
        long key = hf(nombre);
        int index = labs(key) % N;
        //2- leer la pagina de datos,
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(index*sizeof(Bucket), ios::beg);
        Bucket bloque;
        file.read((char*) &bloque, sizeof(Bucket));
        //3- verificar si size < MAX_B, si es asi, se inserta en esa pagina y se re-escribe al archivo
        if(bloque.size < MAX_B){
            bloque.records[bloque.size] = record;
            bloque.size += 1;
            file.seekg(index*sizeof(Bucket), ios::beg);
            file.write((char*) &bloque, sizeof(Bucket));
        }
        //4- caso contrario, crear nuevo bucket, insertar ahi el nuevo registro, y enlazar
        else{
            long actpos = index;
            while(bloque.next_bucket != -1){
                actpos = bloque.next_bucket;
                file.seekg(bloque.next_bucket*sizeof(Bucket), ios::beg);
                file.read((char*) &bloque, sizeof(Bucket));
                if(bloque.size < MAX_B){
                    bloque.records[bloque.size] = record;
                    bloque.size += 1;
                    file.seekg(actpos*sizeof(Bucket), ios::beg);
                    file.write((char*) &bloque, sizeof(Bucket));
                    file.close();
                    return;
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
        file.close();
        // si hay espacio libre en vez de crear el nuevo bucket al final se crea en el espacio libre
        // leer el archivo de posiciones libres hasta encontrar la primera posición que no sea -1
        // luego meter -1 a esa posición
    }

    Record search(string nombre){
        hash<string> hf;
        long key = hf(nombre);
        int index = labs(key) % N;
        ifstream file(filename, ios::binary);
        file.seekg(index*sizeof(Bucket), ios::beg);
        Bucket bloque;
        Record ret;
        file.read((char*) &bloque, sizeof(Bucket));
        for(int i = 0; i < bloque.size; i++){
            if(string(bloque.records[i].nombre) == nombre){
                ret = bloque.records[i];
                file.close();
                return ret;
            }
        }
        while(bloque.next_bucket != -1){
            file.seekg(bloque.next_bucket*sizeof(Bucket), ios::beg);
            file.read((char*) &bloque, sizeof(Bucket));
            for(int i = 0; i < bloque.size; i++){
                if(string(bloque.records[i].nombre) == nombre){
                    ret = bloque.records[i];
                    file.close();
                    return ret;
                }
            }
        }
        file.close();
        return ret;
    }

    bool remove(string nombre){
        hash<string> hf;
        long key = hf(nombre);
        int index = labs(key) % N;
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(index*sizeof(Bucket), ios::beg);
        Bucket bloque;
        Record del;
        file.read((char*) &bloque, sizeof(Bucket));
        for(int i = 0; i < bloque.size; i++){
            if(string(bloque.records[i].nombre) == nombre){
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
                if(string(bloque.records[i].nombre) == nombre){
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

    void scanAll(){
        ifstream file(filename, ios::binary);
        Bucket bloque;
        while(file.read((char*) &bloque, sizeof(Bucket))){
            bloque.showData();
        }
    }
};

