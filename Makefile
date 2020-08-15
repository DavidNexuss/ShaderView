OPTS=-O2
LOPTS=-lGL -lglfw -lGLEW -lpthread

shaderview: main.o util.o init.o reload.o
	g++ -o $@ $(OPTS) $(LOPTS) $^

%.o : %.cc
	g++ -c $^ $(OPTS) -o $@

clean:
	rm -f main *.o
