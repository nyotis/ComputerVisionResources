package com.sbrl.vision;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.util.Arrays;

/**
 * Created by radhoo on 26/04/14.
 */
public class Utils {
    public static Mat getGrayMat(byte data[], int offset, int w, int h) {

        // rebuild mat structure
        Mat frameGray = new Mat(h, w, CvType.CV_8UC1);
        try {
            int inputDataImageArea = w * h;//
            frameGray.put(0, 0, Arrays.copyOfRange(data, offset, inputDataImageArea));
            //Core.transpose(frameGray, frameGray);
            //Core.flip(frameGray, frameGray, 0);
        } catch (Exception e) {}
        return frameGray;
    }

    public static Mat getGrayMat(byte data[], int w, int h) {
        return getGrayMat(data,0, w, h);
    }
}
