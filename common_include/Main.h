#ifndef MAIN7_LOW_H
#define MAIN7_LOW_H
#include <iostream>
namespace snaf {
	// Set up for C function definitions, even when using C++ //
	//#ifdef __cplusplus
	//	extern "C" {
	//#endif

	// size in bytes for name array for chunk.
	// Used for storing textual flags
#define SNAF_NAMESIZE 255

	// default size of integer in bytes
#define SNAF_SIZEINT 4
	// generic header size
#define SNAF_GENERICHEADER_SIZE SNAF_SIZEINT * 4

// how many bytes at a time to process for: compression, decompression and checksum for whole file.
#define SNAFCHUNKSIZE 32768

#define ChunkEOFZ_SIZE 	SNAF_GENERICHEADER_SIZE + SNAF_SIZEINT * 2

#define ERROR_HERE	printf(">>> ERROR --->  file: '%s', %s(), line: %d \n", __FILE__, __FUNCTION__, __LINE__);
	
}

#endif
