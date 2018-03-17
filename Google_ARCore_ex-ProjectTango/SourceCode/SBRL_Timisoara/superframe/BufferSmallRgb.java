package com.sbrl.superframe;

/**
 * Created by radhoo on 26/04/14.
 */
public class BufferSmallRgb extends Buffer {

    public BufferSmallRgb(byte[] data) {
        super(data, SuperFrameCts.SF_WIDTH, SuperFrameCts.SF_START_SMALLRGB, SuperFrameCts.SF_LENGTH_SMALLRGB, 8);
    }

    public int getWidth() {
        return SuperFrameCts.SF_WIDTH_SMALLRGB;
    }

    public int getHeight() {
        return SuperFrameCts.SF_HEIGHT_SMALLRGB;
    }
}
