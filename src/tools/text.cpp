#include "ada/tools/text.h"

#include <algorithm>
#include <regex>

namespace ada {

std::string toLower(const std::string& _string) {
    std::string std = _string;
    for (int i = 0; _string[i]; i++) {
        std[i] = tolower(_string[i]);
    }
    return std;
}

std::string toUpper(const std::string& _string) {
    std::string std = _string;
    for (int i = 0; _string[i]; i++) {
        std[i] = toupper(_string[i]);
    }
    return std;
}

std::string toUnderscore(const std::string& _string){
    std::string std = _string;
    std::replace(std.begin(), std.end(), ' ', '_');
    return std;
}

std::string purifyString(const std::string& _string) {
    std::string std = _string;
    for (std::string::iterator it = std.begin(), itEnd = std.end(); it!=itEnd; ++it) {
        if (static_cast<uint32_t>(*it) < 32 || 
            static_cast<uint32_t>(*it) > 127 || 
            *it == '.' ||
            *it == '-' ||
            *it == '\\'||
            *it == '/' ) {
            (*it) = '_';
        }
    }
    return std;
}

bool isInt(const std::string& _string) {
  return _string.find_first_not_of( "0123456789-" ) == std::string::npos;
}

bool isDigit(const std::string& _string) {
  return _string.find_first_not_of( "0123456789" ) == std::string::npos;
}

bool isNumber(const std::string& _string) {
  return _string.find_first_not_of( "-0123456789." ) == std::string::npos;
}

bool isFloat(const std::string& _string) {
    std::istringstream iss(_string);
    float dummy;
    iss >> std::skipws >> dummy;
    // if it's a number
    if (iss && iss.eof()) {
        std::string::const_iterator it = _string.begin();
        while (it != _string.end()) {
            if (*it == '.') {
                // That contain a .
                return true;
            }
            ++it;
        }
    }
    return false;
}

//---------------------------------------- Conversions
int toInt(const std::string& _string) {
    int x = 0;
    std::istringstream cur(_string);
    cur >> x;
    return x;
}

float toFloat(const std::string& _string) {
    float x = 0;
    std::istringstream cur(_string);
    cur >> x;
    return x;
}

double toDouble(const std::string& _string) {
    double x = 0;
    std::istringstream cur(_string);
    cur >> x;
    return x;
}

bool toBool(const std::string& _string) {
    std::string lower = toLower(_string);

    if (lower == "true")
        return true;
    
    if (lower == "false")
        return false;

    bool x = false;
    std::istringstream cur(lower);
    cur >> x;
    return x;
}

char toChar(const std::string& _string) {
    char x = '\0';
    std::istringstream cur(_string);
    cur >> x;
    return x;
}

std::string toString(bool _bool) {
    std::ostringstream strStream;
    strStream << (_bool?"true":"false") ;
    return strStream.str();
}

std::string toString(const glm::vec2& _vec, char _sep) {
    std::ostringstream strStream;
    strStream << std::fixed << std::setprecision(3) << _vec.x << _sep;
    strStream << std::fixed << std::setprecision(3) << _vec.y;
    return strStream.str();
}

std::string toString(const glm::vec3& _vec, char _sep) {
    std::ostringstream strStream;
    strStream << std::fixed << std::setprecision(3) << _vec.x << _sep;
    strStream << std::fixed << std::setprecision(3) << _vec.y << _sep; 
    strStream << std::fixed << std::setprecision(3) << _vec.z;
    return strStream.str();
}

std::string toString(const glm::vec4& _vec, char _sep) {
    std::ostringstream strStream;
    strStream << std::fixed << std::setprecision(3) << _vec.x << _sep;
    strStream << std::fixed << std::setprecision(3) << _vec.y << _sep;
    strStream << std::fixed << std::setprecision(3) << _vec.z << _sep; 
    strStream << std::fixed << std::setprecision(3) << _vec.w;
    return strStream.str();
}

// std::string toString(const glm::quat& _quat, char _sep) {
//     std::ostringstream strStream;
//     strStream << std::fixed << std::setprecision(3) << _quat.a << _sep;
//     strStream << std::fixed << std::setprecision(3) << _quat.x << _sep;
//     strStream << std::fixed << std::setprecision(3) << _quat.y << _sep; 
//     strStream << std::fixed << std::setprecision(3) << _quat.z;
//     return strStream.str();
// }

std::string toString(const glm::mat4& _mat, char _sep) {
    std::ostringstream strStream;
    strStream << _mat[0] << _sep << _mat[4] << _sep << _mat[8]  << _sep << _mat[12] << '\n';
    strStream << _mat[1] << _sep << _mat[5] << _sep << _mat[9]  << _sep << _mat[13] << '\n';
    strStream << _mat[2] << _sep << _mat[6] << _sep << _mat[10] << _sep << _mat[14] << '\n'; 
    strStream << _mat[3] << _sep << _mat[7] << _sep << _mat[11] << _sep << _mat[15] << '\n';
    return strStream.str();
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << vec.x << ", " << vec.y << ", " << vec.z; 
    return os;
}

std::istream& operator>>(std::istream& is, glm::vec3& vec) {
    is >> vec.x;
    is.ignore(2);
    is >> vec.y;
    is.ignore(2);
    is >> vec.z;
    return is;
}

std::vector<std::string> split(const std::string& _string, char _sep, bool _tolerate_empty) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = _string.find(_sep, start)) != std::string::npos) {
        if (end != start || _tolerate_empty) {
          tokens.push_back(_string.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start || _tolerate_empty) {
       tokens.push_back(_string.substr(start));
    }
    return tokens;
}

bool beginsWith(const std::string& _stringA, const std::string& _stringB) {
    for (uint32_t i = 0; i < _stringB.size(); i++) {
        if (_stringB[i] != _stringA[i]) {
            return false;
        }
    }
    return true;
}

std::string getLineNumber(const std::string& _source, unsigned _lineNumber) {
    std::string delimiter = "\n";
    std::string::const_iterator substart = _source.begin(), subend;

    unsigned index = 1;
    while (true) {
        subend = search(substart, _source.end(), delimiter.begin(), delimiter.end());
        std::string sub(substart, subend);

        if (index == _lineNumber) {
            return sub;
        }
        index++;

        if (subend == _source.end()) {
            break;
        }

        substart = subend + delimiter.size();
    }

    return "NOT FOUND";
}

std::string getVersion(const std::string& _src, size_t& _versionNumber) {
    bool srcVersionFound = _src.substr(0, 8) == "#version"; 
    std::string srcVersion = "";

    if (srcVersionFound) {
        // split _src into srcVersion and srcBody
        std::istringstream srcIss(_src);

        // the version line can be read without checking the result of getline(), srcVersionFound == true implies this
        std::getline(srcIss, srcVersion);

        std::istringstream versionIss(srcVersion);
        std::string dataRead;
        versionIss >> dataRead;             // consume the "#version" string which is guaranteed to be there
        versionIss >> _versionNumber;    // try to read the next token and convert it to a number

    }
    else
        _versionNumber = 100;

    return srcVersion;
}

}