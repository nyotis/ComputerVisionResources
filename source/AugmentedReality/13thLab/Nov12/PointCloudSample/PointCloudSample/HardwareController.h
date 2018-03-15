/**
 * Copyright (C) 2012 13th Lab AB
 *
 * See the LICENSE file that came with this example code.
 */

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreMotion/CoreMotion.h>

#import "GLView.h"

class PointCloudApplication;

@interface HardwareController : UIViewController <GLViewDelegate, UINavigationControllerDelegate, AVCaptureVideoDataOutputSampleBufferDelegate, UIAlertViewDelegate> {
	AVCaptureSession *captureSession;
    GLView *glView;
    CVPixelBufferRef pixelBuffer;
    Float64 timestamp;
	CMMotionManager *motionManager;
	
	PointCloudApplication* pointcloudApplication;
	
	BOOL restartingCamera;
	BOOL accelerometer_available;
	BOOL device_motion_available;
	double g_scale;
}

- (void)startGraphics;

- (void)stopGraphics;

- (void)startCamera;

- (void)restartCamera;

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;

- (void)drawView:(GLView*)view;

- (void)setupView:(GLView*)view;

- (void)realCaptureOutput: (id)pixelData;

- (void)initCapture;

- (void)eventHandler:(id)data;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

@property (nonatomic, retain) AVCaptureSession *captureSession;
@property (nonatomic, retain) GLView *glView;
@property (nonatomic) CVPixelBufferRef pixelBuffer;
@property (nonatomic, retain) CMMotionManager *motionManager;
@property (nonatomic) double g_scale;
@property (nonatomic) Float64 timestamp;
@property (nonatomic) BOOL restartingCamera;
@property (nonatomic) BOOL accelerometer_available;
@property (nonatomic) BOOL device_motion_available;
@property (nonatomic) PointCloudApplication* pointcloudApplication;

@end
