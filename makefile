# Nome dell'eseguibile
TARGET = my_app

# Compilatore e flag
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# File sorgente
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Flags per SFML (usa pkg-config)
SFML_CFLAGS  = $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS    = $(shell pkg-config --libs sfml-graphics)

# Regola di default
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

# Regola per compilare i .cpp in .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(TARGET) *.o
