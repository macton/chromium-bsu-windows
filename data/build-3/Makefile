all: simulation.bsu bsu.ksy

# View ksy
ksv: bsu.ksy simulation.bsu
	ksv simulation.bsu bsu.ksy

# Preprocess simulation.yml 
simulation.i.yml: simulation.yml spp.js
	node spp.js simulation.yml simulation.i.yml

# Create binary from preprocessed yml
simulation.bsu: simulation.i.yml bsu.js
	node bsu.js simulation.i.yml bsu.schema.yml simulation.bsu

# Create ksy from binary schema
bsu.ksy: bsu.schema.yml bsu.ksy.js
	node bsu.ksy.js bsu.schema.yml bsu.ksy
