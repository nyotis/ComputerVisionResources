package com.sbrl.utils;

/**
 * @author radhoo
 * Radu Motisan , radu.motisan@gmail.com, www.pocketmagic.net
 * Camera 3D
 * Google Tango Hackathon
 * Timisoara, April 2014
 */
public interface CameraPreviewListener {
	 
	public void onPreviewFrame(byte[] data, int w, int h);

	public void onCameraClosed();

	public void onCameraReady(int width, int height);

	public void onFocusDone(boolean result);
	
	public void onPictureTaken();
	
	public void onPictureTakenDataJPG(byte[] data, int w, int h);
}