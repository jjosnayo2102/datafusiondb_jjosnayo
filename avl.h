#include "seqfile.h"

struct Record
{
  int codigo;
  char nombre[12];
  char apellido[12];
  int ciclo;
  // Agregar campos extra si los necesitan
  int left = -1;
  int right = -1;
  void showData() {
    cout << "\nCodigo: " << codigo;
    cout << "\nNombre: " << nombre;
    cout << "\nApellido: " << apellido;
    cout << "\nCiclo : " << ciclo << endl;
    cout << left << endl;
    cout << right << endl;
  }

  bool operator==(const Record& other) const {
    return codigo == other.codigo && strcmp(nombre, other.nombre) == 0 && strcmp(apellido, other.apellido) == 0 && ciclo == other.ciclo;
  }
};

template <typename TK>
class AVLFile {
private :
  string filename ;
  long pos_root ;
  bool es_hoja(Record record);
  vector<Record> inOrder(int pos);
  // para ser AVL
  int height(int pos);
  void balance(int pos);
  int balancingFactor(int pos);
  void leftRotate(int pos);
  void rightRotate(int pos);
public :
  AVLFile(string filename);
  //agregue todas las funciones necesarias
  Record find(TK codigo);
  void insert(Record record);
  vector<Record> inorder();
  bool remove(TK key);
};

template <typename TK>
AVLFile<TK>::AVLFile(string filename){
    this->filename = filename;
    this->pos_root = 0;
}

template <typename TK>
bool AVLFile<TK>::es_hoja(Record record){
    return (record.left == -1 && record.right == -1);
}

template <typename TK>
vector<Record> AVLFile<TK>::inOrder(int pos) {
    vector<Record> records;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return records;
    Record nodo;
    file.seekg(pos * sizeof(Record), ios::beg);
    file.read((char*)&nodo, sizeof(Record));
    if (nodo.left != -1) {
        vector<Record> leftRecords = inOrder(nodo.left);
        records.insert(records.end(), leftRecords.begin(), leftRecords.end());
    }
    records.push_back(nodo);
    if (nodo.right != -1) {
        vector<Record> rightRecords = inOrder(nodo.right);
        records.insert(records.end(), rightRecords.begin(), rightRecords.end());
    }
    return records;
}

template <typename TK>
Record AVLFile<TK>::find(TK codigo){
    Record ret;
    ret.codigo = -1;
    string nd = "default";
    string ad = "default";
    strncpy(ret.nombre, nd.c_str(), sizeof(ret.nombre));
    strncpy(ret.apellido, ad.c_str(), sizeof(ret.apellido));
    ret.ciclo = -1;
    ret.left = -1;
    ret.right = -1;
    ifstream file(this->filename, ios::binary);
    if(!file.is_open()) return ret;
    file.seekg(pos_root*sizeof(Record), ios::beg);
    Record nodo;
    file.read((char*) &nodo, sizeof(Record));
    while(1){
        if(es_hoja(nodo)) break;
        if(codigo > nodo.codigo){
            if(nodo.right == -1) return ret;
            file.seekg(nodo.right*sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
        }
        else if(codigo < nodo.codigo){
            if(nodo.left == -1) return ret;
            file.seekg(nodo.left*sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
        }
        else{
          return nodo;
        }
    }
    if(nodo.codigo == codigo){
      return nodo;
    }
    else{
      return ret;
    }
}

template <typename TK>
void AVLFile<TK>::insert(Record record){
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if(!file.is_open()){
        file.close();
        ofstream archivo(this->filename, ios::binary);
        archivo.write((char*) &record, sizeof(Record));
        archivo.close();
        return;
    }
    if(find(record.codigo).codigo != -1){
      cout << "Codigo ya registrado en el archivo" << endl;
      return;
    }
    file.seekg(pos_root*sizeof(Record), ios::beg);
    Record nodo;
    int old_pos = pos_root;
    file.read((char*) &nodo, sizeof(Record));
    while(1){
        if(es_hoja(nodo)) break;
        if(record.codigo > nodo.codigo){
            if(nodo.right == -1) break;
            old_pos = nodo.right;
            file.seekg(nodo.right*sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
        }
        else{
            if(nodo.left == -1) break;
            old_pos = nodo.left;
            file.seekg(nodo.left*sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
        }
    }
    file.seekp(0, ios::end);
    int pos = file.tellp()/sizeof(Record);
    if(record.codigo > nodo.codigo){
        nodo.right = pos;
    }
    else{
        nodo.left = pos;
    }
    file.seekp(old_pos*sizeof(Record), ios::beg);
    file.write((char*) &nodo, sizeof(Record));
    file.seekp(pos*sizeof(Record), ios::beg);
    file.write((char*) &record, sizeof(Record));
    file.close();
    // balancear si se pierde la propiedad del AVL
}

template <typename TK>
vector<Record> AVLFile<TK>::inorder(){
    return inOrder(pos_root);
}

template <typename TK>
bool AVLFile<TK>::remove(TK key) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return false;

    int parent_pos = -1;  // Posición del nodo padre
    int current_pos = pos_root;  // Comenzamos desde la raíz
    bool is_left_child = false;  // Determina si el nodo es un hijo izquierdo
    Record nodo;

    // Buscar el nodo que se va a eliminar
    while (current_pos != -1) {
        file.seekg(current_pos * sizeof(Record), ios::beg);
        file.read((char*)&nodo, sizeof(Record));
        
        if (key == nodo.codigo) {
            break; // Nodo encontrado
        }

        parent_pos = current_pos;
        if (key < nodo.codigo) {
            is_left_child = true;
            current_pos = nodo.left;
        } else {
            is_left_child = false;
            current_pos = nodo.right;
        }
    }

    if (current_pos == -1) {
        // Nodo no encontrado
        file.close();
        return false;
    }

    // Caso 1: Nodo es una hoja
    if (nodo.left == -1 && nodo.right == -1) {
        if (parent_pos == -1) {
            // Nodo es la raíz
            pos_root = -1;
        } else {
            // Nodo no es la raíz
            file.seekg(parent_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            if (is_left_child) {
                nodo.left = -1;
            } else {
                nodo.right = -1;
            }
            file.seekp(parent_pos * sizeof(Record), ios::beg);
            file.write((char*)&nodo, sizeof(Record));
        }
    }
    // Caso 2: Nodo tiene un solo hijo
    else if (nodo.left == -1 || nodo.right == -1) {
        int child_pos = (nodo.left != -1) ? nodo.left : nodo.right;

        if (parent_pos == -1) {
            // Nodo es la raíz
            pos_root = child_pos;
        } else {
            // Nodo no es la raíz
            file.seekg(parent_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            if (is_left_child) {
                nodo.left = child_pos;
            } else {
                nodo.right = child_pos;
            }
            file.seekp(parent_pos * sizeof(Record), ios::beg);
            file.write((char*)&nodo, sizeof(Record));
        }
    }
    // Caso 3: Nodo tiene dos hijos
    else {
        // Encontrar el sucesor inorden (el nodo más pequeño en el subárbol derecho)
        int successor_pos = nodo.right;
        int successor_parent_pos = current_pos;
        Record successor;

        while (true) {
            file.seekg(successor_pos * sizeof(Record), ios::beg);
            file.read((char*)&successor, sizeof(Record));
            if (successor.left == -1) break;
            successor_parent_pos = successor_pos;
            successor_pos = successor.left;
        }

        // Reemplazar datos del nodo a eliminar con el sucesor
        file.seekp(current_pos * sizeof(Record), ios::beg);
        file.write((char*)&successor, sizeof(Record));

        // Ajustar los punteros del sucesor
        if (successor_parent_pos != current_pos) {
            file.seekg(successor_parent_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            nodo.left = successor.right;
            file.seekp(successor_parent_pos * sizeof(Record), ios::beg);
            file.write((char*)&nodo, sizeof(Record));
        } else {
            // El sucesor está directamente a la derecha del nodo a eliminar
            file.seekg(current_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            nodo.right = successor.right;
            file.seekp(current_pos * sizeof(Record), ios::beg);
            file.write((char*)&nodo, sizeof(Record));
        }
    }

    file.close();
    return true;
}

template <typename TK>
int AVLFile<TK>::height(int pos) {
    if (pos == -1) return -1;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return -1;
    Record nodo;
    file.seekg(pos * sizeof(Record), ios::beg);
    file.read((char*)&nodo, sizeof(Record));
    int left_height = height(nodo.left);
    int right_height = height(nodo.right);
    return max(left_height, right_height) + 1;
}

template <typename TK>
int AVLFile<TK>::balancingFactor(int pos) {
    if (pos == -1) return 0;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return 0;
    Record nodo;
    file.seekg(pos * sizeof(Record), ios::beg);
    file.read((char*)&nodo, sizeof(Record));
    int left_height = height(nodo.left);
    int right_height = height(nodo.right);
    return left_height - right_height;
}

template <typename TK>
void AVLFile<TK>::leftRotate(int pos) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return;
    Record node;
    file.seekg(pos * sizeof(Record), ios::beg);
    file.read((char*)&node, sizeof(Record));
    Record rightNode;
    file.seekg(node.right * sizeof(Record), ios::beg);
    file.read((char*)&rightNode, sizeof(Record));
    node.right = rightNode.left;
    file.seekp(pos * sizeof(Record), ios::beg);
    file.write((char*)&node, sizeof(Record));
    rightNode.left = pos;
    pos = node.right; // actualizar la posición del nodo original al nuevo nodo raíz
    file.seekp(pos * sizeof(Record), ios::beg);
    file.write((char*)&rightNode, sizeof(Record));
    file.close();
}

template <typename TK>
void AVLFile<TK>::rightRotate(int pos) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return;
    Record node;
    file.seekg(pos * sizeof(Record), ios::beg);
    file.read((char*)&node, sizeof(Record));
    Record leftNode;
    file.seekg(node.left * sizeof(Record), ios::beg);
    file.read((char*)&leftNode, sizeof(Record));
    node.left = leftNode.right;
    file.seekp(pos * sizeof(Record), ios::beg);
    file.write((char*)&node, sizeof(Record));
    leftNode.right = pos;
    pos = node.left; // actualizar la posición del nodo original al nuevo nodo raíz
    file.seekp(pos * sizeof(Record), ios::beg);
    file.write((char*)&leftNode, sizeof(Record));
    file.close();
}

template <typename TK>
void AVLFile<TK>::balance(int pos) {
    if (balancingFactor(pos) == 2) {
      Record nodo;
      ifstream file(this->filename, ios::binary);
      file.seekg(pos*sizeof(Record), ios::beg);
      file.read((char*) &nodo, sizeof(Record));
      file.close();
      if(balancingFactor(nodo.left) == -1) {
          leftRotate(nodo.left);
      }
      rightRotate(pos);
    }
    if (balancingFactor(pos) == -2) {
      Record nodo;
      ifstream file(this->filename, ios::binary);
      file.seekg(pos*sizeof(Record), ios::beg);
      file.read((char*) &nodo, sizeof(Record));
      file.close();
      if(balancingFactor(nodo.right) == 1) {
        rightRotate(nodo.right);
      }
      leftRotate(pos);
    }
}