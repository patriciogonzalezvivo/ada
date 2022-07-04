#pragma once

#include <map>
#include <string>

#include "glm/glm.hpp"

namespace ada {

typedef std::map<std::string,std::string>                   DefinesMap;
typedef std::map<std::string,std::string>::iterator         DefinesMap_it;
typedef std::map<std::string,std::string>::const_iterator   DefinesMap_cit;

class HaveDefines {
public:
    HaveDefines();
    virtual ~HaveDefines();

    void    addDefine( const std::string &_define, int _n );
    void    addDefine( const std::string &_define, float _n );
    void    addDefine( const std::string &_define, double _n );
    void    addDefine( const std::string &_define, glm::vec2 _v );
    void    addDefine( const std::string &_define, glm::vec3 _v );
    void    addDefine( const std::string &_define, glm::vec4 _v );
    void    addDefine( const std::string& _define, float* _value, int _nValues);
    void    addDefine( const std::string& _define, double* _value, int _nValues);
    void    addDefine( const std::string &_define, const std::string &_value = "");
    void    delDefine( const std::string &_define );

    void    mergeDefines( HaveDefines *_haveDefines );
    void    mergeDefines( const HaveDefines *_haveDefines );
    void    mergeDefines( const DefinesMap &_defines );
    void    replaceDefines( const DefinesMap &_defines );
    
    void    printDefines();

protected:
    DefinesMap  m_defines;
    bool        m_defineChange;
};

}