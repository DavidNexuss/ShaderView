CC=g++
OPTS=-I thirdparty
LOPTS=-lGL -lglfw -lGLEW -lpthread
MODULES=main.o util.o init.o thirdparty/stb_image.o reload.o
WINMODULES=main.o util.o init.o thirdparty/stb_image.o thirdparty/glew.o

release: OPTS+=-O2
release: shaderview

shaderview.exe: CC=x86_64-w64-mingw32-g++
shaderview.exe: OPTS+= -O2 -I /x86_64-w64-mingw32/include
shaderview.exe: LOPTS= -m64 -L/usr/x86_64-w64-mingw32/bin -Wl,-Bstatic -static-libgcc -static-libstdc++ -lstdc++ -lwinpthread -lopengl32 -Wl,-Bdynamic -lglfw3

shaderview.exe: $(WINMODULES)
	$(CC) -o $@ $(OPTS) $^ $(LOPTS)
windows-release: shaderview.exe
	strip shaderview.exe glfw3.dll
	zip shaderview.zip shaderview.exe fragment.glsl glfw3.dll
shaderview: $(MODULES)
	$(CC) -o $@ $(OPTS) $(LOPTS) $^

install: shaderview
	cp -f shaderview /bin/
	chmod 755 /bin/shaderview
	strip /bin/shaderview
%.o : %.cc
	$(CC) -c $^ $(OPTS) -o $@

thirdparty/%.o : thirdparty/%.cc
	$(CC) -c $^ $(OPTS) -o $@

clean:
	rm -f *.o thirdparty/*.o
clean-all: clean
	rm shaderview*
