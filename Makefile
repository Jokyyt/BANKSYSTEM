# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra

# Répertoires contenant les fichiers source
SRC_DIRS = main bankfunctions

# Ajout des nouveaux fichiers source
SRC_DIRS += bankfunctions

# Recherche des fichiers source dans les répertoires
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))

# Ajout des nouveaux fichiers source
SOURCES += $(wildcard bankfunctions/*.c)

# Noms des fichiers objets générés
OBJECTS = $(SOURCES:.c=.o)

# Nom du binaire final
EXECUTABLE = bank_program

# Ancien nom du binaire (après compilation)
OLD_EXECUTABLE = bank_program.exe

# Chemin vers la bibliothèque libjson.a
LIBJSON = lib/libjson.a

# Chemin vers la bibliothèque cJSON
CJSON_LIB = -L./Librairies/cJSON -lcjson

# Cible par défaut
all: $(EXECUTABLE)

# Compilation du binaire
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBJSON) $(CJSON_LIB)
	@mv $(OLD_EXECUTABLE) $(EXECUTABLE) 2>/dev/null || true

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Nettoyage des fichiers objets et du binaire
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
