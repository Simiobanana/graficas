#include "CCanvas.h"
#include <algorithm>
using namespace std;

CCanvas::CCanvas()
{
	m_pBuffer = nullptr;
	m_pLimits = nullptr;
	m_nPitch = 0;
	m_nSizeX = 0;
	m_nSizeY = 0;
	m_Border = { 0,0,0,0 };
}

CCanvas::~CCanvas()
{
}

CCanvas* CCanvas::Create(int nSizeX, int nSizeY)
{
	CCanvas* pNewCanvas = new CCanvas;
	pNewCanvas->m_pBuffer = new PIXEL[nSizeX * nSizeY];
	pNewCanvas->m_pLimits = new LIMIT[nSizeY];
	pNewCanvas->m_nSizeX = nSizeX;
	pNewCanvas->m_nSizeY = nSizeY;
	pNewCanvas->m_nPitch = nSizeX * sizeof(PIXEL);
	return pNewCanvas;
}
void CCanvas::Destroy(CCanvas* pCanvas)
{
	delete[] pCanvas->m_pBuffer;
	delete[] pCanvas->m_pLimits;
	delete pCanvas;
}

void CCanvas::Clear(PIXEL Color)
{
	int c = m_nSizeX * m_nSizeY;
	PIXEL* pixel = m_pBuffer;
	while (c--)
		*pixel++ = Color;
}
CCanvas* CCanvas::Clone()
{
	CCanvas* pClone = CCanvas::Create(m_nSizeX, m_nSizeY);
	int c = m_nSizeX * m_nSizeY;
	pClone->m_Border = m_Border;
	PIXEL* dst = pClone->m_pBuffer;
	PIXEL* src = m_pBuffer;
	while (c--) *dst++ = *src++;
	return pClone;
}

CCanvas::PIXEL& CCanvas::operator()(int i, int j)
{
	if (i >= 0 && i < m_nSizeX && j >= 0 && j < m_nSizeY)
		return *(PIXEL*)(
			(char*)(m_pBuffer)+ //Direccion del primer pixel
			m_nPitch * j +    //Desplazamiento por filas 
			i * sizeof(PIXEL)); // Desplazamiento por columna
	else
		return m_Border;
}

CCanvas::PIXEL CCanvas::Lerp(PIXEL A, PIXEL B, int p)
{
	//Lerp(A, B, p) = A + (B-A)*p, 0<=p<=1, p E R
	// short int x [ 0000 0000 0000 1001. ] = 9
	//             [ 0000 0000 0001 0001 ] = 1/2 + 1/16 +
	//             [ 0000 0000 0001 1010 ] = 1 + 1/2 + 1 / 8
	return{
		(unsigned char)((int)A.r + (((int)B.r - (int)A.r) * p) / 256),
		(unsigned char)((int)A.g + (((int)B.g - (int)A.g) * p) / 256),
		(unsigned char)((int)A.b + (((int)B.b - (int)A.b) * p) / 256),
		(unsigned char)((int)A.a + (((int)B.a - (int)A.a) * p) / 256)
	};
}

CCanvas::PIXEL CCanvas::SatAdd(PIXEL A, PIXEL B)
{
	//
	return{
		(unsigned char)min(255,(int)A.r + (int)B.r),
		(unsigned char)min(255,(int)A.g + (int)B.g),
		(unsigned char)min(255,(int)A.b + (int)B.b),
		(unsigned char)min(255,(int)A.a + (int)B.a)
	};
}

CCanvas::PIXEL CCanvas::SatSub(PIXEL A, PIXEL B)
{
	return{
		(unsigned char)max(255, (int)A.r + (int)B.r),
		(unsigned char)max(255,(int)A.g + (int)B.g),
		(unsigned char)max(255,(int)A.b + (int)B.b),
		(unsigned char)max(255,(int)A.a + (int)B.a)
	};
}

CCanvas::PIXEL CCanvas::AlphaBlendSrc(PIXEL dst, PIXEL src)
{
	return{
		(unsigned char)((((int)dst.r * (255 - (int)src.a) + (int)src.r * (int)src.a)) / 256),
		(unsigned char)((((int)dst.g * (255 - (int)src.a) + (int)src.g * (int)src.a)) / 256),
		(unsigned char)((((int)dst.b * (255 - (int)src.a) + (int)src.b * (int)src.a)) / 256),
		(unsigned char)((((int)dst.a * (255 - (int)src.a) + (int)src.a * (int)src.a)) / 256),


	};
}

CCanvas::PIXEL CCanvas::AlphaBlendDst(PIXEL dst, PIXEL src)
{
	return AlphaBlendSrc(src, dst);
}

CCanvas::PIXEL CCanvas::Complement(PIXEL A)
{
	return{
		(unsigned char)(255 - A.r),
		(unsigned char)(255 - A.g),
		(unsigned char)(255 - A.b),
		(unsigned char)(255 - A.a),
	};
}

void CCanvas::Line(int x0, int y0, int x1, int y1, PIXEL Color)
{
	// y = m*x +b
	int dx = x1 - x0;
	int dy = y1 - y0;
	int incx = (dx >= 0) ? +1 : -1;
	int incy = (dy >= 0) ? +1 : -1;
	dx = abs(dx);
	dy = abs(dy);

	int x = x0;
	int y = y0;
	int _2dx;
	int _2dy;
	_2dx = dx + dx;
	_2dy = dy + dy;
	int p;//

			  //	p = dx * y + dy * x < 0

	if (dx >= dy)
	{
		int c = dx;
		p = dx - _2dy;
		int _2dx_2dy = p + dx;
		while (c--)
		{
			(*this)(x, y) = Color;
			x += incx;
			if (p < 0)
			{
				y += incy;
				//subir
				p += _2dx_2dy;
			}
			else
			{
				//seguir
				p += -_2dy;
			}
		}
	}
	else // 1 < m <Inf
	{
		int c = dy;
		p = dy - _2dx;
		int _2dy_2dx = p + dy;
		while (c--)
		{
			(*this)(x, y) = Color;
			y += incy;
			if (p < 0)
			{
				x += incx;
				p += _2dy_2dx;
			}
			else
			{
				p += -_2dx;
			}
		}
	}
}

void CCanvas::ResetLimits()
{
	for (int j = 0; j < m_nSizeY; j++)
	{
		m_pLimits[j].left = m_nSizeX;
		m_pLimits[j].right = -1;
	}
}
void CCanvas::SetLimit(int x, int y)
{
	if (y >= 0 && y < m_nSizeY)
	{
		LIMIT* limit = &m_pLimits[y];
		limit->left = (limit->left > x) ? max(0, x) : limit->left;
		limit->right = (limit->right < x) ? min(m_nSizeX, x) : limit->right;
	}
}
void CCanvas::FillLimits(PIXEL Color)
{
	for (int j = 0; j < m_nSizeY; j++)
	{
		if (m_pLimits[j].left < m_pLimits[j].right)
		{
			PIXEL* pRow = &(*this)(0, j);
			pRow += m_pLimits[j].left;
			int c = m_pLimits[j].right - m_pLimits[j].left;
			while (c--) *pRow++ = Color;
		}
	}
}

void CCanvas::LineLimits(int x0, int y0, int x1, int y1)
{
	// y = m*x +b
	int dx = x1 - x0;
	int dy = y1 - y0;
	int incx = (dx >= 0) ? +1 : -1;
	int incy = (dy >= 0) ? +1 : -1;
	dx = abs(dx);
	dy = abs(dy);

	int x = x0;
	int y = y0;
	int _2dx;
	int _2dy;
	_2dx = dx + dx;
	_2dy = dy + dy;
	int p;//

	//	p = dx * y + dy * x < 0

	if (dx >= dy)
	{
		int c = dx;
		p = dx - _2dy;
		int _2dx_2dy = p + dx;
		while (c--)
		{
			SetLimit(x, y);
			x += incx;
			if (p < 0)
			{
				y += incy;
				//subir
				p += _2dx_2dy;
			}
			else
			{
				//seguir
				p += -_2dy;
			}
		}
	}
	else // 1 < m <Inf
	{
		int c = dy;
		p = dy - _2dx;
		int _2dy_2dx = p + dy;
		while (c--)
		{
			SetLimit(x, y);
			y += incy;
			if (p < 0)
			{
				x += incx;
				p += _2dy_2dx;
			}
			else
			{
				p += -_2dx;
			}
		}
	}
}

void CCanvas::FillLimits(PIXELSHADER Shader)
{
	for (int j = 0; j < m_nSizeY; j++)
	{
		if (m_pLimits[j].left < m_pLimits[j].right)
		{
			PIXEL* pRow = &(*this)(0, j);
			pRow += m_pLimits[j].left;
			int c = m_pLimits[j].right - m_pLimits[j].left;
			int i = m_pLimits[j].left;
			while (c--) *pRow++ = Shader(i++, j);
		}
	}
}

void CCanvas::Circle(int xc, int yc, int r, PIXEL Color)
{
	int x, y, p;
	p = 5 - 4 * r;
	x = 0;
	y = r;
	int _8x = 0;
	int	_8y = 8 * r;

	while (x <= y)
	{
		(*this)(x + xc, y + yc) = Color;
		(*this)(x + xc, -y + yc) = Color;
		(*this)(-x + xc, y + yc) = Color;
		(*this)(-x + xc, -y + yc) = Color;

		(*this)(y + xc, x + yc) = Color;
		(*this)(y + xc, -x + yc) = Color;
		(*this)(-y + xc, x + yc) = Color;
		(*this)(-y + xc, -x + yc) = Color;
		if (p > 0)
		{
			p += _8x - _8y + 20;
			y--;
			_8y -= 8;
		}
		else
			p += _8x + 12;
		x++;
		_8x += 8;
	}

}

void CCanvas::CircleLimits(int xc, int yc, int r)
{
	int x, y, p;
	p = 5 - 4 * r;
	x = 0;
	y = r;
	int _8x = 0;
	int	_8y = 8 * r;

	while (x <= y)
	{
		SetLimit(x + xc, y + yc);
		SetLimit(x + xc, -y + yc);
		SetLimit(-x + xc, y + yc);
		SetLimit(-x + xc, -y + yc);

		SetLimit(y + xc, x + yc);
		SetLimit(y + xc, -x + yc);
		SetLimit(-y + xc, x + yc);
		SetLimit(-y + xc, -x + yc);
		if (p > 0)
		{
			p += _8x - _8y + 20;
			y--;
			_8y -= 8;
		}
		else
			p += _8x + 12;
		x++;
		_8x += 8;
	}

}

CCanvas::PIXEL CCanvas::PointSampler(float s, float t)
{
	float p = s + 0.5f, q = t + 0.5f;
	int i = (int)p, j = (int)q;
	return Address(i, j);
}

CCanvas::PIXEL CCanvas::BilinearSampler(float s, float t)
{
	PIXEL A, B, C, D;
	int p = (int)s;
	int q = (int)t;
	// 1- Calcular los parametros de interpolacion (a,b) /* como mejorar la calidad! */
	/* fraq=(f-i) */
	int a, b;
	if (s < 0)
	{
		a = 255 + (s - p) * 256;
		p--;
	}
	else
		a = (s - p) * 256;
	if (t < 0)
	{
		b = 255 + (t - q) * 256;
		q--;
	}
	else
		b = (t - q) * 256;
	// 2- Interpolar bilinealmente
	A = Address(p, q);
	B = Address(p + 1, q);
	C = Address(p, q + 1);
	D = Address(p + 1, q + 1);
	return Lerp(Lerp(A, B, a), Lerp(C, D, a), b);

}

void CCanvas::InverseMapping(MATRIX4D M, CCanvas* pSource, PIXEL(CCanvas::* sampler)(float, float))
{
	// x' = x*M.m00 + y*M.m10 + z*M.m20 + 1*M.m30
	// y' = x*M.m01 + y*M.m11 + z*M.m21 + 1*M.m31

	float _x, __x, _y, __y;

	_y = M.m31;
	_x = M.m30;
	for (int y = 0; y < m_nSizeY; y++)
	{
		__x = _x;
		__y = _y;
		if (m_pLimits[y].left < m_pLimits[y].right)
		{
			int c = m_pLimits[y].right - m_pLimits[y].left;
			__x += m_pLimits[y].left * M.m00;
			__y += m_pLimits[y].left * M.m01;
			PIXEL* pPixel = &(*this)(m_pLimits[y].left, y);
			while (c--)
			{
				*pPixel++ = (pSource->*sampler)(__x, __y);
				__x += M.m00;
				__y += M.m01;
			}
		}
		_x += M.m10;
		_y += M.m11;
	}
}

CCanvas::PIXEL CCanvas::Address(int i, int j)
{
	int p, q;
	switch (m_am)
	{
	case ADDRESS_MODE_BORDER:
		return (*this)(i, j);
	case ADDRESS_MODE_WRAP:
		i = i % m_nSizeX;
		j = j % m_nSizeY;
		if (i < 0) i = (m_nSizeX - 1) + i;
		if (j < 0) j = (m_nSizeY - 1) + j;
		return *(PIXEL*)((char*)m_pBuffer + (size_t)m_nPitch * j + i * sizeof(PIXEL));
	case ADDRESS_MODE_MIRROR:
		p = i / m_nSizeX;
		q = j / m_nSizeY;
		i = abs(i % m_nSizeX);
		j = abs(j % m_nSizeY);
		i = ((p & 1) ? (m_nSizeX - 1) - i : i);
		j = ((q & 1) ? (m_nSizeY - 1) - j : j);
		return *(PIXEL*)((char*)m_pBuffer + (size_t)m_nPitch * j + i * sizeof(PIXEL));
	case ADDRESS_MODE_CLAMP:
		i = max(0, min(i, m_nSizeX - 1));
		j = max(0, min(j, m_nSizeY - 1));

		return *(PIXEL*)((char*)m_pBuffer + (size_t)m_nPitch * j + i * sizeof(PIXEL));
	}


}

#include<fstream>
#include<Windows.h>
using namespace std;

bool CCanvas::CreateFileFromCanvas(const char* pszFileName)
{
	fstream file;
	file.open(pszFileName, ios::out | ios::binary);
	if (!file.is_open())
		return false;
	BITMAPFILEHEADER bif;
	BITMAPINFOHEADER bih;
	memset(&bif, 0, sizeof(bif));
	memset(&bih, 0, sizeof(bih));
	//1.- File header
	bif.bfType = 'MB';
	bif.bfSize = sizeof(bif) + sizeof(bih) + m_nSizeX * m_nSizeY * sizeof(PIXEL);
	bif.bfOffBits = sizeof(bif) + sizeof(bih);
	file.write((char*)&bif, sizeof(bif));
	//2- Bitmap header
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = m_nSizeX;
	bih.biHeight = m_nSizeY;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biClrImportant = 0;
	bih.biCompression = BI_RGB;
	bih.biXPelsPerMeter = 3000;
	bih.biYPelsPerMeter = 3000;
	bih.biSizeImage = m_nSizeX * (m_nSizeY * sizeof(PIXEL));
	file.write((char*)&bih, sizeof(bih));
	//3.- Escribir bits de la imagen
	for (int j = m_nSizeY - 1; j >= 0; j--)
	{
		for (int i = 0; i < m_nSizeX; i++)
		{
			PIXEL p = (*this)(i, j);
			file.write((char*)&p.b, sizeof(p.b));
			file.write((char*)&p.g, sizeof(p.g));
			file.write((char*)&p.r, sizeof(p.r));
			file.write((char*)&p.a, sizeof(p.a));

		}
	}

	file.close();
	return true;
}

CCanvas* CCanvas::CreateCanvasFromFile(const char* pszFileName)
{
	CCanvas* pNewCanvas;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	memset(&bfh, 0, sizeof(bfh));
	memset(&bih, 0, sizeof(bih));
	fstream file;
	file.open(pszFileName, ios::in | ios::binary);
	if (!file.is_open())
		return nullptr;
	file.read((char*)&bfh, sizeof(bfh));
	if (bfh.bfType != 'MB')
		return nullptr;
	file.read((char*)&bih, sizeof(bih));

	pNewCanvas = CCanvas::Create(bih.biWidth, bih.biHeight);

	//3.- Escribir bits de la imagen
	int nRowLength = (((bih.biWidth * bih.biBitCount) + 31) / 32) * 4;
	unsigned char* pRow = new unsigned char[nRowLength];
	RGBQUAD Pal[256];
	switch (bih.biBitCount)
	{
	case 32:
		for (int j = pNewCanvas->m_nSizeY - 1; j >= 0; j--)
		{
			for (int i = 0; i < pNewCanvas->m_nSizeX; i++)
			{
				PIXEL p = (*pNewCanvas)(i, j);
				file.write((char*)&p.b, sizeof(p.b));
				file.write((char*)&p.g, sizeof(p.g));
				file.write((char*)&p.r, sizeof(p.r));
				file.write((char*)&p.a, sizeof(p.a));

			}
		}
		break;
	case 24:
		for (int j = pNewCanvas->m_nSizeY - 1; j >= 0; j--)
		{
			file.read((char*)pRow, nRowLength);
			for (int i = 0; i < pNewCanvas->m_nSizeX; i++)
			{
				PIXEL P = { pRow[3 * i + 2], pRow[3 * i + 1],pRow[3 * i + 0], 255 };
				(*pNewCanvas)(i, j) = P;
			}
		}
		break;
	case 8:
		file.read((char*)Pal, (bih.biClrUsed ? bih.biClrUsed : 256) * sizeof(RGBQUAD));
		for (int j = pNewCanvas->m_nSizeY - 1; j >= 0; j--)
		{
			file.read((char*)pRow, nRowLength);
			for (int i = 0; i < pNewCanvas->m_nSizeX; i++)
			{
				RGBQUAD Color = Pal[pRow[i]];
				(*pNewCanvas)(i, j) = { Color.rgbRed, Color.rgbGreen, Color.rgbBlue, 255 };
			}
		}
		break;
	case 4: //16 colors tarea !!! mucha manipulacion de bits!!!!
		break;
	case 1: // 2 colors
		break;
	}
	delete[] pRow;
	return pNewCanvas;
}

void CCanvas::InverseTextureMapping(
	const VERTEX V[], CCanvas* pTexture,
	PIXEL(CCanvas::* sampler)(float, float))
{
	MATRIX4D M = Identity();
	float det;
	det = V[0].Position.x * (V[1].Position.y - V[2].Position.y) +
		V[0].Position.y * (V[2].Position.x - V[1].Position.x) +
		(V[1].Position.x * V[2].Position.y - V[2].Position.x * V[1].Position.y);
	if (det < 0) return;
	M.m00 = (V[1].TexCoord.x * V[2].Position.y -
		V[2].TexCoord.x * V[1].Position.y) +
		(V[2].TexCoord.x * V[0].Position.y -
			V[0].TexCoord.x * V[2].Position.y) +
		(V[0].TexCoord.x * V[1].Position.y -
			V[1].TexCoord.x * V[0].Position.y);
	M.m10 = (V[1].Position.x * V[2].TexCoord.x -
		V[2].Position.x * V[1].TexCoord.x) +
		(V[2].Position.x * V[0].TexCoord.x -
			V[0].Position.x * V[2].TexCoord.x) +
		(V[0].Position.x * V[1].TexCoord.x -
			V[1].Position.x * V[0].TexCoord.x);
	M.m30 = V[0].Position.x * (
		V[1].Position.y * V[2].TexCoord.x -
		V[2].Position.y * V[1].TexCoord.x) +
		V[0].Position.y * (
			V[2].Position.x * V[1].TexCoord.x -
			V[1].Position.x * V[2].TexCoord.x) +
		V[0].TexCoord.x * (
			V[1].Position.x * V[2].Position.y -
			V[2].Position.x * V[1].Position.y);

	M.m01 = (V[1].TexCoord.y * V[2].Position.y -
		V[2].TexCoord.y * V[1].Position.y) +
		(V[2].TexCoord.y * V[0].Position.y -
			V[0].TexCoord.y * V[2].Position.y) +
		(V[0].TexCoord.y * V[1].Position.y -
			V[1].TexCoord.y * V[0].Position.y);
	M.m11 = (V[1].Position.x * V[2].TexCoord.y -
		V[2].Position.x * V[1].TexCoord.y) +
		(V[2].Position.x * V[0].TexCoord.y -
			V[0].Position.x * V[2].TexCoord.y) +
		(V[0].Position.x * V[1].TexCoord.y -
			V[1].Position.x * V[0].TexCoord.y);
	M.m31 = V[0].Position.x * (
		V[1].Position.y * V[2].TexCoord.y -
		V[2].Position.y * V[1].TexCoord.y) +
		V[0].Position.y * (
			V[2].Position.x * V[1].TexCoord.y -
			V[1].Position.x * V[2].TexCoord.y) +
		V[0].TexCoord.y * (
			V[1].Position.x * V[2].Position.y -
			V[2].Position.x * V[1].Position.y);
	det = 1.0f / det;
	M.m00 *= det * (pTexture->GetSizeX() - 1);
	M.m10 *= det * (pTexture->GetSizeX() - 1);
	M.m30 *= det * (pTexture->GetSizeX() - 1);
	M.m01 *= det * (pTexture->GetSizeY() - 1);
	M.m11 *= det * (pTexture->GetSizeY() - 1);
	M.m31 *= det * (pTexture->GetSizeY() - 1);
	//M es una matriz que transforma cualquier punto (x,y
	ResetLimits();
	LineLimits(V[0].Position.x, V[0].Position.y, V[1].Position.x, V[1].Position.y);
	LineLimits(V[1].Position.x, V[1].Position.y, V[2].Position.x, V[2].Position.y);
	LineLimits(V[2].Position.x, V[2].Position.y, V[0].Position.x, V[0].Position.y);
	InverseMapping(M, pTexture, sampler);

}