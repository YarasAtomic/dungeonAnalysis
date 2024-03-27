#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include <math.h>
#include <string>
#include <vector>

struct Plane
{
    Vector3 point;
    Vector3 normal;
};

// --------- Vector3

float Vector3DotProduct(Vector3 v1, Vector3 v2);

Vector3 Vector3Subtract(Vector3 v1, Vector3 v2);

Vector3 Vector3Sum(Vector3 v1, Vector3 v2);

Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2);

Vector3 Vector3Normalized(Vector3 vector);

float Vector3Length(Vector3 vector);

Vector2 Vector3PlaneProject(Vector3 point, Vector3 planeX, Vector3 planeY);

Vector3 Vector3ScalarProduct(Vector3 v, float n);

Vector3 Vector3NormalProject(Vector3 vector,Vector3 normal);

bool isPointInside(Vector2 * poly,int pCount, Vector2 point);

bool isPointLeftOf(Vector2 a,Vector2 b,Vector2 p);

bool simpleRaycastSphere(Vector3 o, Vector3 d, Vector3 so, float radius2);

std::string Vector3String(Vector3 v);

std::string Vector2String(Vector2 v);

Matrix MatrixProduct(Matrix m0, Matrix m1);

Quaternion QuaternionNormalized(Quaternion q);

std::string MatrixString(Matrix m);

Matrix QuaternionMatrix(Quaternion);

Quaternion VectorAngleQuaternion(Vector3 v, float angle);

void ScaleMatrix(Matrix * m,Vector3 v);

Quaternion QuaternionProduct(Quaternion q0, Quaternion q1);

std::string QuaternionString(Quaternion q);

float Vector3LengthSquared(Vector3 vector);

// -------- WORLD

Ray CameraRay(Camera3D camera,const Vector2 mousePosition, float screenWidth,float screenHeight);

// -------- FILE

std::string commandFifo(std::string command,std::string fifoFile = "temp");

// -------- STRING

int CharArrayLength(char * array);

#endif