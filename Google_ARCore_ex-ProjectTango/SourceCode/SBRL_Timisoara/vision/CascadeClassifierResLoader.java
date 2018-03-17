/**
 * @author Meda Motisan 
 * meda.chiorean@gmail.com
 * 2014  
 */
package com.sbrl.vision;

import android.content.Context;
import android.util.Log;
import org.opencv.objdetect.CascadeClassifier;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class CascadeClassifierResLoader extends CascadeClassifier {
	
	private final static String TAG = "CascadeClassifierResLoader";
	
	public static CascadeClassifier Load(Context context, int resid) {
		try {
            // load cascade file from application resources
            InputStream is = context.getResources().openRawResource(resid);
            File cascadeDir = context.getDir("cascade", Context.MODE_PRIVATE);
            File mCascadeFile = new File(cascadeDir, "tmp.xml");
            FileOutputStream os = new FileOutputStream(mCascadeFile);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();
           
            CascadeClassifier res = new CascadeClassifier(mCascadeFile.getAbsolutePath());
            if (res.empty()) {
                Log.e(TAG, "Failed to load cascade classifier");
                res = null;
            } else
                Log.i(TAG, "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());

            
            cascadeDir.delete();
            return res;

        } catch (IOException e) {
            e.printStackTrace();
            Log.e(TAG, "Failed to load cascade. Exception thrown: " + e);
            return null;
        }
	}
}
