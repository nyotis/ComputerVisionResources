
package com.sbrl.camera3d;

/**
 * @author radhoo
 * Radu Motisan , radu.motisan@gmail.com, www.pocketmagic.net
 * Camera 3D
 * Google Tango Hackathon
 * Timisoara, April 2014
 */
 
public class SuperFrame {

    public class SFStruct {
        byte m_data[];
        private int SF_WIDTH = 1280;

        byte header[] = new byte[16 * SF_WIDTH];
        byte smallRgb[] = new byte[240 * SF_WIDTH];
        byte unused1[] = new byte[96 * SF_WIDTH]; //vtrack pyramids not implemented
        byte depth[] = new byte[90 * SF_WIDTH];
        byte unused2[] = new byte[6 * SF_WIDTH];
        byte bigRgb[] = new byte[720 * SF_WIDTH];

        public SFStruct(byte data[]) {
            m_data = data;
        }


    };

    private SuperFrame() {
    } // Prevent instantiation of class.

    // The Superframe's width is 1280 pixels, where each pixel is a byte.
    public final static int SF_WIDTH = 1280;
    // The Superframe's height is 1168 pixels (height can be thought of as
    //   pixels or lines).
    public final static int SF_HEIGHT = 1168;

    // 1. the header buffer
    // Header data is stored in first 16 lines of a Superframe.
    public final static int SF_LINES_HEADER = 16;


    // 2. SMALL RGB buffer with the fisheye image
    // A Y plane of wide angle lens (Y contains the luminance of a YUV image)
    // is stored in next 240 lines.
    public final static int SF_LINES_SMALLIMAGE = 240;
    public final static int SF_START_LINE_SMALLIMAGE = SF_LINES_HEADER;
    public final static int SF_START_INDEX_SMALLIMAGE = SF_START_LINE_SMALLIMAGE * SF_WIDTH;
    public final static int SMALLIMAGE_WIDTH = 640;
    public final static int SMALLIMAGE_HEIGHT = 480;
    public final static int SMALLIMAGE_SIZE = SMALLIMAGE_WIDTH * SMALLIMAGE_HEIGHT;

    // 3. VTRACK Pyramids , not used here
    // An Image pyramid is stored in next 96 lines, currently a placeholder.
    public final static int SF_LINES_PYRAMID = 96;

    // 4. The DEPTH BUFFER
    // A Depth buffer is stored in next 96 lines: it is actually 90 + 6 padding
    public final static int SF_LINES_DEPTH = 96;
    public final static int SF_START_LINE_DEPTH = SF_LINES_HEADER  + SF_LINES_SMALLIMAGE + SF_LINES_PYRAMID;
    public final static int SF_START_INDEX_DEPTH = SF_START_LINE_DEPTH * SF_WIDTH;
    // The depth buffer is contained in a Superframe as a 320x180 array
    //   of 16 bit (2 contiguous bytes) values.
    public final static int DB_WIDTH = 320;
    public final static int DB_HEIGHT =  180;
    // DB_SIZE is the number of elements, not the number of bytes, in the depth
    //   buffer.
    // An element can be though of as a pixel, where the value is the depth.
    // A single element is a 2 byte int in the Superframe.
    public final static int DB_SIZE = DB_WIDTH * DB_HEIGHT;
    public final static int SF_DEPTH_SIZE = DB_WIDTH * DB_HEIGHT * 2;

    // 5. THE BIG RGB BUFFER
    // The Y plane of the 4 MP standard field of view camera YUV image
    //   is stored in next 720 lines.
    public final static int SF_LINES_BIGIMAGE_Y = 720;
    public final static int SF_LINES_BIGIMAGE_U = 180;
    public final static int SF_LINES_BIGIMAGE_V = 180;
    public final static int SF_LINES_BIGIMAGE_SP = 112; //indentation due to U,V for other components
    public final static int SF_WIDTH_BIGIMAGE = 1280;
    public final static int SF_HEIGHT_BIGIMAGE = 720;

    public final static int SF_START_LINE_BIGIMAGE_Y = SF_START_LINE_DEPTH + SF_LINES_DEPTH;
    // start index of the Y . Same for YUV as it is in the same block
    public final static int SF_START_INDEX_BIGIMAGE_Y = SF_START_LINE_BIGIMAGE_Y * SF_WIDTH;
    // Size of the Y portion of the 4 MP standard field YUV image.
    public final static int SF_SIZE_BIGIMAGE_Y = SF_WIDTH * SF_LINES_BIGIMAGE_Y; // this gets only the Y part of the YUF frame

    // Number of bytes in YUV bitmap.
    public final static int SF_SIZE_BIGIMAGE_YUV = SF_SIZE_BIGIMAGE_Y + (SF_SIZE_BIGIMAGE_Y / 2); // this gets the entire YUF frame

    public final static int SF_START_LINE_BIGIMAGE_U = SF_START_LINE_BIGIMAGE_Y + SF_LINES_BIGIMAGE_Y + SF_LINES_BIGIMAGE_SP;
    public final static int SF_START_INDEX_BIGIMAGE_U = SF_START_LINE_BIGIMAGE_U * SF_WIDTH;
    public final static int SF_SIZE_BIGIMAGE_U =  SF_WIDTH * SF_LINES_BIGIMAGE_U;

    public final static int SF_START_LINE_BIGIMAGE_V = SF_START_LINE_BIGIMAGE_U + SF_LINES_BIGIMAGE_U + SF_LINES_BIGIMAGE_SP;
    public final static int SF_START_INDEX_BIGIMAGE_V = SF_START_LINE_BIGIMAGE_V * SF_WIDTH;
    public final static int SF_SIZE_BIGIMAGE_V =  SF_WIDTH * SF_LINES_BIGIMAGE_V;




}
