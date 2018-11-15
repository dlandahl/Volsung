SRCO := AudioDataflow AudioObject StringFormat Yggdrasil Patch
OBJO := ~add ~fileout ~filter ~mult ~noise ~oscillator ~square ~crush ~delay
STDLIB := -static-libstdc++ -std=c++17

FLAG := -Wall -Wextra -Wpedantic

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
	g++ $(FLAG) $(STDLIB) $^ -o $@

$(INTER)%.o: %.cpp
	g++ $(FLAG) -c $^ -o $@

$(INTER)%.o: objects/%.cpp
	g++ $(FLAG) -c $^ -o $@

.PHONY : clean
clean:
	rm $(INTER)*.o
