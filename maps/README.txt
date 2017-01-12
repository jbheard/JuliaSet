This folder contains the contents of maps.h as precompiled binaries. 
Color mappings are arrays composed of 51 uint32_t objects, as such, these files contain the binary data for 51 uint32_t 
objects in the form of 204 encoded bytes. These files can be read back into color mappings like so:

	uint32_t map[51];
	std::ifstream file("filename.cmap", std::ios::binary);
	file.read((char*)map, 204);
	file.close();

Additionally, if a new color mapping is neededc a new cmap file can be created like so:

	uint32_t map[51] = { ... };
	std::ofstream file("filename.cmap", std::ios::binary);
	file.write( (char*)map, 204);
	file.close();

Note that uint32_t is not a regular type and can be found in the include file <cinttypes>.
