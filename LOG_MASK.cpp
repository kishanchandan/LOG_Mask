// kingimageView.cpp : implementation of the CKingimageView class
//

#include "stdafx.h"
#include "kingimage.h"

#include "kingimageDoc.h"
#include "kingimageView.h"
#include <ctgmath>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKingimageView

IMPLEMENT_DYNCREATE(CKingimageView, CScrollView)

BEGIN_MESSAGE_MAP(CKingimageView, CScrollView)
	//{{AFX_MSG_MAP(CKingimageView)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_PROCESS, OnProcess)
	ON_COMMAND(ID_any, Onany)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_SOBEL, &CKingimageView::OnSobel)
	ON_COMMAND(ID_USHARPMASKING, &CKingimageView::OnUnsharpmasking)
	ON_COMMAND(ID_LOGMASK7, &CKingimageView::OnLogmask7)
	ON_COMMAND(ID_LOGMASK11, &CKingimageView::OnLogmask11)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKingimageView construction/destruction

CKingimageView::CKingimageView()
{
	// TODO: add construction code here

}

CKingimageView::~CKingimageView()
{
}

BOOL CKingimageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView drawing

void CKingimageView::OnDraw(CDC* pDC)
{
	CKingimageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//	pDC->SetStretchBltMode(COLORONCOLOR);
	//	int x,y,bytes;
	if (pDoc->imagetype == PCX)
		(pDoc->_pcx)->draw(pDC);
	if (pDoc->imagetype == BMP)
		(pDoc->_bmp)->draw(pDC);
	if (pDoc->imagetype == GIF)
		(pDoc->_gif)->draw(pDC);
	if (pDoc->imagetype == JPG)
		(pDoc->_jpg)->draw(pDC);

}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView printing

BOOL CKingimageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKingimageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKingimageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView diagnostics

#ifdef _DEBUG
void CKingimageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CKingimageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CKingimageDoc* CKingimageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKingimageDoc)));
	return (CKingimageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKingimageView message handlers



void CKingimageView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CSize totalSize = CSize(::GetSystemMetrics(SM_CXSCREEN),
		::GetSystemMetrics(SM_CYSCREEN));
	CSize pageSize = CSize(totalSize.cx / 2, totalSize.cy / 2);
	CSize lineSize = CSize(totalSize.cx / 10, totalSize.cy / 10);

	SetScrollSizes(MM_TEXT, totalSize, pageSize, lineSize);
}

void CKingimageView::OnMouseMove(UINT nFlags, CPoint point)
{
	CScrollView::OnMouseMove(nFlags, point);
}




void CKingimageView::OnLogmask7()
{
	CKingimageDoc* pDoc = GetDocument();
	int iBitPerPixel = pDoc->_bmp->bitsperpixel;
	int iWidth = pDoc->_bmp->width;
	int iHeight = pDoc->_bmp->height;
	BYTE *pImg = pDoc->_bmp->point;

	int Wp = iWidth;
	if (iBitPerPixel == 8)  ////Grey scale 8 bits image
	{
		int r = iWidth % 4;
		int p = (4 - r) % 4;
		Wp = iWidth + p;

		double *LOG_Mask = new double[Wp*iHeight];
		int logfilter[7][7] = { 0 };
		int size = 7;

		for (int w = 0; w < iWidth; w++)
			for (int h = 0; h < iHeight; h++)
			{
				LOG_Mask[w*iWidth + h] = 0;
			}
		std::ofstream ofs;
		ofs.open("LOG_MASK7x7.txt", std::ofstream::out | std::ofstream::app);

		for (int i = -(size / 2); i <= (size / 2); i++)
		{
			for (int j = -(size / 2); j <= (size / 2); j++)
			{
				double log_filter = ((i*i + j*j - 2 * 1.4*1.4) * (exp(-(i*i + j*j) / (2 * 1.4*1.4))) / (2 * 3.14*1.4*1.4*1.4*1.4)) * 400;
				logfilter[i + (size / 2)][j + (size / 2)] = trunc(log_filter);

				ofs << logfilter[i + (size / 2)][j + (size / 2)] << "\t";

			}
			ofs << "\n";
		}
		for (int w = 0; w < iWidth; w++)
			for (int h = 0; h < iHeight; h++)
			{
				for (int kernely = 0; kernely < size; kernely++)
					for (int kernelx = 0; kernelx < size; kernelx++)
					{
						int pixelx = (w - (size / 2) + kernelx + iWidth) % iWidth;
						int pixely = (h - (size / 2) + kernely + iHeight) % iHeight;
						LOG_Mask[pixely * iWidth + pixelx] = LOG_Mask[pixely * iWidth + pixelx] + (logfilter[kernely][kernelx] * pImg[pixely * iWidth + pixelx]) / 49;
						LOG_Mask[pixely * iWidth + pixelx] = ((LOG_Mask[pixely * iWidth + pixelx] < 0) ? 0 : LOG_Mask[pixely * iWidth + pixelx]);
						LOG_Mask[pixely * iWidth + pixelx] = ((LOG_Mask[pixely * iWidth + pixelx] > 255) ? 255 : LOG_Mask[pixely * iWidth + pixelx]);

					}
			}

		for (int i = 3; i < iHeight - 3; i++)
			for (int j = 3; j < iWidth - 3; j++)
			{
				pImg[i*Wp + j] = LOG_Mask[i*Wp + j];
			}
	}
	OnDraw(GetDC());


}


void CKingimageView::OnLogmask11()
{
	CKingimageDoc* pDoc = GetDocument();
	int iBitPerPixel = pDoc->_bmp->bitsperpixel;
	int iWidth = pDoc->_bmp->width;
	int iHeight = pDoc->_bmp->height;
	BYTE *pImg = pDoc->_bmp->point;

	int Wp = iWidth;
	if (iBitPerPixel == 8)  ////Grey scale 8 bits image
	{
		int r = iWidth % 4;
		int p = (4 - r) % 4;
		Wp = iWidth + p;

		int size = 11;
		double *LOG_Mask = new double[Wp*iHeight];
		int logfilter[11][11] = { 0 };

		for (int w = 0; w < iWidth; w++)
			for (int h = 0; h < iHeight; h++)
			{
				LOG_Mask[w*iWidth + h] = 0;
			}

		std::ofstream ofs;
		ofs.open("LOG_MASK_11X11.txt", std::ofstream::out | std::ofstream::app);

		for (int i = -(size / 2); i <= (size / 2); i++)
		{
			for (int j = -(size / 2); j <= (size / 2); j++)
			{
				double log_filter = ((((i*i + j*j - 2.0 * 5 * 5)*(exp(-(i*i + j*j) / (2.0 * 5 * 5)))) / (2 * 3.14*5.0 * 5.0 * 5.0 * 5.0)) * -300);
				logfilter[i + (size / 2)][j + (size / 2)] = trunc(log_filter);
				ofs << logfilter[i + (size / 2)][j + (size / 2)] << "\t";

			}
			ofs << "\n";

		}


		for (int w = 0; w < iWidth; w++)
			for (int h = 0; h < iHeight; h++)
			{
				for (int kernely = 0; kernely < size; kernely++)
					for (int kernelx = 0; kernelx < size; kernelx++)
					{
						int pixelx = (w - (size / 2) + kernelx + iWidth) % iWidth;
						int pixely = (h - (size / 2) + kernely + iHeight) % iHeight;
						LOG_Mask[pixely * iWidth + pixelx] += (logfilter[kernely][kernelx] * pImg[pixely * iWidth + pixelx]) / 121;
						LOG_Mask[pixely * iWidth + pixelx] = ((LOG_Mask[pixely * iWidth + pixelx] < 0) ? 0 : LOG_Mask[pixely * iWidth + pixelx]);
						LOG_Mask[pixely * iWidth + pixelx] = ((LOG_Mask[pixely * iWidth + pixelx] > 255) ? 255 : LOG_Mask[pixely * iWidth + pixelx]);
					}
			}

		for (int i = 5; i < iHeight - 5; i++)
			for (int j = 5; j < iWidth - 5; j++)
			{
				pImg[i*Wp + j] = LOG_Mask[i*Wp + j];
			}
	}
	OnDraw(GetDC());
}
