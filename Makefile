OPTS=-O2 -I thirdparty
LOPTS=-lGL -lglfw -lGLEW -lpthread

shaderview: main.o util.o init.o reload.o thirdparty/stb_image.o
	g++ -o $@ $(OPTS) $(LOPTS) $^

%.o : %.cc
	g++ -c $^ $(OPTS) -o $@

thirdparty/%.o : thirdparty/%.cc
	g++ -c $^ $(OPTS) -o $@
clean:
	rm -f main *.o thirdparty/*.o
