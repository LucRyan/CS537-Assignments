using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace RayPick.RenderSys.OpenGL
{
    public class GLU
    {
        [DebuggerNonUserCode]
        private GLU() { }
        [DllImport(GLULib)]
        public static extern void gluPerspective(double fovy, double aspect, double zNear, double zFar);
        [DllImport(GLULib)]
        public static extern int gluUnProject(double winX, double winY, double winZ, double[] model, double[] proj, int[] view, out double objX, out double objY, out double objZ);


        private const string GLULib = "GLU32.DLL";


    }

}
