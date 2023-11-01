# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra

# Noms des fichiers source
SOURCES = main.c bankfunctions.c

# Noms des fichiers objets générés
OBJECTS = $(SOURCES:.c=.o)

# Nom du binaire final avec extension .exe
EXECUTABLE = bank_program.exe

# Nom du binaire final sans extension .exe
EXECUTABLE_WITHOUT_EXT = bank_program

# Chemin vers l'en-tête cJSON
CJSON_INCLUDE = -I./Librairies/cJSON

# Chemin vers la bibliothèque cJSON
CJSON_LIB = -L./Librairies/cJSON -lcjson

# Règle par défaut
all: $(EXECUTABLE_WITHOUT_EXT)

# Compilation du binaire sans extension .exe
$(EXECUTABLE_WITHOUT_EXT): $(OBJECTS)
	$(CC) $(CFLAGS) $(CJSON_INCLUDE) -o $(EXECUTABLE) $^ $(CJSON_LIB)
	@if [ -f $(EXECUTABLE) ]; then mv $(EXECUTABLE) $(EXECUTABLE_WITHOUT_EXT); fi

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) $(CJSON_INCLUDE) -c -o $@ $<

# Nettoyage des fichiers objets et du binaire
clean:
	rm -f $(OBJECTS) $(EXECUTABLE_WITHOUT_EXT)
