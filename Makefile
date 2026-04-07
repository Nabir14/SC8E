all: clean compile run
compile:
	mkdir bin
	g++ src/main.cpp -o bin/sc8e -lSDL3
run:
	./sc8e
clean:
	rm -rf bin
