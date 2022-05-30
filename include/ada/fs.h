#pragma once

#include "string.h"

namespace ada {

// files
bool urlExists(const std::string& _filename);
bool haveExt(const std::string& _filename, const std::string& _ext);
std::string getExt(const std::string& _filename);

// paths
std::string getBaseDir (const std::string& filepath);
std::string getAbsPath (const std::string& _filename);
std::string urlResolve(const std::string& _filename, const std::string& _pwd, const StringList& _include_folders);
std::vector<std::string> glob(const std::string& _pattern);

// GLSL source code files
std::string loadGlslFrom(const std::string& _path);
std::string loadGlslFrom(const std::string& _path, const StringList& _include_folders, StringList *_dependencies);
bool loadGlslFrom(const std::string& _path, std::string *_into);
bool loadGlslFrom(const std::string& _filename, std::string *_into, const StringList& _include_folders, StringList *_dependencies);

// Binnaries
std::string encodeBase64(const unsigned char* _src, size_t _size);
size_t decodeBase64(const std::string& _src, unsigned char *_to);

}