package com.sbrl.utils;

import android.graphics.Color;

/**
 * @author radhoo
 * Radu Motisan , radu.motisan@gmail.com, www.pocketmagic.net
 * Camera 3D
 * Google Tango Hackathon
 * Timisoara, April 2014
 */
 public class ColorSpace {
/*
int rgb[] = new int[previewW * previewH];
				AppUtils.decodeYUV420SP(rgb, data, previewW, previewH);
				// create bitmap from camera preview data as imutable bitmap
				Bitmap bmp = Bitmap.createBitmap(rgb, previewW, previewH, Bitmap.Config.RGB_565);
 */
    static public void decodeYUV420SP(int[] rgb, byte[] yuv420sp, int offset, int width, int height) {
        final int frameSize = width * height;

        for (int j = 0, yp = 0; j < height; j++) {
            int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
            for (int i = 0; i < width; i++, yp++) {
                int y = (0xff & ((int) yuv420sp[offset + yp])) - 16;
                if (y < 0) y = 0;
                if ((i & 1) == 0) {
                    v = (0xff & yuv420sp[offset + uvp++]) - 128;
                    u = (0xff & yuv420sp[offset + uvp++]) - 128;
                }
                int y1192 = 1192 * y;
                int r = (y1192 + 1634 * v);
                int g = (y1192 - 833 * v - 400 * u);
                int b = (y1192 + 2066 * u);

                if (r < 0) r = 0; else if (r > 262143) r = 262143;
                if (g < 0) g = 0; else if (g > 262143) g = 262143;
                if (b < 0) b = 0; else if (b > 262143) b = 262143;

                rgb[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
            }
        }
    }

    public static int getGradientComponent(int rgb1, int rgb2, int percent, int transparency) {
        int r1 = Color.red(rgb1),
                 g1 = Color.green(rgb1),
                b1 = Color.blue(rgb1),
                r2 = Color.red(rgb2),
                g2 = Color.green(rgb2),
                b2 = Color.blue(rgb2);

        return Color.argb(transparency,
                (r1 * percent + r2 * (100 - percent)) / 200,
                (g1 * percent + g2 * (100 - percent)) / 200,
                (b1 * percent + b2 * (100 - percent)) / 200);

    }
}
