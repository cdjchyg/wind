using UnityEngine;

public class Hermite
{
    /// <summary>
    /// 
    /// </summary>
    /// <param name="p0"></param>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="p3"></param>
    /// <param name="t"></param>
    /// <param name="tension">1 is high, 0 normal, -1 is low</param>
    /// <param name="bias">0 is even, positive is towards first segment, negative towards the other</param>
    /// <returns></returns>
    public static Vector3 GetPoint(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, float t, float tension, float bias)
    {
        tension = Mathf.Clamp(tension, -1, 1);
        bias = Mathf.Clamp(bias, -1, 1);

        float t2 = t * t;
        float t3 = t2 * t;

        Vector3 m0 = (p1 - p0) * (1 + bias) * (1 - tension) / 2;
        m0 += (p2 - p1) * (1 - bias) * (1 - tension) / 2;

        Vector3 m1 = (p2 - p1) * (1 + bias) * (1 - tension) / 2;
        m1 += (p3 - p2) * (1 - bias) * (1 - tension) / 2;

        float a0 = 2 * t3 - 3 * t2 + 1;
        float a1 = t3 - 2 * t2 + t;
        float a2 = t3 - t2;
        float a3 = -2 * t3 + 3 * t2;

        return (a0 * p1 + a1 * m0 + a2 * m1 + a3 * p2);
    }
}