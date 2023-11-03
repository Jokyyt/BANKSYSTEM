# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra

# Chemin vers les répertoires des fichiers source
SRC_DIR = main bankfunctions

# Noms des fichiers source (en tenant compte des répertoires)
SOURCES = $(wildcard $(addsuffix /*.c, $(SRC_DIR)))

# Noms des fichiers objets générés (en tenant compte des répertoires)
OBJECTS = $(SOURCES:.c=.o)

# Nom du binaire final
EXECUTABLE = bank_program

# Chemin vers la bibliothèque libjson.a
LIBJSON = lib/libjson.a

# Chemin vers la bibliothèque cJSON
CJSON_LIB = -L./Librairies/cJSON -lcjson

# Règle par défaut
all: $(EXECUTABLE)

# Compilation du binaire
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBJSON) $(CJSON_LIB)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Nettoyage des fichiers objets et du binaire
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
