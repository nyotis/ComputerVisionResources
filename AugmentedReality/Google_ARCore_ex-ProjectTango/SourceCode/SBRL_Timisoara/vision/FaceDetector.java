/**
 * @author Meda Motisan 
 * meda.chiorean@gmail.com
 * 2014  
 */
package com.sbrl.vision;

import android.content.Context;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Size;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.objdetect.Objdetect;

public class FaceDetector {
	
	CascadeClassifier      	m_cc;
	Rect 					lastFaceRect = null;
	long					lastFaceTime;
	
	public boolean init(Context context, int cascadeRes) {
		m_cc = CascadeClassifierResLoader.Load(context, cascadeRes);
		return (m_cc != null);
	}
	
	
	public Rect isOneFace(Mat inGrayFrame, Size min, Size max, int msCacheTime) {
		if (m_cc == null) return null;
		long time = System.currentTimeMillis();
		MatOfRect faces = new MatOfRect();
        m_cc.detectMultiScale(inGrayFrame, faces, 1.1, 2, Objdetect.CASCADE_FIND_BIGGEST_OBJECT, min, max); //def: CASCADE_SCALE_IMAGE
        Rect[] facesArray = faces.toArray();
        // check we have ONE face that is centered in our ellipse
        if (facesArray == null) {// we want only the biggest face
        	// if we have no results, but we use caching, return old res instead
        	if (time - lastFaceTime < msCacheTime)
        		return lastFaceRect;
        	return null;
        }
        else {
            int size = 0;
            Rect biggest = null;
            for (Rect face:facesArray) {
                if (face.size().width > size) {
                    size = (int)face.size().width;
                    biggest = face;
                }
            }

        	lastFaceRect = biggest;
        	lastFaceTime = time;
        	return lastFaceRect;
        }
     }
}
