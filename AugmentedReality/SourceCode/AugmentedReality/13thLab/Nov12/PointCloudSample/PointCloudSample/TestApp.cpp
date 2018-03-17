/**
 * Copyright (C) 2012 13th Lab AB
 *
 * See the LICENSE file that came with this example code.
 */ 

#include "TestApp.h"
#include "PointCloud.h"
#include <string>

TestApp::TestApp(int viewport_width, int viewport_height, 
				 int video_width, int video_height, 
				 pointcloud_video_format video_format,
				 const char* resource_path, 
				 const char* documents_path,
				 const char* device,
				 double ui_scale_factor) :
PointCloudApplication(viewport_width, viewport_height,
					  video_width, video_height,
					  video_format,
					  resource_path,
					  documents_path,
					  device,
					  ui_scale_factor)
{
	setup_cuboid();
	
	// Add images to look for (detection will not start until images are activated, though)
	std::string image_target_1_path = resource_path + std::string("image_target_1.model");
    std::string image_target_2_path = resource_path + std::string("image_target_2.model");
    
    pointcloud_add_image_target("image_1", image_target_1_path.c_str(), 0.3, -1);
    pointcloud_add_image_target("image_2", image_target_2_path.c_str(), 0.3, -1);
    	
	// Create the texture used for the UI
	ui_texture = read_png_texture("ui.png", true);
}


// Most convoluted way to make a cuboid
void TestApp::setup_cuboid() {
	float corners[8][3];
	float normals[6][3];
	
    double len = 0.1;
    
	for (int i = 0; i < 8; i++) {
		corners[i][0] = -len/2;
		corners[i][1] = 0;
		corners[i][2] = -len/2;
	}
	
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			normals[i][j] = 0;
		}
	}
	
	normals[0][0] = 1;
	normals[1][2] = 1;
	normals[2][1] = 1;
	normals[3][0] = -1;
	normals[4][2] = -1;
	normals[5][1] = -1;
		
	corners[1][0] += len;
	corners[2][0] += len;
	corners[5][0] += len;
	corners[6][0] += len;
	
	corners[2][2] += len;
	corners[3][2] += len;
	corners[6][2] += len;
	corners[7][2] += len;
	
	corners[4][1] += len*3;
	corners[5][1] += len*3;
	corners[6][1] += len*3;
	corners[7][1] += len*3;
	
	int strip[] = {4,0,5,1,6,2,7,3,4,0,0,1,3,2,2,6,7,5,4};
	int side[] =  {4,4,4,4,0,0,1,1,3,3,5,5,5,5,2,2,2,2,2};
	
	for (int i = 0; i < sizeof(strip)/sizeof(int); i++) {
		int c = strip[i];
		int s = side[i];
		for (int j = 0; j < 3; j++) {
			cuboid_vertices[i][j] = corners[c][j];
			cuboid_normals[i][j] = normals[s][j];
		}
	}
    
    this->selected_button = SLAM;
}

void TestApp::draw_ui() {
	// Draws a basic UI to control the demo app
	pointcloud_state state = pointcloud_get_state();
	double now = current_time();
	bool is_slam = !(state == POINTCLOUD_LOOKING_FOR_IMAGES || state == POINTCLOUD_TRACKING_IMAGES);
	
	int w = context.viewport_width;
	int h = context.viewport_height;
	double scale_factor = w == 320 ? 0.5 : 1.0;
	double text_w = 138*scale_factor;
	double text_h = 112*scale_factor;
	
	double footer_h = 112*scale_factor;
	static double third = 1.0/3.0;
	
	
	
	// Header background
	draw_image(ui_texture, 0, 20, w, 60*scale_factor, 1/512.0, 332/512.0, 25/512.0, (332+60)/512.0);
	
	// Header text
	draw_image(ui_texture, (w-283*scale_factor)/2, 20+((60-36)/2)*scale_factor, 283*scale_factor, 36*scale_factor, 1/512.0, 289/512.0, 283/512.0, (289+36)/512.0);
	

	// Footer
	if (this->selected_button == IMAGE) {
		// Image Recognition button selected
		draw_image(ui_texture, 0,			h-footer_h, w*third, footer_h,   1/512.0, 400/512.0, 47/512.0, 512/512.0);
		draw_image(ui_texture, w*third,		h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
		draw_image(ui_texture, 2*w*third,	h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
	}
	else if (this->selected_button == SLAM){
		// SLAM button selected
		draw_image(ui_texture, 0,			h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
		draw_image(ui_texture, w*third,		h-footer_h, w*third, footer_h,   1/512.0, 400/512.0, 47/512.0, 512/512.0 );
		draw_image(ui_texture, 2*w*third,	h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
	} 
    else if (this->selected_button == SLAM_FROM_IMAGE){
		// SLAM with Image button selected
		draw_image(ui_texture, 0,			h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
		draw_image(ui_texture, w*third,		h-footer_h, w*third, footer_h,  49/512.0, 400/512.0, 95/512.0, 512/512.0);
		draw_image(ui_texture, 2*w*third,	h-footer_h, w*third, footer_h,   1/512.0, 400/512.0, 47/512.0, 512/512.0);
	} 
	
	// Footer text
	draw_image(ui_texture, w/6-text_w/2,	h-text_h, text_w, text_h,  96/512.0, 400/512.0, (96+138)/512.0, 512/512.0);
	draw_image(ui_texture, w/2-text_w/2,	h-text_h, text_w, text_h,  235/512.0, 400/512.0, (235+138)/512.0, 512/512.0);
	draw_image(ui_texture, 5*w/6-text_w/2,	h-text_h, text_w, text_h,  374/512.0, 400/512.0, (374+138)/512.0, 512/512.0);
	
	// Initialization
	if (is_slam) {
		if (state == POINTCLOUD_TRACKING_SLAM_MAP && last_state == POINTCLOUD_INITIALIZING) {
			init_fadeout_time = now;
		}
		
		double init_fadeout_factor = 1-(now - init_fadeout_time)/2.0; // Fade out in 2 seconds
		
		if (this->selected_button == SLAM && ((state != POINTCLOUD_TRACKING_SLAM_MAP && state != POINTCLOUD_RELOCALIZING) || init_fadeout_factor > 0)) {
			int box_x = (w-429*scale_factor)/2;
			int box_y = (h-289*scale_factor)/2;
			
			// Arrow
			int arrowOffset = 0;
			double opacity = 1.0;
			if (state == POINTCLOUD_INITIALIZING) {
				arrowOffset = 59*scale_factor; 
			}
			else if (state == POINTCLOUD_TRACKING_SLAM_MAP) {
				opacity = init_fadeout_factor;
				arrowOffset = 146*scale_factor;
			}
			
			// Draw init box
			draw_image(ui_texture,box_x, box_y, 429*scale_factor, 289*scale_factor,  1/512.0, 1/512.0, 429/512.0, 289/512.0, opacity);	
			
			// Draw arrow
			draw_image(ui_texture, box_x+26*scale_factor, box_y+86*scale_factor+arrowOffset, 30*scale_factor, 28*scale_factor,  45/512.0, 349/512.0, (45+30)/512.0, (349+28)/512.0, opacity);	
			
		}
	}
	else if (state == POINTCLOUD_LOOKING_FOR_IMAGES) {
		// Show Looking for images.. 
		// Draw top of init box
		int box_x = (w-429*scale_factor)/2;
		int box_y = (h-49*scale_factor)/2;
		draw_image(ui_texture,box_x, box_y, 429*scale_factor, 49*scale_factor,  1/512.0, 1/512.0, 20/512.0, 49/512.0);	
		
		// Draw text 268x34
		draw_image(ui_texture, (w-268*scale_factor)/2, (h-34*scale_factor)/2, 268*scale_factor, 34*scale_factor, 113/512.0, 349/512.0, (113+268)/512.0, (349+34)/512.0);
	}
}


void TestApp::render_content(double time_since_last_frame) {
	
    pointcloud_state state = pointcloud_get_state();
	
	// Draw the content if we have SLAM or image tracking
	if (state == POINTCLOUD_TRACKING_SLAM_MAP || state == POINTCLOUD_TRACKING_IMAGES) {

		switch_to_camera();

        // Set light position
        static const float light_position[4] = {1, 6, 0.5, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glColor4f(1,0,0,1);
		
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_FLAT);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		
		glVertexPointer(3, GL_FLOAT, 0, (float *)cuboid_vertices);
		glNormalPointer(GL_FLOAT, 0, (float *)cuboid_normals);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 19);
		
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		
		glShadeModel(GL_SMOOTH);
		glDisable(GL_COLOR_MATERIAL);
		glColor4f(1, 1, 1, 1);
	}
	
	// Draw the UI on top of the content
	switch_to_ortho();
	draw_ui();
}


bool TestApp::on_touch_started(double x, double y) {
	
	pointcloud_state state = pointcloud_get_state();
	int W = context.viewport_width;
	int H = context.viewport_height;
	
	if (y > H-112) {
		if (x < W/3) {
			// Image recogntition button pressed
            this->selected_button = IMAGE;
			pointcloud_reset();
            pointcloud_disable_map_expansion();
			pointcloud_activate_image_target("image_1");
			pointcloud_activate_image_target("image_2");
		}
		else if (x >= W/3 && x < W*2/3) {
			// SLAM button pressed
            this->selected_button = SLAM;
			pointcloud_reset();
			pointcloud_deactivate_image_target("image_1");
			pointcloud_deactivate_image_target("image_2");
		} 
        else {
			// Image from SLAM button pressed
            this->selected_button = SLAM_FROM_IMAGE;
            pointcloud_reset();
            pointcloud_enable_map_expansion();
            pointcloud_activate_image_target("image_1");
            pointcloud_activate_image_target("image_2");
		}
        
		return true;
	}
	else if (state != POINTCLOUD_LOOKING_FOR_IMAGES && state != POINTCLOUD_TRACKING_IMAGES) { 
		if (pointcloud_get_state() == POINTCLOUD_IDLE) {
			printf("Start initialization\n");
			pointcloud_start_slam();
		} else {
			printf("Resetting\n");
			pointcloud_reset();
		}
		return true;
	}
	
	return false;
}

bool TestApp::on_touch_moved(double x, double y) {
	return false;
}

bool TestApp::on_touch_ended(double x, double y) {
	return false;
}

bool TestApp::on_touch_cancelled(double x, double y) {
	return false;
}


