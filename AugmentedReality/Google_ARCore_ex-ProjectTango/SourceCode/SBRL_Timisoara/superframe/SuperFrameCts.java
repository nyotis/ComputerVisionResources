package com.sbrl.superframe;

/**
 * Created by radhoo on 26/04/14.
 */
public class SuperFrameCts {
    // The Superframe's width is 1280 pixels, where each pixel is a byte.
    public final static int SF_WIDTH = 1280;
    // The Superframe's height is 1168 pixels (height can be thought of as
    //   pixels or lines).
    public final static int SF_HEIGHT = 1168;

    public final static int SF_START_HEADER = 0;
    public final static int SF_LENGTH_HEADER = 16;

    public final static int SF_START_SMALLRGB = SF_START_HEADER + SF_LENGTH_HEADER;
    public final static int SF_LENGTH_SMALLRGB = 240;
    public final static int SF_WIDTH_SMALLRGB = 640;
    public final static int SF_HEIGHT_SMALLRGB = 480;

    public final static int SF_START_UNUSED1 = SF_START_SMALLRGB + SF_LENGTH_SMALLRGB;
    public final static int SF_LENGTH_UNUSED1 = 96;

    public final static int SF_START_DEPTH = SF_START_UNUSED1 + SF_LENGTH_UNUSED1;
    public final static int SF_LENGTH_DEPTH = 90;
    public final static int SF_WIDTH_DEPTH = 320;
    public final static int SF_HEIGHT_DEPTH = 180;

    public final static int SF_START_UNUSED2 = SF_START_DEPTH + SF_LENGTH_DEPTH;
    public final static int SF_LENGTH_UNUSED2 = 6;

    public final static int SF_START_BIGRGB = SF_START_UNUSED2 + SF_LENGTH_UNUSED2;
    public final static int SF_LENGTH_BIGRGB = 720; // Y only
    //public final static int SF_LENGTH_BIGRGB = 1080; // Y+UV
    public final static int SF_WIDTH_BIGRGB = 1280;
    public final static int SF_HEIGHT_BIGRGB = 720;


}
