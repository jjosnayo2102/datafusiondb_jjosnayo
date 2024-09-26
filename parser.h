#include "inicializacion.h"

template<typename TK>
class SQLParser_SF {
public:
    SQLParser_SF(Seqfile& db_) : db(db_) {}

    void ejecutar(const std::string& consulta) {
        if (consulta.find("select") == 0) {
            procesar_select(consulta);
        } else if (consulta.find("insert") == 0) {
            procesar_insert(consulta);
        } else if (consulta.find("delete") == 0) {
            procesar_delete(consulta);
        } else if (consulta.find("where") != std::string::npos) {
            procesar_where(consulta);
        } else {
            std::cout << "Consulta no soportada." << std::endl;
        }
    }

private:
    Seqfile& db;

    void procesar_select(const std::string& consulta) {
        std::string id_str = extraer_valor(consulta, "where id = ");
        if (!id_str.empty()) {
            int id = std::stoi(id_str);
            Anime anime = db.buscar(id);
            std::cout << "Resultado: Anime(id=" << anime.id << ", name=" << anime.name << ")" << std::endl;
        }
    }

    void procesar_insert(const std::string& consulta) {
        std::string valores_str = extraer_valor(consulta, "values(");
        if (!valores_str.empty()) {
            std::istringstream iss(valores_str);
            std::string id_str, name;
            std::getline(iss, id_str, ',');  // Obtener el id
            std::getline(iss, name, ',');    // Obtener el nombre

            int id = std::stoi(id_str);
            // Quitar comillas del nombre
            name.erase(std::remove(name.begin(), name.end(), '\''), name.end());

            db.insertar(Anime(id, name));
        }
    }

    void procesar_delete(const std::string& consulta) {
        std::string id_str = extraer_valor(consulta, "where id = ");
        if (!id_str.empty()) {
            int id = std::stoi(id_str);
            db.remover(id);
        }
    }

    void procesar_where(const std::string& consulta) {
        std::string id1_str = extraer_valor(consulta, "where id between ");
        if (!id1_str.empty()) {
            size_t pos = id1_str.find(" and ");
            if (pos != std::string::npos) {
                std::string id2_str = id1_str.substr(pos + 5);
                id1_str = id1_str.substr(0, pos);

                int id1 = std::stoi(id1_str);
                int id2 = std::stoi(id2_str);

                std::vector<Anime> animes = db.buscar_por_rango(id1, id2);
                for (const Anime& anime : animes) {
                    std::cout << "Resultado: Anime(id=" << anime.id << ", name=" << anime.name << ")" << std::endl;
                }
            }
        }
    }

    std::string extraer_valor(const std::string& consulta, const std::string& clave) {
        size_t pos = consulta.find(clave);
        if (pos != std::string::npos) {
            size_t start = pos + clave.length();
            size_t end = consulta.find(')', start);
            return consulta.substr(start, end - start);
        }
        return "";
    }
};
