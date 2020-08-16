#pragma once

#include <string>

#include "ada/tools/list.h"

namespace ada {

bool urlExists(const std::string& _filename);
bool haveExt(const std::string& _filename, const std::string& _ext);
std::string getExt(const std::string& _filename);

bool loadFromPath(const std::string& _filename, std::string *_into, const List& _include_folders, List *_dependencies);

std::string getBaseDir (const std::string& filepath);
std::string getAbsPath (const std::string& _filename);
std::string urlResolve(const std::string& _filename, const std::string& _pwd, const List& _include_folders);

}