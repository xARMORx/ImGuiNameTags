#pragma once

struct VectorCompressed {
    unsigned short x, y, z;
};

class CVector {
public:
    float x, y, z;

    CVector();
    CVector(float x, float y, float z);

    void    Set(float x, float y, float z);
    float   GetLength() const;
    float   GetLengthSquared() const;
    void    Normalize();
    float   Dot(const CVector& vec) const;
    CVector Cross(const CVector& vec) const;
    void    ZeroNearZero();
};