package com.sbrl.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.provider.MediaStore;
import android.view.View;
import android.widget.RelativeLayout;

import java.io.*;

public class Utils {


// interface elements
	public static RelativeLayout.LayoutParams getRLP(int vw, int vh, int styles[][], int left, int top){
		RelativeLayout.LayoutParams lpv = new RelativeLayout.LayoutParams(vw,vh);
		if (styles!= null)
			for (int i=0;i<styles.length;i++) {
				if (styles[i] != null) {
					if (styles[i].length == 1) lpv.addRule(styles[i][0]);
					if (styles[i].length == 2) lpv.addRule(styles[i][0], styles[i][1]);
				}
			}
	    if (top!=-1) lpv.topMargin = top;
	    if (left!=-1) lpv.leftMargin = left;
	    return lpv;
	}

	public static void addView(RelativeLayout panel, View v, int vw, int vh, int styles[][], int left, int top ) {
		panel.addView(v, getRLP(vw, vh, styles, left, top));
	}
	public static void updateView(RelativeLayout panel, View v, int vw, int vh, int styles[][], int left, int top ) {
		panel.updateViewLayout(v, getRLP(vw, vh, styles, left, top));
	}


    public static boolean saveDatatoFile(byte[] data, int offset, int length, String filePath) {
        File f = new File(filePath);

        boolean res = true;
        try {
            f.createNewFile();
            OutputStream outStream =  new FileOutputStream(f);
            outStream.write(data, offset, length);
            outStream.close();
        }
        catch (FileNotFoundException e) { e.printStackTrace(); res = false; }
        catch (IOException e) { e.printStackTrace(); res = false; }
        return res;
    }

    public static boolean saveDatatoFile(byte[] data, String filePath) {
       return saveDatatoFile(data,0,data.length, filePath);
    }

    public static boolean saveImage(Context context, Bitmap bmp,  String path, String filename, int type) {
        File f = new File(path , filename);
        boolean res = true;
        try {
            f.createNewFile();
            OutputStream outStream = new FileOutputStream(f);
            if (type == 0) //png
                res = bmp.compress(Bitmap.CompressFormat.PNG, 100, outStream);
            else
                res = bmp.compress(Bitmap.CompressFormat.JPEG, 100, outStream);
            // insert into gallery
            if (context != null)
                MediaStore.Images.Media.insertImage(context.getContentResolver(), bmp, "Camera 3D", "www.sbrl.com");
            outStream.close();
        } catch (IOException e) { e.printStackTrace(); res = false; }
        return res;
    }


}
