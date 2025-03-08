all: clean compile
compile:
	g++ src/main.cpp -o sc8e -lSDL2
clean:
	rm -rf sc8e
