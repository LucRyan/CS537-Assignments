using System;
using System.Collections.Generic;
using System.Text;
using RayPick.RenderSys.OpenGL;

namespace RayPick.RenderSys.Base
{
    /// <summary>
    /// 用于初始化OpenGL的类
    /// </summary>
    class CDevice
    {
        protected int myDC, myRC;
        protected IntPtr myHandle;
        public CDevice(IntPtr handle)
        {
            myDC = WGL.GetDC(handle.ToInt32());
            myHandle = handle;
            WGL.wglSwapBuffers(myDC);

            WGL.PIXELFORMATDESCRIPTOR pfd = new WGL.PIXELFORMATDESCRIPTOR();

            pfd.nVersion = 1;
            pfd.dwFlags = (WGL.PFD_DRAW_TO_WINDOW | WGL.PFD_SUPPORT_OPENGL | WGL.PFD_DOUBLEBUFFER);
            pfd.iPixelType = (byte)(WGL.PFD_TYPE_RGBA);
            pfd.cColorBits = 32;
            pfd.cDepthBits = 32;
            pfd.iLayerType = (byte)(WGL.PFD_MAIN_PLANE);
            pfd.nSize = 40;

            int pm;
            uint ret;
            pm = WGL.ChoosePixelFormat(myDC, ref pfd);
            if (pm != 0)
            {
                if (WGL.SetPixelFormat(myDC, pm, ref pfd) != 0)
                {
                    myRC = WGL.wglCreateContext(myDC);
                    ret = WGL.wglMakeCurrent(myDC, myRC);
                    if (ret == 0) { throw new Exception(""); }
                }
                else { throw new Exception(""); }
            }
            else { throw new Exception(""); }
        }
        public void EndRendering()
        {
            WGL.wglSwapBuffers(myDC);
        }
        public void Dispose()
        {
            WGL.wglMakeCurrent(myDC, myRC);
            WGL.wglDeleteContext(myRC);
            WGL.ReleaseDC(myHandle.ToInt32(), myDC);
        }

    }

}

