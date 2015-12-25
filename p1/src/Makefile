DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ansi
LDFLAGS := -lGL -lGLU -lglut -lGLEW
CC := g++

all: dirs triangles

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

triangles: $(DIROBJ)triangles.o $(DIROBJ)LoadShaders.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

run:
	./exec/triangles

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
