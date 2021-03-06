//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2006 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
// BCGDrawManager.h: interface for the CBCGDrawManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGDRAWMANAGER_H__56EC7162_B29E_11D4_95C8_00A0C9289F1B__INCLUDED_)
#define AFX_BCGDRAWMANAGER_H__56EC7162_B29E_11D4_95C8_00A0C9289F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bcgcontrolbar.h"

class BCGCONTROLBARDLLEXPORT CBCGDrawManager : public CObject
{
public:
	CBCGDrawManager(CDC& dc);
	virtual ~CBCGDrawManager();

// Operatioins:
	BOOL DrawShadow (CRect rect,					// Shadow will be draw around this rect
					int nDepth,						// Shadow depth (pixels)
					int iMinBrightness = 100,		// Min. brighttness
					int iMaxBrightness = 50,		// Max. beightness
					CBitmap* pBmpSaveBottom = NULL,	// Bitmap may be saved in 
					CBitmap* pBmpSaveRight = NULL,	// two bitmaps
					BOOL bRightShadow = TRUE,		// Shadow on right side
					COLORREF clrBase =(COLORREF)-1);	// Base color

	BOOL HighlightRect (CRect rect,					// Rectangle to be highlighted
						int nPercentage = -1,		// < 100 - darker, > 100 - lighter, -1 - default light
						COLORREF clrTransparent =	// Don't change this color
							(COLORREF)-1,
						int nTolerance = 0,
						COLORREF clrBlend = (COLORREF)-1);

	void FillGradient (	CRect rect, 
						COLORREF colorStart, COLORREF colorFinish,
						BOOL bHorz = TRUE,
						int nStartFlatPercentage = 0,
						int nEndFlatPercentage = 0);

	void FillGradient2 (CRect rect, COLORREF colorStart, COLORREF colorFinish, 
						int nAngle = 0 /* 0 - 360 */);

	BOOL GrayRect (	CRect rect,					// Rectangle to be grayed
					int nPercentage = -1,		// < 100 - darker, > 100 - lighter, -1 - default light
					COLORREF clrTransparent =	// Don't change this color
							(COLORREF)-1,
					COLORREF clrDisabled =		// Use this color for desaturation
							(COLORREF)-1);

	void MirrorRect (CRect rect, BOOL bHorz = TRUE);

	BOOL DrawGradientRing (	CRect rect,
							COLORREF colorStart, COLORREF colorFinish,
							COLORREF colorBorder,
							int nAngle /* 0 - 360 */,
							int nWidth,
							COLORREF clrFace = (COLORREF) -1);
// Helpers:
	static COLORREF PixelAlpha (COLORREF srcPixel, int percent);
	static COLORREF PixelAlpha (COLORREF srcPixel, double percentR, double percentG, double percentB);
	static COLORREF PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent);
	static COLORREF PixelAlpha (COLORREF srcPixel, int percentR, int percentG, int percentB);
	static void SetAlphaPixel (COLORREF* pBits, CRect rect, int x, int y, int percent, int iShadowSize,
		COLORREF clrBase = (COLORREF)-1, BOOL bIsRight = TRUE);
	static void SetPixel (COLORREF* pBits, int cx, int cy, int x, int y, COLORREF color);

// Conversion between the HSL (Hue, Saturation, and Luminosity) and RGB:
	static double HuetoRGB (double m1, double m2, double h);
	static BYTE HueToRGB (float rm1, float rm2, float rh);
	static COLORREF HLStoRGB_ONE (double H, double L, double S);
	static COLORREF HLStoRGB_TWO (double H, double L, double S);
	static void RGBtoHSL (COLORREF rgb, double *H, double *S, double *L);

// Conversion between the HSV (Hue/Saturation/Value) and RGB:
	static void RGBtoHSV (COLORREF rgb, double *H, double *S, double *V);
	static COLORREF HSVtoRGB (double H, double S, double V);

	static COLORREF SmartMixColors (COLORREF color1, COLORREF color2,
		double dblLumRatio = 1., int k1 = 1, int k2 = 1);

// Attributes:
protected:
	CDC&	m_dc;
};

#endif // !defined(AFX_BCGDRAWMANAGER_H__56EC7162_B29E_11D4_95C8_00A0C9289F1B__INCLUDED_)
