OPTS=-O2
LOPTS=-lGL -lglfw -lGLEW

shaderview: main.o util.o init.o
	g++ -o $@ $(OPTS) $(LOPTS) $^

%.o : %.cc
	g++ -c $^ $(OPTS) -o $@

clean:
	rm -f main *.o
