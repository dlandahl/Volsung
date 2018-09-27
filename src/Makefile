SRCO := AudioDataflow AudioObject StringFormat Yggdrasil Patch
OBJO := ~add ~fileout ~filter ~mult ~noise ~oscillator ~square ~crush
STDLIB := -static-libgcc -static-libstdc++ -std=c++17

SRCO := $(addsuffix .o, $(addprefix ../inter/, $(SRCO)))
OBJO := $(addsuffix .o, $(addprefix ../inter/, $(OBJO)))

ifeq ($(OS),Windows_NT)
	BINARY := Yggdrasil.exe
else
	BINARY := Yggdrasil.out
endif

../build/$(BINARY): $(OBJO) $(SRCO)
	g++ -Wall $(STDLIB) $^ -o $@

../inter/%.o: %.cpp
	g++ -Wall -c $^ -o $@

../inter/%.o: objects/%.cpp
	g++ -Wall -c $^ -o $@