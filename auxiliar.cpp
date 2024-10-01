#include "inicializacion.h"

void mostrar(string indice){
  ifstream file(indice, ios::binary);
  Anime anime;
  while(file.read((char*) &anime, sizeof(Anime))){
    anime.show();
  }
}

void mostrar_eh(string indice){
  ifstream file(indice, ios::binary);
  Bucket bucket;
  while(file.read((char*) &bucket, sizeof(Bucket))){
    bucket.showData();
  }
}

int main() {
    string nombreArchivo = "datos.csv";
    string indice;
    // creación de índice secuencial
    leerCSV_Seqfile(nombreArchivo);
    // creación de índice AVL
    // leerCSV_AVL(nombreArchivo);
    // creación de índice hash
    // leerCSV_ExtHash(nombreArchivo);
    
    //seqfile:
    // prueba de funciones auxiliares
    // inserción
    //leerCSV_Seqfile(nombreArchivo);
    // indice = "prueba_secuencial.bin";
    // mostrar(indice);
    // cout << endl;
    // búsqueda
    // auto sf1 = new Seqfile<int>("prueba_secuencial");
    // Anime res = sf1->buscar(6682);
    // res.show();
    //cout << endl;
    // búsqueda por rango
    //auto rango = sf1->buscar_por_rango(18677, 38016);
    //cout << rango.size() << endl;
    //for(int i = 0; i < rango.size(); i++){
    //  rango[i].show();
    //}
    //cout << endl;
    // eliminación
    // sf1->remover(37393);
    // mostrar(indice);
    // delete sf1;
    // funciona
    // prueba general
    // funciona
    
    //avl:
    // inserción:
    // leerCSV_AVL(nombreArchivo);
    // mostrar("prueba_binario_balanceado.bin");
    // AVLFile<int> avl1("prueba_binario_balanceado");
    //búsqueda:
    // Anime ret = avl1.buscar(170);
    // ret.show();
    // búsqueda por rango:
    // auto animes = avl1.buscar_por_rango(1000, 2000);
    // for(auto it: animes){
    //   it.show();
    // }
    // eliminación:
    // avl1.remover(25777);
    // auto animes = avl1.buscar_por_rango(20000,30000);
    // for(auto it: animes){
    //   it.show();
    // }
    // funciona
    
    // exthash:
    // inserción: error con valores grandes
    // indice = "tabla_hash_extensible.bin";
    // mostrar(indice);
    // ExtendibleHashFile<int> eh("prueba_hash_extensible");
    // búsqueda
    // Anime ret = eh.buscar(170);
    // ret.show();
    // funciona
    return 0;
}