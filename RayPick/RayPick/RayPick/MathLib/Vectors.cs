using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace RayPick.MathLib
{

    /// <summary>
    /// 表示空间点和向量
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec3
    {
        public double x, y, z;

        public Vec3(double vx, double vy, double vz) { x = vx; y = vy; z = vz; }

        /// <summary>
        /// 转为单位向量
        /// </summary>
        public void Normalise()
        {
            double l = 1 / Math.Sqrt(x * x + y * y + z * z);
            x *= l; y *= l; z *= l;
        }

        //求距离
        public static double operator &(Vec3 a, Vec3 b)
        { return Math.Sqrt((Math.Pow(b.x - a.x, 2) + Math.Pow(b.y - a.y, 2)) + Math.Pow(b.z - a.z, 2)); }

        //向量积
        public static Vec3 operator %(Vec3 U, Vec3 V)
        {
            Vec3 ret;
            ret.x = (U.y * V.z) - (U.z * V.y);
            ret.y = (U.z * V.x) - (U.x * V.z);
            ret.z = (U.x * V.y) - (U.y * V.x);
            return ret;
        }

        //数量积
        public static double operator *(Vec3 a, Vec3 b) { return (((a.x * b.x) + (a.y * b.y)) + (a.z * b.z)); }

        public static Vec3 operator -(Vec3 a, Vec3 b) { return new Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
        public static Vec3 operator +(Vec3 a, Vec3 b) { return new Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }


    }


    [StructLayout(LayoutKind.Sequential)]
    public struct Vec2i
    {
        public int x, y;

        public Vec2i(int vx, int vy) { x = vx; y = vy; }
        public static Vec2i operator -(Vec2i a, System.Drawing.Point b) { return new Vec2i(a.x - b.X, a.y - b.Y); }

    }
}
