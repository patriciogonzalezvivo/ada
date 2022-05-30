#include "ada/fs.h"
#include "ada/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>     // cout
#include <fstream>      // File
#include <iterator>     // std::back_inserter
#include <algorithm>    // std::unique
#include <sys/stat.h>

#ifdef _WIN32
#include <errno.h>
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else
#include "glob.h"
#endif


namespace ada {

bool haveExt(const std::string& _file, const std::string& _ext){
    return _file.find( "." + _ext) != std::string::npos;
}

bool urlExists(const std::string& _name) {
    struct stat buffer;
    return (stat (_name.c_str(), &buffer) == 0);
}

std::string getExt(const std::string& _filename) {
    if (_filename.find_last_of(".") != std::string::npos)
        return _filename.substr(_filename.find_last_of(".") + 1);
    return "";
}

std::string getBaseDir(const std::string& filepath) {
    std::string base_dir = "";

    if (filepath.find_last_of("/\\") != std::string::npos)
        base_dir =  filepath.substr(0, filepath.find_last_of("/\\"));
    else 
        base_dir = ".";
    
#ifdef _WIN32
    base_dir += "\\";
#else
    base_dir += "/";
#endif

    return base_dir;
}

#if defined (_WIN32)
const char* realpath(const char* str, void*)
{
    return str;
}
#endif 
std::string getAbsPath(const std::string& _path) {
    std::string abs_path = realpath(_path.c_str(), NULL);
    std::size_t found = abs_path.find_last_of("\\/");
    if (found) return abs_path.substr(0, found);
    else return "";
}

std::string urlResolve(const std::string& _path, const std::string& _pwd, const StringList &_include_folders) {
    std::string url = _pwd +'/'+ _path;

    // If the path is not in the same directory
    if (urlExists(url)) 
        return realpath(url.c_str(), NULL);

    // .. search on the include path
    else {
        for ( uint32_t i = 0; i < _include_folders.size(); i++) {
            std::string new_path = _include_folders[i] + "/" + _path;
            if (urlExists(new_path)) 
                return realpath(new_path.c_str(), NULL);
        }
        return _path;
    }
}

bool extractDependency(const std::string &_line, std::string *_dependency) {
    // Search for ocurences of #include or #pargma include (OF)
    if (_line.find("#include ") == 0 || _line.find("#pragma include ") == 0) {
        unsigned begin = _line.find_first_of("\"");
        unsigned end = _line.find_last_of("\"");
        if (begin != end) {
            (*_dependency) = _line.substr(begin+1,end-begin-1);
            return true;
        }
    }
    return false;
}

bool alreadyInclude(const std::string &_path, StringList *_dependencies) {
    for (unsigned int i = 0; i < _dependencies->size(); i++) {
        if ( _path == (*_dependencies)[i]) {
            return true;
        }
    }
    return false;
}

std::string loadGlslFrom(const std::string& _path) {
    const std::vector<std::string> folders;
    StringList deps;
    return loadGlslFrom(_path, folders, &deps);
}

std::string loadGlslFrom(const std::string &_path, const StringList& _include_folders, StringList *_dependencies) {
    std::string str = "";
    loadGlslFrom(_path, &str, _include_folders, _dependencies);
    return str;
}

bool loadGlslFrom(const std::string &_path, std::string *_into) {
    const std::vector<std::string> folders;
    StringList deps;

    _into->clear();
    return loadGlslFrom(_path, _into, folders, &deps);
}

bool loadGlslFrom(const std::string &_path, std::string *_into, const std::vector<std::string> &_include_folders, StringList *_dependencies) {
    std::ifstream file;
    file.open(_path.c_str());

    // Skip if it's already open
    if (!file.is_open()) 
        return false;

    // Get absolute home folder
    std::string original_path = getAbsPath(_path);

    std::string line;
    std::string dependency;
    std::string newBuffer;
    while (!file.eof()) {
        dependency = "";
        getline(file, line);

        if (extractDependency(line, &dependency)) {
            dependency = urlResolve(dependency, original_path, _include_folders);
            newBuffer = "";
            if (loadGlslFrom(dependency, &newBuffer, _include_folders, _dependencies)) {
                if (!alreadyInclude(dependency, _dependencies)) {
                    // Insert the content of the dependency
                    (*_into) += "\n" + newBuffer + "\n";

                    // Add dependency to dependency list
                    _dependencies->push_back(dependency);
                }
            }
            else {
                std::cerr << "Error: " << dependency << " not found at " << original_path << std::endl;
            }
        }
        else {
            (*_into) += line + "\n";
        }
    }

    file.close();
    return true;
}


std::vector<std::string> glob(const std::string& _pattern) {
    std::vector<std::string> files;

#if defined(_WIN32)
    int err = 0;
    WIN32_FIND_DATAA finddata;
    HANDLE hfindfile = FindFirstFileA(_pattern.c_str(), &finddata);

    if (hfindfile != INVALID_HANDLE_VALUE) {
        do {
            files.push_back(std::string(finddata.cFileName));
        } while (FindNextFileA(hfindfile, &finddata));
        FindClose(hfindfile);
    }

#else
    std::vector<std::string> folders = split(_pattern, '/', true);
    std::string folder = "";

    for (size_t i = 0; i < folders.size() - 1; i++)
        folder += folders[i] + "/";

    glob::glob glob(_pattern.c_str());
    while (glob) {
        files.push_back( folder + glob.current_match() );
        glob.next();
    }

#endif
    return files;
}


static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

std::string encodeBase64(const unsigned char* _src, size_t _size) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (_size--) {
        char_array_3[i++] = *(_src++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }

    return ret;
}

size_t decodeBase64(const std::string& _src, unsigned char *_to) {
    size_t in_len = _src.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    unsigned char *p = _to;

    while (in_len-- && ( _src[in_] != '=') && is_base64(_src[in_])) {
        char_array_4[i++] = _src[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                *p++ = char_array_3[i] ;
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) 
            *p++ = char_array_3[j] ;
    }

    return (p - _to);
}

}