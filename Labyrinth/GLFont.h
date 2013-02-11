#ifndef _GL_FONT_H_
#define _GL_FONT_H_

#include <windows.h>
#include "gl/glut.h"

class GLFont
{
    HFONT hFont;
    COLORREF color;

public:
    GLFont();
    ~GLFont();

    void SetTextColor(COLORREF textcolor);

    void CreateFont(wchar_t *facename, int height, int weight, bool italic,bool underline,bool strikeout);
  

	void ShowText(int x, int y, const char *str);


    void Show2DText(const char *str);


    void Show3DText(unsigned char *str);
};

#endif //_GL_FONT_H_