all: main.out

run: main.out
	./main.out

main.out: main.cpp Recommend.cpp 
	g++ -Wall main.cpp Recommend.cpp -o main.out 

clean: 
	rm -f main.out