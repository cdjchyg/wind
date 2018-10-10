using System;
using UnityEngine;

public class HermiteSpline : MonoBehaviour
{
    //[SerializeField]
    private Vector3[] points;
    //[SerializeField]
    private float[] curveLenght;

    private float totalLenght = 0;

    public float speed = 10;

    public float tension = 0;
    public float bias = 0;

    public Transform[] trans;

    public void Awake()
    {
        InitPoints();
    }
    
    void InitPoints()
    {
        Array.Resize(ref points, trans.Length);

        for (int i = 0; i < trans.Length; ++i)
        {
            points[i] = trans[i].position;
        }
        CalcAllCurveLenght();
    }

    void OnDrawGizmos()
    {
        if(points == null)
        {
            InitPoints();
        }

        if (points == null)
        {
            return;
        }

        int count = points.Length;
        if (count < 4)
        {
            return;
        }

        Vector3 p0, p1, p2, p3;
        p0 = p1 = p2 = p3 = Vector3.zero;
        Gizmos.color = new Color(1, 0, 0, 1);
        for (int i = 1; i < count; ++i)
        {
            GetCurvePoint(i, ref p0, ref p1, ref p2, ref p3);

            const float lineStep = 100;
            Vector3 lineStart = Hermite.GetPoint(p0, p1, p2, p3, 0, tension, bias);
            for (int index = 1; index <= lineStep; ++index)
            {
                Vector3 lineEnd = Hermite.GetPoint(p0, p1, p2, p3, index / (float)lineStep, tension, bias);
                Gizmos.DrawLine(lineStart, lineEnd);
                lineStart = lineEnd;
            }
        }
    }

    void Update()
    {

        totalLenght += Time.deltaTime * speed;
        Vector3 point = Vector3.zero, dirction = Vector3.zero;
        GetPointByLenght(ref point, ref dirction, totalLenght);
        transform.position = point;

        //transform.LookAt(point + dirction);
    }

    private float CalcCurveLenght(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3)
    {
        float len = 0;
        const float lineStep = 1000;
        Vector3 lineStart = Hermite.GetPoint(p0, p1, p2, p3, 0, tension, bias);
        for(int i = 1; i <= lineStep; ++i)
        {
            Vector3 lineEnd = Hermite.GetPoint(p0, p1, p2, p3, i / (float)lineStep, tension, bias);
            len += (lineEnd - lineStart).magnitude;
            lineStart = lineEnd;
        }
        return len;
    }

    private void GetCurvePoint(int i, ref Vector3 p0, ref Vector3 p1, ref Vector3 p2, ref Vector3 p3)
    {
        /*
            自动闭合
     public void SetAutoCloseMode(float joiningPointTime)
	{
		if (mState != "Reset")
			throw new System.Exception("Cannot change mode after start");

		mEndPointsMode = eEndPointsMode.AUTOCLOSED;

		mNodes.Add(new SplineNode(mNodes[0] as SplineNode));
		mNodes[mNodes.Count - 1].Time = joiningPointTime;

		Vector3 vInitDir = (mNodes[1].Point - mNodes[0].Point).normalized;
		Vector3 vEndDir = (mNodes[mNodes.Count - 2].Point - mNodes[mNodes.Count - 1].Point).normalized;
		float firstLength = (mNodes[1].Point - mNodes[0].Point).magnitude;
		float lastLength = (mNodes[mNodes.Count - 2].Point - mNodes[mNodes.Count - 1].Point).magnitude;

		SplineNode firstNode = new SplineNode(mNodes[0] as SplineNode);
		firstNode.Point = mNodes[0].Point + vEndDir * firstLength;

		SplineNode lastNode = new SplineNode(mNodes[mNodes.Count - 1] as SplineNode);
		lastNode.Point = mNodes[0].Point + vInitDir * lastLength;

		mNodes.Insert(0, firstNode);
		mNodes.Add(lastNode);
	}
         
            非自动闭合时，取首位点作为他们的控制点
        */
        int count = points.Length;
        if (i == 1)
        {
            p0 = p1 = points[0];
        }
        else
        {
            p0 = points[i - 2];
            p1 = points[i - 1];
        }

        if (i == count - 1)
        {
            p2 = points[i];
            p3 = points[i];
        }
        else
        {
            p2 = points[i];
            p3 = points[i + 1];
        }
    }

    public void CalcAllCurveLenght()
    {

        int count = points.Length;
        if (count < 4)
        {
            return;
        }
        
        Array.Resize(ref curveLenght, count);
        Vector3 p0, p1, p2, p3;
        curveLenght[0] = 0;
        p0 = p1 = p2 = p3 = Vector3.zero;
        for (int i = 1; i < count; ++i)
        {
            GetCurvePoint(i, ref p0, ref p1, ref p2, ref p3);


            curveLenght[i] = CalcCurveLenght(p0, p1, p2, p3);


        }
    }

    public bool GetPointByLenght(ref Vector3 point, ref Vector3 direction, float len)
    {
        int count = curveLenght.Length;
        for (int i = 1; i < count;)
        {
            float curLen = curveLenght[i];
            if(len < curLen)
            {
                float progress = len / curLen;
                Vector3 p0, p1, p2, p3;
                p0 = p1 = p2 = p3 = Vector3.zero;
                GetCurvePoint(i, ref p0, ref p1, ref p2, ref p3);

                point = Hermite.GetPoint(p0, p1, p2, p3, progress, tension, bias);

                return true;
                    
                
            }
            else
            {
                len -= curLen;
            }

            ++i;
            if(i >= count)
            {
                i = 1;
            }
        }

        return false;
    }
}