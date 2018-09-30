using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SplineWalker : MonoBehaviour {

    public BezierSpline spline;

    //public float duration;
    public float speed = 5;

    public bool isSecond = false;

    private float progress;
    private float totalLenght;

    public enum SplineWalkerMode
    {
        Once,
        Loop,
        PingPong
    }

    public SplineWalkerMode mode;
    private bool goingForward = true;

	
	// Update is called once per frame
	void Update () {
        /*
        if (goingForward)
        {
            progress += Time.deltaTime / duration;
            if (progress > 1f)
            {
                if (mode == SplineWalkerMode.Once)
                {
                    progress = 1f;
                }
                else if (mode == SplineWalkerMode.Loop)
                {
                    progress -= 1f;
                }
                else
                {
                    progress = 2f - progress;
                    goingForward = false;
                }
            }
        }
        else
        {
            progress -= Time.deltaTime / duration;
            if (progress < 0f)
            {
                progress = -progress;
                goingForward = true;
            }
        }


        Vector3 position = spline.GetPoint(progress);
        transform.localPosition = position;

        transform.LookAt(position + spline.GetDirection(progress));
        */

        spline.isSecond = isSecond;

        totalLenght += Time.deltaTime * speed;
        Vector3 point = Vector3.zero, dirction = Vector3.zero;
        spline.GetPointByLenght(ref point, ref dirction, totalLenght) ;
        transform.localPosition = point;

        transform.LookAt(point + dirction);
    }
}
