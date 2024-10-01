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
        size_t pos = 0;
        bool in_quotes = false;

        // Función lambda para obtener el siguiente token, teniendo en cuenta las comillas
        auto get_next_token = [&](char delimiter) {
            string result;
            while (pos < valores_str.size()) {
                char current_char = valores_str[pos];
                pos++;
                if (current_char == '\"') {
                    in_quotes = !in_quotes;  // Cambia el estado de si estamos dentro o fuera de comillas
                } else if (current_char == delimiter && !in_quotes) {
                    break;
                } else {
                    result += current_char;
                }
            }
            return result;
        };

        // Procesamos cada campo usando el delimitador de comas, pero considerando comillas
        token = get_next_token(',');
        anime.id = stoi(token);  // ID es numérico, por lo que no hay necesidad de comillas

        token = get_next_token(',');
        strncpy(anime.nombre, token.c_str(), sizeof(anime.nombre));

        token = get_next_token(',');
        anime.puntaje = stof(token);  // El puntaje es numérico

        token = get_next_token(',');
        strncpy(anime.genero, token.c_str(), sizeof(anime.genero));

        token = get_next_token(',');
        strncpy(anime.tipo, token.c_str(), sizeof(anime.tipo));

        token = get_next_token(',');
        anime.temporada = stof(token);  // La temporada es numérica

        token = get_next_token(',');
        strncpy(anime.estado, token.c_str(), sizeof(anime.estado));

        token = get_next_token(',');
        strncpy(anime.estudio, token.c_str(), sizeof(anime.estudio));

        bool check = db->insertar(anime);
        if (check) {
            cout << "Elemento insertado" << endl;
        } else {
            cout << "El elemento ya existe" << endl;
        }
        cout << endl;
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
            cout << endl;
        }
    }

    string extraer_valor(const string& consulta, const string& clave) {
        size_t pos = consulta.find(clave);
        if (pos != string::npos) {
            size_t start = pos + clave.length();
            size_t end;
            if (consulta.find("insert into Anime ") == 0) {
                end = consulta.find(')', start);
            } else {
                // Para otras consultas, utiliza ';'
                end = consulta.find(';', start);
            }
            return consulta.substr(start, end - start);
        }
        return "";
    }
};
