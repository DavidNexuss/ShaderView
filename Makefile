CC=g++
OPTS=-I thirdparty -I /usr/include/freetype2
LOPTS=-lGL -lglfw -lGLEW -lpthread -lfreetype
MODULES=main.o util.o init.o reload.o scale.o \
		thirdparty/stb_image.o \
		thirdparty/FontAtlas.o \
		thirdparty/FTLabel.o \
		thirdparty/GLFont.o \
		thirdparty/GLUtils.o
WINMODULES=main.o util.o init.o thirdparty/stb_image.o thirdparty/FontAtlas.o thirdparty/FTLabel.o thirdparty/GLFont.o thirdparty/GLUtils.o thirdparty/glew.o 

shaderview: $(MODULES) mono.ttf
	$(CC) -o $@ $(OPTS) $(LOPTS) $(MODULES)

shaderview.exe: CC=x86_64-w64-mingw32-g++
shaderview.exe: OPTS+= -O2 -I /x86_64-w64-mingw32/include
shaderview.exe: LOPTS= -m64 -L/usr/x86_64-w64-mingw32/bin -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread -lopengl32 -Wl,-Bdynamic -lglfw3

shaderview.exe: $(WINMODULES)
	$(CC) -o $@ $(OPTS) $^ $(LOPTS)

release: OPTS+=-O2
release: shaderview

debug: OPTS+=-g
debug: shaderview

install: release
	cp -f shaderview /bin/
	chmod 755 /bin/shaderview
	strip /bin/shaderview
%.o : src/%.cc
	$(CC) -c $^ $(OPTS) -o $@

thirdparty/%.o : thirdparty/%.cc
	$(CC) -c $^ $(OPTS) -o $@

clean:
	rm -f *.o thirdparty/*.o
