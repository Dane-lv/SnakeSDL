build: main.o
	gcc-14 -o game main.o -lSDL2main -lSDL2 -lSDL2_ttf -L/opt/homebrew/lib


buildc: mainc.o
	gcc-14 -o game mainc.o -lSDL2main -lSDL2 -L/opt/homebrew/lib 


main.o: ./source/main.c
	gcc-14 -c -g -I/opt/homebrew/include/SDL2 ./source/main.c

mainc.o: ./source/main copy.c
	gcc-14 -c -g -I/opt/homebrew/include/SDL2 ./source/main.c

clean:
	rm game
	rm *.o