all: simulation.bsu bsu.ksy bsu.h main-bsu-c
clean:
	rm -f simulation.i.yml 
	rm -f simultion.bsu 
	rm -f simultion.bsu.o
	rm -f bsu.ksy 
	rm -f bsu.h
	rm -f main-bsu-c

# View ksy
ksv: bsu.ksy simulation.bsu
	ksv simulation.bsu bsu.ksy

# Preprocess simulation.yml 
simulation.i.yml: simulation.yml spp.js
	node spp.js simulation.yml simulation.i.yml

# Create binary from preprocessed yml
simulation.bsu: simulation.i.yml bsu.js bsu.schema.yml
	node bsu.js simulation.i.yml bsu.schema.yml simulation.bsu

# Create ksy from binary schema
bsu.ksy: bsu.schema.yml bsu.ksy.js bsu.schema.yml
	node bsu.ksy.js bsu.schema.yml bsu.ksy

# Create c header from binary schema
bsu.h: bsu.schema.yml bsu.hpp.js bsu.schema.yml
	node bsu.hpp.js bsu.schema.yml bsu.h

# view symbols in obj:
# $ readelf -Ws simulation.bsu.o
simulation.bsu.o: simulation.bsu
	objcopy -I binary -O default simulation.bsu simulation.bsu.o

main-bsu-c: main-bsu.c simulation.bsu.o bsu.h
	clang -g -O2 main-bsu.c simulation.bsu.o -lm -lncursesw -Wl,-Map,main-bsu-c.map -o main-bsu-c
