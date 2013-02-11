using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using RayPick.RenderSys;
using RayPick.RenderSys.Base;
using RayPick.RenderSys.OpenGL;
using RayPick.MathLib;

namespace RayPick
{
    public partial class Form1 : Form
    {
        Vec2i mpos;
        CDevice decice;
        Vec3 rayStart = new Vec3(), rayEnd = new Vec3();

        CTestMesh[] obj = new CTestMesh[4];

        float yrot = 0;

        double[] mv = new double[16], prj = new double[16];
        
        public Form1()
        {
            InitializeComponent();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            decice = new CDevice(this.Handle);

            for (int i = 0; i <= 3; i++)
            { obj[i] = new CTestMesh(); }

            obj[0].Shift(new Vec3(0, 0, -15));
            obj[1].Shift(new Vec3(0, 0, 15));
            obj[2].Shift(new Vec3(15, 0, 5));
            obj[3].Shift(new Vec3(-15, 0, -5));

            GL.glEnable(GL.GL_AUTO_NORMAL);
            GL.glEnable(GL.GL_LIGHTING);

            GL.glLightfv(GL.GL_LIGHT0, GL.GL_AMBIENT, new float[4] { 0.5f, 0.5f, 0.5f, 0 });
            GL.glEnable(GL.GL_LIGHT0);

            GL.glEnable(GL.GL_DEPTH_TEST);
            SetView();
        }
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            decice.Dispose();
        }
        private void SetView()
        {
            GL.glViewport(0, 0, ClientSize.Width, ClientSize.Height);

            GL.glMatrixMode(GL.GL_PROJECTION);
            GL.glLoadIdentity();
            GLU.gluPerspective(90.0, ClientSize.Width / ClientSize.Height, 0.1, 2500);

            GL.glMatrixMode(GL.GL_MODELVIEW);
            GL.glLoadIdentity();
        }
        private void Form1_Resize(object sender, EventArgs e)
        {
            SetView();
        }

        //--------------------------------------核心代码-------------------------------------

        private void timer1_Tick(object sender, EventArgs e)
        {
            GL.glLoadIdentity();

            GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
            GL.glRotated(15, 1, 0, 0);

            GL.glRotated(yrot, 0, 1, 0);
            GL.glTranslated(0, -5, 0);
            yrot += 0.3f;

            PickRay(); 

            for (int i = 0; i <= 3; i++) { obj[i].Render(); }

            decice.EndRendering();
        }

        private void PickRay()
        {
            double o1x, o1y, o1z;
            double o2x, o2y, o2z;

            WGL.GetCursorPos(ref mpos);
            Point p = this.PointToClient(new Point(mpos.x, mpos.y));
            mpos.x = p.X; mpos.y = p.Y;

            //更新矩阵 
            GL.glGetDoublev(GL.GL_MODELVIEW_MATRIX , mv);
            GL.glGetDoublev(GL.GL_PROJECTION_MATRIX, prj);

            int[] viewPort = new int[4];
            GL.glGetIntegerv(GL.GL_VIEWPORT, viewPort);

            GLU.gluUnProject(mpos.x, (viewPort[3] - mpos.y - 1), 0.0, mv, prj, viewPort, out o1x, out o1y, out o1z);
            GLU.gluUnProject(mpos.x, (viewPort[3] - mpos.y - 1), 1.0, mv, prj, viewPort, out o2x, out o2y, out o2z);

            rayStart = new Vec3(o1x, o1y, o1z);
            rayEnd = new Vec3(o2x, o2y, o2z);

        }

        private static Vec3 CalcPlaneNormal(Vec3 P1, Vec3 P2, Vec3 P3)
        {
            Vec3 V1, V2;
            Vec3 result;

            V1 = P2 - P1;
            V2 = P3 - P1;

            result = V1 % V2;

            result.Normalise();

            return result;
        }

        public static bool IsInTrg(Vec3 vtxA, Vec3 vtxB, Vec3 vtxC, TriEqaResult pHit)
        {
            LineEqv l1, l2, l3;

            l1 = new LineEqv(vtxA, vtxB - vtxA);
            l2 = new LineEqv(vtxC, vtxA - vtxC);
            l3 = new LineEqv(vtxB, vtxC - vtxB);

            double[] res1 = new double[3], res2 = new double[3];

            Vec3 pos = new Vec3(pHit.x, pHit.y, pHit.z);

            l1.TestPoint(pos, ref res1[0], ref res2[0]);
            l2.TestPoint(pos, ref res1[1], ref res2[1]);
            l3.TestPoint(pos, ref res1[2], ref res2[2]);

            int i;
            int ca = 0, cm = 0;
            for (i = 0; i <= 2; i++)
            {
                if (res1[i] < 0) { cm++; }
                if (res1[i] >= 0) { ca++; }

            }

            if ((ca == 3) | (cm == 3))
            {
                ca = 0;
                cm = 0;
                for (i = 0; i <= 2; i++)
                {
                    if (res2[i] < 0) { cm++; }
                    if (res2[i] >= 0) { ca++; }
                }

                if ((ca == 3) | (cm == 3))
                {
                    return true;
                }
                else 
                {
                    ca = 0; cm = 0;
                    for (i = 0; i <= 2; i++)
                    {
                        if (res2[i] <= 0) { cm++; }
                        if (res2[i] > 0) { ca++; }
                    }
                    if ((ca == 3) | (cm == 3))
                    {
                        return true;
                    }
                    else 
                    {
                        return false;
                    }
                }
            }
            else
            {
                ca = 0; cm = 0;
                for (i = 0; i <= 2; i++)
                {
                    if (res1[i] <= 0) { cm++; }
                    if (res1[i] > 0) { ca++; }

                }

                if ((ca == 3) | (cm == 3)) 
                {
                    ca = 0;
                    cm = 0;
                    for (i = 0; i <= 2; i++)
                    {
                        if (res2[i] < 0) { cm++; }
                        if (res2[i] >= 0) { ca++; }
                    }

                    if ((ca == 3) | (cm == 3))
                    {
                        return true;
                    }
                    else
                    {
                        ca = 0; cm = 0;
                        for (i = 0; i <= 2; i++)
                        {
                            if (res2[i] <= 0) { cm++; }
                            if (res2[i] > 0) { ca++; }
                        }
                        if ((ca == 3) | (cm == 3))
                        {
                            return true;
                        }
                        else
                        { return false; }
                    }
                } 
                else 
                { return false; }
            }
        }

        public bool Intersect(Vec3 vtxA, Vec3 vtxB, Vec3 vtxC, ref Vec3 pos)
        {
            PlaneEqv curFace = new PlaneEqv(vtxA, CalcPlaneNormal(vtxA, vtxB, vtxC));
            Vec3 ori = rayEnd - rayStart;
            LineEqv curSpd = new LineEqv(rayStart, ori);

            TriEqaResult pHit = new TriEqaResult(curSpd, curFace);

            if (!pHit.hasSolution) { return false; }

            pos = new Vec3(pHit.x, pHit.y, pHit.z);

            if ((pos - rayStart) * ori > 0)
            {
                return IsInTrg(vtxA, vtxB, vtxC, pHit);
            }
            else { return false; }
        }

        private void Form1_Click(object sender, EventArgs e)
        {
            int bestIdx = -1;
            double bestDist = double.MaxValue;

            for (int i = 0; i < 4; i++)
            {
                Vec3 intersect = new Vec3();
                if (Intersect(obj[i].vtx[obj[i].faces.idxA], obj[i].vtx[obj[i].faces.idxB], obj[i].vtx[obj[i].faces.idxC], ref intersect))
                {
                    double dist = intersect & rayStart;
                    if (dist < bestDist)
                    {
                        bestDist = dist; bestIdx = i;
                    }
                }
            }
            if (bestIdx != -1) { System.Windows.Forms.MessageBox.Show(bestIdx.ToString()); }

        }

    }

    public class CTestMesh 
    {
        public struct Face 
        {
            public int idxA, idxB, idxC;
        }

        public Vec3[] vtx = new Vec3[3];
        public Face faces;

        public CTestMesh()
        {
            vtx[0].x = 5; vtx[0].y = 0; vtx[0].z = 0.1;
            vtx[1].x = 0; vtx[1].y = 5; vtx[1].z = 0.1;
            vtx[2].x = 0; vtx[2].y = 0; vtx[2].z = 0.1;
            faces.idxA = 0; faces.idxB = 2; faces.idxC = 1;
        }

        public void Shift(Vec3 offset)
        {
            for (int i = 0; i < 3; i++) { vtx[i] = vtx[i] + offset; }
        }

        public void Render()
        {
            GL.glBegin(GL.GL_TRIANGLES);

            GL.glVertex3d(vtx[faces.idxA].x, vtx[faces.idxA].y, vtx[faces.idxA].z);
            GL.glVertex3d(vtx[faces.idxB].x, vtx[faces.idxB].y, vtx[faces.idxB].z);
            GL.glVertex3d(vtx[faces.idxC].x, vtx[faces.idxC].y, vtx[faces.idxC].z);

            GL.glEnd();
        }
    }
}

