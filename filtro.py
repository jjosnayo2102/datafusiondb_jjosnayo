import pandas as pd
import random

# archivo fuente
df = pd.read_csv("anime-dataset-2023.csv")
filtro = df[["anime_id", "Name", "Score", "Genres", "Type", "Premiered", "Status", "Studios"]]

# Crear una lista para almacenar las filas filtradas
nuevas_filas = []

while len(nuevas_filas) < 100000:
    e = random.randint(0, filtro.shape[0] - 1)  # Seleccionar una fila aleatoria
    fila = filtro.iloc[e].copy()  # Hacer una copia de la fila para evitar la modificación directa

    # Convertir "Premiered" a float
    if fila["Premiered"] is not None and isinstance(fila["Premiered"], str):
        l = fila["Premiered"].split(" ")
        if l[0] == "summer":
            fila["Premiered"] = int(l[1]) + 0.1
        elif l[0] == "fall":
            fila["Premiered"] = int(l[1]) + 0.2
        elif l[0] == "winter":
            fila["Premiered"] = int(l[1]) + 0.3
        elif l[0] == "spring":
            fila["Premiered"] = int(l[1]) + 0.4
        else:  # l[0] == "UNKNOWN"
            continue  # Saltar filas desconocidas

        # Agregar la fila procesada a la lista
        nuevas_filas.append(fila)

# Crear un nuevo DataFrame con las nuevas filas
nuevo_df = pd.DataFrame(nuevas_filas)

# archivo a usar
nuevo_df.to_csv("datos.csv", index=False, sep='|')

