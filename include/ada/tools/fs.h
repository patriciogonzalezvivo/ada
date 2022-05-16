#pragma once

#include <string>

#include "ada/tools/list.h"

namespace ada {

// files
bool urlExists(const std::string& _filename);
bool haveExt(const std::string& _filename, const std::string& _ext);
std::string getExt(const std::string& _filename);

// paths
std::string getBaseDir (const std::string& filepath);
std::string getAbsPath (const std::string& _filename);
std::string urlResolve(const std::string& _filename, const std::string& _pwd, const List& _include_folders);
std::vector<std::string> glob(const std::string& _pattern);

// GLSL source code files
std::string loadGlslFrom(const std::string& _path);
std::string loadGlslFrom(const std::string& _path, const List& _include_folders, List *_dependencies);
bool loadGlslFrom(const std::string& _path, std::string *_into);
bool loadGlslFrom(const std::string& _filename, std::string *_into, const List& _include_folders, List *_dependencies);

// Binnaries
char* encodeBase64(unsigned char *_src, int _size); 
int   decodeBase64(const char *_src, unsigned char *_to);

}