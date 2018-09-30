using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BezierSpline : MonoBehaviour
{

    [SerializeField]
    private Vector3[] points;

    [SerializeField]
    private BezierControlPointMode[] modes;
    [SerializeField]
    private float[] curvesLenght;

    const bool useSimpson = true;

    public bool isSecond = false;

    public int ControlPointCount 
    {
        get {
            return points.Length;
        }
    }
 
    public Vector3 GetControlPoint (int index) {
        return points[index];
    }

    public void SetControlPoint(int index, Vector3 point)
    {
        if (index % 3 == 0)
        {
            Vector3 delta = point - points[index];
            if (loop)
            {
                if (index == 0)
                {
                    points[1] += delta;
                    points[points.Length - 2] += delta;
                    points[points.Length - 1] = point;
                }
                else if (index == points.Length - 1)
                {
                    points[0] = point;
                    points[1] += delta;
                    points[index - 1] += delta;
                }
                else
                {
                    points[index - 1] += delta;
                    points[index + 1] += delta;
                }
            }
            else
            {
                if (index > 0)
                {
                    points[index - 1] += delta;
                }
                if (index + 1 < points.Length)
                {
                    points[index + 1] += delta;
                }
            }
        }

        points[index] = point;
        EnforceMode(index);
    }

    public void Reset()
    {
        points = new Vector3[] {
            new Vector3(1f, 0f, 0f),
            new Vector3(2f, 0f, 0f),
            new Vector3(3f, 0f, 0f),
            new Vector3(4f, 0f, 0f)
        };

        modes = new BezierControlPointMode[] {
        BezierControlPointMode.Free,
        BezierControlPointMode.Free
        };
    }

    public Vector3 GetPoint(float t)
    {
        int i;
        if (t >= 1f)
        {
            t = 1f;
            i = points.Length - 4;
        }
        else
        {
            t = Mathf.Clamp01(t) * CurveCount;
            i = (int)t;
            t -= i;
            i *= 3;
        }
        return transform.TransformPoint(Bezier.GetPoint(
            points[i], points[i + 1], points[i + 2], points[i + 3], t));
    }

    public Vector3 GetVelocity(float t)
    {

        int i;
        if (t >= 1f)
        {
            t = 1f;
            i = points.Length - 4;
        }
        else
        {
            t = Mathf.Clamp01(t) * CurveCount;
            i = (int)t;
            t -= i;
            i *= 3;
        }
        return transform.TransformPoint(Bezier.GetFirstDerivative(
            points[i], points[i + 1], points[i + 2], points[i + 3], t)) - transform.position;
    }

    public Vector3 GetDirection(float t)
    {
        return GetVelocity(t).normalized;
    }

    private float CalcCurveLenght(int curveIndex)
    {
        float len = 0;
        const int lineStep = 1000;
        int beginIndex = curveIndex * 3;
        Vector3 p0 = points[beginIndex];
        Vector3 p1 = points[beginIndex + 1];
        Vector3 p2 = points[beginIndex + 2];
        Vector3 p3 = points[beginIndex + 3];

        
        if (useSimpson)
        {
            mp0 = p0;
            mp1 = p1;
            mp2 = p2;
            mp3 = p3;

            len = beze_length(1);
        }
        else
        {

            Vector3 lineStart = Bezier.GetPoint(p0, p1, p2, p3, 0);
            for (int i = 1; i <= lineStep; ++i)
            {

                Vector3 lineEnd = Bezier.GetPoint(p0, p1, p2, p3, i / (float)lineStep);
                len += (lineEnd - lineStart).magnitude;
                lineStart = lineEnd;
            }
        }

        return len;
    }

    public void CalcAllCurveLenght()
    {
        int count = CurveCount;
        Array.Resize(ref curvesLenght, count);
        for(int i = 0; i < count; ++i)
        {
            curvesLenght[i] = CalcCurveLenght(i);
        }
    }

    public bool GetPointByLenght(ref Vector3 point, ref Vector3 direction, float len)
    {
        int count = curvesLenght.Length;
        for (int curveIndex = 0; curveIndex < count;)
        {
            float curLen = curvesLenght[curveIndex];
            if (len < curLen)
            {
                float progress = len / curLen;

                int i = curveIndex * 3;
                Vector3 p0 = points[i];
                Vector3 p1 = points[i + 1];
                Vector3 p2 = points[i + 2];
                Vector3 p3 = points[i + 3];

                mp0 = p0;
                mp1 = p1;
                mp2 = p2;
                mp3 = p3;

                float avgProg = progress;//beze_even(curLen, progress);
                if (isSecond)
                {
                    avgProg = beze_even(curLen, progress);
                }


                point = transform.TransformPoint(Bezier.GetPoint(points[i], points[i + 1], points[i + 2], points[i + 3], avgProg));
                direction = (transform.TransformPoint(Bezier.GetFirstDerivative(points[i], points[i + 1], points[i + 2], points[i + 3], avgProg)) - transform.position).normalized;
                return true;
            }
            else
            {
                len -= curLen;
            }

            ++curveIndex;
            if (curveIndex >= count)
            {
                curveIndex = 0;
            }
        }

        return false;
    }

    public void AddCurve()
    {
        Vector3 point = points[points.Length - 1];
        Array.Resize(ref points, points.Length + 3);
        point.x += 1f;
        points[points.Length - 3] = point;
        point.x += 1f;
        points[points.Length - 2] = point;
        point.x += 1f;
        points[points.Length - 1] = point;

        Array.Resize(ref modes, modes.Length + 1);
        modes[modes.Length - 1] = modes[modes.Length - 2];

        EnforceMode(points.Length - 4);

        if(loop)
        {
            points[points.Length - 1] = points[0];
            modes[modes.Length - 1] = modes[0];
            EnforceMode(0);
        }
    }

    public int CurveCount
    {
        get
        {
            return (points.Length - 1) / 3;
        }
    }

    private int GetModeIndex(int index)
    {
        return (index + 1) / 3;
    }

    public BezierControlPointMode GetControlPointMode (int index) 
    {
        return modes[GetModeIndex(index)];
    }

    public void SetControlPointMode(int index, BezierControlPointMode mode)
    {
        int modeIndex = GetModeIndex(index);
        modes[modeIndex] = mode;

        if (loop)
        {
            if (modeIndex == 0)
            {
                modes[modes.Length - 1] = mode;
            }
            else if (modeIndex == modes.Length - 1)
            {
                modes[0] = mode;
            }
        }

        EnforceMode(index);
    }

    private void EnforceMode(int index)
    {
        int modeIndex = GetModeIndex(index);

        BezierControlPointMode mode = modes[modeIndex];
        if (mode == BezierControlPointMode.Free || !loop && (modeIndex == 0 || modeIndex == modes.Length - 1))
        {
            return;
        }

        int middleIndex = modeIndex * 3;
        int fixedIndex, enforcedIndex;
        if (index <= middleIndex)
        {
            fixedIndex = middleIndex - 1;
            if(fixedIndex < 0)
            {
                fixedIndex = points.Length - 2;
            }

            enforcedIndex = middleIndex + 1;
            if(enforcedIndex >= points.Length)
            {
                enforcedIndex = 1;
            }
        }
        else
        {
            fixedIndex = middleIndex + 1;

            if(fixedIndex >= points.Length)
            {
                fixedIndex = 1;
            }

            enforcedIndex = middleIndex - 1;
            if(enforcedIndex < 0)
            {
                enforcedIndex = points.Length - 2;
            }
        }

        Vector3 middle = points[middleIndex];
        Vector3 enforcedTangent = middle - points[fixedIndex];

        if (mode == BezierControlPointMode.Aligned)
        {
            enforcedTangent = enforcedTangent.normalized * Vector3.Distance(middle, points[enforcedIndex]);
        }

        points[enforcedIndex] = middle + enforcedTangent;
    }

    [SerializeField]
    private bool loop;

    public bool Loop
    {
        get
        {
            return loop;
        }
        set
        {
            loop = value;
            if (value == true)
            {
                modes[modes.Length - 1] = modes[0];
                SetControlPoint(0, points[0]);
            }
        }
    }



    //https://blog.csdn.net/kongbu0622/article/details/10123989
    //https://blog.csdn.net/kongbu0622/article/details/10124065
    //长度方程,使用Simpson积分算法
    Vector3 mp0 = Vector3.zero, mp1 = Vector3.zero, mp2 = Vector3.zero, mp3 = Vector3.zero;

    float beze_speed(float t)

    {

        //double sx = beze_speed_x(t);

        //double sy = beze_speed_y(t);



        //return sqrt(sx * sx + sy * sy);
        float it = 1 - t;

        //-3 * P0.y * it * it + 3 * P1.y * it * it - 6 * P1.y * it * t + 6 * P2.y * it * t - 3 * P2.y * t * t + 3 * P3.y * t * t;
        Vector3 speed = -3 * mp0 * it * it + 3 * mp1 * it * it - 6 * mp1 * it * t 
            + 6 * mp2 * it * t - 3 * mp2 * t * t + 3 * mp3 * t * t;
        return speed.magnitude;
    }

    float beze_length(float t)
    {
        const int TOTAL_SIMPSON_STEP = 1000;
        int stepCounts = (int)(TOTAL_SIMPSON_STEP * t);
        if (stepCounts % 2 != 0) stepCounts++;	//偶数
        if (stepCounts == 0) return 0.0f;

        int halfCounts = stepCounts / 2;

        float sum1 = 0.0f, sum2 = 0.0f;

        float dStep = t / stepCounts;



        for (int i = 0; i < halfCounts; i++)

        {

            sum1 += beze_speed((2 * i + 1) * dStep);

        }

        for (int i = 1; i < halfCounts; i++)

        {

            sum2 += beze_speed((2 * i) * dStep);

        }



        return (beze_speed(0.0f) + beze_speed(1.0f) + 2 * sum2 + 4 * sum1) * dStep / 3.0f;

    }

    float beze_even(float total_length, float t)

    {

        float len = t * total_length; //如果按照匀速增长,此时对应的曲线长度

        float t1 = t, t2;

        int count = 0;

        do

        {
            ++count;
            t2 = t1 - (beze_length(t1) - len) / beze_speed(t1);

            if (count > 10 || Mathf.Abs(t1 - t2) < 0.00001) break;

            t1 = t2;

        } while (true);

        Debug.Log("beze_even" + count);

        return t2;

    }
}
