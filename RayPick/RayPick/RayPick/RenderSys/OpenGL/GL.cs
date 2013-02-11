using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace RayPick.RenderSys.OpenGL
{
    [Serializable]
    public class GL
    {
        [DebuggerNonUserCode]
        private GL() { }
        [DllImport(OGLLib)]
        public static extern void glBegin(uint mode);
        [DllImport(OGLLib)]
        public static extern void glClear(uint mask);
        [DllImport(OGLLib)]
        public static extern void glEnable(uint cap);
        [DllImport(OGLLib)]
        public static extern void glEnd();
        [DllImport(OGLLib)]
        public static extern void glGetDoublev(uint pname, double[] someParams);
        [DllImport(OGLLib)]
        public static extern void glGetIntegerv(uint pname, int[] someParams);
        [DllImport(OGLLib)]
        public static extern void glLightfv(uint light, uint pname, float[] someParams);
        [DllImport(OGLLib)]
        public static extern void glLoadIdentity();
        [DllImport(OGLLib)]
        public static extern void glMatrixMode(uint mode);
        [DllImport(OGLLib)]
        public static extern void glRotated(double angle, double x, double y, double z);
        [DllImport(OGLLib)]
        public static extern void glTranslated(double x, double y, double z);
        [DllImport(OGLLib)]
        public static extern void glVertex3d(double x, double y, double z);
        [DllImport(OGLLib)]
        public static extern void glViewport(int x, int y, int width, int height);
                
        public const uint GL_AMBIENT = 0x1200;    
        public const uint GL_AUTO_NORMAL = 0xd80;      
        public const uint GL_COLOR_BUFFER_BIT = 0x4000;   
        public const uint GL_DEPTH_BUFFER_BIT = 0x100;
        public const uint GL_DEPTH_TEST = 0xb71;
        public const uint GL_LIGHT0 = 0x4000;
        public const uint GL_LIGHTING = 0xb50;
        public const uint GL_MODELVIEW = 0x1700;
        public const uint GL_MODELVIEW_MATRIX = 0xba6;
        public const uint GL_PROJECTION = 0x1701;
        public const uint GL_PROJECTION_MATRIX = 0xba7;
        public const uint GL_TRIANGLES = 4;
        public const uint GL_VIEWPORT = 0xba2;
    
        private const string OGLLib = "OPENGL32.DLL";
    }

}

