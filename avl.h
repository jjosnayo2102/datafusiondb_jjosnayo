#include "seqfile.h"

template<typename TK>
class AVLFile {
private:
  string filename;
  long pos_root;
  bool es_hoja(Anime record);
  vector<Anime> inOrder(int pos);
  bool insert(int pos, Anime record, fstream& file);
  // para ser AVL
  int height(int pos);
  void balance(int pos, fstream& file);
  int balancingFactor(int pos);
  void leftRotate(int pos);
  void rightRotate(int pos);

public :
  AVLFile(string filename){
    this->filename = filename;
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        this->pos_root = 0;
        ofstream archivo(filename, ios::binary);
        file.write((char*) &pos_root, sizeof(int)); //primer valor
    }else{
        file.read((char*) &pos_root, sizeof(int)); //primer valor
    }
  }

  bool insert(Anime record) {
    fstream file(filename, ios::in | ios::out | ios::binary);
    bool res = insert(this->root, record, file);
    file.close();
    return res;
  }

  Anime find(TK codigo);

  vector<Anime> inorder(){
     return inOrder(pos_root);
  }
  // hacer una variación para la búsqueda por rango

  bool remove(TK key);
};

template <typename TK>
bool AVLFile<TK>::es_hoja(Anime record){
    return (record.left == -1 && record.right == -1);
}

template <typename TK>
vector<Anime> AVLFile<TK>::inOrder(int pos) {
    vector<Anime> records;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return records;
    Anime nodo;
    file.seekg(sizeof(int) + pos*sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));
    if (nodo.left != -1) {
        vector<Anime> leftRecords = inOrder(nodo.left);
        records.insert(records.end(), leftRecords.begin(), leftRecords.end());
    }
    records.push_back(nodo);
    if (nodo.right != -1) {
        vector<Anime> rightRecords = inOrder(nodo.right);
        records.insert(records.end(), rightRecords.begin(), rightRecords.end());
    }
    return records;
}

template <typename TK>
bool AVLFile<TK>::insert(int pos, Anime record, fstream &file) {
    //caso especial: 0 elementos
    file.seekg(0, ios::end);
    if(sizeof(int) == file.tellg()){
        file.write((char*)(&record), sizeof(Anime));
        return true;
    }
    // Leer el nodo en la posición actual
    file.seekg(sizeof(int) + sizeof(Anime)*pos, ios::beg);
    Anime currentNode;
    file.read((char*)(&currentNode), sizeof(Anime));
    // Si el nodo es hoja enlazar con el nuevo nodo
    if (es_hoja(currentNode)) {
        // Mover al final del archivo
        file.seekp(0, ios::end);
        int npos = (file.tellp() - sizeof(int))/ sizeof(Anime);
        if(record.id < currentNode.id){
            currentNode.left = npos;
            // Actualizar el nodo modificado (escribiéndolo de vuelta al archivo)
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)(&currentNode), sizeof(Anime));
        }else if(record.id < currentNode.id){
            currentNode.right = npos;
            // Actualizar el nodo modificado (escribiéndolo de vuelta al archivo)
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)(&currentNode), sizeof(Anime));
        }else{
            return false;
        }
        // Escribir el nuevo nodo en el archivo
        file.seekp(0, ios::end);
        file.write((char*)(&record), sizeof(Anime));
        return true;
    }

    // Insertar en el subárbol izquierdo o derecho
    bool l = true;
    bool r = true;
    if (record.id < currentNode.id) {
        l = insert(currentNode.left, record, file);  // Recursión para la izquierda
    } else {
        r = insert(currentNode.right, record, file); // Recursión para la derecha
    }
    // Balancear el nodo después de la inserción
    if(l == 0 || r == 0){
        return false;
    }
    balance(pos, file); // revisa para cada nodo en el camino si es necesario balancear
    return true;
}

template <typename TK>
Anime AVLFile<TK>::find(TK codigo){
    Anime ret;
    ifstream file(this->filename, ios::binary);
    if(!file.is_open()) return ret;
    file.seekg(sizeof(int) + pos_root*sizeof(Anime), ios::beg);
    Anime nodo;
    file.read((char*) &nodo, sizeof(Anime));
    while(1){
        if(es_hoja(nodo)) break;
        if(codigo > nodo.id){
            if(nodo.right == -1) return ret;
            file.seekg(sizeof(int) + nodo.right*sizeof(Anime), ios::beg);
            file.read((char*) &nodo, sizeof(Anime));
        }
        else if(codigo < nodo.id){
            if(nodo.left == -1) return ret;
            file.seekg(sizeof(int) + nodo.left*sizeof(Anime), ios::beg);
            file.read((char*) &nodo, sizeof(Anime));
        }
        else{
          return nodo;
        }
    }
    if(nodo.id == codigo){
      return nodo;
    }
    else{
      return ret;
    }
}

//modificar para implementar balanceo
template <typename TK>
bool AVLFile<TK>::remove(TK key) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return false;

    int parent_pos = -1;  // Posición del nodo padre
    int current_pos = pos_root;  // Comenzamos desde la raíz
    bool is_left_child = false;  // Determina si el nodo es un hijo izquierdo
    Anime nodo;

    // Buscar el nodo que se va a eliminar
    while (current_pos != -1) {
        file.seekg(sizeof(int) + current_pos * sizeof(Record), ios::beg);
        file.read((char*)&nodo, sizeof(Record));
        
        if (key == nodo.id) {
            break; // Nodo encontrado
        }

        parent_pos = current_pos;
        if (key < nodo.id) {
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
            file.seekg(sizeof(int) + parent_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            if (is_left_child) {
                nodo.left = -1;
            } else {
                nodo.right = -1;
            }
            file.seekp(sizeof(int) + parent_pos * sizeof(Record), ios::beg);
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
            file.seekg(sizeof(int) + parent_pos * sizeof(Record), ios::beg);
            file.read((char*) &nodo, sizeof(Record));
            if (is_left_child) {
                nodo.left = child_pos;
            } else {
                nodo.right = child_pos;
            }
            file.seekp(sizeof(int) + parent_pos * sizeof(Record), ios::beg);
            file.write((char*)&nodo, sizeof(Record));
        }
    }
    // Caso 3: Nodo tiene dos hijos
    else {
        // Encontrar el sucesor inorden (el nodo más pequeño en el subárbol derecho)
        int successor_pos = nodo.right;
        int successor_parent_pos = current_pos;
        Anime successor;

        while (true) {
            file.seekg(sizeof(int) + successor_pos * sizeof(Anime), ios::beg);
            file.read((char*)&successor, sizeof(Anime));
            if (successor.left == -1) break;
            successor_parent_pos = successor_pos;
            successor_pos = successor.left;
        }

        // Reemplazar datos del nodo a eliminar con el sucesor
        file.seekp(sizeof(int) + current_pos * sizeof(Anime), ios::beg);
        file.write((char*)&successor, sizeof(Anime));

        // Ajustar los punteros del sucesor
        if (successor_parent_pos != current_pos) {
            file.seekg(sizeof(int) + successor_parent_pos * sizeof(Anime), ios::beg);
            file.read((char*) &nodo, sizeof(Anime));
            nodo.left = successor.right;
            file.seekp(sizeof(int) + successor_parent_pos * sizeof(Anime), ios::beg);
            file.write((char*)&nodo, sizeof(Anime));
        } else {
            // El sucesor está directamente a la derecha del nodo a eliminar
            file.seekg(sizeof(int) + current_pos * sizeof(Anime), ios::beg);
            file.read((char*) &nodo, sizeof(Anime));
            nodo.right = successor.right;
            file.seekp(sizeof(int) + current_pos * sizeof(Anime), ios::beg);
            file.write((char*)&nodo, sizeof(Anime));
        }
    }

    file.close();
    return true;
}

// modificar funciones de balanceo: caso base es nodo hoja, no posición -1

template <typename TK>
int AVLFile<TK>::height(int pos) {
    if (pos == -1) return -1;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return -1;
    Anime nodo;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));
    int left_height = height(nodo.left);
    int right_height = height(nodo.right);
    return max(left_height, right_height) + 1;
}

template <typename TK>
int AVLFile<TK>::balancingFactor(int pos) {
    if (pos == -1) return 0;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return 0;
    Anime nodo;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));
    int left_height = height(nodo.left);
    int right_height = height(nodo.right);
    return left_height - right_height;
}

template <typename TK>
void AVLFile<TK>::leftRotate(int pos) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return;
    Anime node;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&node, sizeof(Anime));
    Anime rightNode;
    file.seekg(sizeof(int)+node.right * sizeof(Anime), ios::beg);
    file.read((char*)&rightNode, sizeof(Anime));
    node.right = rightNode.left;
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&node, sizeof(Anime));
    rightNode.left = pos;
    pos = node.right; // actualizar la posición del nodo original al nuevo nodo raíz
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&rightNode, sizeof(Anime));
    file.close();
}

template <typename TK>
void AVLFile<TK>::rightRotate(int pos) {
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return;
    Anime node;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&node, sizeof(Anime));
    Anime leftNode;
    file.seekg(sizeof(int)+node.left * sizeof(Anime), ios::beg);
    file.read((char*)&leftNode, sizeof(Anime));
    node.left = leftNode.right;
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&node, sizeof(Anime));
    leftNode.right = pos;
    pos = node.left; // actualizar la posición del nodo original al nuevo nodo raíz
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&leftNode, sizeof(Anime));
    file.close();
}

template <typename TK>
void AVLFile<TK>::balance(int pos, fstream& file) {
    if (balancingFactor(pos) == 2) {
      Anime nodo;
      file.seekg(sizeof(int)+pos*sizeof(Anime), ios::beg);
      file.read((char*) &nodo, sizeof(Anime));
      if(balancingFactor(nodo.left) == -1) {
          leftRotate(nodo.left);
      }
      rightRotate(pos);
    }
    if (balancingFactor(pos) == -2) {
      Anime nodo;
      file.seekg(sizeof(int)+pos*sizeof(Anime), ios::beg);
      file.read((char*) &nodo, sizeof(Anime));
      if(balancingFactor(nodo.right) == 1) {
        rightRotate(nodo.right);
      }
      leftRotate(pos);
    }
}