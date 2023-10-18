# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra

# Noms des fichiers source
SOURCES = main.c bankfunctions.c

# Noms des fichiers objets générés
OBJECTS = $(SOURCES:.c=.o)

# Nom du binaire final
EXECUTABLE = myprogram

# Chemin vers l'en-tête cJSON
CJSON_INCLUDE = -I./Librairies/cJSON

# Chemin vers la bibliothèque cJSON
CJSON_LIB = -L./Librairies/cJSON -lcjson

# Règle par défaut
all: $(EXECUTABLE)

# Compilation du binaire
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(CJSON_INCLUDE) -o $@ $^ $(CJSON_LIB)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) $(CJSON_INCLUDE) -c -o $@ $<

# Nettoyage des fichiers objets et du binaire
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	