
#include "FileLoadAndSave.h"






FileLoadAndSave::FileLoadAndSave(void){
	myBuffer[0] =
	myBuffer[1] =
	myBuffer[2] =
	myBuffer[3] =
	myBuffer[4] =
	myBuffer[5] =
	myBuffer[254] =
	myBuffer[255] = 0;
	strcpy(myFilename, "atlas.defs");
	myFilename[254] = myFilename[255] = 0;
	mFop = nullptr;

	// * * * * * * * * * * //
	strcpy(myIniFilename, "atlas.ini");
	myIniBaseFilename[254] = myIniBaseFilename[255] = 0;
	myIniFilename[254] = myIniFilename[255] = 0;
	myIniBuffer[254] = myIniBuffer[255] = 0;
	myIniBufferPath[254] = myIniBufferPath[255] = 0;
	myIniBufferLine3[254] = myIniBufferLine3[255] = 0;
	myIniBufferLine4[254] = myIniBufferLine4[255] = 0;
	mFopIni = nullptr;
	// * * * * * * * * * * //
}
FileLoadAndSave::~FileLoadAndSave(void){}
void FileLoadAndSave::debugOn() { debuggy = true; }
void FileLoadAndSave::debugOff() { debuggy = false; }
bool FileLoadAndSave::isDebug() { return debuggy; }


void FileLoadAndSave::set_base_filename(char *basefilename) {
	strncpy(myFilename, basefilename,    254);
	strncat(myFilename,".defs",254);
}
char *FileLoadAndSave::get_base_filename() {
	return (char *)&myFilename[0];
}

bool FileLoadAndSave::open_rectangle_defs_for_write() {
	return _fopen_read_write_append_autocreate( myFilename ); // open for both read and write
}
// return false on error
bool FileLoadAndSave::open_rectangle_defs_for_read() {
	return _fopen_read( myFilename ); // open for read
}

bool FileLoadAndSave::writeString(char *shortstring) {
	int retval = fwrite((unsigned char *)shortstring, strlen(shortstring),1,mFop);
	return true;
}

bool FileLoadAndSave::writeLine( char *buffer, int size) {
	strcat(buffer, "\n");
	int retval = fwrite(buffer, size,1,mFop);
	return true;
}

char *FileLoadAndSave::readStringHeader() {	// read 4 characters only
	static char header[5];
	header[0] = myBuffer[0];
	header[1] = myBuffer[1];
	header[2] = myBuffer[2];
	header[3] = myBuffer[3];
	header[4] = 0;
	return (char *)&header[0];
}


bool FileLoadAndSave::readString(char *shortstring, int length) {
	int retval = fread((unsigned char *)shortstring, length, 1, mFop);
	return true;
}

unsigned char * FileLoadAndSave::readLineString() {
	myBuffer[0] = 0;
	fgets((char *)&myBuffer[0], 254, mFop); // read full line
	return (unsigned char *)&myBuffer[0];
}





bool FileLoadAndSave::close_rectangle_defs() {
	fclose(mFop);
	mFop = nullptr;
	return true;
}




/** "r+"	Open a file for update both reading and writing.
* if the file doesn't exist, it is created.
* if the file already exists... (TODO... )
* @return error if false, else true
*/
bool FileLoadAndSave::_fopen_read_write_append_autocreate(char *filename) // open for both read and write
{
	//	debugOff();
		//debugOn();
//		DENTRY();
	if (mFop != nullptr) {
		fclose(mFop);		// close
		mFop = nullptr;
		if (isDebug()) {
			printf("File is already opened, closing()\n");
		}
	}
	mFop = fopen(filename, "r+b");
	if (mFop == nullptr) {
			if (isDebug()) {
			printf("filename '%s', open for update failure, trying next option\n", filename);
			}
		mFop = fopen(filename, "wb");
		if (mFop == nullptr) {
			if (isDebug()) {
				printf("Error, open for write, file: '%s'\n", filename);
			}
			return false;
		}
		fflush(mFop);
		fclose(mFop);

		mFop = fopen(filename, "r+b");
		if (mFop == nullptr) {
		//	mydebug->printf("Error, open error '%s', open for read-write failure, second time, exit code (-101)\n", filename);
			exit(-101);
		}
	}
	fflush(mFop);
	if (isDebug()) {
	//	mydebug->printf("Succesfull Opening of File: '%s' for read-write-append\n", filename);
	}
	//		DEXIT();
	return true;
}

bool FileLoadAndSave::_fopen_read( char *filename )
{
	if (mFop != nullptr) { // already opened
		if (isDebug()) {
			printf("File is already opened, closing()\n");
		}
		fclose(mFop);		// close
		mFop = nullptr;
	}
	mFop = fopen(filename, "rb");	// open file
	if (mFop == nullptr) {
		if (isDebug()) {
			printf("Error, open for read, file: '%s'\n", filename);
		}
		return false;
	}
	debugOff();
	return true;
}




bool FileLoadAndSave::setParameters(int px1, int py1, int width, int height, int spaceX, int spaceY, int numberOfX, int numberOfY, 
	float _zoom, ImVec2 _offsetxy, int _alphacolor){
	apx1 = px1;
	apy1 = py1;
	awidth = width;
	aheight = height;
	aspaceX = spaceX;
	aspaceY = spaceY;
	anumberOfX = numberOfX;
	anumberOfY = numberOfY;
	azoom = _zoom;
	aoffsetxy = _offsetxy;
	aalphacolor = _alphacolor;
	return true;
}

bool FileLoadAndSave::getParameters(int& px1, int& py1, int& width, int& height, int& spaceX, int& spaceY, int& numberOfX, int& numberOfY,
	float& _zoom, ImVec2& _offsetxy, int& _alphacolor) {

	 px1    = apx1;
	 py1    = apy1;
	 width  = awidth;
	 height = aheight;
	 spaceX = aspaceX;
	 spaceY = aspaceY;
	 numberOfX = anumberOfX;
	 numberOfY = anumberOfY;
	 _zoom = azoom;
	 _offsetxy = aoffsetxy;
	 _alphacolor = aalphacolor;
	 return true;
}



bool FileLoadAndSave::writeParametersIntoFilesystem()
{
	fprintf(mFop, "%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%u\n", apx1, apy1, awidth, aheight, aspaceX, aspaceY, anumberOfX, anumberOfY,
		azoom, aoffsetxy.x, aoffsetxy.y, aalphacolor);
	return true;
}

bool FileLoadAndSave::scanParametersFromBuffer()
{
//	debugOn();
	int zpx1=1, zpy1=2, zwidth=3, zheight=4, zspaceX=5, zspaceY=6, znumberOfX=7, znumberOfY=8;
	float zzoom=1.0f, zaoffset_x=0.0f, zaoffset_y=0.0f;
	unsigned int zalphacolor = 0;
	//myBuffer[6] = '\0';
	sscanf((char *)&myBuffer[5], "%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%u\n",
		&zpx1, &zpy1, &zwidth, &zheight, &zspaceX, &zspaceY, &znumberOfX, &znumberOfY,
		&zzoom, &zaoffset_x, &zaoffset_y, &zalphacolor);
	apx1 = zpx1;
	apy1 = zpy1;
	awidth = zwidth;
	aheight = zheight;
	aspaceX = zspaceX;
	aspaceY = zspaceY;
	anumberOfX = znumberOfX;
	anumberOfY = znumberOfY;
	azoom = zzoom;
	aoffsetxy.x = zaoffset_x;
	aoffsetxy.y = zaoffset_y;
	aalphacolor = zalphacolor;
	if (isDebug()) {
		printf("apx1 = %d\n", apx1);
		printf("apy1 = %d\n", apy1);
		printf("awidth = %d\n", awidth);
		printf("aheight = %d\n", aheight);
		printf("aspaceX = %d\n", aspaceX);
		printf("aspaceY = %d\n", aspaceY);
		printf("anumberOfX = %d\n", anumberOfX);
		printf("anumberOfY = %d\n", anumberOfY);
		printf("azoom       = %f\n", azoom );
		printf("aoffsetxy.x = %f\n", aoffsetxy.x);
		printf("aoffsetxy.y = %f\n", aoffsetxy.y);
		printf("alphacolor = %u\n",  aalphacolor );
	}
	return true;
}











bool FileLoadAndSave::setRectParameters(bool _ashowFilenameFlag,bool _ashowPositionFlag,bool _ashowSizeFlag,	bool _selectedflag, bool _ahilightedflag, ImVec2 _begin, ImVec2 _end)
{
	ashowFilenameFlag = _ashowFilenameFlag;
	ashowPositionFlag = _ashowPositionFlag;
	ashowSizeFlag = _ashowSizeFlag;
	aselectedflag = _selectedflag;
	ahilightedflag = _ahilightedflag;
	abegin = _begin;
	aend = _end;
	return true;
}
bool FileLoadAndSave::getRectParameters(bool& _ashowFilenameFlag, bool& _ashowPositionFlag, bool& _ashowSizeFlag, bool& _selectedflag, bool& _ahilightedflag,ImVec2& _begin, ImVec2& _end)
{
	_ashowFilenameFlag = ashowFilenameFlag;
	_ashowPositionFlag = ashowPositionFlag;
	_ashowSizeFlag = ashowSizeFlag;
	_selectedflag = aselectedflag;
	_ahilightedflag = ahilightedflag;
	_begin = abegin;
	_end = aend;
	return true;
}
bool FileLoadAndSave::writeRectParametersIntoFilesystem()
{
	fprintf(mFop, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		fromBoolToInteger(ashowFilenameFlag), fromBoolToInteger(ashowPositionFlag), fromBoolToInteger(ashowSizeFlag),
		fromBoolToInteger(aselectedflag), fromBoolToInteger(ahilightedflag), (int) abegin.x, (int)abegin.y, (int)aend.x, (int)aend.y);
	return true;
}
bool FileLoadAndSave::readRectParametersFromFilesystem()
{
	int isselected  = 0;
	int ishilighted = 0;
	int ishowFilenameFlag = 0;
	int ishowPositionFlag = 0;
	int ishowSizeFlag = 0;
	int ibx=0, iby=0;
	int iex=0, iey=0;
	sscanf((char *)&myBuffer[5], "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
		&ishowFilenameFlag, &ishowPositionFlag, &ishowSizeFlag,
		&isselected, &ishilighted, 
		&ibx, &iby, &iex, &iey);
	aselectedflag  = fromIntegerToBool( isselected  );
	ahilightedflag = fromIntegerToBool( ishilighted );
	ashowFilenameFlag = fromIntegerToBool(ishowFilenameFlag);
	ashowPositionFlag = fromIntegerToBool(ishowPositionFlag);
	ashowSizeFlag = fromIntegerToBool(ishowSizeFlag);
	abegin.x = (float) ibx;
	abegin.y = (float)iby;
	aend.x = (float)iex;
	aend.y = (float)iey;
	return true;
}



int FileLoadAndSave::fromBoolToInteger(bool ain){
	if (ain == false)
		return 0;
	return 1;
}
bool FileLoadAndSave::fromIntegerToBool(int ain){
	if (ain == 0)
		return false;
	return true;
}



/** make an allocated copy of string
*/
char *FileLoadAndSave::pCS(char *input) {
	int size = strlen(input);
	char *output = new char[size + 7];
	int i = 0;
	for (i = 0; i < size; i++) {
		output[i] = input[i];
	}
	output[i] = 0;
	return output;
}


void FileLoadAndSave::getDefsParameters(char *&prefilename, char *&postfilename) {
	char tokenString[3] = { ',', '\n', '\0' };
/*
	std::string sz = (char *)&myBuffer[0];
	std::stack< char *> myStack;
	strtokimplementation(myStack, sz, &tokenString[0]);
*/
	std::string sz0 = (char *)&myBuffer[0];
	std::vector< char *> myVector;
	strtokimplementation2(myVector, sz0, &tokenString[0]);

	if (isDebug()) {
/*
		int size = myStack.size();
		printf("stack size = %d\n", size);
		for (int i = 0;i < size;i++) {
			char *tokenedstring = myStack.top();
			printf(" +%s+ ", tokenedstring);
			myStack.pop();
		}
		printf("\n");
*/
		int size = myVector.size();
		printf("stack size = %d\n", size);
		for (int i = 0;i < size;i++) {
			char *tokenedstring = myVector[i];
			printf(" -%s- ", tokenedstring);
		}
		printf("\n");
	}

	prefilename = pCS(myVector[1]);
	postfilename = pCS(myVector[2]);

	for (int i = 0;i < (int) myVector.size();i++) {
		myVector.pop_back();
	}
}








void FileLoadAndSave::strtokimplementation2(std::vector<char *> &nmyVector, std::string &szi, const char *tokenString)
{
	// ------------------------------------------------------- //
	//  strtok implementation
	// ------------------------------------------------------- //
		// char tokenString[3] = { WINDOWS_PATH_SEPARATOR, LINUX_PATH_SEPARATOR, '\0' };
	char * cstr = new char[szi.length() + 1];
	std::strcpy(cstr, szi.c_str());		// cstr now contains a c-string copy of szi
	char * p = std::strtok(cstr, tokenString);
	while (p != 0)
	{
		nmyVector.push_back(pCS(p));
		p = std::strtok(NULL, tokenString);
	}
	delete[] cstr;
	// ------------------------------------------------------- //
}
void FileLoadAndSave::strtokimplementation(std::stack<char *> &nmyStack, std::string &szi, const char *tokenString)
{
	// ------------------------------------------------------- //
	//  strtok implementation
	// ------------------------------------------------------- //
		// char tokenString[3] = { WINDOWS_PATH_SEPARATOR, LINUX_PATH_SEPARATOR, '\0' };
	char * cstr = new char[szi.length() + 1];
	std::strcpy(cstr, szi.c_str());		// cstr now contains a c-string copy of szi
	char * p = std::strtok(cstr, tokenString);
	while (p != 0)
	{
		nmyStack.push(pCS(p));
		p = std::strtok(NULL, tokenString);
	}
	delete[] cstr;
	// ------------------------------------------------------- //
}


//const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& FileLoadAndSave::rtrim(std::string& s)
{
	const char* t = " \t\n\r\f\v";
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
inline std::string& FileLoadAndSave::ltrim(std::string& s)
{
	const char* t = " \t\n\r\f\v";
	s.erase(0, s.find_first_not_of(t));
	return s;
}
// trim from both ends of string (right then left)
inline std::string& FileLoadAndSave::trim(std::string& s)
{
	return ltrim(rtrim(s));
}





// DONE: see if .exe filename extension exists, if so remove it before adding .ini to filename
void FileLoadAndSave::set_ini_filename(char *baseinifilename) {
	strncpy(myIniFilename, baseinifilename, 254);

	char *bif = strrchr(myIniFilename, '.');
	if (bif != nullptr) { // extension exists
		if (_strnicmp(bif, ".exe", 254) == 0) {
			bif[0] = 0; // remove extension
		}
	}
	strncpy(myIniBaseFilename, myIniFilename, 254);
	strncat(myIniFilename, ".ini", 254);
}
char *FileLoadAndSave::get_ini_filename() {
	return (char *)&myIniFilename[0];
}
char *FileLoadAndSave::get_ini_base_filename() {
	return (char *)&myIniBaseFilename[0];
}




bool FileLoadAndSave::open_ini_file_for_write() {
	mFopIni = fopen(myIniFilename, "w");
	if (mFopIni == nullptr) {
	//	if (isDebug()) {
			printf("Error, open for write, file: '%s'\n", myIniFilename);
	//	}
		return false;
	}
	//fflush(mFopIni);
	return true;
}
// return false on error
bool FileLoadAndSave::open_ini_file_for_read() {
	mFopIni = fopen(myIniFilename, "r");	// open file
	if (mFopIni == nullptr) {
		if (isDebug()) {
			printf("Error, open for read, file: '%s'\n", myIniFilename);
		}
		return false;
	}
	return true;
}
bool FileLoadAndSave::close_ini_file() {
	if(mFopIni != nullptr)
		fclose( mFopIni );
	mFopIni = nullptr;
	return true;
}

// read from ini file, whatever is in buffer
// returns empty string on read error (mFopIni is nullptr)
void FileLoadAndSave::readIniLineStrings() {
	myIniBuffer[0] = 0;
	myIniBufferPath[0] = 0;
	myIniBufferLine3[0] = 0;
	myIniBufferLine4[0] = 0;

	if (mFopIni != nullptr) {
		// first string line
		fgets((char *)&myIniBuffer[0], 254, mFopIni); // read full line	
		char *bif = strrchr(myIniBuffer, '\n');		  // remove '\n'
		if (bif != nullptr) { bif[0] = '\0'; }// remove enter
		// second string line
		fgets((char *)&myIniBufferPath[0], 254, mFopIni); // read full line	
		      bif = strrchr(myIniBufferPath, '\n');		  // remove '\n'
		if (bif != nullptr) { bif[0] = '\0'; }// remove enter
		// 
		// third string line
		fgets((char *)&myIniBufferLine3[0], 254, mFopIni); // read full line	
		bif = strrchr(myIniBufferLine3, '\n');		  // remove '\n'
		if (bif != nullptr) { bif[0] = '\0'; }// remove enter
		// 
		// fourth string line
		fgets((char *)&myIniBufferLine4[0], 254, mFopIni); // read full line	
		bif = strrchr(myIniBufferLine4, '\n');		  // remove '\n'
		if (bif != nullptr) { bif[0] = '\0'; }// remove enter
		// 
	}


}
char *FileLoadAndSave::getIniLineString1() {
	return (char *)&myIniBuffer[0];
}
char *FileLoadAndSave::getIniLineString2() {
	return (char *)&myIniBufferPath[0];
}
char *FileLoadAndSave::getIniLineString3() {
	return (char *)&myIniBufferLine3[0];
}
char *FileLoadAndSave::getIniLineString4() {
	return (char *)&myIniBufferLine4[0];
}



void FileLoadAndSave::writeIniLineString(char *inlinestring, char *inlinestring2, char *inlinestring3, char *inlinestring4) {
	if (mFopIni != nullptr) {
		fprintf(mFopIni, "%s\n", inlinestring);
		fprintf(mFopIni, "%s\n", inlinestring2);
		if (inlinestring3 != nullptr) {
			fprintf(mFopIni, "%s\n", inlinestring3);
		}
		if (inlinestring4 != nullptr) {
			fprintf(mFopIni, "%s\n", inlinestring4);
		}
	}
}




