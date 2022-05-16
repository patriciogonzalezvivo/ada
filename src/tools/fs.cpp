#include "ada/tools/fs.h"
#include "ada/tools/text.h"


#include <iostream>     // cout
#include <fstream>      // File
#include <iterator>     // std::back_inserter
#include <algorithm>    // std::unique
#include <sys/stat.h>

#ifdef _WIN32
#include <stdlib.h>
#include <stdio.h>
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

std::string urlResolve(const std::string& _path, const std::string& _pwd, const List &_include_folders) {
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

bool alreadyInclude(const std::string &_path, List *_dependencies) {
    for (unsigned int i = 0; i < _dependencies->size(); i++) {
        if ( _path == (*_dependencies)[i]) {
            return true;
        }
    }
    return false;
}

std::string loadGlslFrom(const std::string& _path) {
    const std::vector<std::string> folders;
    List deps;
    return loadGlslFrom(_path, folders, &deps);
}

std::string loadGlslFrom(const std::string &_path, const List& _include_folders, List *_dependencies) {
    std::string str = "";
    loadGlslFrom(_path, &str, _include_folders, _dependencies);
    return str;
}

bool loadGlslFrom(const std::string &_path, std::string *_into) {
    const std::vector<std::string> folders;
    List deps;

    _into->clear();
    return loadGlslFrom(_path, _into, folders, &deps);
}

bool loadGlslFrom(const std::string &_path, std::string *_into, const std::vector<std::string> &_include_folders, List *_dependencies) {
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


/**
 *  Implementation of base64 encoding/decoding.
 *  by Jan-Henrik Haukeland, <hauk@tildeslash.com>
 *  version base64.c,v 1.19 2007/07/25 12:54:31 hauk Exp $
 */


/**
 * Base64 encode one byte
 */
static char encode(unsigned char u) {
    if (u < 26) return 'A' + u;
    if (u < 52) return 'a' + (u - 26);
    if (u < 62) return '0' + (u - 52);
    if (u == 62) return '+';
    return '/';
}

/**
 * Decode a base64 character
 */
static unsigned char decode(char c) {
    if (c >= 'A' && c <= 'Z') return (c - 'A');
    if (c >= 'a' && c <= 'z') return (c - 'a' + 26);
    if (c >= '0' && c <= '9') return (c - '0' + 52);
    if (c == '-') return 62;
    return 63;
}

/**
 * Return TRUE if 'c' is a valid base64 character, otherwise FALSE
 */
static int is_base64(char c) {
    // return 1;
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || (c == '+') ||
            (c == '/') || (c == '=')) {
        return 1;
    }
    return 0;
}

/**
 * Base64 encode and return size data in 'src'. The caller must free the
 * returned string.
 * @param src The data to be base64 encode
 * @param size The size of the data in src
 * @return encoded string otherwise NULL
 */
char *encodeBase64(unsigned char *_src, int _size) {
    int i;
    char *out, *p;

    if (!_src)
        return 0;

    if (!_size)
        _size = strlen((char *) _src);

    out = (char*) calloc(sizeof (char), _size * 4 / 3 + 4);
    p = out;

    for (i = 0; i < _size; i += 3) {
        unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0;
        b1 = _src[i];

        if (i + 1 < _size)
            b2 = _src[i + 1];

        if (i + 2 < _size)
            b3 = _src[i + 2];

        b4 = b1 >> 2;
        b5 = ((b1 & 0x3) << 4) | (b2 >> 4);
        b6 = ((b2 & 0xf) << 2) | (b3 >> 6);
        b7 = b3 & 0x3f;

        *p++ = encode(b4);
        *p++ = encode(b5);

        if (i + 1 < _size)
            *p++ = encode(b6);
        else
            *p++ = '=';

        if (i + 2 < _size)
            *p++ = encode(b7);
        else 
            *p++ = '=';
    }

    return out;

}

/**
 * Decode the base64 encoded string 'src' into the memory pointed to by
 * 'dest'. The dest buffer is <b>not</b> NUL terminated.
 * @param _src A base64 encoded string.
 * @param _to Pointer to memory for holding the decoded string.
 * Must be large enough to recieve the decoded string.
 * @return TRUE (the length of the decoded string) if decode
 * succeeded otherwise FALSE.
 */
int decodeBase64(const char *_src, unsigned char *_to) {
    if (_src && *_src) {

        unsigned char *p = _to;
        int k, l = strlen(_src) + 1;
        unsigned char *buf = (unsigned char*) calloc(sizeof (unsigned char), l);


        /* Ignore non base64 chars as per the POSIX standard */
        for (k = 0, l = 0; _src[k]; k++)
            if (is_base64(_src[k]))
                buf[l++] = _src[k];
            
        for (k = 0; k < l; k += 4) {
            char c1 = 'A', c2 = 'A', c3 = 'A', c4 = 'A';
            unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0;

            c1 = buf[k];

            if (k + 1 < l)
                c2 = buf[k + 1];
            

            if (k + 2 < l)
                c3 = buf[k + 2];

            if (k + 3 < l)
                c4 = buf[k + 3];

            b1 = decode(c1);
            b2 = decode(c2);
            b3 = decode(c3);
            b4 = decode(c4);

            *p++ = ((b1 << 2) | (b2 >> 4));

            if (c3 != '=')
                *p++ = (((b2 & 0xf) << 4) | (b3 >> 2));

            if (c4 != '=')
                *p++ = (((b3 & 0x3) << 6) | b4);
        }

        free(buf);
        return (p - _to);
    }

    return 0;
}

}