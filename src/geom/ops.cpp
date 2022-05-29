#include "ada/geom/ops.h"



#include <algorithm>
#include <iostream>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef TAU
#define TAU 6.2831853071795864769252867665590
#endif

namespace ada {

float getArea(const std::vector<glm::vec2>& _points) {
    float area = 0.0;

    for (int i=0;i<(int)_points.size()-1;i++) {
        area += _points[i].x * _points[i+1].y - _points[i+1].x * _points[i].y;
    }
    area += _points[_points.size()-1].x * _points[0].y - _points[0].x * _points[_points.size()-1].y;
    area *= 0.5;

    return area;
}

glm::vec2 getCentroid(const std::vector<glm::vec2>& _points) {
    glm::vec2 centroid;
    for (size_t i = 0; i < _points.size(); i++) {
        centroid += _points[i] / (float)_points.size();
    }
    return centroid;
}


BoundingBox getBoundingBox(const Mesh& _mesh) {
    return getBoundingBox(_mesh.getVertices());
}

BoundingBox getBoundingBox(const std::vector<glm::vec2>& _points ) {
    BoundingBox bbox;
    for (std::vector<glm::vec2>::const_iterator it = _points.begin(); it != _points.end(); ++it)
        bbox.expand(*it);
    return bbox;
}

BoundingBox getBoundingBox(const std::vector<glm::vec3>& _points ) {
    BoundingBox bbox;
    for (std::vector<glm::vec3>::const_iterator it = _points.begin(); it != _points.end(); ++it)
        bbox.expand(*it);
    return bbox;
}

BoundingBox getBoundingBox(const std::vector<Line>& _lines) {
    BoundingBox bbox;
    for (std::vector<Line>::const_iterator it = _lines.begin(); it != _lines.end(); ++it) {
        bbox.expand(it->getPoint(0));
        bbox.expand(it->getPoint(1));
    }
    return bbox;
}

BoundingBox getBoundingBox(const std::vector<Triangle>& _triangles) {
    BoundingBox bbox;
    for (std::vector<Triangle>::const_iterator it = _triangles.begin(); it != _triangles.end(); ++it) {
        bbox.expand(it->getVertex(0));
        bbox.expand(it->getVertex(1));
        bbox.expand(it->getVertex(2));
    }
    return bbox;
}

glm::vec3 getCentroid(const std::vector<glm::vec3>& _points) {
    glm::vec3 centroid;
    for (size_t i = 0; i < _points.size(); i++) {
        centroid += _points[i] / (float)_points.size();
    }
    return centroid;
}

void calcNormal(const glm::vec3& _v0, const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3& _N) {
    glm::vec3 v10 = _v1 - _v0;
    glm::vec3 v20 = _v2 - _v0;

    _N.x = v20.x * v10.z - v20.z * v10.y;
    _N.y = v20.z * v10.x - v20.x * v10.z;
    _N.z = v20.x * v10.y - v20.y * v10.x;
    
    _N = glm::normalize(_N);
}


bool lexicalComparison(const glm::vec2 &_v1, const glm::vec2 &_v2) {
    if (_v1.x > _v2.x) return true;
    else if (_v1.x < _v2.x) return false;
    else if (_v1.y > _v2.y) return true;
    else return false;
}

bool isRightTurn(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c) {
    // use the cross product to determin if we have a right turn
    return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}

std::vector<glm::vec2> getConvexHull(const std::vector<glm::vec2> &_pts){
    std::vector<glm::vec2> pts;
    pts.assign(_pts.begin(),_pts.end());
    
    return getConvexHull(pts);
}

std::vector<glm::vec2> getConvexHull(std::vector<glm::vec2> &pts){
    std::vector<glm::vec2> hull;
    glm::vec2 h1,h2,h3;
    
    if (pts.size() < 3) {
        std::cout << "Error: you need at least three points to calculate the convex hull" << std::endl;
        return hull;
    }
    
    std::sort(pts.begin(), pts.end(), &lexicalComparison);
    
    hull.push_back(pts.at(0));
    hull.push_back(pts.at(1));
    
    int currentPoint = 2;
    int direction = 1;
    
    for (int i=0; i<3000; i++) { //max 1000 tries
        
        hull.push_back(pts.at(currentPoint));
        
        // look at the turn direction in the last three points
        h1 = hull.at(hull.size()-3);
        h2 = hull.at(hull.size()-2);
        h3 = hull.at(hull.size()-1);
        
        // while there are more than two points in the hull
        // and the last three points do not make a right turn
        while (!isRightTurn(h1, h2, h3) && hull.size() > 2) {
            
            // remove the middle of the last three points
            hull.erase(hull.end() - 2);
            
            if (hull.size() >= 3) {
                h1 = hull.at(hull.size()-3);
            }
            h2 = hull.at(hull.size()-2);
            h3 = hull.at(hull.size()-1);
        }
        
        // going through left-to-right calculates the top hull
        // when we get to the end, we reverse direction
        // and go back again right-to-left to calculate the bottom hull
        if (currentPoint == pts.size() -1 || currentPoint == 0) {
            direction = direction * -1;
        }
        
        currentPoint+=direction;
        
        if (hull.front()==hull.back()) {
            if(currentPoint == 3 && direction == 1){
                currentPoint = 4;
            } else {
                break;
            }
        }
    }
    
    return hull;
}

//This is for polygon/contour simplification - we use it to reduce the number of m_points needed in
//representing the letters as openGL shapes - will soon be moved to ofGraphics.cpp

// From: http://softsurfer.com/Archive/algorithm_0205/algorithm_0205.htm
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.


#define norm2(v)   glm::dot(v,v)        // norm2 = squared length of vector
#define norm(v)    sqrt(norm2(v))  // norm = length of vector
#define d2(u,v)    norm2(u-v)      // distance squared = norm2 of difference
#define d(u,v)     norm(u-v)       // distance = norm of difference

//--------------------------------------------------
static void simplifyDP(float tol, glm::vec2* v, int j, int k, int* mk ){
    
    if (k <= j+1) // there is nothing to simplify
        return;

    typedef struct{
        glm::vec2 P0, P1;
    } Segment;
    
    // check for adequate approximation by segment S from v[j] to v[k]
    int     maxi	= j;          // index of vertex farthest from S
    float   maxd2	= 0;         // distance squared of farthest vertex
    float   tol2	= tol * tol;  // tolerance squared
    Segment S		= {v[j], v[k]};  // segment from v[j] to v[k]
    glm::vec2 u;
    u				= S.P1 - S.P0;   // segment direction vector
    float  cu		= glm::dot(u,u);     // segment length squared
    
    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_ofPoint_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    glm::vec2  w;
    glm::vec2  Pb;                // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared
    
    for (int i = j + 1; i < k; i++) {
        // compute distance squared
        w = v[i] - S.P0;
        cw = glm::dot(w,u);
        if ( cw <= 0 ) dv2 = d2(v[i], S.P0);
        else if ( cu <= cw ) dv2 = d2(v[i], S.P1);
        else {
            b = (float)(cw / cu);
            Pb = S.P0 + u*b;
            dv2 = d2(v[i], Pb);
        }
        // test with current max distance squared
        if (dv2 <= maxd2) continue;
        
        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }

    if (maxd2 > tol2)        // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
    // else the approximation is OK, so ignore intermediate vertices
    return;
}

std::vector<glm::vec2> getSimplify(std::vector<glm::vec3> &_pts, float _tolerance){
    std::vector<glm::vec2> rta;
    rta.assign(_pts.begin(),_pts.end());
    simplify(rta);
    return rta;
}

void simplify(std::vector<glm::vec2> &_pts, float _tolerance){
    if(_pts.size() < 2) 
        return;
    
    int n = _pts.size();
    
    if(n == 0)
        return;
    
    std::vector<glm::vec2> sV;
    sV.resize(n);
    
    int    i, k, m, pv;            // misc counters
    float  tol2 = _tolerance * _tolerance;       // tolerance squared
    std::vector<glm::vec2> vt;
    std::vector<int> mk;
    vt.resize(n);
    mk.resize(n,0);
    
    // STAGE 1.  Vertex Reduction within tolerance of prior vertex cluster
    vt[0] = _pts[0];              // start at the beginning
    for (i=k=1, pv=0; i<n; i++) {
        if (d2(_pts[i], _pts[pv]) < tol2) continue;
        
        vt[k++] = _pts[i];
        pv = i;
    }
    if (pv < n-1) vt[k++] = _pts[n-1];      // finish at the end
    
    // STAGE 2.  Douglas-Peucker polyline simplification
    mk[0] = mk[k-1] = 1;       // mark the first and last vertices
    simplifyDP( _tolerance, &vt[0], 0, k-1, &mk[0] );
    
    // copy marked vertices to the output simplified polyline
    for (i=m=0; i<k; i++)
        if (mk[i]) sV[m++] = vt[i];
    
    //get rid of the unused points
    if( m < (int)sV.size() )
        _pts.assign( sV.begin(),sV.begin()+m );
    else
        _pts = sV;
}


void transform(std::vector<glm::vec3>& _points, const glm::quat& _quat) {
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it = _quat * *it;
}

void transform(std::vector<glm::vec3>& _points, const glm::mat3& _mat) {
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it = _mat * (*it);
}

void transform(std::vector<glm::vec3>& _points, const glm::mat4& _mat) {
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it = glm::vec3(_mat * glm::vec4(*it, 0.0));
}

void scale(std::vector<glm::vec3>& _points, float _v){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it *= _v;
}

void scaleX(std::vector<glm::vec3>& _points, float _x){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->x *= _x;
}

void scaleY(std::vector<glm::vec3>& _points, float _y){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->y *= _y;
}

void scaleZ(std::vector<glm::vec3>& _points, float _z){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->z *= _z;
}

void scale(std::vector<glm::vec3>& _points, float _x, float _y, float _z){
    scale(_points, glm::vec3(_x, _y, _z));
}

void scale(std::vector<glm::vec3>& _points, const glm::vec3& _v ){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it *= _v;
}


void translateX(std::vector<glm::vec3>& _points, float _x){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->x += _x;
}

void translateY(std::vector<glm::vec3>& _points, float _y){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->y += _y;
}

void translateZ(std::vector<glm::vec3>& _points, float _z){
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        it->z += _z;
}

void translate(std::vector<glm::vec3>& _points, float _x, float _y, float _z){
    translate(_points, glm::vec3(_x, _y, _z));
}

void translate(std::vector<glm::vec3>& _points, const glm::vec3& _v) {
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it += _v;
}

void rotate(std::vector<glm::vec3>& _points, float _rad, const glm::vec3& _axis ) {
    glm::quat q = glm::angleAxis(_rad, _axis);
    glm::mat3 M = glm::mat3_cast(q);
    for (std::vector<glm::vec3>::iterator it = _points.begin(); it != _points.end(); ++it)
        *it = M * *it;
}

void rotate(std::vector<glm::vec3>& _points, float _rad, float _x, float _y, float _z ) {
    rotate( _points, _rad, glm::vec3(_x, _y, _z));
}

void rotateX(std::vector<glm::vec3>& _points, float _rad){
    rotate( _points, _rad, 1.0f, 0.0f, 0.0f);
}

void rotateY(std::vector<glm::vec3>& _points, float _rad){
    rotate( _points, _rad, 0.0f, 1.0f, 0.0f);
}

void rotateZ(std::vector<glm::vec3>& _points, float _rad){
    rotate( _points, _rad, 0.0f, 0.0f, 1.0f);
}

void center(std::vector<glm::vec3>& _points){
    BoundingBox bbox = getBoundingBox(_points);
    glm::vec3 center = bbox.getCenter();
    translate(_points, -center);
}

void rotateX(Mesh& _mesh, float _rad) {
    rotateX(_mesh.m_vertices, _rad);
    if (_mesh.haveNormals()) rotateX(_mesh.m_normals, _rad);
}

void rotateY(Mesh& _mesh, float _rad) {
    rotateY(_mesh.m_vertices, _rad);
    if (_mesh.haveNormals()) rotateY(_mesh.m_normals, _rad);
}

void rotateZ(Mesh& _mesh, float _rad) {
    rotateZ(_mesh.m_vertices, _rad);
    if (_mesh.haveNormals()) rotateZ(_mesh.m_normals, _rad);
}

void rotate(Mesh& _mesh, float _rad, const glm::vec3& _axis ) {
    rotate(_mesh.m_vertices, _rad, _axis);
    if (_mesh.haveNormals()) rotate(_mesh.m_normals, _rad, _axis);
}

void rotate(Mesh& _mesh, float _rad, float _x, float _y, float _z ) {
    rotate(_mesh.m_vertices, _rad, _x, _y, _z);
    if (_mesh.haveNormals()) rotate(_mesh.m_normals, _rad, _x, _y, _z);
}


}
