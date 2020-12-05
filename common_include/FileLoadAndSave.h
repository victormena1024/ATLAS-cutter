#ifndef FILELOADANDSAVE_H
#define FILELOADANDSAVE_H

#include <stdio.h>
#include <stack>
#include <vector>
#include "main.h"
#include "imgui.h"

class FileLoadAndSave
{
public:
	FileLoadAndSave(void);
	~FileLoadAndSave(void);
	// -----
		// -----
	void debugOn();
	void debugOff();
	bool isDebug();
	// -----
	void set_base_filename(char *basefilename);
	char *get_base_filename();
	bool open_rectangle_defs_for_write();
	bool open_rectangle_defs_for_read();
	bool _fopen_read(char *filename);
	bool _fopen_read_write_append_autocreate(char *filename);

	bool writeString(char *shortstring);
	bool writeLine(char *buffer, int size);

	bool readString(char *string, int length);
	char *readStringHeader();	// read 4 bytes
	unsigned char *readLineString();	// read one line at a time

	bool close_rectangle_defs();



	bool setParameters(int px1, int py1, int width, int height, int spaceX, int spaceY, int numberOfX, int numberOfY, float zoom, ImVec2 offsetxy, int alphacolor);
	bool getParameters(int& px1, int& py1, int& width, int& height, int& spaceX, int& spaceY, int& numberOfX, int& numberOfY,float& _zoom, ImVec2& _offsetxy, int& alphacolor);
	bool writeParametersIntoFilesystem();
	bool scanParametersFromBuffer();


	bool setRectParameters(bool _ashowFilenameFlag, bool _ashowPositionFlag, bool _ashowSizeFlag, bool _selectedflag, bool _ahilightedflag, ImVec2 _begin, ImVec2 _end);
	bool getRectParameters(bool& _ashowFilenameFlag, bool& _ashowPositionFlag, bool& _ashowSizeFlag, bool& _selectedflag, bool& _ahilightedflag, ImVec2& _begin, ImVec2& _end);
	bool writeRectParametersIntoFilesystem();
	bool readRectParametersFromFilesystem();


	void getDefsParameters(char *&prefilename, char *&postfilename);


	char *pCS(char *input);
	void strtokimplementation(std::stack<char *> &nmyStack, std::string &szi, const char *tokenString);
	void strtokimplementation2(std::vector<char *> &nmyVector, std::string &szi, const char *tokenString);
	std::string& rtrim(std::string& s);
	std::string& ltrim(std::string& s);
	std::string& trim(std::string& s);

	int fromBoolToInteger(bool);
	bool fromIntegerToBool(int);


	// * * * * * * * * * * //
	// * * * * * * * * * * //
	void set_ini_filename(char *filename);
	char *get_ini_filename();
	char *get_ini_base_filename();
	bool open_ini_file_for_write();
	bool open_ini_file_for_read();
	bool close_ini_file();
	void  readIniLineStrings();
	char *getIniLineString1();
	char *getIniLineString2();
	char *getIniLineString3();
	char *getIniLineString4();
	void writeIniLineString(char *inlinestring, char *inlinestring2, char *inlinestring3, char *inlinestring4);
	// * * * * * * * * * * //
	// * * * * * * * * * * //


private:

	FILE *mFop = nullptr;
	bool debuggy = false;
	int		apx1 = 0, apy1 = 0, awidth = 0, aheight = 0, aspaceX = 0, aspaceY = 0, anumberOfX = 0, anumberOfY = 0;
	float	azoom = 1.0f;
	ImVec2	aoffsetxy = {0,0};
	int		aalphacolor = 0;

	 bool aselectedflag  = false;
	 bool ahilightedflag = false;
	 ImVec2 abegin;
	 ImVec2 aend;

	 char *aprefilename = nullptr;
	 char *apostfilename = nullptr;

	bool ashowFilenameFlag = false;
	bool ashowPositionFlag = false;
	bool ashowSizeFlag     = false;

	char myFilename[256];
	char myBuffer[256];

	// * * * * * * * * * * //
	char myIniBaseFilename[256];
	char myIniFilename[256];
	char myIniBuffer[256];
	char myIniBufferPath[256];
	char myIniBufferLine3[256];
	char myIniBufferLine4[256];
	FILE *mFopIni = nullptr;
	// * * * * * * * * * * //


};





#endif