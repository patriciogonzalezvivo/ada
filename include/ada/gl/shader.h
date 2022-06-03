#pragma once

#include <memory>
#include <string>

#include "gl.h"
#include "fbo.h"
#include "texture.h"
#include "defines.h"
#include "textureCube.h"

#include "glm/glm.hpp"

// tmp
#include <iostream>

namespace ada {

class Shader : public HaveDefines {
public:
    Shader();
    // Shader(const Shader&) { std::cout << "copy-constructed\n"; }
    // Shader(Shader&&) { std::cout << "move-constructed\n"; }
    virtual ~Shader();

    void    operator = (const Shader &_parent );

    void    use();
    bool    load(const std::string& _fragmentSrc, const std::string& _vertexSrc, bool _verbose = false, bool _error_screen = true);
    bool    reload(bool _verbose = false);
    
    const   GLuint  getProgram() const { return m_program; };
    const   GLuint  getFragmentShader() const { return m_fragmentShader; };
    const   GLuint  getVertexShader() const { return m_vertexShader; };
    const   GLint   getAttribLocation(const std::string& _attribute) const;

    const std::string& getFragmentSource() const { return m_fragmentSource; };
    const std::string& getVertexSource() const { return m_vertexSource; };

    bool    isInUse() const;
    bool    isLoaded() const;
   
    void    setUniform(const std::string& _name, int _x);
    void    setUniform(const std::string& _name, int _x, int _y);
    void    setUniform(const std::string& _name, int _x, int _y, int _z);
    void    setUniform(const std::string& _name, int _x, int _y, int _z, int _w);
    void    setUniform(const std::string& _name, const int *_array, size_t _size);

    void    setUniform(const std::string& _name, float _x);
    void    setUniform(const std::string& _name, float _x, float _y);
    void    setUniform(const std::string& _name, float _x, float _y, float _z);
    void    setUniform(const std::string& _name, float _x, float _y, float _z, float _w);
    void    setUniform(const std::string& _name, const float *_array, size_t _size);

    void    setUniform(const std::string& _name, const glm::vec2& _value) { setUniform(_name,_value.x,_value.y); }
    void    setUniform(const std::string& _name, const glm::vec3& _value) { setUniform(_name,_value.x,_value.y,_value.z); }
    void    setUniform(const std::string& _name, const glm::vec4& _value) { setUniform(_name,_value.x,_value.y,_value.z,_value.w); }

    void    setUniform(const std::string& _name, const glm::vec2 *_array, size_t _size);
    void    setUniform(const std::string& _name, const glm::vec3 *_array, size_t _size);
    void    setUniform(const std::string& _name, const glm::vec4 *_array, size_t _size);

    void    setUniform(const std::string& _name, const glm::mat2& _value, bool transpose = false);
    void    setUniform(const std::string& _name, const glm::mat3& _value, bool transpose = false);
    void    setUniform(const std::string& _name, const glm::mat4& _value, bool transpose = false);

    void    setUniformTexture(const std::string& _name, const Texture* _tex);
    void    setUniformTexture(const std::string& _name, const Fbo* _fbo);
    void    setUniformDepthTexture(const std::string& _name, const Fbo* _fbo);
    void    setUniformTextureCube(const std::string& _name, const TextureCube* _tex);

    void    setUniformTexture(const std::string& _name, GLuint _textureId, size_t _texLoc);
    void    setUniformTexture(const std::string& _name, const Texture* _tex, size_t _texLoc);
    void    setUniformTexture(const std::string& _name, const Fbo* _fbo, size_t _texLoc);
    void    setUniformDepthTexture(const std::string& _name, const Fbo* _fbo, size_t _texLoc);
    void    setUniformTextureCube(const std::string& _name, const TextureCube* _tex, size_t _texLoc);

    void    detach(GLenum type);

    size_t  textureIndex;

private:
    GLuint      compileShader(const std::string& _src, GLenum _type, bool _verbose);
    GLint       getUniformLocation(const std::string& _uniformName) const;

    std::string m_fragmentSource;
    std::string m_vertexSource;
    
    GLuint      m_program;
    GLuint      m_fragmentShader;
    GLuint      m_vertexShader;
};

typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::unique_ptr<Shader> ShaderUnPtr;

}