SRCO = AudioDataflow.o AudioObject.o Block.o StringFormat.o Yggdrasil.o
OBJO = ~add.o ~fileout.o ~filter.o ~mult.o ~noise.o ~oscillator.o ~square.o ~crush.o
STDLIB = -static-libgcc -static-libstdc++

iSRCO = $(addprefix inter/,$(SRCO))
iOBJO = $(addprefix inter/,$(OBJO))

Yggdrasil.exe: $(iOBJO) $(iSRCO)
	g++ -Wall -g -static-libgcc -static-libstdc++ $^ -o $@

inter/%.o: %.cpp
	g++ -Wall -g -fsanitize-undefined-trap-on-error -c $^ -o $@

inter/%.o: objects/%.cpp
	g++ -Wall -g -fsanitize-undefined-trap-on-error -c $^ -o $@