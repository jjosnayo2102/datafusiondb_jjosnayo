#include <iostream>
#include <fstream>
using namespace std;

string filename = "data.dat";

struct Registro{
  int codigo;
  char nombre[12];
  int next;
  Registro(){next = -1;}
  void setdata(){
    cin >> codigo;
    cin >> nombre;
  }
  void show(){
    cout << this->codigo << endl;
    cout << this->nombre << endl;
    cout << next << endl;
  }
};

int get_size(string filename){
    ifstream file(filename, ios::binary);
    file.seekg(0, ios::end);
    long final = file.tellg();
    int size = final/sizeof(Registro);
    file.close();
    return size;
}

int buscar_posicion(int llave){
  int inicio = 0;
  int fin = get_size(filename)-1;
  int m = -1; // si el inicio no es menor al final devuelve -1
  while(inicio < fin){
    m = (fin - inicio)/2;
    ifstream file(filename, ios::binary);
    file.seekg(m, ios::beg);
    Registro reg;
    file.read((char*) &reg, sizeof(Registro));
    if(reg.codigo > llave){
      fin = m - 1;
    }else if(reg.codigo < llave){
      inicio = m + 1;
    }else{
      return m;
    }
  }
  return m; // si no está devuelve el menor más cercano
}

int main(){
  
  return 0;
}