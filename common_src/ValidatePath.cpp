// used from SNAF at 2020.08.20
// 2020.06.14.


#include "ValidatePath.h"
//#include <stdio.h>
//#include <iostream>
//#include <cstring>

//#include <cctype>
#include <string>
#include <direct.h>	// for _mkdir(...)
//#include <algorithm>
#include <stack> 


#define LINUX_PATH_SEPARATOR '/'
#define WINDOWS_PATH_SEPARATOR '\\'
#ifdef _WIN32
#define DEFAULT_PATH_SEPARATOR  WINDOWS_PATH_SEPARATOR
#else 
#define DEFAULT_PATH_SEPARATOR  LINUX_PATH_SEPARATOR
#endif


ValidatePath::ValidatePath()
{
	debugOff();
	reset();
}
void ValidatePath::reset(){
	errorstring = "";
	returnedFileNameOnly = "";
	returnedFilePathOnly = "";
	returnedFilePathAndName = "";
	returnedFileNameOnly_cstr = nullptr;
	returnedFilePathOnly_cstr = nullptr;
	returnedFilePathAndName_cstr = nullptr;

}
ValidatePath::~ValidatePath()
{
	reset();
}

void ValidatePath::debugOn() { debuggy = true; }
void ValidatePath::debugOff() { debuggy = false; }
bool ValidatePath::isDebug() { return debuggy; }

// ******************************************* //
// ******************************************* //
// ******************************************* //
// ******************************************* //
// ******************************************* //
//#include <sstream>
//#include <cstddef>        // std::size_t



bool ValidatePath::scanString(char *stringz, bool AutoCreateDirectory)
{
	returnedFilePathAndName = "";
	int len = strlen(stringz);
	if (len == 0) {		// if empty string in , empty string out
	//	fullPathString = "";
		return true;
	}
	if (len > 255) {
		errorstring = "Stringlength is > 255!";
		return false;
	}
	if(isDebug())
		std::cout << "Input string is:   '" << stringz << "'\n";
	char *newline = pCS(stringz);
	if (std::strrchr(newline, '.') == nullptr) {
		errorstring = "There is no '.' in filename!";
		return false;
	}
	if (std::strrchr(newline, ':') != nullptr) {
		errorstring = "There is a colon ':' in filename!";
		return false;
	}
/*
	if (std::strrchr(newline, '*') != nullptr) {
		errorstring = "There is a star '*' in filename!";
		return false;
	}
*/
	std::string si = newline;
	std::string sz = trim(si);
	si.clear();
	if (isDebug())
		std::cout << "trimmed string is: '" << sz << "'\n";

	int stringsize = sz.size();
	char pcc  = sz.at( stringsize-1 );
	char psep = sz.at(0);
	if ( psep == LINUX_PATH_SEPARATOR || psep == WINDOWS_PATH_SEPARATOR ) {
		errorstring = "First character is path separator. (root access not allowed)";
	return false;
	}
	if (psep == '.')
	{
		errorstring = ". dot file is not allowed";
		return false;
	}
	// ------------------------- //
	if (pcc == '.') {
		errorstring = "Last character in filename is DOT '.'";
		return false;
	}
	if ( pcc == LINUX_PATH_SEPARATOR || pcc == WINDOWS_PATH_SEPARATOR ) {
		errorstring = "Last character in filename is path Separator";
		return false;
	}
	// ------------------------- //
	char *strl1 = std::strrchr(newline, LINUX_PATH_SEPARATOR);
	char *strl2 = std::strrchr(newline, WINDOWS_PATH_SEPARATOR);
	if (strl1 == nullptr && strl2 == nullptr) { // no path separator found
		// single file
		if (isDebug())
			std::cout << "single file is:    '" << sz << "'\n";
		// ------------------------------------------------------- //
		// set filename
		returnedFilePathAndName = sz;		
		returnedFileNameOnly = sz;
		returnedFilePathOnly = "";
		returnedFileNameOnly_cstr = (char *)returnedFileNameOnly.c_str();
		returnedFilePathOnly_cstr = (char *)returnedFilePathOnly.c_str();
		returnedFilePathAndName_cstr  = (char *)sz.c_str();
		return true;
		// ------------------------------------------------------- //
	}
	else {	// path separator is found
	   // path + file
		if (isDebug())
			std::cout << "path + file is:    '" << sz << "'\n";
		// ------------------------------------------------------- //
		//   fix path separator stuff.
		// ------------------------------------------------------- //
		std::size_t found = sz.find_first_of("\\/");
		while (found != std::string::npos)
		{
			sz[found] = DEFAULT_PATH_SEPARATOR;
			found = sz.find_first_of("\\/", found + 1);
		}
		if (isDebug())
			std::cout << sz << '\n';
		// ------------------------------------------------------- //
		// stack implementation
		// sz is now the string
		std::stack< char *> myStack;
		char tokenString[3] = { WINDOWS_PATH_SEPARATOR, LINUX_PATH_SEPARATOR, '\0' };
		strtokimplementation(myStack, sz, &tokenString[0]);
		// ------------------------------------------------------- //
		//  stack implementation
		// ------------------------------------------------------- //
		std::string myFileName;
		std::stack< std::string > mySecondStack;
		int stackSize = myStack.size();
		if (isDebug())
			std::cout << "stackSize = " << stackSize << '\n';
		if (stackSize == 0)
		{
			if (isDebug()) {
				errorstring = "Stack is zero!";
				return false;
			}
		}
		for (int i = 0;i < stackSize;i++) {
			char *css = myStack.top();
			myStack.pop();
			std::string sinput = css;
			std::string soutput = trim(sinput);
			if (isDebug()) {
				std::cout << "Stack Element = " << css << "     ";
				std::cout << "trimmed string is: '" << soutput << "'\n";
			}
			mySecondStack.push(soutput);
			if (i == 0) {
				myFileName = mySecondStack.top();
			}
			sinput.clear();
			delete css;
		}

		if (myFileName.at(0) == '.') {
			errorstring = ". dot file is not allowed";
			return false;
		}

		char *cstr = new char[myFileName.length() + 1];
		std::strcpy(cstr, myFileName.c_str());		// cstr now contains a c-string copy of myFileName
		strl1 = std::strrchr(cstr, '.');
		if (strl1 == nullptr) {
			errorstring = "Filename must have an extension, there's no dot '.'";
			return false;
		}

		if (isDebug())
		{
			std::cout << "myFileName = '" << myFileName << "'\n";
			std::cout << "second Stack: \n";
		}


		returnedFilePathOnly = "";
		std::string newFilePath;
		newFilePath.clear();
		std::string mys;
		int path_depth = 0;
		for (int i = 0;i < stackSize;i++) {
			mys = mySecondStack.top();
			mySecondStack.pop();
			if (isDebug())
				std::cout << "Stack Element = " << mys << "\n";

			//			// TODO, make it possible to scan directories, one by one
			//				mydirectories.push_back(  mys  );

			if (mys == "..") {
				path_depth--;
			}
			else {
				path_depth++;
			}
			if (path_depth < 0) {
				errorstring = "Negative path depth!";
				newFilePath.clear();
				mys.clear();
				sz.clear();
				return false;
			}


			if (i == (stackSize - 1)) {	// last element
				returnedFilePathOnly = newFilePath;
			}
			newFilePath += mys;
			if (i != (stackSize - 1)) {
				newFilePath += DEFAULT_PATH_SEPARATOR;
				mys.clear();


				if (AutoCreateDirectory == true) {
					/* --- */
//					std::cout << "newFilePath: (" << newFilePath << ")\n";
					_mkdir(newFilePath.c_str());	// DONE: make directories
					/* --- */
				}

			}
			else {
				// last element, filename
				int stringsize0 = mys.size();
				//std::cout << "[[++]] mys filename only = " << mys << "\n";
				if (mys.at((stringsize0 - 1)) == '.') {
					errorstring = "Last character in filename is DOT '.'";
					newFilePath.clear();
					mys.clear();
					sz.clear();
					return false;
				}
				break;
				// ------------------------------------------- //
			}
		}

		if(isDebug()){
			std::cout << "Filename only = '" << myFileName << "'\n";
			std::cout << "File Path only = " << returnedFilePathOnly << "\n";// file path alone
		}
		returnedFileNameOnly = myFileName;
		returnedFilePathOnly = returnedFilePathOnly;
		returnedFileNameOnly_cstr = (char *) returnedFileNameOnly.c_str();
		returnedFilePathOnly_cstr = (char *) returnedFilePathOnly.c_str();
		returnedFilePathAndName_cstr  = (char *) newFilePath.c_str();
		sz.clear();
		returnedFilePathAndName = newFilePath;
		myFileName.clear();
		if (isDebug())
			std::cout << "new Path + File = '" << newFilePath << "'\n";
	}
	return true;
}


char *ValidatePath::getFileNameOnly()
{
	return returnedFileNameOnly_cstr;
}
char *ValidatePath::getFilePathOnly()
{
	return returnedFilePathOnly_cstr;
}
char *ValidatePath::getFileNameAndPath() 
{
	return returnedFilePathAndName_cstr;
}


const char *ValidatePath::getErrorString() 
{
	return errorstring;
}

std::string ValidatePath::getPathString() {
	return returnedFilePathAndName;
}
char *ValidatePath::getPathStringChar() {
	return pCS(returnedFilePathAndName);
}


void ValidatePath::strtokimplementation(std::stack<char *> &nmyStack, std::string &szi, const char *tokenString)
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

/** make an allocated copy of string
*/
char *ValidatePath::pCS(char *input) {
	int size = strlen(input);
	char *output = new char[size + 7];
	int i = 0;
	for (i = 0; i < size; i++) {
		output[i] = input[i];
	}
	output[i] = 0;
	return output;
}
// ******************************************* //
char *ValidatePath::pCS(std::string input) {
	int size = input.size();
	char *output = new char[size + 7];
	int i = 0;
	for (i = 0; i < size; i++) {
		output[i] = input[i];
	}
	output[i] = 0;
	return output;
}





//const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& ValidatePath::rtrim(std::string& s)
{
	const char* t = " \t\n\r\f\v";
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
inline std::string& ValidatePath::ltrim(std::string& s)
{
	const char* t = " \t\n\r\f\v";
	s.erase(0, s.find_first_not_of(t));
	return s;
}
// trim from both ends of string (right then left)
inline std::string& ValidatePath::trim(std::string& s)
{
	return ltrim(rtrim(s));
}

char *ValidatePath::validateAndRemakeThisPath(char *inputString, bool AutoCreateDirectories ) {
	bool bretval = this->scanString(inputString, AutoCreateDirectories);
	if (bretval) {
		return this->getPathStringChar(); // SUCCESS
	}
	else {
		const char  *errs = this->getErrorString();
		std::cout << "ERROR,Path Invalid,  " << (const char *)errs << ", exit (-1)\n";
		exit(-1);
	}
}

char *ValidatePath::validateAndRemakeThisPath(char *inputString) {
	bool bretval = this->scanString(inputString, false);
	if (bretval) {
		return this->getPathStringChar(); // SUCCESS
	}else{
		const char  *errs = this->getErrorString();
		std::cout << "ERROR,Path Invalid,  " << (const char *)errs << ", exit (-1)\n";
		exit(-1);
	}
}



