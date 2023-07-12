#pragma once
#include "Matrix4D.h"
class CCanvas
{
public:
    union PIXEL
    {
        struct
        {
            unsigned char r, g, b, a; //R8G8B8A8
        };
        struct
        {
            unsigned char c, m, y, k;
        };
        unsigned long p;
    };
    typedef PIXEL(*PIXELSHADER)(int, int);
    enum AddressMode {
        ADDRESS_MODE_BORDER, ADDRESS_MODE_WRAP, ADDRESS_MODE_MIRROR,
        ADDRESS_MODE_CLAMP
    };
    struct VERTEX
    {
        VECTOR4D Position;
        VECTOR4D TexCoord;
    };
protected:
    struct LIMIT
    {
        int left;
        int right;
    };
    PIXEL* m_pBuffer; //Pixel buffer
    int m_nSizeX; //Canva X length in pixels
    int m_nSizeY; //Cnava Y length in pixels
    int m_nPitch; //Canvas now length in bytes
    PIXEL m_Border; //Pixel Border (used as a dummy pixel)
    LIMIT* m_pLimits;
    AddressMode  m_am;

protected:
    CCanvas();
    ~CCanvas();
public:
    void SetAddressMode(AddressMode mode) { m_am = mode; }
    PIXEL Address(int i, int j);
    static CCanvas* Create(int nSizeX, int nSizeY);
    static void Destroy(CCanvas* pCanvas);
    CCanvas* Clone();
    void Clear(PIXEL Color);
    PIXEL& operator()(int i, int j);
    int GetSizeX() { return m_nSizeX; }
    int GetSizeY() { return m_nSizeY; }
    PIXEL& GetBorder() { return m_Border; }
    friend class CDXManager;
    static PIXEL Lerp(PIXEL A, PIXEL B, int p);
    static PIXEL SatAdd(PIXEL A, PIXEL B);
    static PIXEL SatSub(PIXEL A, PIXEL B);
    static PIXEL AlphaBlendSrc(PIXEL dst, PIXEL src);
    static PIXEL AlphaBlendDst(PIXEL dst, PIXEL src);
    static PIXEL Complement(PIXEL A);

    void Line(int x0, int y0, int x1, int y1, PIXEL Color);
    void ResetLimits();
    void SetLimit(int x, int y);
    void FillLimits(PIXEL Color);
    void LineLimits(int x0, int y0, int x1, int y1);
    void FillLimits(PIXELSHADER Shader);
    void Circle(int xc, int yc, int r, PIXEL Color);
    void CircleLimits(int xc, int yc, int r);

    PIXEL PointSampler(float s, float t);
    PIXEL BilinearSampler(float s, float y);
    void InverseMapping(MATRIX4D M, CCanvas* pSource, PIXEL(CCanvas::* sampler)(float, float));

    bool CreateFileFromCanvas(const char* pszFileName);//Save to disk
    static CCanvas* CreateCanvasFromFile(const char* pszFileName); //Load from disk
    void InverseTextureMapping(
        const VERTEX V[], CCanvas* pTexture,
        PIXEL(CCanvas::* sampler)(float, float));
};