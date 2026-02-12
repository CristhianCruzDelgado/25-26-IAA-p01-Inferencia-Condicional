# Universidad de La Laguna   
# Escuela Superior de Ingeniería y Tecnología
# Grado en Ingeniería Informática
# Asignatura: Diseño y Análisis de Algoritmos
# Curso: 3º
# Práctica 1: Análisis de algoritmos
# Autor: Michelangelo da Cruz Bochicchio 
# Autor: Cristhian Adoney Cruz Delgado
# Fecha: 06/02/2026
# Archivo: Makefile

CXX = g++
CXXFLAGS = -g -std=c++17 -Wall

SOURCES = inferencia-condicional.cc
EXECUTABLE = inferencia-condicional

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES:.cc=.o)
	$(CXX) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)

