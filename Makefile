OPTS=-O2
LOPTS=-lGL -lglfw -lGLEW

shaderview: main.o util.o init.o
	g++ -o $@ $(OPTS) $(LOPTS) $^
clean:
	rm -f main *.o

util.o: util.cc
	g++ -c $^ $(OPTS) -o $@
init.o: init.cc
	g++ -c $^ $(OPTS) -o $@
main.o: main.cc
	g++ -c $^ $(OPTS) -o $@

