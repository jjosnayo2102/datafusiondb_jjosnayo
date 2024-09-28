#include "seqfile.h"

template<typename TK>
class AVLFile: public DataFusion<TK> {
private:
  string filename;
  int pos_root;
  bool es_hoja(Anime record);
  bool insertar(int pos, Anime record, fstream& file);
  vector<Anime> buscar_por_rango(int pos, TK id1, TK id2);
  void actualizar_padre(int pos, int pos_hijo_actual, int nuevo_hijo, fstream &file);
  int buscar_sucesor(int pos, fstream& file);
  bool remover(int pos, TK key, fstream &file);
  // para ser AVL
  int height(int pos, fstream& file);
  void balance(int pos, fstream& file);
  int balancingFactor(int pos, fstream& file);
  void leftRotate(int pos, fstream& file);
  void rightRotate(int pos, fstream& file);

public :
  AVLFile(string filename){
    this->filename = filename + ".bin";
    ifstream file(this->filename, ios::binary);
    if(!file.is_open()){
        this->pos_root = 0;
        ofstream archivo(this->filename, ios::binary);
        archivo.write((char*) &pos_root, sizeof(int)); //primer valor
    }else{
        file.read((char*) &pos_root, sizeof(int)); //primer valor
    }
  }

  bool insertar(Anime record) override{
    fstream file(filename, ios::in | ios::out | ios::binary);
    bool res = false;
    file.seekg(0, ios::end);
    if(sizeof(int) != file.tellg()){
        res = insertar(this->pos_root, record, file);
        file.close();
        return res;
    }else{ //caso especial: 0 elementos
        file.write((char*)(&record), sizeof(Anime));
        file.close();
        return true;
    }
  }

  Anime buscar(TK codigo) override;
  
  vector<Anime> buscar_por_rango(TK id1, TK id2) override{
    return buscar_por_rango(this->pos_root, id1, id2);
  }

  bool remover(TK key) override{
    fstream file(this->filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return false;
    bool res = false;
    file.seekg(0, ios::end);
    // Si el archivo no está vacío, llama a la función auxiliar de eliminación
    if (sizeof(int) != file.tellg()) {
        res = remover(this->pos_root, key, file);
    }
    file.close();
    return res;
  }
};

template <typename TK>
bool AVLFile<TK>::es_hoja(Anime record){
    return (record.left == -1 && record.right == -1);
}

template <typename TK>
bool AVLFile<TK>::insertar(int pos, Anime record, fstream &file) {
    // Leer el nodo en la posición actual
    file.seekg(sizeof(int) + sizeof(Anime)*pos, ios::beg);
    Anime currentNode;
    file.read((char*)(&currentNode), sizeof(Anime));
    // Si el nodo es hoja enlazar con el nuevo nodo
    if (es_hoja(currentNode)) {
        // Mover al final del archivo
        file.seekp(0, ios::end);
        int npos = (file.tellp() - sizeof(int))/ sizeof(Anime);
        record.padre = pos;
        if(record.id < currentNode.id){
            currentNode.left = npos;
            // Actualizar el nodo modificado (escribiéndolo de vuelta al archivo)
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)(&currentNode), sizeof(Anime));
        }else if(record.id > currentNode.id){
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
        if(currentNode.left == -1){
            file.seekp(0, ios::end);
            int npos = (file.tellp() - sizeof(int))/ sizeof(Anime);
            record.padre = pos;
            file.write((char*)(&record), sizeof(Anime));
            currentNode.left = npos;
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)(&currentNode), sizeof(Anime));
            return true;
        }
        l = insertar(currentNode.left, record, file);  // Recursión para la izquierda
    } else if (record.id > currentNode.id) {
        if(currentNode.right == -1){
            file.seekp(0, ios::end);
            int npos = (file.tellp() - sizeof(int))/ sizeof(Anime);
            record.padre = pos;
            file.write((char*)(&record), sizeof(Anime));
            currentNode.right = npos;
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)(&currentNode), sizeof(Anime));
            return true;
        }
        r = insertar(currentNode.right, record, file); // Recursión para la derecha
    }else{
        return false;
    }
    if(!l || !r){
        return false;
    }
    // Balancear el nodo después de la inserción
    balance(pos, file); // revisa para cada nodo en el camino si es necesario balancear
    return true;
}

template <typename TK>
Anime AVLFile<TK>::buscar(TK codigo){
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

template <typename TK>
vector<Anime> AVLFile<TK>::buscar_por_rango(int pos, TK id1, TK id2) {
    vector<Anime> records;
    ifstream file(this->filename, ios::binary);
    if (!file.is_open()) return records;

    Anime nodo;
    file.seekg(sizeof(int) + pos * sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));

    // Si el nodo actual tiene un hijo izquierdo y el id1 es menor o igual que el id del nodo actual,
    // exploramos el subárbol izquierdo (ya que puede contener valores en el rango)
    if (nodo.left != -1 && id1 <= nodo.id) {
        vector<Anime> leftRecords = buscar_por_rango(nodo.left, id1, id2);
        records.insert(records.end(), leftRecords.begin(), leftRecords.end());
    }

    // Si el id del nodo está dentro del rango, lo agregamos al resultado
    if (nodo.id >= id1 && nodo.id <= id2) {
        records.push_back(nodo);
    }

    // Si el nodo actual tiene un hijo derecho y el id2 es mayor o igual que el id del nodo actual,
    // exploramos el subárbol derecho (ya que puede contener valores en el rango)
    if (nodo.right != -1 && id2 >= nodo.id) {
        vector<Anime> rightRecords = buscar_por_rango(nodo.right, id1, id2);
        records.insert(records.end(), rightRecords.begin(), rightRecords.end());
    }

    return records;
}



template <typename TK>
void AVLFile<TK>::actualizar_padre(int pos_padre, int pos_hijo_actual, int nuevo_hijo, fstream &file) {
    // Obtener el nodo padre en la posición pos_padre
    file.seekg(sizeof(int) + sizeof(Anime) * pos_padre, ios::beg);
    Anime nodo_padre;
    file.read((char*)&nodo_padre, sizeof(Anime));

    // Actualizar el puntero al hijo correspondiente (izquierdo o derecho)
    if (nodo_padre.left == pos_hijo_actual) {
        nodo_padre.left = nuevo_hijo;  // Actualizar el hijo izquierdo
    } else if (nodo_padre.right == pos_hijo_actual) {
        nodo_padre.right = nuevo_hijo; // Actualizar el hijo derecho
    }

    // Escribir los cambios de vuelta al archivo
    file.seekp(sizeof(int) + sizeof(Anime) * pos_padre, ios::beg);
    file.write((char*)&nodo_padre, sizeof(Anime));
}

template <typename TK>
int AVLFile<TK>::buscar_sucesor(int pos, fstream &file) {
    if (pos == -1) return -1;  // No hay sucesor si el subárbol está vacío

    Anime nodo;

    // Movernos al nodo en la posición 'pos'
    file.seekg(sizeof(int) + sizeof(Anime) * pos, ios::beg);
    file.read((char*)&nodo, sizeof(Anime));

    // Seguir moviéndose a la izquierda hasta encontrar el nodo más pequeño
    while (nodo.left != -1) {
        pos = nodo.left;
        file.seekg(sizeof(int) + sizeof(Anime) * pos, ios::beg);
        file.read((char*)&nodo, sizeof(Anime));
    }
    // Devolver la posición del sucesor
    return pos;
}


template <typename TK>
bool AVLFile<TK>::remover(int pos, TK key, fstream &file) {
    if (pos == -1) return false;  // El nodo no se encontró

    // Leer el nodo en la posición actual
    file.seekg(sizeof(int) + sizeof(Anime) * pos, ios::beg);
    Anime nodo;
    file.read((char*)&nodo, sizeof(Anime));

    if (key < nodo.id) {
        // Eliminar del subárbol izquierdo
        bool res = remover(nodo.left, key, file);
        if (res) balance(pos, file);  // Balancear si se eliminó
        return res;
    } else if (key > nodo.id) {
        // Eliminar del subárbol derecho
        bool res = remover(nodo.right, key, file);
        if (res) balance(pos, file);  // Balancear si se eliminó
        return res;
    } else {
        // El nodo a eliminar ha sido encontrado
        if (nodo.left == -1 && nodo.right == -1) {
            // Caso 1: El nodo es una hoja
            if (nodo.padre != -1) {
                actualizar_padre(nodo.padre, pos, -1, file);  // Desconectar el nodo del padre
            } else {
                // Si es la raíz, actualizamos la raíz a -1
                this->pos_root = -1;
            }
        } else if (nodo.left == -1 || nodo.right == -1) {
            // Caso 2: El nodo tiene un solo hijo
            int hijo = (nodo.left != -1) ? nodo.left : nodo.right;
            if (nodo.padre != -1) {
                actualizar_padre(nodo.padre, pos, hijo, file);  // Enlazar el hijo con el padre
            } else {
                // Si es la raíz, el hijo pasa a ser la nueva raíz
                this->pos_root = hijo;
            }
        } else {
            // Caso 3: El nodo tiene dos hijos
            int sucesor_pos = buscar_sucesor(nodo.right, file);
            Anime sucesor;
            file.seekg(sizeof(int) + sizeof(Anime) * sucesor_pos, ios::beg);
            file.read((char*)&sucesor, sizeof(Anime));

            // Reemplazar los datos del nodo a eliminar con el sucesor
            nodo.id = sucesor.id;
            file.seekp(sizeof(int) + sizeof(Anime) * pos, ios::beg);
            file.write((char*)&nodo, sizeof(Anime));

            // Eliminar el sucesor
            remover(sucesor_pos, sucesor.id, file);
        }

        // Balancear el árbol después de la eliminación
        balance(nodo.padre, file);
        return true;
    }
}


template <typename TK>
int AVLFile<TK>::height(int pos, fstream& file) {
    Anime nodo;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));
    if(es_hoja(nodo)){
        return 0;
    }
    int left_height = 0;
    int right_height = 0;
    if(nodo.left != -1){
        int left_height = height(nodo.left, file);
    }
    if(nodo.right != -1){
        int right_height = height(nodo.right, file);
    }
    return max(left_height, right_height) + 1;
}

template <typename TK>
int AVLFile<TK>::balancingFactor(int pos, fstream& file) {
    Anime nodo;
    file.seekg(sizeof(int)+pos*sizeof(Anime), ios::beg);
    file.read((char*)&nodo, sizeof(Anime));
    int left_height = -1;
    int right_height = -1;
    if(nodo.left != -1) left_height = height(nodo.left, file);
    if(nodo.right != -1) right_height = height(nodo.right, file);
    return left_height - right_height;
}

template <typename TK>
void AVLFile<TK>::leftRotate(int pos, fstream& file) {
    Anime node;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&node, sizeof(Anime));

    Anime rightNode;
    file.seekg(sizeof(int)+node.right * sizeof(Anime), ios::beg);
    file.read((char*)&rightNode, sizeof(Anime));

    Anime parentNode;
    if(node.padre != -1){
        file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
        file.read((char*)&parentNode, sizeof(Anime));
        if(parentNode.left == pos){
            parentNode.left = node.right;
            file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
            file.write((char*)&parentNode, sizeof(Anime));
        }
        else{
            parentNode.right = node.right;
            file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
            file.write((char*)&parentNode, sizeof(Anime));
        }
    }

    int derecha = node.right; // posición del hijo
    int padre = node.padre;

    node.right = rightNode.left;
    node.padre = derecha;
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&node, sizeof(Anime)); // guardar en nodo

    rightNode.left = pos;
    rightNode.padre = padre;
    file.seekp(sizeof(int)+derecha*sizeof(Anime), ios::beg);
    file.write((char*)&rightNode, sizeof(Anime)); // guardar en hijo

    //ver caso donde el nodo es raíz
    if(padre == -1){
        this->pos_root = derecha;
        file.seekg(0, ios::beg);
        file.write((char*)&pos_root, sizeof(int));
    }
}

template <typename TK>
void AVLFile<TK>::rightRotate(int pos, fstream& file) {
    Anime node;
    file.seekg(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.read((char*)&node, sizeof(Anime));

    Anime leftNode;
    file.seekg(sizeof(int)+node.left * sizeof(Anime), ios::beg);
    file.read((char*)&leftNode, sizeof(Anime));

    Anime parentNode;
    if(node.padre != -1){
        file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
        file.read((char*)&parentNode, sizeof(Anime));
        if(parentNode.left == pos){
            parentNode.left = node.left;
            file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
            file.write((char*)&parentNode, sizeof(Anime));
        }
        else{
            parentNode.right = node.left;
            file.seekg(sizeof(int)+node.padre * sizeof(Anime), ios::beg);
            file.write((char*)&parentNode, sizeof(Anime));
        }
    }

    int izquierda = node.left; // posición del hijo
    int padre = node.padre;

    node.left = leftNode.right;
    node.padre = izquierda;
    file.seekp(sizeof(int)+pos * sizeof(Anime), ios::beg);
    file.write((char*)&node, sizeof(Anime)); // guardar en nodo

    leftNode.right = pos;
    leftNode.padre = padre;
    file.seekp(sizeof(int)+izquierda*sizeof(Anime), ios::beg);
    file.write((char*)&leftNode, sizeof(Anime)); // guardar en hijo

    //ver caso donde el nodo es raíz
    if(padre == -1){
        this->pos_root = izquierda;
        file.seekg(0, ios::beg);
        file.write((char*)&pos_root, sizeof(int));
    }
}

template <typename TK>
void AVLFile<TK>::balance(int pos, fstream& file) {
    int bal = balancingFactor(pos, file);
    if (bal == 2) { //desbalanceado a la izquierda
      Anime nodo;
      file.seekg(sizeof(int)+pos*sizeof(Anime), ios::beg);
      file.read((char*) &nodo, sizeof(Anime));
      if(balancingFactor(nodo.left, file) == -1) {
          leftRotate(nodo.left, file);
      }
      rightRotate(pos, file);
    }
    if (bal == -2) { //desbalanceado a la derecha
      Anime nodo;
      file.seekg(sizeof(int)+pos*sizeof(Anime), ios::beg);
      file.read((char*) &nodo, sizeof(Anime));
      if(balancingFactor(nodo.right, file) == 1) {
        rightRotate(nodo.right, file);
      }
      leftRotate(pos, file);
    }
}