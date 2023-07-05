#pragma once

class CRect {
public:
    long left, top;
    long right, bottom;

    CRect();
    CRect(long left, long top, long right, long bottom);

    long GetWidth();
    long GetHeight();
    void Move(long x, long y);
    void Resize(long x, long y);
    void SetSize(long w, long h);
    void GetCenter(long* x, long* y);
    bool IsPointInside(long x, long y);
};