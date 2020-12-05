#include "FileOperation.h"
#include "main.h"
#include <iostream>
#include <cstdio>


#ifdef NEIVERX

namespace snaf
{
	FileOperation::FileOperation(void) {
		debugOff();
		flagIsMemorybufferEnabled = false;	// use default file instead of memory by default
		memoryBuffer = nullptr;
		memorySize = 0;
		memoryPosition = 0;
		flagIsERROR = false;
		flagIsEOF = false;
		union {
			int theInteger;
			char singleByte;
		} endianTest;
		endianTest.theInteger = 1;
		if (endianTest.singleByte)
			littleEndian = true;	// intel
		else
			littleEndian = false;	// motorola
		fileIsOpenUpdate = false;
		mFop = nullptr;				// very important
		//
		SNAF_MAXALLOC = 100123456;		// maximum limit for memory allocation, 100 Mb should do fine
//		mLow = new LEVEL_Low();
		reset_checksum();
		disable_checksum();
//		DCONSTRUCT_ALLOCATE();
	}

	FileOperation::~FileOperation(void) {
		if (memoryBuffer != nullptr) delete memoryBuffer;
//		delete mLow;
		if (mFop != nullptr) fclose(mFop);
//		DCONSTRUCT_DEALLOCATE();
	}
	//


	void FileOperation::debugOn() { debuggy = true; }
	void FileOperation::debugOff() { debuggy = false; }
	bool FileOperation::isDebug() { return debuggy; }

	bool FileOperation::_fopen_from_memory(unsigned char *buffer, int size) {
		if (buffer==nullptr) return false;
		if (size < 0) return false;
		flagIsMemorybufferEnabled = true;		// true if memory functionality is enabled
		memoryBuffer = buffer;
		memorySize = size;		// size of ram buffer
		memoryPosition = 0;	// index pointer inside buffer
		if (isDebug()) {
			mydebug->printf("open_from_memory()...\n");
			mydebug->printf("buffer size = 0x%x\n", size);
		}
		return true;
	}
	bool FileOperation::_fclose_from_memory() {
		flagIsMemorybufferEnabled = false;
		if (memoryBuffer != nullptr){
			// delete memoryBuffer;		// we don't free buffer
			memoryBuffer = nullptr;		// just make it unavailable from this class
		}
		memorySize =			// size of ram buffer
		memoryPosition = 0;	// index pointer inside buffer
		return true;
	}

	// 
	// return true on success	
	bool FileOperation::_fopen(char *filename, char *mode)
	{
		//debugOn();
		if (isDebug()) {
		}

		if (mFop != nullptr) { // already opened
			if (isDebug()) {
				mydebug->printf("File is already opened, closing()\n");
			}
			_fclose();		// close
			mFop = nullptr;
		}
		mFop = fopen(filename, mode);	// open file
		reset_checksum();
		if (mFop == nullptr) {
			if (isDebug()) {
				mydebug->printf("Fopen ERROR : '%s', mode : %s", filename, mode);
			}
			//exit(-1);		
			return false;
		}
		debugOff();
		return true;
	}


	bool FileOperation::deleteFilename(char *filename)
	{
		_unlink(filename);
		return true;
	}


	/** "r+"	Open a file for update both reading and writing.
	* if the file doesn't exist, it is created.
	* if the file already exists... (TODO... )
	* @return this
	*/
	bool FileOperation::_fopen_read_write_append_autocreate(char *filename) // open for both read and write
	{
		//	debugOff();
			//debugOn();

//		DENTRY();

			if (mFop != nullptr) {
				_fclose();		// close
				mFop = nullptr;
				if (isDebug()) {
					mydebug->printf("File is already opened, closing()\n");
				}
			}
		mFop = fopen(filename, "r+b");
		reset_checksum();
		if (mFop == nullptr) {
			if (isDebug()) {
				mydebug->printf("filename '%s', open for update failure, trying next option\n", filename);
			}
			mFop = fopen(filename, "wb");
			if (mFop == nullptr) {
				mydebug->printf("Error, open error '%s', open for write, exit code (-100)\n", filename);
				exit(-100);
			}
			_fflush();
			_fclose();
			mFop = fopen(filename, "r+b");
			if (mFop == nullptr) {
				mydebug->printf("Error, open error '%s', open for read-write failure, second time, exit code (-101)\n", filename);
				exit(-101);
			}
		}
		_fflush();

		if (isDebug()) {
			mydebug->printf("Succesfull Opening of File: '%s' for read-write-append\n", filename);
		}
//		DEXIT();
		return true;
	}




	int FileOperation::_fflush() {
		if (mFop == nullptr) return 0;
		//	if (flagIsMemorybufferEnabled){		// memory system
		//		return 0;
		//	}
		return fflush(mFop);
	}

bool FileOperation::memory_adjust_end_position_and_EOF() {

	flagIsEOF = false;
	if (memoryPosition < 0) {
		flagIsEOF = true;
		memoryPosition = 0;
	}
	if (memoryPosition > memorySize) {
			flagIsEOF = true;
		memoryPosition = memorySize - 1;
	}
	return true;
}
int FileOperation::memory_ftell() {
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return memoryPosition;
	}
	return 0;
}
bool FileOperation::memory_fseek(int offset, int origin)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		if (memoryBuffer == nullptr) return false;
		if (origin == SEEK_SET) {
			if (offset < 0){ 
				memoryPosition = 0;
				return false;
			}
			memoryPosition = offset;
			}		// fseek() returns zero upon success, non - zero on failure.You can use fseek() to move beyond a file, but not before the beginning.Using fseek() clears the EOF flag associated with that stream.
		}
		if (origin == SEEK_CUR) {
			memoryPosition += offset;
			if (memoryPosition > memorySize) {
				flagIsEOF = true;
			}
		}
		if (origin == SEEK_END) {
			memoryPosition = memorySize + offset + 1;
		}
		if (memoryPosition < 0) memoryPosition = 0;
		// ********************
		return true;
	}


bool FileOperation::_fseek(int offset, int origin)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		memory_fseek( offset, origin );
		memory_adjust_end_position_and_EOF();
		return true;
	}
	int tmp = 0;
	if (mFop == NULL) return false;
	//	_fclearerr();
	flagIsERROR = false;
	flagIsEOF = false;
	_fflush();
	int iretval = fseek(mFop, offset, origin);
	if (iretval == 0){
		if (isDebug())
			mydebug->printf("OK,fseek, 0x%x, (%d)\n", offset, offset);
		flagIsERROR = false;
		flagIsEOF = false;
		_fflush();
		return true;
	}
	else{
		if (isDebug())
			mydebug->printf("ERROR,fseek, 0x%x, (%d)\n", offset, offset);
		flagIsERROR = true;
		flagIsEOF = true;
		_fflush();
		return false;
	}
	_fflush();
	return false;
}

int FileOperation::_ftell()
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled		
		return memory_ftell();
	}
	if (mFop == nullptr) return 0;
	return (int)ftell(mFop);
}
bool FileOperation::_fclose()
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		_fclose_from_memory();
		return true;
	}
	if (mFop == nullptr) return false;
	fflush(mFop);
	fclose(mFop);
	mFop = nullptr;	// very important!
	return true;
}
/*  *************************************************** */
void FileOperation::enable_checksum() {		enabledChecksum = true;	}
void FileOperation::disable_checksum() {	enabledChecksum = false;}
void FileOperation::reset_checksum() {
//	unsigned char baffer[7] = { 1,2,3,4,0 };
	runningChecksum = mLow->getBeginValueCrc32();
//	runningChecksum = mLow->calc_crc32(runningChecksum, &baffer[0], 4);
	}
int	FileOperation::get_checksum(){
	return runningChecksum;
}
/*  *************************************************** */

//
int FileOperation::_fread(unsigned char *destinationBuffer, int elementsize)
{
	if (elementsize == 0) return 0;
	int iretval = 0;
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		if (memoryBuffer == nullptr) return -1;
		if (flagIsEOF == true) {
			mydebug->printf("EOF IS ENABLED, read 0 bytes\n");
			// memory_adjust_end_position_and_EOF();
			return 0;
		}
		int howmuchtocopy = 0;
		//		char *memoryBuffer = nullptr;		// ram buffer for file
		//		int   memorySize = 0;		// size of ram buffer
		//		int   memoryPosition = 0;	// index pointer inside buffer
		int limit0 = memoryPosition - 1;
		int limit1 = memorySize;
		int sizediff = limit1 - limit0; // maximum index
		if (isDebug()) {
			mydebug->printf("sizediff              0x%x, (%d)\n", sizediff, sizediff);
			mydebug->printf("memorySize            0x%x, (%d)\n", memorySize, memorySize);
			mydebug->printf("wanted amount to read 0x%x, (%d)\n", elementsize, elementsize);
		}
		if (elementsize >= sizediff) {
			howmuchtocopy = sizediff;
			flagIsEOF = true;
		}
		else {
			howmuchtocopy = elementsize;
		}
		if (isDebug()){
			mydebug->printf("how much to read   =  0x%x, (%d)\n", howmuchtocopy, howmuchtocopy);
			mydebug->printf("memoryPosition (0) =  0x%x, (%d)\n", memoryPosition, memoryPosition);
		}
		int index = 0;
		if (isDebug())
			std::cout << "[ ";
		for ( index = 0;index < howmuchtocopy;index++) {
			unsigned char cc;
			cc = memoryBuffer[memoryPosition + index];						
			if (isDebug())
				std::cout << int(cc) << ", ";
			destinationBuffer[index] = cc;
			}
		if (isDebug())
			std::cout << " ]\n";
		memoryPosition += howmuchtocopy;
		if (isDebug())
			mydebug->printf("memoryPosition (1) =  0x%x, (%d)\n", memoryPosition, memoryPosition);
		iretval = index;
	}else {
		// ...
		if (mFop == nullptr) 
			return 0;
		iretval = fread(destinationBuffer, 1, elementsize, mFop);
		// ...
	}
	if (enabledChecksum==true) {
		runningChecksum = mLow->calc_crc32(runningChecksum, destinationBuffer, elementsize );
	}
	return iretval;
}

int FileOperation::_fwrite(unsigned char *buffer, int elementsize)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return 0;
	}
	if (mFop == nullptr) return 0;
	// If this number differs from the count parameter, a writing error prevented the function from completing.
	// In this case, the error indicator(ferror) will be set for the stream.
//	int ret = fwrite(buffer, elementsize, count, mFop);	// wrong usage
//	int ret = fwrite(buffer, count, elementsize, mFop);	// correct usage, returns elementsize on success
	int ret = fwrite(buffer, 1, elementsize, mFop);	// correct usage, returns elementsize on success
	if (enabledChecksum==true) {
		runningChecksum = mLow->calc_crc32(runningChecksum, buffer, elementsize);
	}
	clearerr(mFop);
	return ret;	// returns: elementsize is successfully written size
}

/** is end-of-file reached?
* @return true on End-Of-File
*/
bool FileOperation::_feof(){
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return flagIsEOF;
	}
	if (mFop == nullptr){ 		
		mydebug->printf("FATAL: mFop is NULL!, exit code(-117)\n");
		exit(-117);
	}
	int iretval = feof(mFop);
	if (iretval != 0)
		return true;
	else
		return false;
}

void FileOperation::_fclearerr(){
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		flagIsERROR = false;
		flagIsEOF = false;
		return;
	}
	flagIsERROR = false;
	flagIsEOF = false;
	clearerr(mFop);
}



// ..................................................................................................................
// ..................................................................................................................
// ..................................................................................................................
// ..................................................................................................................
// ..................................................................................................................
// ..................................................................................................................



/** get current file size
*
* return@ -1 on error, else filesize
*/
int FileOperation::_getFileSize()
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return memorySize;
	}
	if (mFop == nullptr) return -1;
	if (flagIsMemorybufferEnabled){
		return memorySize;
	}
	int sourcePointer = _ftell();	 // get original file position
	_fseek(0, SEEK_END);
	int iretval = _ftell();
	_fseek(sourcePointer, SEEK_SET); // restore file position
	return iretval;
}
// return 0 on error
int FileOperation::_getFileSize(char *filename)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return memorySize;
	}
	if (filename == nullptr){
		return 0;
	}
	if (strlen(filename) <= 0){
		return 0;
	}
	FILE *fp = fopen((char *)filename, "rb");
	if (fp == nullptr) {
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	int iretval = ftell(fp);
	fclose(fp);
	return iretval;
}


bool FileOperation::privateReadString(unsigned char *buffer, int size)
{
	if (flagIsMemorybufferEnabled == true && memoryBuffer == nullptr){
		mydebug->printf("flagIsMemorybufferEnabled = true while memoryBuffer is nullptr\n");
		mydebug->printf("exit(-434)!\n");
		exit(-434);
		return false;
	}
	if (flagIsMemorybufferEnabled==false && mFop == nullptr) {
		if (isDebug()) {
			mydebug->printf("FileOperation::privateReadString(...\n");
			mydebug->printf("ERROR, mFop is null!\n");
			mydebug->printf("exit(-234)!\n");
			exit(-234);
		}
		return false;
	}
	if (buffer == nullptr) {
		if (isDebug()) {
			mydebug->printf("FileOperation::privateReadString(...\n");
			mydebug->printf("ERROR, buffer is null!\n");
			mydebug->printf("exit(-235)!\n");
			exit(-235);
		}
		return false;
	}
	if (size == 0){
		return true;
	}
	for (int i = 0; i < size; i++) 	buffer[i] = 0;
	int iretval = _fread(buffer, size);

	if (isDebug()) {
		int ts = (int) buffer[0];
		mydebug->printf("\n************************\n");
		mydebug->printf("BUFFER CONTAINS: '%s, %d\n", buffer,ts );
		mydebug->printf("BUFFER SIZE    : '%d\n", size);
		mydebug->printf("************************\n");
	}

	if (iretval != size){ // can't read expected amount, exit
		if (isDebug()){
			mydebug->printf("ERROR at file position = 0x%x\n", _ftell());
			mydebug->printf("ERROR  size (wanted)   = 0x%x\n", size);
			mydebug->printf("ERROR  iretval (got)   = 0x%x\n", iretval);
			mydebug->printf("ERROR, can't read expected amount, exit code (-600)\n");
		}
		_fclose();
		exit(-600);
		return false;
	}
	return true;
}

// reads up to 255 characters
// read first byte, wich is a count of up to 255 bytes.
// variable amount of bytes are read into a buffer.
// return false on any error.

bool FileOperation::readVariableAmountOfText(unsigned char *_myTextBuffer)
{
//	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
//		return true;
//	}
//	debugOff();

//	DENTRY();
	if (flagIsMemorybufferEnabled==false && mFop == nullptr) {
		if (isDebug()) {
			mydebug->printf("mFop is nullptr!\n");
		}
			return false;
	}
	if (_myTextBuffer == nullptr){ 
		if (isDebug()) {
			mydebug->printf("_myTextBuffer is nullptr!\n");
		}
		return false;
	}
	unsigned char characterInput[4] = { 0,0,0,0 };
	for (int i = 0;i < SNAF_NAMESIZE; i++) {
		_myTextBuffer[i] = 0;
	}
	bool retval = privateReadString((unsigned char *)&characterInput[0], 1); //read first byte, string size
	unsigned char a = characterInput[0];
	short b = (short)a;
	int howMuchToRead = (int)b;
	if (isDebug()) {
		mydebug->printf("howmuchtoread = 0x%x\n", howMuchToRead);
	}
	howMuchToRead &= 255; // limit maximum length to read
	if (retval == false) {
		if (isDebug()) {
			mydebug->printf("ERROR, cant read single character from file.At position: 0x%x\n", _ftell());
		}

		return false;
	}
	if (howMuchToRead != 0) {
		retval = privateReadString((unsigned char *)_myTextBuffer, howMuchToRead);
		if (retval == false) {
			if (isDebug()) {
				mydebug->printf("ERROR, cant read string from file.At position: 0x%x, reading bytes: 0x%x\n\n", _ftell(), howMuchToRead);
			}
		}
	}
//	_myTextBuffer[howMuchToRead] = 0;
//	DEXIT();
	return true;
}

// writes up to 255 characters of text
// writes first byte, wich is a count of up to 255 bytes.
// variable amount of bytes are written from a buffer.
bool FileOperation::writeVariableAmountOfText(unsigned char *_myTextBuffer)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return true;
	}
//	DENTRY();
	if (mFop == nullptr) {
		mydebug->printf("Internal FILE structure is null, error\n");
		mydebug->printf("exit(-579)\n");
		exit(-579);
		return false;
	}
	if (_myTextBuffer == nullptr){
		if (isDebug()) {
			mydebug->printf("input string is NULL!\n");
			mydebug->printf("exit(777)\n");
		}
		exit(777);
	}
	int _mySize = (int)strlen((char *)_myTextBuffer);
	if (isDebug()) {
		mydebug->printf("mySize    = %d\n", _mySize);
	}
	if (_mySize >= 255) {
		_mySize = 254;
	}
	if (_mySize < 0) {
		_mySize = 0;
	}
	unsigned char mysizebuffer[16] = { 0,0,0,0 };
	mysizebuffer[0] = (unsigned char)_mySize;
	bool bretval = privateWriteString((unsigned char *)&mysizebuffer[0], 1);			// write first byte, string size
	if (bretval == false) { // failed to write single byte
		if (isDebug()) {
			mydebug->printf("ERROR, failed to write a single byte.At position: 0x%x\n", _ftell());	
			mydebug->printf("exit(-177)\n");
			exit(-177);
		}
		return false;
	}
	if (_mySize > 0) {
		bretval = privateWriteString((unsigned char *)_myTextBuffer, _mySize);	// variable amount to write.
		if (bretval == false) {		// failed to write variable length data
			if (isDebug()) {
				mydebug->printf("ERROR, failed to write variable length data\n");
				mydebug->printf("ERROR, failed to write 0x%x bytes. at position 0x%x\n", _mySize - 1, _ftell());
				mydebug->printf("exit(-17)\n");
			}
			exit(-17);
		}
	}
	if (isDebug()) {
		mydebug->printf("buffer = '%s'\n", _myTextBuffer );
		mydebug->printf("size   = 0x%x\n", _mySize);

	}
//	DEXIT();
	return true;
}





/*
* read 32 bits.
* @return -1 on error. else data
*/
int  FileOperation::privateRead32bits()
{
	unsigned char buffer[SNAF_SIZEINT * 8];

	if (flagIsMemorybufferEnabled==false && mFop == nullptr) return -1;

//	_fclearerr();

	int retval = _fread(buffer, SNAF_SIZEINT);
	if (retval != SNAF_SIZEINT){
		if (isDebug()) {
			mydebug->printf("ERROR, wrong retval on fread()! : %d, reading %d bytes\n", retval, SNAF_SIZEINT);
		}

		if (_feof())
			return -1;
	}
	int a = 0, b = 0, c = 0, d = 0;
	a = (int) buffer[0]; b = (int)buffer[1]; c = (int)buffer[2]; d = (int)buffer[3];
/*
	if (littleEndian){	// x86 intel cpu
		a <<= 24; b <<= 16; c <<= 8;
	}else{		// motorola
		d <<= 24; c <<= 16; b <<= 8;
	}
*/
//	debugOff();
	a <<= 24; b <<= 16; c <<= 8;
	return (a | b | c | d);
}


// return true on success
bool FileOperation::privateWriteString(unsigned char *buffer, int size)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return true;
	}
	//	debugOff();
	if (buffer == nullptr) {
		if (isDebug()) {		
			mydebug->printf("   FileOperation::privateWriteString() : !!! buffer is null !!! \n");
		}
		return true;
	}
	if (mFop == nullptr) {
		if (isDebug()) {
			mydebug->printf("   FileOperation::privateWriteString() : !!! Mfop is null !!!\n");
		}
		return false;
	}
	if (size < 0) {
		if (isDebug()) {
			mydebug->printf("   FileOperation::privateWriteString() :  !!! size is negative !!!\n");
			mydebug->printf("exit(-9) \n");
		}
		exit(-9);
		return false;
		}
	if (size == 0) {	
		if (isDebug()) {
			mydebug->printf("   FileOperation::privateWriteString() : !!! size is zero !!! \n");
		}
		return true;	
	}

	int retvali = _fwrite(buffer, size);

	if (retvali != size){

		if (isDebug()) {
			mydebug->printf("ERROR, ===>>>  FileOperation :: privateWriteString(unsigned char *buffer, int size)\n");
			mydebug->printf("ERROR, File write error, requested bytes to write = 0x%x\n, actually wrote = 0x%x\n",size,retvali);
			mydebug->printf("size   = 0x%x\n", size);
			mydebug->printf("buffer = '%s'\n", buffer);
		}
		return false;
	}
//	debugOff();
	return true;
}

// returns false on any error
bool FileOperation::privateWrite32bits(int data)
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return true;
	}
	// debugOn();
//	DENTRY();
	if (mFop == nullptr){
		mydebug->printf("ERROR, mFop is null!\n");
		mydebug->printf("returning with false\n");
		return false;
	}
	unsigned char buffer[SNAF_SIZEINT * 32];
	unsigned int a = data, b = data, c = data, d = data;
	a >>= 24;
	b >>= 16;
	c >>= 8;
	a &= 0xff;		b &= 0xff;	c &= 0xff;		d &= 0xff;
/*	
	if (littleEndian){	// x86 intel cpu
		buffer[0] = a; buffer[1] = b; buffer[2] = c; buffer[3] = d;
	}
	else{			// motorola
		buffer[3] = a; buffer[2] = b; buffer[1] = c; buffer[0] = d;
	}
*/
	buffer[0] = a; buffer[1] = b; buffer[2] = c; buffer[3] = d;

	int retval = _fwrite((unsigned char *)&buffer[0], SNAF_SIZEINT);

	if (retval != SNAF_SIZEINT){
		mydebug->printf("\nERROR, retval is not equal SNAF_SIZEINT\n");
		mydebug->printf("retval is       0x%x\n", retval);
		mydebug->printf("SNAF_SIZEINT is 0x%x\n", SNAF_SIZEINT);
		mydebug->printf("returning with false\n\n");
		return false;
	}
//	DEXIT();
	return true;
}






// returns true if file exists
bool FileOperation::fileExist(char *filename) // RETURNS true on success
{
	if (flagIsMemorybufferEnabled) {		// true if memory functionality is enabled
		return true;
	}
	if (filename == nullptr) return false;
	FILE *fp = fopen(filename, "r");
	if (fp == NULL){ return false; }
	fclose(fp);
	return true;
}





int FileOperation::writeAlignmentBytes(){
	int size = calcAlignmentBytes();
	unsigned char baffer[16 + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	privateWriteString((unsigned char *)&baffer[0], size);
	return size;
}

int FileOperation::readAlignmentBytes(){
	int size = calcAlignmentBytes();
	unsigned char baffer[16 + 1] = { 'A', 'L', 'I', 'G', 'N', 'M', 'E', 'N', 'T', ' ', 'O', 'P', 'T', 'I', 'O', 'N', 0 };
	privateReadString((unsigned char *)&baffer[0], size);
	return size;
}

// DONE, 16 bytes alignment	
int FileOperation::calcAlignmentBytes()
{
//debugOff();
	int positionBegin = _ftell();
	int masking  = 0xfffffff0;
	int imasking = 0x0000000f;
	int positionEnd = (positionBegin + 0x10) & masking;
	int size = positionEnd - positionBegin;
	int isPositionExtra = positionBegin & imasking;
	if (isPositionExtra == 0) size = 0;
	if (isDebug()){
		mydebug->printf("=======================\n");
		mydebug->printf("__FUNCTION__    \n", __FUNCTION__ );
		mydebug->printf("position begin = 0x%x\n", positionBegin);
		mydebug->printf("position end   = 0x%x\n", positionEnd);
		mydebug->printf("masking        = 0x%x\n", masking);
		mydebug->printf("size           = 0x%x\n", size);
		mydebug->printf(" -----------------------\n");
		mydebug->printf("isPositionExtra = 0x%x\n", isPositionExtra);	
		mydebug->printf("=======================\n");
	}
	return size;
}


bool FileOperation::copyRawFileData(FileOperation *fopRead, FileOperation *fopWrite, int totalSize)
{
	if (totalSize <= 0) return false;
//	debugOff();
	// totalSize = 0x4007;
	// DONE, divide into blocks
	int blockSize = 0x1000;
	int amountOfBlocks = totalSize / blockSize;
	int modulusOfBlocks = totalSize % blockSize;
	if (isDebug()){
		mydebug->printf("__FUNCTION__    \n", __FUNCTION__);
		mydebug->printf("totalsize       = 0x%x\n", totalSize);
		mydebug->printf("blocksize       = 0x%x\n", blockSize);
		mydebug->printf("amountOfBlocks  = 0x%x\n", amountOfBlocks);
		mydebug->printf("modulusOfBlocks = 0x%x\n", modulusOfBlocks);
	}
	unsigned char *buffer = new unsigned char[blockSize + 1];
	for (int i = 0; i < amountOfBlocks; i++){
		fopRead->privateReadString(buffer, blockSize);
		fopWrite->privateWriteString(buffer, blockSize);
	}
	if (modulusOfBlocks != 0){
		fopRead->privateReadString(buffer, modulusOfBlocks);
		fopWrite->privateWriteString(buffer, modulusOfBlocks);
	}
	return true;
}

FILE *FileOperation::debug_get_FILE() { return mFop; }
void  FileOperation::debug_set_FILE(FILE *fop) { mFop = fop; }






bool FileOperation::seekOverEOFZ()
{
	_fseek(
		-(SNAF_GENERICHEADER_SIZE + SNAF_SIZEINT * 2),
		SEEK_END
	);	// seek to end-of-file minus some bytes
	return true;		// ready to overwrite chunk EOFZ.
}



}//namespace

#endif
