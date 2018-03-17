
package com.sbrl.camera3d;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import com.sbrl.utils.CameraPreviewListener;
import com.sbrl.utils.Utils;
import com.tango.view3d.MainActivity;

/**
 * @author radhoo
 * Radu Motisan , radu.motisan@gmail.com, www.pocketmagic.net
 * Camera 3D
 * Google Tango Hackathon
 * Timisoara, April 2014
 */
public class DepthScanActivity extends Activity implements CameraPreviewListener{
    public final String TAG = "MainActivity";

    DepthViewSurface mDepthView = null;  //Surface for drawing on.
    RelativeLayout m_panel = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Set window to full screen view with no title.
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        // create a simple UI
        m_panel = new RelativeLayout(this);
        setContentView(m_panel);

        mDepthView = new DepthViewSurface(this);
        Utils.addView(m_panel, mDepthView, RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT,
                new int[][]{new int[]{RelativeLayout.CENTER_IN_PARENT}}, -1, -1);

        // set the listener
        mDepthView.setPreviewListener(this);


        // create a button
        ImageView b = new ImageView(this);
        //b.setText("Snap");
        b.setImageResource(R.drawable.snap_button_final);
        b.setOnClickListener(snapButtonClick);
        Utils.addView(m_panel, b, RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT,
                new int[][]{new int[]{RelativeLayout.ALIGN_PARENT_LEFT}, new int[]{RelativeLayout.ALIGN_PARENT_BOTTOM}}, -1, -1);

        b = new ImageView(this);
        //b.setText("Snap");
        b.setImageResource(R.drawable.snap_button_final);
        b.setOnClickListener(emptyButtonClick);
        Utils.addView(m_panel, b, RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT,
                new int[][]{new int[]{RelativeLayout.ALIGN_PARENT_RIGHT}, new int[]{RelativeLayout.ALIGN_PARENT_BOTTOM}}, -1, -1);

    }

    View.OnClickListener snapButtonClick = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
           Log.d(TAG, "snapButtonClick");

            int avg = mDepthView.getFaceDistAvg();
            int min = mDepthView.getFaceDistMin();
            int max = mDepthView.getFaceDistMax();

			// grab the depth info and the image for texturing and open the Open GL renderer
            mDepthView.saveDepth("/mnt/sdcard/depth.bin");
            mDepthView.saveColor("/mnt/sdcard/color.bin");

            //int depth[] = mDepthView.getDepthBuffer();
            Intent i=new Intent(DepthScanActivity.this, MainActivity.class);
            //i.putExtra("depthArray", depth);
            i.putExtra("modelPath", "/mnt/sdcard/depth.bin");
            i.putExtra("colorPath", "/mnt/sdcard/color.bin");
            i.putExtra("facemindist" , min);
            i.putExtra("facemaxdist" , max);
            i.putExtra("faceavgdist", avg);
            startActivity(i);
        }
    };

    View.OnClickListener emptyButtonClick = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            Log.d(TAG, "snapButtonClick");

            //int depth[] = mDepthView.getDepthBuffer();
            Intent i=new Intent(DepthScanActivity.this, MainActivity.class);
            startActivity(i);
        }
    };
    @Override
    protected void onResume() {
        mDepthView.startCamera();
        super.onResume();
    }

    @Override
    protected void onPause() {
        mDepthView.stopCamera();
        super.onPause();
    }

    @Override public void onPreviewFrame(byte[] data, int w, int h) {
        Log.d(TAG, "onPreviewFrame call:" + w + "x" + h);
    }

    @Override public void onCameraClosed() {
        Log.d(TAG, "onCameraClosed call.");
    }

    @Override public void onCameraReady(int width, int height) {
        Log.d(TAG, "onCameraReady call:" + width + "x" + height);
    }

    @Override public void onFocusDone(boolean result) {
        Log.d(TAG, "onFocusDone call:" + result);
    }

    @Override public void onPictureTaken() {
        Log.d(TAG, "onPictureTaken call.");
    }

    @Override public void onPictureTakenDataJPG(byte[] data, int w, int h) {
    }
}
