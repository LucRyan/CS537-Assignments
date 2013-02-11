using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using RayPick.MathLib;

namespace RayPick.RenderSys.OpenGL
{
    public class WGL
    {
        [DebuggerNonUserCode]
        private WGL() { }
        [DllImport("gdi32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int ChoosePixelFormat(int hdc, ref PIXELFORMATDESCRIPTOR pfd);
        [DllImport("user32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int GetCursorPos(ref Vec2i lpPoint);
        [DllImport("user32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int GetDC(int hwnd);
        [DllImport("user32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int ReleaseDC(int hwnd, int hDC);
        [DllImport("user32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int SetCursorPos(int x, int y);
        [DllImport("gdi32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int SetPixelFormat(int hdc, int pm, ref PIXELFORMATDESCRIPTOR pfd);
        [DllImport("opengl32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int wglCreateContext(int dc);
        [DllImport("opengl32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int wglDeleteContext(int hglrc);
        [DllImport("opengl32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern uint wglMakeCurrent(int hdc, int hglrc);
        [DllImport("opengl32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int wglSwapBuffers(int hdc);
        [DllImport("opengl32", EntryPoint = "wglUseFontBitmapsA", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        public static extern int wglUseFontBitmaps(int hdc, int first, int count, uint listBase);

        public const uint PFD_DEPTH_DONTCARE = 0x20000000;
        public const uint PFD_DOUBLEBUFFER = 1;
        public const uint PFD_DOUBLEBUFFER_DONTCARE = 0x40000000;
        public const uint PFD_DRAW_TO_BITMAP = 8;
        public const uint PFD_DRAW_TO_WINDOW = 4;
        public const uint PFD_GENERIC_ACCELERATED = 0x1000;
        public const uint PFD_GENERIC_FORMAT = 0x40;
        public const uint PFD_MAIN_PLANE = 0;
        public const uint PFD_NEED_PALETTE = 0x80;
        public const uint PFD_NEED_SYSTEM_PALETTE = 0x100;
        public const uint PFD_OVERLAY_PLANE = 1;
        public const uint PFD_STEREO = 2;
        public const uint PFD_SUPPORT_DIRECTDRAW = 0x2000;
        public const uint PFD_SUPPORT_GDI = 0x10;
        public const uint PFD_SUPPORT_OPENGL = 0x20;
        public const uint PFD_SWAP_COPY = 0x400;
        public const uint PFD_SWAP_EXCHANGE = 0x200;
        public const uint PFD_SWAP_LAYER_BUFFERS = 0x800;
        public const uint PFD_TYPE_COLORINDEX = 1;
        public const uint PFD_TYPE_RGBA = 0;
        public const uint PFD_UNDERLAY_PLANE = 0xff;

        [StructLayout(LayoutKind.Sequential)]
        public struct PIXELFORMATDESCRIPTOR
        {
            public ushort nSize;
            public ushort nVersion;
            public uint dwFlags;
            public byte iPixelType;
            public byte cColorBits;
            public byte cRedBits;
            public byte cRedShift;
            public byte cGreenBits;
            public byte cGreenShift;
            public byte cBlueBits;
            public byte cBlueShift;
            public byte cAlphaBits;
            public byte cAlphaShift;
            public byte cAccumBits;
            public byte cAccumRedBits;
            public byte cAccumGreenBits;
            public byte cAccumBlueBits;
            public byte cAccumAlphaBits;
            public byte cDepthBits;
            public byte cStencilBits;
            public byte cAuxBuffers;
            public byte iLayerType;
            public byte bReserved;
            public uint dwLayerMask;
            public uint dwVisibleMask;
            public uint dwDamageMask;
        }
    }
 

}
