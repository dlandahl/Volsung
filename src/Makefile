SRCO := AudioDataflow AudioObject StringFormat Yggdrasil Patch
OBJO := ~add ~fileout ~filter ~mult ~noise ~oscillator ~square ~crush
STDLIB := -static-libgcc -static-libstdc++

SRCO := $(addsuffix .o, $(addprefix ../inter/, $(SRCO)))
OBJO := $(addsuffix .o, $(addprefix ../inter/, $(OBJO)))

../Yggdrasil.exe: $(OBJO) $(SRCO)
	g++ -Wall $(STDLIB) $^ -o $@

../inter/%.o: %.cpp
	g++ -Wall -c $^ -o $@

../inter/%.o: objects/%.cpp
	g++ -Wall -c $^ -o $@