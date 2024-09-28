#include "ext_hash.h"

template<typename TK>
class SQLParser {
private:
   DataFusion<TK>* db;
public:
    SQLParser(DataFusion<TK>* _db) : db(_db) {}

    void ejecutar(const string& consulta) {
        if (consulta.find("select * from Anime ") == 0) {
            if (consulta.find("between") != string::npos) {
                cout << "funciona" << endl;
                procesar_range(consulta);
            }else{
                procesar_select(consulta);
            }
        } else if (consulta.find("insert into Anime ") == 0) {
            procesar_insert(consulta);
        } else if (consulta.find("delete from Anime ") == 0) {
            procesar_delete(consulta);
        } else {
            cout << "Consulta no soportada." << endl;
        }
    }

private:
    void procesar_select(const string& consulta) {
        string id_str = extraer_valor(consulta, "where id = ");
        if (!id_str.empty()) {
            int id = stoi(id_str);
            Anime anime = db->buscar(id);
            anime.show();
            cout << endl;
        }
    }

    void procesar_range(const string& consulta) {
        string id1_str = extraer_valor(consulta, "where id between ");
        if (!id1_str.empty()) {
            size_t pos = id1_str.find(" and ");
            if (pos != string::npos) {
                string id2_str = id1_str.substr(pos + 5);
                id1_str = id1_str.substr(0, pos);
                int id1 = stoi(id1_str);
                int id2 = stoi(id2_str);
                vector<Anime> animes = db->buscar_por_rango(id1, id2);
                for (auto anime: animes) {
                    anime.show();
                    cout << endl;
                }
            }
        }
    }

    void procesar_insert(const string& consulta) {
        string valores_str = extraer_valor(consulta, "values(");
        if (!valores_str.empty()) {
            Anime anime;
            string token;
            istringstream iss(valores_str);
            getline(iss, token, ',');
            anime.id = stoi(token);
            getline(iss, token, ',');
            strncpy(anime.nombre, token.c_str(), sizeof(anime.nombre));
            getline(iss, token, ',');
            anime.puntaje = stof(token);
            getline(iss, token, ',');
            strncpy(anime.genero, token.c_str(), sizeof(anime.genero));
            getline(iss, token, ',');
            strncpy(anime.tipo, token.c_str(), sizeof(anime.tipo));
            getline(iss, token, ',');
            anime.temporada = stof(token);
            getline(iss, token, ',');
            strncpy(anime.estado, token.c_str(), sizeof(anime.estado));
            getline(iss, token, ',');
            strncpy(anime.estudio, token.c_str(), sizeof(anime.estudio));
            bool check = db->insertar(anime);
            if(check){
                cout << "Elemento insertado" << endl;
            }else{
                cout << "El elemento ya existe" << endl;
            }
        }
    }

    void procesar_delete(const std::string& consulta) {
        string id_str = extraer_valor(consulta, "where id = ");
        if (!id_str.empty()) {
            int id = stoi(id_str);
            bool check = db->remover(id);
            if(check){
                cout << "Elemento eliminado" << endl;
            }else{
                cout << "El elemento ya ha sido eliminado" << endl;
            }
        }
    }

    string extraer_valor(const string& consulta, const string& clave) {
        size_t pos = consulta.find(clave);
        if (pos != string::npos) {
            size_t start = pos + clave.length();
            size_t end = consulta.find(';', start);
            return consulta.substr(start, end - start);
        }
        return "";
    }
};
