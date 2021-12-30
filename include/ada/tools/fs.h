#pragma once

#include <string>

#include "ada/tools/list.h"

namespace ada {

bool urlExists(const std::string& _filename);
bool haveExt(const std::string& _filename, const std::string& _ext);
std::string getExt(const std::string& _filename);

std::string loadSrcFrom(const std::string& _path);
std::string loadSrcFrom(const std::string& _path, const List& _include_folders, List *_dependencies);

bool loadSrcFrom(const std::string& _path, std::string *_into);
bool loadSrcFrom(const std::string& _filename, std::string *_into, const List& _include_folders, List *_dependencies);

std::string getBaseDir (const std::string& filepath);
std::string getAbsPath (const std::string& _filename);
std::string urlResolve(const std::string& _filename, const std::string& _pwd, const List& _include_folders);
std::vector<std::string> glob(const std::string& _pattern);

}