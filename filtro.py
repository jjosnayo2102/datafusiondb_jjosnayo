import pandas as pd

# archivo fuente
df = pd.read_csv("anime-dataset-2023.csv")
filtro = df[["anime_id","Name","Score","Genres","Type","Premiered","Status","Studios"]]
for i in range(0,filtro.shape[0]):
    fila = filtro.iloc[i]
    l = fila["Premiered"].split(" ")
    if l[0] == "summer":
        fila["Premiered"] = int(l[1]) + 0.1
    elif l[0] == "fall":
        fila["Premiered"] = int(l[1]) + 0.2
    elif l[0] == "winter":
        fila["Premiered"] = int(l[1]) + 0.3
    elif l[0] == "spring":
        fila["Premiered"] = int(l[1]) + 0.4
    else: # l[0] == "UNKNOWN"
        fila["Premiered"] = None
    filtro.iloc[i] = fila
# archivo a usar
filtro.to_csv("prueba.csv",index=False, sep=';')
