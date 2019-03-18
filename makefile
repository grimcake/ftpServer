objects = main.o ThreadPool.o

main : $(objects)
	g++ -o main $(objects) -lpthread

main.o : main.cpp ThreadPool.h
	g++ -c -g main.cpp

ThreadPool.o : ThreadPool.h
	g++ -c -g ThreadPool.cpp

clean:
	rm main main.o ThreadPool.o
