/** @file PointCloud.h
 *
 *  @brief Public API for libpointcloud
 *
 *  Copyright 2012 13th Lab AB. All rights reserved.
 *
 *  The terms and condition for libpointcloud can be found in the LICENSE folder of the libpointcloud download,
 *  or at http://developer.pointcloud.io/sdk/termsofuse
 *
 */

#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#ifdef __cplusplus
extern "C" {
#endif
	
    /*******
     * STATES
     ********/

	/** System state codes
     *
	 */
    typedef enum pointcloud_state {
        POINTCLOUD_NOT_CREATED, /**< @brief System not created: pointcloud_create() has not been called yet. */
        POINTCLOUD_WAITING_FOR_CAMERA_FRAMES, /**< No valid camera frames have been received in pointcloud_on_camera_frame(). */
        POINTCLOUD_IDLE, /**< System is ready and waiting for instructions. */
        POINTCLOUD_INITIALIZING, /**< SLAM intitialization process is running. */
        POINTCLOUD_TRACKING_SLAM_MAP, /**< System is tracking a SLAM map. */
        POINTCLOUD_RELOCALIZING, /**< Tracking is lost, trying to relocalize. */
        POINTCLOUD_LOOKING_FOR_IMAGES, /**< Looking for images. */
        POINTCLOUD_TRACKING_IMAGES, /**< Tracking images. */
    } pointcloud_state;
	
    /** Camera video formats.
     *
     */
    typedef enum pointcloud_video_format {
        POINTCLOUD_RGB_565, /**< 16 bit RGB 565 */
        POINTCLOUD_NV21, /**< 12 bit NV21 Planar YUV 4:2:0 */
        POINTCLOUD_BGRA_8888, /**< 32 bit RGBA 8888 */
        POINTCLOUD_GRAY_8, /**< 8 bit grayscale */
    } pointcloud_video_format;

    
    /*******
     * STRUCTS
     ********/
	
    /** Camera video frame and viewport info.
     *
     */
    typedef struct pointcloud_context {
        pointcloud_video_format video_format;
        int video_width;
        int video_height;
        int viewport_width;
        int viewport_height;
        int video_crop_x;
        int video_crop_y;
    } pointcloud_context;

    /** A 4x4 matrix of floats.
     *
     */
    typedef struct pointcloud_matrix_4x4 {
        float data[16];
    } pointcloud_matrix_4x4;

    /** 2D point.
     *
     */
    typedef struct pointcloud_vector_2 {
        float x;
        float y;
    } pointcloud_vector_2;
	
    /** 3D point.
     *
     */
    typedef struct pointcloud_vector_3 {
        float x;
        float y;
        float z;
    } pointcloud_vector_3;
	
    /** A point cloud, i.e. an array of 3D points.
     *
     */
    typedef struct pointcloud_point_cloud {
        pointcloud_vector_3* points;
        unsigned int* point_ids;
        unsigned int size;
    } pointcloud_point_cloud;

    /** A target image.
     *
     */
    typedef struct pointcloud_image_target {
        char* image_id;
        pointcloud_vector_2 dimensions;
    } pointcloud_image_target;
	
    /** A list of target images.
     *
     */
    typedef struct pointcloud_image_targets {
        pointcloud_image_target* targets;
        unsigned int size;
    } pointcloud_image_targets;
	
    /*******
     * CONTRUCTOR/DESTRUCTOR
     ********/

    /** Initialize the pointcloud system. This needs to be done before calling any other methods.
     *
     * @param viewport_width	The width of the viewport the camera frame is projected on
     * @param viewport_height	The height of the viewport the camera frame is projected on
     * @param video_width		The width of a camera frame
     * @param video_height      The height of a camera frame
     * @param video_format      The format of the camera frame data
     * @param device			The device type
     * @param app_key           The developer key for the application.
     *                          This is generated on http://developer.pointcloud.io and is specific to each application using PointCloud SDK
     */
    void pointcloud_create(int viewport_width, int viewport_height, int video_width, int video_height,
                           pointcloud_video_format video_format, const char* device, const char* app_key);
	
    /** Destroy the pointcloud system.
     *
     */
    void pointcloud_destroy();

    /*******
     * DATA HANDLERS
     ********/
	
    /** Analyze a camera frame. Call this on on each frame.
     *
     * @param data The camera frame pixel data
     * @param timestamp Timestamp (in seconds) of video frame
     */
    void pointcloud_on_camera_frame(char* data, double timestamp);
	
    /** Notify the system of accelerometer data updates.
     *
     * @param x,y,z	The acceleration
     * @param timestamp Timestamp (in seconds) of acceleration update
     */
    void pointcloud_on_accelerometer_update(float x, float y, float z, double timestamp);
	
    /** Notify the system of user acceleration, rotation rate and gravity updates.
     *
     * @param x,y,z	The user acceleration
     * @param rot_x,rot_y,rot_z	The rotation rate
     * @param g_x,g_y,g_z The gravity direction
     * @param timestamp Timestamp (in seconds) of device motion update
     */
    void pointcloud_on_device_motion_update(float x, float y, float z, float rot_x, float rot_y, float rot_z, float g_x, float g_y, float g_z, double timestamp);
	
    /*******
     * ACTIONS
     ********/

    /** Start the initialization process. The user should move the device slowly left/right, up/down, in/out until a SLAM map has been created. */
    void pointcloud_start_slam();

    /** Stop tracking or initialization and reset to an idle state. This will destroy the current point cloud. */
    void pointcloud_reset();

    /** Enabling map expansion will create an expanding SLAM map.
     *  This way you can initialize a SLAM map from an image target. This is enabled by default when pointcloud_start_slam() is called.
     */
    void pointcloud_enable_map_expansion();

	/** Stops any ongoing map expansion. Freezes the current map. */
    void pointcloud_disable_map_expansion();
	
    /** Add a reference image to the system.
     *  Note that you need to activate the image (pointcloud_activate_image_target) if you want the system to detect it.
     *  The physical size of the reference image will define the scale of the SLAM coordinates.
     *
     * @param image_id user-defined image id
     * @param file_path path to the file containing the image model
     * @param physical_width, physical_height physical size of the image. Only one of these parameters need to be defined (the other can be set to 0).
     */
    void pointcloud_add_image_target(const char* image_id, const char* file_path, float physical_width, float physical_height);
	
    /** Remove a reference image from the system.
     *
     *  @param image_id user-defined image id
     */
    void pointcloud_remove_image_target(const char* image_id);
	
    /** Activate a reference image. Tell the system to detect and track a reference image.
     *
     *  @param image_id user-defined image id
     */
    void pointcloud_activate_image_target(const char* image_id);
	
    /** Deactivate a reference image. Tell the system to stop detecting or tracking a reference image.
     *
     *  @param image_id user-defined image id
     */
    void pointcloud_deactivate_image_target(const char* image_id);

    /** Save the current 3d map to disk.
     *
     *  @param file_path path to the file to save the map to
     */
    void pointcloud_save_current_map(const char* file_path);

    /** Load a 3d map from disk.
     *
     *  @param file_path path to the file containing the saved map
     */
    void pointcloud_load_map(const char* file_path);

    /*******
     * GETTERS
     ********/

    /** Get the viewport and video context.
     *
     * @return The viewport and video contex
     */
    pointcloud_context pointcloud_get_context();
	
    /** Get the current system state.
     *
     * @return The current system state
     */
    pointcloud_state pointcloud_get_state();
	
    /** Get the current revision of the point cloud.
     *
     * @return The current point cloud revision number
     */
    int pointcloud_get_point_cloud_revision();

    /** Get number of points in the point cloud.
     *
     * @return The number of points in the point cloud
     */
    int pointcloud_point_cloud_size();

    /** Get a copy of the point cloud used when tracking.
     *
     * The copy is only valid during tracking or relocalization states.
     * Make sure to do pointcloud_destroy_point_cloud() on the point cloud after you are done using it.
     *
     * @return A copy of the point cloud
     */
    pointcloud_point_cloud* pointcloud_get_points();
	
    /** Destroy a point cloud copy.
     *
     * This will not affect the internal system point cloud.
     * Always call this after you are finished using a point cloud.
     *
     * @param points The point cloud copy to destroy.
     */
    void pointcloud_destroy_point_cloud(pointcloud_point_cloud* points);
	
    /** Get a copy of the current camera matrix.
     *
     * The result is only valid during the tracking states.
     * The camera matrix describes the transform from 3D map space to 3D camera space. The
	 * camera matrix is the inverse of the camera pose matrix.
	 *
     * @return The camera matrix (4x4) represented as an array of 16 floats in column major order
     */
    pointcloud_matrix_4x4 pointcloud_get_camera_matrix();
	
	/** Get a copy of the current camera pose matrix.
     *
     * The matrix is only valid during the tracking states.
	 * The camera pose matrix represents the current position and rotation of the device camera
	 * in the map space. The camera pose matrix is the inverse of the camera matrix.
	 *
     * @return The camera pose matrix (4x4) represented as an array of 16 floats in column major order.
     */
    pointcloud_matrix_4x4 pointcloud_get_camera_pose();

    /** Get a vector representing the gravity direction. The result is only valid when there is a map.
     *
     * @return the gravity vector
     */
    pointcloud_vector_3 pointcloud_get_gravity_vector();
	
    /** Get the currently tracked images.
     *
     * @return the currently tracked images, or an empty pointcloud_image_targets if no image is tracked.
     */
    pointcloud_image_targets pointcloud_get_tracked_images();

    /** Get a OpenGL compatible view frustum matrix
     *
     * The function calculates a view frustum matrix compatible with the video feed when clipped according to the result of pointcloud_get_video_clipping().
     *
     * @param near,far clipping planes
     *
     * @return 4x4 column major matrix of floats
     */
    pointcloud_matrix_4x4 pointcloud_get_frustum(float near, float far);
	
	/*******
     * COORDINATE TRANSFORMS
     ********/
	
    /** Unproject a 2d video coordinate to a camera relative 3d coordinate.
     *
     * @param x,y video coordinate
     * @return a unprojected point in camera relative 3d coordinate
     */
    pointcloud_vector_3 pointcloud_video_to_camera(float x, float y);
	
    /** Project a camera relative 3d coordinate to a 2d video coordinate.
     *
     * @param x,y,z camera relative 3d coordinate
     * @return a projected point in video coordinate
     */
    pointcloud_vector_2 pointcloud_camera_to_video(float x, float y, float z);
	
	/** Transform a 3d coordinate relative to the current camera into a map 3d coordinate.
     *
     * @param x,y,z camera relative 3d coordinate
     * @return a 3d map coordinate
     */
	pointcloud_vector_3 pointcloud_camera_to_map(float x, float y, float z);
	
	/** Transform a map 3d coordinate into a coordinate relative to the current camera.
     *
     * @param x,y,z 3d map coordinate
     * @return a camera relative 3d coordinate
     */
	pointcloud_vector_3 pointcloud_map_to_camera(float x, float y, float z);
	
    /** Transform a 2d video coordinate to a 2d viewport coordinate.
     *
     * @param x,y video coordinate
     * @return a 2d viewport coordinate
     */
    pointcloud_vector_2 pointcloud_video_to_viewport(float x, float y);
	
    /** Transform a 2d viewport coordinate to a 2d video coordinate.
     *
     * @param x,y 2d viewport coordinate
     * @return a 2d video coordinate
     */
    pointcloud_vector_2 pointcloud_viewport_to_video(float x, float y);
	
#ifdef __cplusplus
}
#endif

#endif // POINTCLOUD_H
