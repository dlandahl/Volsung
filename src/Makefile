SRCO := AudioDataflow AudioObject StringFormat Yggdrasil Patch
OBJO := ~add ~fileout ~filter ~mult ~noise ~oscillator ~square ~crush
STDLIB := -static-libstdc++ -std=c++17

INTER := ../inter/

SRCO := $(addsuffix .o, $(addprefix $(INTER), $(SRCO)))
OBJO := $(addsuffix .o, $(addprefix $(INTER), $(OBJO)))

ifeq ($(OS),Windows_NT)
	BINARY := Yggdrasil.exe
else
	BINARY := Yggdrasil.out
endif

$(shell mkdir -p $(INTER))

../build/$(BINARY): $(OBJO) $(SRCO)
	g++ -Wall $(STDLIB) $^ -o $@

$(INTER)%.o: %.cpp
	g++ -Wall -c $^ -o $@

$(INTER)%.o: objects/%.cpp
	g++ -Wall -c $^ -o $@

.PHONY : clean
clean:
	rm $(INTER)*.o
