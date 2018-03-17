
package com.sbrl.camera3d;

import android.content.Context;
import android.graphics.*;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import com.sbrl.superframe.BufferBigRgb;
import com.sbrl.superframe.BufferDepth;
import com.sbrl.superframe.BufferSmallRgb;
import com.sbrl.utils.CameraPreviewListener;
import com.sbrl.utils.ColorSpace;
import com.sbrl.utils.Constants;
import com.sbrl.utils.Utils;
import com.sbrl.vision.FaceDetector;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;

import java.util.List;

/**
 * @author radhoo
 * Radu Motisan , radu.motisan@gmail.com, www.pocketmagic.net
 * Camera 3D
 * Google Tango Hackathon
 * Timisoara, April 2014
 */
public class DepthViewSurface extends SurfaceView implements Callback, PreviewCallback {
    static {
        System.loadLibrary("opencv_java");

    }
    public final String                     TAG                     = "DepthViewSurface";

    private Context                         mContext;
    // Camera and surfaces.
    private Camera                          mCamera                 = null;
    private SurfaceHolder                   mSurfaceHolder          = null;
    // This bitmap has color data, color is based on depth buffer values.
    private int[]                           mDepthColorPixels       = null;
    // Create a bitmap to draw to screen.
    private Bitmap                          mDepthBitmapARGB8888    = null;
    // report some data to the activity hosting the camera preview
    private CameraPreviewListener           listener                = null;


    FaceDetector                             m_face = new FaceDetector();
    float m_faceScale;
    Rect m_faceRect;
    // no time to document!!! :(
    int faceDistAvg, faceDistMin, faceDistMax ;

    BufferDepth depth;
    BufferBigRgb bigRgb;
    BufferSmallRgb smallRgb;

    public void setPreviewListener(CameraPreviewListener l) {
        listener = l;
    }

    /**
    * Create surface and an array to hold color pixels.
    */
    private void init() {
        setWillNotDraw(false); // Enable view to draw.
        mSurfaceHolder = getHolder();
        // Create an array the same size as depth buffer.
        mDepthColorPixels = new int[SuperFrame.DB_WIDTH * SuperFrame.DB_HEIGHT];
        mDepthBitmapARGB8888 = Bitmap.createBitmap(SuperFrame.DB_WIDTH, SuperFrame.DB_HEIGHT, Bitmap.Config.ARGB_8888);
        // prepare face detector
        m_face.init(mContext,  R.raw.lbpcascade_frontalface);
    }

    public DepthViewSurface(Context context) {
        super(context);
        mContext = context;
        init();
    }

    public DepthViewSurface(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    /**
    * Set camera mode then start the camera preview.
    */
    public void startCamera() {
        mSurfaceHolder.addCallback(this);
        try {
            mCamera = Camera.open();
        } catch (RuntimeException e) {
            return;
        }
        Camera.Parameters parameters = mCamera.getParameters();
        // Note: sf modes are "all", "big-rgb", "small-rgb", "depth", "ir".
        parameters.set("sf-mode", "big-rgb");  // Show the RGB image.
        mCamera.setParameters(parameters);
        mCamera.startPreview();
    }

    public void stopCamera() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mSurfaceHolder.removeCallback(this);
            mCamera.release();
            mCamera = null;
        }
    }

    /**
    * Change preview size when width and height change. This can occur when the
    * device orientation changes (user rotates phone).
    */
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (mCamera != null && mSurfaceHolder.getSurface() != null) {
            try {
                mCamera.setPreviewCallback(this);
            } catch (Throwable t) {}

            try {
                mCamera.setPreviewDisplay(mSurfaceHolder);
            } catch (Exception e) { return; }

            // Set the preview size.
            Camera.Parameters parameters = mCamera.getParameters();
            Camera.Size size = getBestPreviewSize(width, height, parameters.getSupportedPreviewSizes());

            if (size != null) {
                parameters.setPreviewSize(size.width, size.height);
                mCamera.setParameters(parameters);
            }
            //
            // See which sizes the camera supports and choose one of those
            List<Camera.Size> sizesPicture = parameters.getSupportedPictureSizes();
            for (Camera.Size sizePic: sizesPicture) {
                Log.d(TAG, "sizePic:" + sizePic.width + "x" + sizePic.height);

            }
            Camera.Size picSize = sizesPicture.get(0); //2048x1536
            parameters.setPictureSize(picSize.width, picSize.height);

            //
            mCamera.startPreview();

            if (listener != null) listener.onCameraReady(size.width, size.height);
        }
    }

    synchronized public void saveDepth(String path) {
        Utils.saveDatatoFile(depth.getData(), 0, depth.getSize(), path);
    }

    synchronized public void saveColor(String path) {
      /*
        Bitmap bmp = BitmapFactory.decodeByteArray(bigRgb.getData(), 0, bigRgb.getSize());


        Bitmap resizedbitmap = Bitmap.createScaledBitmap(bmp, 320, 180, true);

        Utils.saveImage(mContext, resizedbitmap, "/mnt/sdcard/","test.png", 0);
        */
        Utils.saveDatatoFile(bigRgb.getData(), 0, bigRgb.getSize(), path);
    }

    synchronized public int getFaceDistAvg() {
        return faceDistAvg;
    }
    synchronized public int getFaceDistMin(){
        return faceDistMin;
    }
    synchronized public int getFaceDistMax() { return faceDistMax; }
    /**
    * Draw false color overlay on top of camera's image during preview. The
    * data[] parameter is a Superframe, it contains the depth buffer.
    */
    @Override
    public void onPreviewFrame(byte[] data, Camera cameraPreFrame) {
        // Get camera parameters
        Camera.Parameters parameters = mCamera.getParameters();
        int h = parameters.getPreviewSize().height,
                w = parameters.getPreviewSize().width;
        // Feed to our listener
        if (listener != null) listener.onPreviewFrame(data, w, h);

        // Get data buffers from the SUPERFRAME
        depth = new BufferDepth(data);
        smallRgb = new BufferSmallRgb(data);
        bigRgb = new BufferBigRgb(data);
        if (Constants.DEBUG) {
            Utils.saveDatatoFile(depth.getData(), 0, depth.getSize(), "/sdcard/buf-depth.raw");
            Utils.saveDatatoFile(smallRgb.getData(), 0, smallRgb.getSize(), "/sdcard/buf-smallrgb.raw");
            Utils.saveDatatoFile(bigRgb.getData(), 0, bigRgb.getSize(), "/sdcard/buf-bigrgb.raw");

            /*int rgb[] = new int[bigRgb.getWidth() * bigRgb.getHeight()];
            ColorSpace.decodeYUV420SP(rgb, bigRgb.getData(), 0,  bigRgb.getWidth() , bigRgb.getHeight());
            Bitmap bmp = Bitmap.createBitmap(rgb, bigRgb.getWidth() , bigRgb.getHeight(), Bitmap.Config.RGB_565);
            Utils.saveImage(mContext, bmp, "/sdcard/", "testo.png" ,0);*/
        }


        Mat frameGray = com.sbrl.vision.Utils.getGrayMat(bigRgb.getData(), bigRgb.getWidth(), bigRgb.getHeight());
        if (Constants.DEBUG)
            Highgui.imwrite("/mnt/sdcard/testocv1.jpg", frameGray);
        m_faceRect = m_face.isOneFace(frameGray,
                new Size((float)bigRgb.getHeight() * Constants.MIN_FACE, (float)bigRgb.getHeight() * Constants.MIN_FACE) ,
                new Size((float)bigRgb.getHeight() * Constants.MAX_FACE, (float)bigRgb.getHeight() * Constants.MAX_FACE), 500);
        // we have a face computed for the BigRGB frame, we need to init the scale factor for our preview size
        m_faceScale = (float)getWidth() / (float)bigRgb.getWidth();

        int faceX1 = 0, faceY1 = 0, faceX2 = 0, faceY2 = 0;
        // if we have a face, compute the average distance to it
        if (m_faceRect != null) {
            float scaleFaceDepth =  (float)depth.getWidth() / (float)bigRgb.getWidth();
            faceX1 = (int)((float)m_faceRect.tl().x * scaleFaceDepth );
            faceY1 = (int)((float)m_faceRect.tl().y * scaleFaceDepth);
            faceX2 = (int)((float)m_faceRect.br().x * scaleFaceDepth);
            faceY2 = (int)((float)m_faceRect.br().y * scaleFaceDepth);
            faceDistAvg = 0;
            faceDistMin = Integer.MAX_VALUE;
            faceDistMax = 0;
        }

        // Loop over the depth buffer, draw pixels that are at or near scan
        // depth.
        int valid_points = 0;
        for (int bitmapIndex = 0; bitmapIndex < depth.getImgSize(); bitmapIndex++ ) {
            // Read the Superframe to get pixel depth.
            // Depth is contained in two bytes, convert bytes to an Int.
            int pixDepthMm = ((((int) depth.getData()[2* bitmapIndex + 1]) << 8) & 0xff00) |
                    (((int) depth.getData()[2* bitmapIndex]) & 0x00ff);
            int pdx = bitmapIndex % depth.getWidth() ,
                    pdy = bitmapIndex / depth.getWidth();

            // adjust our maximum where needed
            if (pixDepthMm > Constants.MAX_DISTANCE) Constants.MAX_DISTANCE = pixDepthMm;

            if (m_faceRect != null && pdx > faceX1 && pdx < faceX2 && pdy > faceY1 && pdy < faceY2) {
                // avoid some noise
                if (pixDepthMm > 90) {
                    faceDistAvg += pixDepthMm;//
                    if (pixDepthMm < faceDistMin) faceDistMin = pixDepthMm;
                    if (pixDepthMm > faceDistMax) faceDistMax = pixDepthMm;
                    valid_points ++;
                }
            }
            // test code
            int percent = (100 * pixDepthMm) / Constants.MAX_DISTANCE;
            mDepthColorPixels[bitmapIndex] = ColorSpace.getGradientComponent(Color.RED, Color.YELLOW, percent, 150 + percent);
            //

        }
        if (valid_points == 0)
            m_faceRect = null; // invalidate face due to noise

        if (m_faceRect != null) {
            // average to face surface
            faceDistAvg /= valid_points;//(faceX2 - faceX1) * (faceY2 - faceY1);

            // init buffer
            for (int bitmapIndex = 0; bitmapIndex < depth.getImgSize(); bitmapIndex++) {
                // Read the Superframe to get pixel depth.
                // Depth is contained in two bytes, convert bytes to an Int.
                int pixDepthMm = ((((int) depth.getData()[2 * bitmapIndex + 1]) << 8) & 0xff00) |
                        (((int) depth.getData()[2 * bitmapIndex]) & 0x00ff);
                if (pixDepthMm < 90) pixDepthMm = 0;
                if (pixDepthMm > 5000) pixDepthMm = 0;
                //finaldepth[bitmapIndex] = pixDepthMm;
            }
        }

        postInvalidate();  // Cause a redraw.
    }




    /**
    * Draw false color depth overlay on top of camera preview.
    */
    @Override
    public void onDraw(Canvas canvas) {
        // Fill the overlay bitmap with the latest overlay colors.
        mDepthBitmapARGB8888.setPixels(mDepthColorPixels, 0,
            SuperFrame.DB_WIDTH, 0, 0, SuperFrame.DB_WIDTH,
            SuperFrame.DB_HEIGHT);
        // Draw the overlay bitmap onto the canvas.
        canvas.drawBitmap(mDepthBitmapARGB8888, null, new RectF(0.0f, 0.0f,
            (float) getWidth(), (float) getHeight()), null);

        if (m_faceRect != null) {

            Paint p = new Paint();
            p.setStyle(Paint.Style.STROKE);
            p.setStrokeWidth(5);
            p.setColor(Color.GREEN);

            int x1 = (int)((float)m_faceRect.tl().x * m_faceScale ),
                    y1 = (int)((float)m_faceRect.tl().y * m_faceScale),
                    x2 = (int)((float)m_faceRect.br().x * m_faceScale),
                    y2 = (int)((float)m_faceRect.br().y * m_faceScale);
            canvas.drawRect(x1, y1, x2, y2, p);

            Paint paintText = new Paint();
            paintText.setColor(Color.CYAN);
            paintText.setShadowLayer(2,1,1,Color.BLACK);
            paintText.setTextAlign(Paint.Align.CENTER);
            paintText.setTextSize(22);

            canvas.drawText("Dist:" + faceDistAvg + "m Min:" + faceDistMin + "m", (x2 + x1)/2, y2 + 20, paintText);
        }
    }

    /**
    * Helper method to select the best preview size from a list of choices.
    *
    * @param width
    *            Desired width
    * @param height
    *            Desired height
    * @param sizes
    *            List of possible preview sizes
    * @return Best preview size
    */
    private Camera.Size getBestPreviewSize(int width, int height, List<Camera.Size> sizes) {
        Camera.Size result = null;
        for (Camera.Size size : sizes) {
          if (size.width <= width && size.height <= height) {
            if (result == null) {
                result = size;
            } else {
                int resultArea = result.width * result.height;
                int newArea = size.width * size.height;
                if (newArea > resultArea) {
                    result = size;
              }
            }
          }
        }
        return (result);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) { }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mCamera == null) return;
        // Surface will be destroyed when we return, so stop the preview.
        // Because the CameraDevice object is not a shared resource, it's very
        // important to release it when the activity is paused.
        mCamera.setPreviewCallback(null);//fix:java.lang.RuntimeException: Method called after release()

        mCamera.cancelAutoFocus();
        //mCamera.autoFocus(null);


        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;

        if (listener != null) listener.onCameraClosed();
    }
}
