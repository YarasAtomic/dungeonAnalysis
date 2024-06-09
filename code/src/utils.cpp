#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <array>
#include <sstream>

float Vector3DotProduct(Vector3 v1, Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Vector3Subtract(Vector3 v1, Vector3 v2) {
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

Vector3 Vector3Sum(Vector3 v1, Vector3 v2) {
    Vector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2) {
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

Vector3 Vector3Normalized(Vector3 vector) {
    float length = Vector3Length(vector);
    
    // Verificar si la longitud es mayor que 0 para evitar la división por cero
    if (length > 0) {
        vector.x /= length;
        vector.y /= length;
        vector.z /= length;
    }
    
    return vector;
}

float Vector3Length(Vector3 vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

float Vector3LengthSquared(Vector3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

Vector2 Vector3PlaneProject(Vector3 point, Vector3 planeX, Vector3 planeY)
{
    return Vector2 {Vector3DotProduct(point,planeX),Vector3DotProduct(point,planeY)};
}

Vector3 Vector3NormalProject(Vector3 vector,Vector3 normal)
{
    return Vector3Subtract(vector,Vector3ScalarProduct(normal,Vector3DotProduct(vector,normal)));
}

Vector3 Vector3ScalarProduct(Vector3 v, float n)
{
    return {v.x*n,v.y*n,v.z*n};
}

bool isPointInside(Vector2 * poly,int pCount, Vector2 point)
{
    for(int i = 0 ; i < pCount; i++)
    {
        int next = i == pCount-1 ? 0 : i+1;
        if(!isPointLeftOf(poly[i],poly[next],point)) return false;
    }


    return true;
}

bool isPointLeftOf(Vector2 a,Vector2 b,Vector2 p)
{
    float area = 0.5f * (a.x * (b.y - p.y) +
                         b.x * (p.y - a.y) +
                         p.x * (a.y - b.y));
    return (area > 0.0f);
}

bool simpleRaycastSphere(Vector3 o, Vector3 d, Vector3 so, float radius2)
{
    // o: raycast origin
    // d: raycastvector
    // so: sphere centre
    // radius2: sphere radius squared

    // we pass in d non-normalized to keep it's length
    // then we use that length later to compare the intersection point to make sure
    // we're within the actual ray segment
    float l = Vector3Length(d);
    d = {d.x/l,d.y/l,d.z/l};

    Vector3 m = Vector3Subtract(o,so);
    float b = Vector3DotProduct(m, d);
    float c = Vector3DotProduct(m, m) - radius2;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return false;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - sqrtf(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f)
        t = 0.0f;

    // Ray doesnt reach the sphere
    if (t > l)
        return false;

    return true;
}

std::string Vector3String(Vector3 v)
{
    return "("+std::to_string(v.x)+","+std::to_string(v.y)+","+std::to_string(v.z)+")";
}

std::string Vector2String(Vector2 v)
{
    return "("+std::to_string(v.x)+","+std::to_string(v.y)+")";
}

Matrix MatrixProduct(Matrix m, Matrix n)
{
    // return Matrix{
    //     m.m0*n.m0  + m.m4*n.m1  + m.m8*n.m2  + m.m12*n.m3,  m.m1*n.m0  + m.m5*n.m1  + m.m9*n.m2  + m.m13*n.m3,   m.m2*n.m0  + m.m6*n.m1  + m.m10*n.m2  + m.m14*n.m3,   m.m3*n.m0  + m.m7*n.m1  + m.m11*n.m2  + m.m15*n.m3,
    //     m.m0*n.m4  + m.m4*n.m5  + m.m8*n.m6  + m.m12*n.m7,  m.m1*n.m4  + m.m5*n.m5  + m.m9*n.m6  + m.m13*n.m7,   m.m2*n.m4  + m.m6*n.m5  + m.m10*n.m6  + m.m14*n.m7,   m.m3*n.m4  + m.m7*n.m5  + m.m11*n.m6  + m.m15*n.m7,
    //     m.m0*n.m8  + m.m4*n.m9  + m.m8*n.m10 + m.m12*n.m11, m.m1*n.m8  + m.m5*n.m9  + m.m9*n.m10 + m.m13*n.m11,  m.m2*n.m8  + m.m6*n.m9  + m.m10*n.m10 + m.m14*n.m11,  m.m3*n.m8  + m.m7*n.m9  + m.m11*n.m10 + m.m15*n.m11,
    //     m.m0*n.m12 + m.m4*n.m13 + m.m8*n.m14 + m.m12*n.m15, m.m1*n.m12 + m.m5*n.m13 + m.m9*n.m14 + m.m13*n.m15,  m.m2*n.m12 + m.m6*n.m13 + m.m10*n.m14 + m.m14*n.m15,  m.m3*n.m12 + m.m7*n.m13 + m.m11*n.m14 + m.m15*n.m15
    // };
    return Matrix{
        m.m0*n.m0  + m.m4*n.m1  + m.m8*n.m2  + m.m12*n.m3,  m.m0*n.m4  + m.m4*n.m5  + m.m8*n.m6  + m.m12*n.m7,   m.m0*n.m8  + m.m4*n.m9  + m.m8*n.m10 + m.m12*n.m11,   m.m0*n.m12 + m.m4*n.m13 + m.m8*n.m14 + m.m12*n.m15,
        m.m1*n.m0  + m.m5*n.m1  + m.m9*n.m2  + m.m13*n.m3,  m.m1*n.m4  + m.m5*n.m5  + m.m9*n.m6  + m.m13*n.m7,   m.m1*n.m8  + m.m5*n.m9  + m.m9*n.m10 + m.m13*n.m11,   m.m1*n.m12 + m.m5*n.m13 + m.m9*n.m14 + m.m13*n.m15,
        m.m2*n.m0  + m.m6*n.m1  + m.m10*n.m2  + m.m14*n.m3, m.m2*n.m4  + m.m6*n.m5  + m.m10*n.m6  + m.m14*n.m7,  m.m2*n.m8  + m.m6*n.m9  + m.m10*n.m10 + m.m14*n.m11,  m.m2*n.m12 + m.m6*n.m13 + m.m10*n.m14 + m.m14*n.m15,
        m.m3*n.m0  + m.m7*n.m1  + m.m11*n.m2  + m.m15*n.m3, m.m3*n.m4  + m.m7*n.m5  + m.m11*n.m6  + m.m15*n.m7,  m.m3*n.m8  + m.m7*n.m9  + m.m11*n.m10 + m.m15*n.m11,  m.m3*n.m12 + m.m7*n.m13 + m.m11*n.m14 + m.m15*n.m15
    };
}

Quaternion QuaternionNormalized(Quaternion q)
{
    float length = sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    return ( length > 0 ? 
        Quaternion{
            q.x/length,
            q.y/length,
            q.z/length,
            q.w/length
        }
        : q);
}

Quaternion QuaternionProduct(Quaternion q0, Quaternion q1)
{
    // return{
    //     q0.x*
    //     q0.x*q1.x-q0.y*q1.y-q0.z*q1.z-q0.w*q1.w
    // }

    float a = q0.w;
    float b = q0.x;
    float c = q0.y;
    float d = q0.z;

    float e = q1.w;
    float f = q1.x;
    float g = q1.y;
    float h = q1.z;

    float w = a*e-b*f-c*g-d*h;
    float x = a*f+b*e+c*h-d*g;
    float y = a*g-b*h+c*e+d*f;
    float z = a*h+b*g-c*f+d*e;

    return {x,y,z,w};
}

std::string MatrixString(Matrix m)
{
    return
    + "|"+std::to_string(m.m0)+ "\t"+std::to_string(m.m1)+  "\t"+std::to_string(m.m2)+ "\t"+std::to_string(m.m3) +"\t|\n" 
    + "|"+std::to_string(m.m4)+ "\t"+std::to_string(m.m5)+  "\t"+std::to_string(m.m6)+ "\t"+std::to_string(m.m7) +"\t|\n" 
    + "|"+std::to_string(m.m8)+ "\t"+std::to_string(m.m9)+  "\t"+std::to_string(m.m10)+"\t"+std::to_string(m.m11)+"\t|\n" 
    + "|"+std::to_string(m.m12)+"\t"+std::to_string(m.m13)+ "\t"+std::to_string(m.m14)+"\t"+std::to_string(m.m15)+"\t|\n";
}

Matrix QuaternionMatrix(Quaternion q)
{
    float a = q.w;
    float i = q.x;
    float j = q.y;
    float k = q.z;

    // float aa = a * a;
    // float ii = i * i;
    // float jj = j * j;
    // float kk = k * k;

    return {
        2*(a*a + i*i) - 1,  2*(i*j + a*k),      2*(i*k - a*j),      0,
        2*(i*j - a*k),      2*(a*a + j*j) - 1,  2*(j*k + a*i),      0,
        2*(i*k + a*j),      2*(j*k - a*i),      2*(a*a + k*k) - 1,  0,
        0,                  0,                  0,                  1
    };
}

Quaternion VectorAngleQuaternion(Vector3 v, float angle)
{
    float halfAngle = angle/2;

    float w = cosf(halfAngle);

    float sinHalfAngle = sinf(halfAngle);

    float i = v.x * sinHalfAngle;
    float j = v.y * sinHalfAngle;
    float k = v.z * sinHalfAngle;

    return {i,j,k,w};
}

void ScaleMatrix(Matrix * m,Vector3 v)
{
    // m->m0 *= v.x; m->m4 *= v.x; m->m8*=v.x;  m->m12*=v.x;
    // m->m1 *= v.y; m->m5 *= v.y; m->m9*=v.y;  m->m13*=v.y;
    // m->m2 *= v.z; m->m6 *= v.z; m->m10*=v.z; m->m14*=v.z;
    m->m0 *= v.x; m->m1 *= v.x; m->m2*=v.x;  m->m3*=v.x;
    m->m4 *= v.y; m->m5 *= v.y; m->m6*=v.y;  m->m7*=v.y;
    m->m8 *= v.z; m->m9 *= v.z; m->m10*=v.z; m->m11*=v.z;
}

std::string QuaternionString(Quaternion q)
{
    std::string xSign = q.x < 0 ? "\t" : "\t+";
    std::string ySign = q.y < 0 ? "\t" : "\t+";
    std::string zSign = q.z < 0 ? "\t" : "\t+";
    return "("+std::to_string(q.w)+xSign+std::to_string(q.x)+"i"+ySign+std::to_string(q.y)+"j"+zSign+std::to_string(q.z)+"k)";
}


// ------ FILE

std::string commandFifo(std::string command,std::string fifoFile){
    const std::string redirectCommand = std::string(command) + " > " + fifoFile;

    // Execute the command
    int result = std::system(redirectCommand.c_str());

    // Check the result of the system call
    std::string capturedOutput = "";
    if (result == 0) {
        // Read the output from the temporary file
        std::ifstream file(fifoFile);
        capturedOutput = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    // Remove the temporary file
    std::remove(fifoFile.c_str());

    return capturedOutput;
}

int CharArrayLength(char * array){
    int size = ((int)(sizeof(array) / sizeof(*(array))));
    for(int i = 0 ; i < size; i++){
        if(array[i] == '\0') return i;
    }
    return 0;
}

Ray CameraRay(
    Camera3D camera,
    const Vector2 mousePosition,
    float screenWidth,
    float screenHeight
) {
    Vector3 camDir = Vector3Normalized(Vector3Subtract(camera.target,camera.position));

    // Convert FOV from degrees to radians
    float fovRad = camera.fovy * DEG2RAD;

    // Calculate the right vector based on camera direction and up vector
    // glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
    Vector3 cameraRight = Vector3Normalized(Vector3CrossProduct(camera.up,camDir));

    // Calculate the "up" vector for the camera
    // glm::vec3 cameraUpVector = glm::cross(cameraDirection, cameraRight);
    Vector3 cameraUpVector = Vector3CrossProduct(camera.up,cameraRight);

    // Calculate the aspect ratio
    float aspectRatio = screenWidth / screenHeight;

    // Calculate the distance from the camera to the image plane
    float distanceToImagePlane = (screenWidth / 2.0f) / std::tan(fovRad / 2.0f);

    // Calculate normalized device coordinates (NDC) from mouse position
    float ndcX = (2.0f * mousePosition.x / screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePosition.y / screenHeight);

    // Calculate the direction of the ray in world space
    // glm::vec3 rayDirection = glm::normalize(
    //     cameraRight * (ndcX * aspectRatio) +
    //     cameraUpVector * ndcY +
    //     cameraDirection * distanceToImagePlane
    // );

    Vector3 rayDirection = Vector3Normalized(
        Vector3Sum(Vector3Sum(
            Vector3ScalarProduct(cameraRight,(ndcX * aspectRatio)),
            Vector3ScalarProduct(cameraUpVector,ndcY)),
            Vector3ScalarProduct(camDir,distanceToImagePlane))
    );

    // Return the ray origin and direction
    // return glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z), rayDirection;
    Ray ray = {camera.position,rayDirection};
    return ray;
}
