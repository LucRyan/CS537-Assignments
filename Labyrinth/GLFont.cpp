#include "StdAfx.h " 

#include "BitmapFont.h " 

CBitmapFont::CBitmapFont(CDC* dc,char* fontname,char* string,long Hight) 
{ 
bool bb = true; 
DWORD word; 

m_iCount = strlen(string); 
if (dc && fontname && strlen(fontname) > 0) 
{ 
glColor4f(1.0f,1.0f,1.0f,1.0f); 
m_listbase = glGenLists(128); 

LOGFONT logfont; 
logfont.lfHeight = Hight; 
logfont.lfWidth = 0; 
logfont.lfEscapement = 0; 
logfont.lfOrientation = logfont.lfEscapement; 
logfont.lfWeight = FW_NORMAL; 
logfont.lfItalic = FALSE; 
logfont.lfUnderline = FALSE; 
logfont.lfStrikeOut = FALSE; 
logfont.lfCharSet = GB2312_CHARSET;//ANSI_CHARSET; 
logfont.lfOutPrecision = OUT_DEFAULT_PRECIS; 
logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
logfont.lfQuality = DEFAULT_QUALITY; 
logfont.lfPitchAndFamily = VARIABLE_PITCH | TMPF_TRUETYPE | FF_MODERN; 
lstrcpy(logfont.lfFaceName, "&ordm;&Uacute;&Igrave;&aring; "); 

CFont font, OldFont; 
CFont* oldfont = NULL; 
oldfont = &OldFont; 
BOOL success = font.CreateFontIndirect(&logfont); 
oldfont = dc-> SelectObject(&font); 
bb = wglUseFontBitmaps(dc-> m_hDC, 0, 128, m_listbase); 
if(!bb)word = GetLastError(); 
if (!success || !bb) 
{ 
if(success)oldfont-> DeleteObject(); 
glDeleteLists(m_listbase, 128); 
m_listbase = 0; 
} 
else 
{ 
dc-> SelectObject(oldfont); 
} 
} 
} 

CBitmapFont::~CBitmapFont() 
{ 
// Class destructor. 

glDeleteLists(m_listbase, 128); 
m_listbase = 0; 
} 


void CBitmapFont::DrawStringAt(GLfloat x,GLfloat y,GLfloat z,char* s) 
{ 
// Draws the given text string at the given location. 

GLsizei len = GLsizei(strlen(s)); 
if (s && len > 0) 
{ 
glRasterPos3f(x, y, z); 
// Must save/restore the list base. 
glPushAttrib(GL_LIST_BIT); 
{ 
glListBase(m_listbase); 
glCallLists(len, GL_UNSIGNED_BYTE, (const GLvoid*)s);