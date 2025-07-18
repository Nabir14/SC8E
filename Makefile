all: clean compile run
compile:
	g++ src/main.cpp -o sc8e -lSDL3
run:
	./sc8e
clean:
	rm -rf sc8e
