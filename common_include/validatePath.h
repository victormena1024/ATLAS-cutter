#ifndef VALIDATEPATH_H
#define VALIDATEPATH_H
// 2020.06.14.
/**
 * scan a file path string and validate it.
 *  
 *
 *
 *
*/

//#include <cstdio>
//#include <cstring>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

//namespace val {

class ValidatePath
{
public:
	void debugOn();
	void debugOff();
	bool isDebug();
	// -----------
	ValidatePath();
	~ValidatePath();
	void reset();
	// -----------
	// return true on success, else false
	bool scanString(char *string,bool AutoCreateDirectory ); 
	// -----------
	// main function
	char *validateAndRemakeThisPath(char *inputString);
	char *validateAndRemakeThisPath(char *inputString, bool AutoCreateDirectories);
	// -----------
	//	char * getReturnedFilePathOnly();
	char *getFileNameOnly();
	char *getFilePathOnly();
	char *getFileNameAndPath();
	// ------------------ //

   
	// valid string on error
	const	char	*getErrorString();
	// valid string on success
	std::string		getPathString();
	char *			getPathStringChar();


private:
	const char *errorstring;
	bool debuggy;
	// -----------
	char *pCS(char *input);
	// -----------
	char *pCS(std::string input);
	// -----------
	void strtokimplementation(std::stack<char *> &nmyStack, std::string &szi, const char *tokenString);
	// -----------
	inline std::string& rtrim(std::string& s );
	inline std::string& ltrim(std::string& s );
	inline std::string&  trim(std::string& s );
	// -----------

	char * returnedFileNameOnly_cstr = nullptr;
	char * returnedFilePathOnly_cstr = nullptr;
	char * returnedFilePathAndName_cstr = nullptr;
	std::string returnedFileNameOnly;
	std::string returnedFilePathOnly;
	std::string returnedFilePathAndName;
		

};
//}// namespace


#endif