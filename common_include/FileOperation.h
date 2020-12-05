#ifndef FILEOPERATION1_H
#define FILEOPERATION1_H
// used by snaf code. 2020.09.15.
#include <cstdio>
//#include <vector>
//#include <iostream>
//#include "SNAF/LEVEL_Low.h"
//#include "DebugPrintf.h"

namespace snaf{


class FileOperation
	{
	public:
		FileOperation(void);
		~FileOperation(void);
		// -----
		void debugOn();
		void debugOff();
		bool isDebug();
		// -----
		//
		bool _fopen(char *filename, char *mode);
		bool _fopen_read_write_append_autocreate(char *filename);
		
		bool _fopen_from_memory(unsigned char *, int);
		bool _fclose_from_memory();
		// -----
		int  _fflush();
		bool _fseek(int offset, int origin);
		int  _ftell();
		bool _fclose();

		int  _fread(unsigned char *buffer, int elementsize);
		int  _fwrite(unsigned char *buffer, int elementsize);

		bool _feof();
		void _fclearerr();

		int  _getFileSize();
		int  _getFileSize(char *filename);

		bool privateReadString( unsigned char *buffer, int size);
		int  privateRead32bits();
		bool privateWriteString(unsigned char *buffer, int size);
		bool privateWrite32bits(int data);

		bool fileExist(char *filename);
		bool deleteFilename(char *filename);

		int writeAlignmentBytes();	
		int readAlignmentBytes();
		int calcAlignmentBytes();

		typedef struct			//
		{
			FILE *filehandle;	//
			void *graphics;		// (SDL_Surface *)
			void *text;		// wave audio, (AUDIO *)
			int  *textSize;	// byte size
		} minimalAnimal;
		
		bool readVariableAmountOfText( unsigned char *myTextBuffer);
		bool writeVariableAmountOfText(unsigned char *myTextBuffer);
		bool copyRawFileData(FileOperation *fopRead, FileOperation *fopWrite, int totalSize);
		// ============================= //
		void	enable_checksum();	// crc-32 checksum is calculated on every read operation
		void	disable_checksum();	// read operations go faster
		void	reset_checksum();	// reset the checksum start value
		int		get_checksum();		// get running checksum
		// ============================= //
		FILE *debug_get_FILE();
		void  debug_set_FILE(FILE *fop);

		bool seekOverEOFZ();


	private:
		bool memory_adjust_end_position_and_EOF();
		bool memory_fseek(int offset, int origin);
		int  memory_ftell();


		int	 SNAF_MAXALLOC=0;
		// . . . . . . . . . . . . . .
		//         data
		// . . . . . . . . . . . . . .

		bool  flagIsEOF = false;		// is true on end-of-file, used so we dont overwrite outside buffers
		bool  flagIsERROR = false;		// true if an error occured
	
		// +++---+++---+++---+++---+++---+++---+++--- //
		bool  flagIsMemorybufferEnabled = false;		// true if memory functionality is enabled
		unsigned char *memoryBuffer = nullptr;		// ram buffer for file
		int   memorySize=0;		// size of ram buffer
		int   memoryPosition=0;	// index pointer inside buffer
		// +++---+++---+++---+++---+++---+++---+++--- //
		FILE *mFop = nullptr;				// reading and writing
		bool fileIsOpenUpdate;	// true if opening of file for read/write went ok
		bool littleEndian;		// true if CPU is little endian (cpu family: intel x86)
		bool debuggy = false;
		// . . . . . . . . . . . . . .
//		LEVEL_Low *mLow = nullptr;
		int		runningChecksum = 0;
		bool	enabledChecksum = false;
//		DebugPrintf *mydebug = nullptr;
};
}

#endif
