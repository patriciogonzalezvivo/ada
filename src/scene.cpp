#include "ada/scene.h"

#include <sys/stat.h>

#include "extract_depthmap.h"

// #include "tools/text.h"
// #include "types/files.h"

#include "ada/fs.h"
#include "ada/pixel.h"
#include "ada/string.h"

#include "ada/gl/textureBump.h"
#include "ada/gl/textureStreamSequence.h"

#if defined(SUPPORT_MMAL)
#include "ada/gl/textureStreamMMAL.h"
#endif

#if defined(SUPPORT_LIBAV) 
#include "ada/gl/textureStreamAV.h"
#endif

#if defined(DRIVER_BROADCOM) && defined(SUPPORT_OMAX)
#include "ada/gl/textureStreamOMX.h"
#endif

namespace ada {

Scene::Scene(): 
    m_streamsPrevs(0), 
    m_streamsPrevsChange(false), 
    m_activeCamera(nullptr), 
    m_activeCubemap(nullptr), 
    m_enableLights(true), 
    m_change(false) {
    setCamera( "default", new Camera() );
}

Scene::~Scene() {
    clear();
}

void Scene::clear() {
    clearTextures();
    clearStreams();

    clearCubemaps();
    clearLights();
    clearModels();
    clearMaterials();
    clearLabels();
    
    clearBuffers();
    clearCameras();

    flagChange();
}


bool Scene::feedTo(Shader &_shader, bool _lights, bool _buffers ) {
    return feedTo(&_shader, _lights, _buffers);
}

bool Scene::feedTo(Shader *_shader, bool _lights, bool _buffers ) {
    bool update = false;

    // Pass Textures Uniforms
    for (TexturesMap::iterator it = textures.begin(); it != textures.end(); ++it) {
        _shader->setUniformTexture(it->first, it->second, _shader->textureIndex++ );
        _shader->setUniform(it->first+"Resolution", float(it->second->getWidth()), float(it->second->getHeight()));
    }

    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it) {
        for (size_t i = 0; i < it->second->getPrevTexturesTotal(); i++)
            _shader->setUniformTexture(it->first+"Prev["+toString(i)+"]", it->second->getPrevTextureId(i), _shader->textureIndex++);

        _shader->setUniform(it->first+"Time", float(it->second->getTime()));
        _shader->setUniform(it->first+"Fps", float(it->second->getFps()));
        _shader->setUniform(it->first+"Duration", float(it->second->getDuration()));
        _shader->setUniform(it->first+"CurrentFrame", float(it->second->getCurrentFrame()));
        _shader->setUniform(it->first+"TotalFrames", float(it->second->getTotalFrames()));
    }

    // Pass Buffers Texture
    if (_buffers) {
        for (size_t i = 0; i < buffers.size(); i++)
            _shader->setUniformTexture("u_buffer" + toString(i), &buffers[i], _shader->textureIndex++ );

        for (size_t i = 0; i < doubleBuffers.size(); i++)
            _shader->setUniformTexture("u_doubleBuffer" + toString(i), doubleBuffers[i].src, _shader->textureIndex++ );
    }

    // Pass Convolution Piramids resultant Texture
    for (size_t i = 0; i < pyramids.size(); i++)
        _shader->setUniformTexture("u_pyramid" + toString(i), pyramids[i].getResult(), _shader->textureIndex++ );
    
    if (_lights && m_enableLights) {
        // Pass Light Uniforms
        if (lights.size() == 1) {
            LightsMap::iterator it = lights.begin();

            _shader->setUniform("u_lightColor", it->second->color);
            _shader->setUniform("u_lightIntensity", it->second->intensity);
            if (it->second->getLightType() != LIGHT_DIRECTIONAL)
                _shader->setUniform("u_light", it->second->getPosition());
            if (it->second->getLightType() == LIGHT_DIRECTIONAL || it->second->getLightType() == LIGHT_SPOT)
                _shader->setUniform("u_lightDirection", it->second->direction);
            if (it->second->falloff > 0)
                _shader->setUniform("u_lightFalloff", it->second->falloff);
            _shader->setUniform("u_lightMatrix", it->second->getBiasMVPMatrix() );
            _shader->setUniformDepthTexture("u_lightShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
        }
        else {
            // TODO:
            //      - Lights should be pass as structs?? 

            for (LightsMap::iterator it = lights.begin(); it != lights.end(); ++it) {
                std::string name = "u_" + it->first;

                _shader->setUniform(name + "Color", it->second->color);
                _shader->setUniform(name + "Intensity", it->second->intensity);
                if (it->second->getLightType() != ada::LIGHT_DIRECTIONAL)
                    _shader->setUniform(name, it->second->getPosition());
                if (it->second->getLightType() == ada::LIGHT_DIRECTIONAL || it->second->getLightType() == ada::LIGHT_SPOT)
                    _shader->setUniform(name + "Direction", it->second->direction);
                if (it->second->falloff > 0)
                    _shader->setUniform(name +"Falloff", it->second->falloff);

                _shader->setUniform(name + "Matrix", it->second->getBiasMVPMatrix() );
                _shader->setUniformDepthTexture(name + "ShadowMap", it->second->getShadowMap(), _shader->textureIndex++ );
            }
        }
        
        if (m_activeCubemap) {
            _shader->setUniformTextureCube("u_cubeMap", (TextureCube*)m_activeCubemap);
            _shader->setUniform("u_SH", m_activeCubemap->SH, 9);
        }
    }

    return update;
}

void Scene::flagChange() {    
    m_change = true;

    if (m_activeCamera)
        m_activeCamera->bChange = true;
}

void Scene::unflagChange() {
    if (m_change)
        m_change = false;

    for (LightsMap::iterator it = lights.begin(); it != lights.end(); ++it)
        it->second->bChange = false;

    if (m_activeCamera)
        m_activeCamera->bChange = false;
}

bool Scene::haveChange() const {
    if (m_change || streams.size() > 0)
        return true;

    if (m_activeCamera)
        if (m_activeCamera->bChange)
            return true;

    for (LightsMap::const_iterator it = lights.begin(); it != lights.end(); ++it)
        if (it->second->bChange)
            return true;

    return false;
}

void Scene::addDefine(const std::string& _define, const std::string& _value) {
    for (ModelsMap::iterator it = models.begin(); it != models.end(); ++it)
        it->second->addDefine(_define, _value);
}

void Scene::delDefine(const std::string& _define) {
    for (ModelsMap::iterator it = models.begin(); it != models.end(); ++it)
        it->second->delDefine(_define);
}

void Scene::printDefines() {
    for (ModelsMap::iterator it = models.begin(); it != models.end(); ++it) {
        std::cout << std::endl;
        std::cout << it->second->getName() << std::endl;
        std::cout << "-------------- " << std::endl;
        it->second->printDefines();
    }
}

bool Scene::haveTexture( const std::string& _name ) const {
    return textures.find(_name) != textures.end();
}

void Scene::setTexture( const std::string& _name, Texture* _texture ) {
    textures[ _name ] = _texture;
    m_change = true;
}

Texture* Scene::getTexture( const std::string& _name ) {
    TexturesMap::iterator it = textures.find(_name);
    if ( it != textures.end() )
        return it->second;
    return nullptr;
}

bool Scene::addTexture(const std::string& _name, const std::string& _path, bool _flip, bool _verbose) {
    if (textures.find(_name) == textures.end()) {
        struct stat st;

        // If we can not get file stamp proably is not a file
        if (stat(_path.c_str(), &st) != 0 )
            std::cerr << "Error watching for file " << _path << std::endl;

        // If we can lets proceed creating a texgure
        else {

            Texture* tex = new Texture();
            // load an image into the texture
            if (tex->load(_path, _flip)) {
                
                // the image is loaded finish add the texture to the uniform list
                setTexture( _name, tex);

                if (_verbose) {
                    std::cout << "// " << _path << " loaded as: " << std::endl;
                    std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                    std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
                }

                if (haveExt(_path,"jpeg")) {
                    const unsigned char *cv = NULL, *dm = NULL;
                    size_t cv_size = 0, dm_size = 0;
                    image_type_t dm_type = TYPE_NONE;

                    //  proceed to check if it have depth data
                    if (extract_depth(  _path.c_str(), 
                                        &cv, &cv_size,
                                        &dm, &dm_size, &dm_type) == 1) {

                        if (dm_type == TYPE_JPEG) {
                            int width, height;
                            unsigned char* pixels = loadPixels(dm, dm_size, &width, &height, RGB, _flip);

                            Texture* tex_dm = new Texture();
                            if (tex_dm->load(width, height, 3, 8, pixels)) {
                                textures[ _name + "Depth"] = tex_dm;

                                if (_verbose) {
                                    std::cout << "uniform sampler2D   " << _name  << "Depth;"<< std::endl;
                                    std::cout << "uniform vec2        " << _name  << "DepthResolution;"<< std::endl;
                                }
                            }   
                            freePixels(pixels);
                        }
                    }
                }

                return true;
            }
            else
                delete tex;
        }
    }
    return false;
}

bool Scene::addBumpTexture(const std::string& _name, const std::string& _path, bool _flip, bool _verbose) {
    if (textures.find(_name) == textures.end()) {
        struct stat st;

        // If we can not get file stamp proably is not a file
        if (stat(_path.c_str(), &st) != 0 )
            std::cerr << "Error watching for file " << _path << std::endl;
        
        // If we can lets proceed creating a texgure
        else {
            TextureBump* tex = new TextureBump();

            // load an image into the texture
            if (tex->load(_path, _flip)) {

                // the image is loaded finish add the texture to the uniform list
                setTexture( _name, (Texture*)tex);

                if (_verbose) {
                    std::cout << "// " << _path << " loaded and transform to normalmap as: " << std::endl;
                    std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                    std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
                }

                return true;
            }
            else
                delete tex;
        }
    }
    return false;
}


void Scene::printTextures() {
    for (TexturesMap::iterator it = textures.begin(); it != textures.end(); ++it) {
        std::cout << "uniform sampler2D " << it->first << "; // " << it->second->getFilePath() << std::endl;
        std::cout << "uniform vec2 " << it->first << "Resolution; // " << toString(it->second->getWidth(), 1) << "," << toString(it->second->getHeight(), 1) << std::endl;
    }
}

void Scene::clearTextures() {
    for (TexturesMap::iterator it = textures.begin(); it != textures.end(); ++it)
        delete (it->second);
    textures.clear();
}

bool Scene::haveStreamingTexture( const std::string& _name) const {
    return streams.find(_name) != streams.end();
}

void Scene::setStreamingTexture( const std::string& _name, TextureStream* _stream ) {
    streams[ _name ] = _stream;
    m_change = true;
}

TextureStream* Scene::getStreamingTexture( const std::string& _name ) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if ( it != streams.end() )
        return it->second;
    return nullptr;
}

bool Scene::addStreamingTexture( const std::string& _name, const std::string& _url, bool _vflip, bool _device, bool _verbose) {
    if (textures.find(_name) == textures.end()) {

        // Check if it's a PNG Sequence
        if ( haveWildcard(_url) ) {
            TextureStreamSequence *tex = new TextureStreamSequence();

            if (tex->load(_url, _vflip)) {
                // the image is loaded finish add the texture to the uniform list
                setTexture(_name, (Texture*)tex);
                setStreamingTexture(_name, (TextureStream*)tex);

                if (_verbose) {
                    std::cout << "// " << _url << " sequence loaded as streaming texture: " << std::endl;
                    std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                    std::cout << "uniform sampler2D   " << _name  << "Prev[STREAMS_PREVS];"<< std::endl;
                    std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
                    std::cout << "uniform float       " << _name  << "Time;" << std::endl;
                    std::cout << "uniform float       " << _name  << "Duration;" << std::endl;
                    std::cout << "uniform float       " << _name  << "CurrentFrame;"<< std::endl;
                    std::cout << "uniform float       " << _name  << "TotalFrames;"<< std::endl;
                    std::cout << "uniform float       " << _name  << "Fps;"<< std::endl;
                }

                return true;
            }
            else
                delete tex;
            
        }
#if defined(SUPPORT_MMAL)
        // if the user is asking for a device on a RaspberryPI hardware
        else if (_device) {
            TextureStreamMMAL* tex = new TextureStreamMMAL();

            // load an image into the texture
            if (tex->load(_url, _vflip)) {
                // the image is loaded finish add the texture to the uniform list
                setTexture(_name, (Texture*)tex);
                setStreamingTexture(_name, (TextureStream*)tex);

                if (_verbose) {
                    std::cout << "// " << _url << " loaded as streaming texture: " << std::endl;
                    std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                    std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
                }

                return true;
            }
            else
                delete tex;
        }
#endif
#if defined(DRIVER_BROADCOM) && defined(SUPPORT_OMAX)
        else if ( haveExt(_url,"h264") || haveExt(_url,"H264") ) {
            TextureStreamOMX* tex = new TextureStreamOMX();

            // load an image into the texture
            if (tex->load(_url, _vflip)) {
                // the image is loaded finish add the texture to the uniform list
                setTexture(_name, (Texture*)tex);
                setStreamingTexture(_name, (TextureStream*)tex);

                if (_verbose) {
                    std::cout << "// " << _url << " loaded as streaming texture: " << std::endl;
                    std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                    std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
                }

                return true;
            }
            else
                delete tex;
        }
#endif
        else {
#if defined(SUPPORT_LIBAV)
        TextureStreamAV* tex = new TextureStreamAV(_device);

        // load an image into the texture
        if (tex->load(_url, _vflip)) {
            // the image is loaded finish add the texture to the uniform list
            setTexture(_name, (Texture*)tex);
            setStreamingTexture(_name, (TextureStream*)tex);

            if (_verbose) {
                std::cout << "// " << _url << " loaded as streaming texture: " << std::endl;
                std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
                std::cout << "uniform sampler2D   " << _name  << "Prev[STREAMS_PREVS];"<< std::endl;
                std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;

                if (!_device) {
                    std::cout << "uniform float       " << _name  << "Time;" << std::endl;
                    std::cout << "uniform float       " << _name  << "Duration;" << std::endl;
                    std::cout << "uniform float       " << _name  << "CurrentFrame;" << std::endl;
                    std::cout << "uniform float       " << _name  << "TotalFrames;" << std::endl;
                    std::cout << "uniform float       " << _name  << "Fps;" << std::endl;
                }
            }

            return true;
        }
        else
            delete tex;
#endif
        }


    }
    return false;
}

bool Scene::addStreamingAudioTexture(const std::string& _name, const std::string& device_id, bool _flip, bool _verbose) {
#if defined(SUPPORT_LIBAV)
    TextureStreamAudio *tex = new TextureStreamAudio();

    // TODO: add flipping mode for audio texture
    if (tex->load(device_id, _flip)) {
        // the image is loaded finish add the texture to the uniform list
        setTexture(_name, (Texture*)tex);
        setStreamingTexture(_name, (TextureStream*)tex);

        if (_verbose) {
            std::cout << "loaded audio texture: " << std::endl;
            std::cout << "uniform sampler2D   " << _name  << ";"<< std::endl;
            std::cout << "uniform vec2        " << _name  << "Resolution;"<< std::endl;
        }
        
        return true;
    }
    else
#endif
        return false;
}

void Scene::setStreamPlay( const std::string& _name) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->play();
}

void Scene::setStreamStop( const std::string& _name) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->stop();
}

void Scene::setStreamRestart( const std::string& _name ) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->restart();
}

float Scene::getStreamTime( const std::string& _name) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        return it->second->getTime();
    return 0.0f;
}

float Scene::getStreamSpeed( const std::string& _name) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        return it->second->getSpeed();
    return 0.0f;
}

void Scene::setStreamSpeed( const std::string& _name, float _speed ) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->setSpeed(_speed);
}

float Scene::getStreamPct( const std::string& _name) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        return it->second->getPct();
    return 0.0f;
}

void Scene::setStreamPct( const std::string& _name, float _pct ) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->setPct(_pct);
}

void Scene::setStreamTime( const std::string& _name, float _time ) {
    TextureStreamsMap::iterator it = streams.find(_name);
    if (it != streams.end())
        it->second->setTime(_time);
}

void Scene::setStreamsPlay() {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->play();
}

void Scene::setStreamsStop() {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->stop();
}

void Scene::setStreamsRestart() {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->restart();
}

void Scene::setStreamsSpeed( float _speed ) {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->setSpeed(_speed);
}

void Scene::setStreamsTime( float _time ) {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->setTime(_time);
}

void Scene::setStreamsPct( float _pct ) {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        it->second->setPct(_pct);
}

void Scene::setStreamsPrevs( size_t _total ) {
    m_streamsPrevs = _total;
    m_streamsPrevsChange = true;
}

void Scene::updateStreams(size_t _frame) {
    if (m_streamsPrevsChange) {
        m_streamsPrevsChange = false;
        for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); it++)
            it->second->setPrevTextures(m_streamsPrevs);
    }

    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        if (it->second->update())
            m_change = true;
}

void Scene::clearStreams() {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it)
        delete (it->second);
    streams.clear();
}

void Scene::printStreams() {
    for (TextureStreamsMap::iterator it = streams.begin(); it != streams.end(); ++it) {
        std::cout << "uniform sampler2D " << it->first << "; // " << it->second->getFilePath() << std::endl;

        if (m_streamsPrevs > 0)
            std::cout << "uniform sampler2D " << it->first << "Prev;" << std::endl;

        std::cout << "uniform float " << it->first+"CurrentFrame; //" << toString(it->second->getCurrentFrame(), 1) << std::endl;
        std::cout << "uniform float " << it->first+"TotalFrames;  //" << toString(it->second->getTotalFrames(), 1) << std::endl;
        std::cout << "uniform float " << it->first+"Time;         // " << toString(it->second->getTime(), 1) << std::endl;
        std::cout << "uniform float " << it->first+"Duration;     // " << toString(it->second->getDuration(), 1) << std::endl;
        std::cout << "uniform float " << it->first+"Fps;          // " << toString(it->second->getFps(), 1) << std::endl;
    }
}

bool Scene::haveCubemap( const std::string& _name ) const {
    return cubemaps.find(_name) != cubemaps.end(); 
}

void Scene::setCubemap( const std::string& _name, TextureCube* _cubemap ) {
    cubemaps[_name] = _cubemap;
    if (cubemaps.size() == 1)
        setActiveCubemap(_name);
    m_change = true;
}

TextureCube* Scene::getCubemap( const std::string& _name ) {
    TextureCubesMap::iterator it = cubemaps.find(_name);
    if ( it != cubemaps.end() )
        return it->second;
    return nullptr;
}

void Scene::setActiveCubemap( const std::string& _name ) {
    TextureCubesMap::iterator it = cubemaps.find(_name);
    if ( it != cubemaps.end() )
        m_activeCubemap = it->second;
    m_change = true;
}

bool Scene::addCubemap( const std::string& _name, const std::string& _filename, bool _verbose ) {
    struct stat st;
    if ( stat(_filename.c_str(), &st) != 0 )
        std::cerr << "Error watching for cubefile: " << _filename << std::endl;
    
    else {
        TextureCube* tex = new TextureCube();
        if ( tex->load(_filename, true) ) {

            if (_verbose) {
                std::cout << "// " << _filename << " loaded as: " << std::endl;
                std::cout << "uniform samplerCube u_cubeMap;"<< std::endl;
                std::cout << "uniform vec3        u_SH[9];"<< std::endl;
            }

            setCubemap(_name, tex);
            return true;
        }
        else
            delete tex;
    }

    return false;
}

void Scene::clearCubemaps() {
    m_activeCubemap = nullptr;
    for (TextureCubesMap::iterator itr = cubemaps.begin(); itr != cubemaps.end(); ++itr)
        delete (itr->second);
    cubemaps.clear();
}

void Scene::printBuffers() {
    for (size_t i = 0; i < buffers.size(); i++)
        std::cout << "uniform sampler2D u_buffer" << i << ";" << std::endl;

    for (size_t i = 0; i < doubleBuffers.size(); i++)
        std::cout << "uniform sampler2D u_doubleBuffer" << i << ";" << std::endl;

    for (size_t i = 0; i < pyramids.size(); i++)
        std::cout << "uniform sampler2D u_pyramid" << i << ";" << std::endl;    
}

void Scene::clearBuffers() {
    buffers.clear();
    doubleBuffers.clear();
    pyramids.clear();
}

bool Scene::haveCamera( const std::string& _name ) const {
    return cameras.find(_name) != cameras.end();
}

void Scene::setActiveCamera(const std::string& _name) {
    CamerasMap::iterator it = cameras.find(_name);
    if (it != cameras.end())
        m_activeCamera = it->second;
    m_change = true;
}

void Scene::setCamera(const std::string& _name, Camera* _camera) {
    cameras[_name] = _camera;
    if (cameras.size() == 1)
        setActiveCamera(_name);
}

Camera* Scene::getCamera(const std::string& _name ) {
    CamerasMap::iterator it = cameras.find(_name);
    if (it != cameras.end())
        return it->second;
    return nullptr;
}

void Scene::printCameras() {
    std::cout << "// Cameras: " << std::endl;
    for (CamerasMap::iterator it = cameras.begin(); it != cameras.end(); ++it)
        std::cout << "// " << it->first << " " << toString( it->second->getPosition() ) << std::endl;

    std::cout << "// Active Camera: " << std::endl;
    if (m_activeCamera)
        std::cout << "uniform vec3 u_camera; // " << toString( m_activeCamera->getPosition() ) << std::endl;
}

void Scene::clearCameras() {
    m_activeCamera = nullptr;
    for (CamerasMap::iterator it = cameras.begin(); it != cameras.end(); ++it)
        delete (it->second);
    cameras.clear();
}

bool Scene::haveLight(const std::string& _name) const {
    return lights.find(_name) != lights.end();
}

void Scene::setLight(const std::string& _name, Light* _light) {
    lights[_name] = _light;
    m_change = true;
}

Light* Scene::getLight(const std::string& _name) {
    LightsMap::iterator it = lights.find(_name);
    if (it != lights.end())
        return it->second;
    return nullptr;
}

void Scene::setSkyTurbidity(float _turbidity) {
    m_skybox.change += m_skybox.turbidity == _turbidity;
    m_skybox.turbidity = _turbidity;
    if ( !haveCamera("skybox") )
        cubemaps["skybox"] = new TextureCube();
    cubemaps["skybox"]->load(&m_skybox);
}

void Scene::setGroundAlbedo(const glm::vec3& _albedo) {
    m_skybox.change += m_skybox.groundAlbedo == _albedo;
    m_skybox.groundAlbedo = _albedo;
    if ( !haveCamera("skybox") )
        cubemaps["skybox"] = new TextureCube();
    cubemaps["skybox"]->load(&m_skybox);
}

float Scene::getSkyTurbidity() {
    return m_skybox.turbidity;
}

glm::vec3 Scene::getGroundAlbedo() {
    return m_skybox.groundAlbedo;
}

void Scene::setSunPosition(float _az, float _elev) {
    m_skybox.change += m_skybox.elevation == _elev;
    m_skybox.elevation = _elev;

    m_skybox.change += m_skybox.azimuth == _az;
    m_skybox.azimuth = _az;

    if ( !m_skybox.change )
        return;

    if ( !haveCamera("skybox") )
        cubemaps["skybox"] = new TextureCube();
    cubemaps["skybox"]->load(&m_skybox);

    float distance = 100.0f;
    if ( haveLight("sun") ) 
        distance = glm::length( lights["sun"]->getPosition() );

    glm::vec3 p = glm::vec3(0.0f, 0.0f, distance );
    glm::quat lat = glm::angleAxis(-m_skybox.elevation, glm::vec3(1.0, 0.0, 0.0));
    glm::quat lon = glm::angleAxis(m_skybox.azimuth, glm::vec3(0.0, 1.0, 0.0));
    p = lat * p;
    p = lon * p;

    if ( !haveLight("sun") ) 
        lights["sun"] = new Light(p, -1.0);
    else
        lights["sun"]->setPosition(p);
}

void Scene::setSunPosition(const glm::vec3& _v) {
    float elev = atan2(_v.y, sqrt(_v.x * _v.x + _v.z * _v.z) );
    float az = atan2(_v.x, _v.z);

    m_skybox.change += m_skybox.elevation == elev;
    m_skybox.elevation = elev;

    m_skybox.change += m_skybox.azimuth == az;
    m_skybox.azimuth = az;

    if ( !m_skybox.change )
        return;

    if ( !haveCamera("skybox") )
        cubemaps["skybox"] = new TextureCube();
    cubemaps["skybox"]->load(&m_skybox);

    if ( !haveLight("sun") )
        lights["sun"] = new Light(_v, -1.0);
    else
        lights["sun"]->setPosition(_v);   
}

float Scene::getSunAzimuth() {
    return m_skybox.azimuth;
}

float Scene::getSunElevation() {
    return m_skybox.elevation;
}

void Scene::printLights() {
    if (m_enableLights) {
        // Pass Light Uniforms
        if (lights.size() == 1) {
            LightsMap::iterator it = lights.begin();

            if (it->second->getLightType() != LIGHT_DIRECTIONAL)
                std::cout << "unifrom vect3 u_light; // " << toString( it->second->getPosition() ) << std::endl;
            std::cout << "unifrom vect3 u_lightColor; // " << toString( it->second->color )  << std::endl;
            if (it->second->getLightType() == LIGHT_DIRECTIONAL || it->second->getLightType() == LIGHT_SPOT)
                std::cout << "unifrom vect3 u_lightDirection; // " << toString( it->second->direction ) << std::endl;
            std::cout << "unifrom float u_lightIntensity; // " << toString( it->second->intensity, 3) << std::endl;
            if (it->second->falloff > 0.0)
                std::cout << "unifrom float u_lightFalloff; // " << toString( it->second->falloff, 3) << std::endl;
            
            // std::cout << "unifrom mat4 u_lightMatrix;";
            // std::cout << "unifrom sampler2D u_lightShadowMap; ";
        }
        else {
            // TODO:
            //      - Lights should be pass as structs?? 

            for (LightsMap::iterator it = lights.begin(); it != lights.end(); ++it) {
                std::string name = "u_" + it->first;

                if (it->second->getLightType() != LIGHT_DIRECTIONAL)
                    std::cout << "uniform vec3 u_light; // " << toString( it->second->getPosition() ) << std::endl;
                std::cout << "uniform vec3 u_lightColor; // " << toString( it->second->color )  << std::endl;
                if (it->second->getLightType() == LIGHT_DIRECTIONAL || it->second->getLightType() == LIGHT_SPOT)
                    std::cout << "uniform vec3 u_lightDirection; // " << toString( it->second->direction ) << std::endl;
                std::cout << "uniform float u_lightIntensity; // " << toString( it->second->intensity, 3) << std::endl;
                if (it->second->falloff > 0.0)
                    std::cout << "uniform float u_lightFalloff; // " << toString( it->second->falloff, 3) << std::endl;
            }
        }
    }
}

void Scene::clearLights() {
    for (LightsMap::iterator it = lights.begin(); it != lights.end(); ++it)
        delete (it->second);
    lights.clear();
}

bool Scene::haveModel(const std::string& _name) const {
    return models.find(_name) != models.end();
}

void Scene::setModel(const std::string& _name, Model* _model) {
    models[_name] = _model;
    m_change = true;
}

Model* Scene::getModel(const std::string& _name) {
    ModelsMap::iterator it = models.find(_name);
    if (it != models.end())
        return it->second;
    return nullptr;
}

void Scene::printModels() {
    std::cout << "// Models: " << std::endl;
    for (ModelsMap::iterator it = models.begin(); it != models.end(); ++it)
        std::cout << "// " << it->first << std::endl;
}

void Scene::clearModels() {
    for (ModelsMap::iterator it = models.begin(); it != models.end(); ++it)
        delete (it->second);
    models.clear();
}

bool Scene::haveMaterial(const std::string& _name) const {
    return materials.find(_name) != materials.end();
}

void Scene::setMaterial(const std::string& _name, Material& _material) {
    materials[_name] = _material;
    m_change = true;
}

void Scene::printMaterials() {
    std::cout << "// Materials: " << std::endl;
    for (MaterialsMap::iterator it = materials.begin(); it != materials.end(); ++it)
        std::cout << "// " << it->first << std::endl;
}

void Scene::clearMaterials() {
    materials.clear();
}

void Scene::printLabels() {
    std::cout << "// Labels: " << std::endl;
    for (size_t i = 0; i < labels.size(); i++)
        std::cout << "// " << labels[i]->getText() << std::endl;
}

void Scene::clearLabels() {
    for (size_t i = 0; i < labels.size(); i++)
        delete labels[i];
    labels.clear();
}

}