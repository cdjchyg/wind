using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * //http://gad.qq.com/article/detail/36557
 * 
 * B（t）=（1-t）P0 + t P1
 * 
 * B（t）=（1-t）（（1-t）P0 + t P1）+ t（（1-t）P1 + t P2）  ->  B（t）=（1-t）2 P0 + 2（1-t）t P1 + t2 P2
 * 导数:  B'（t）= 2（1-t）（P1-P0）+ 2t（P2-P1）  (方向, 解释为沿着曲线移动的速度)
 * 
 * 
 */

public class Bezier{

    public static Vector3 GetPoint(Vector3 p0, Vector3 p1, Vector3 p2, float t)
    {
        //return Vector3.Lerp(p0, p2, t);
        //return Vector3.Lerp(Vector3.Lerp(p0, p1, t), Vector3.Lerp(p1, p2, t), t);

        t = Mathf.Clamp01(t);
        float oneMinusT = 1f - t;
        return oneMinusT * oneMinusT * p0
            + 2f * oneMinusT * t * p1
            + t * t * p2;
    }

    // 方向
    public static Vector3 GetFirstDerivative(Vector3 p0, Vector3 p1, Vector3 p2, float t)
    {
        return
            2f * (1f - t) * (p1 - p0) +
            2f * t * (p2 - p1);
    }

    public static Vector3 GetPoint(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
    {
        t = Mathf.Clamp01(t);
        float oneMinusT = 1f - t;
        return
            oneMinusT * oneMinusT * oneMinusT * p0 +
            3f * oneMinusT * oneMinusT * t * p1 +
            3f * oneMinusT * t * t * p2 +
            t * t * t * p3;
    }

    public static Vector3 GetFirstDerivative(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t)
    {
        t = Mathf.Clamp01(t);
        float oneMinusT = 1f - t;
        return
            3f * oneMinusT * oneMinusT * (p1 - p0) +
            6f * oneMinusT * t * (p2 - p1) +
            3f * t * t * (p3 - p2);
    }
}
