package com.sbrl.superframe;

/**
 * Created by radhoo on 26/04/14.
 */
public class BufferDepth extends Buffer {

    public BufferDepth(byte[] data) {
        super(data, SuperFrameCts.SF_WIDTH, SuperFrameCts.SF_START_DEPTH, SuperFrameCts.SF_LENGTH_DEPTH, 16);
    }

    public int getWidth() {
        return SuperFrameCts.SF_WIDTH_DEPTH;
    }

    public int getHeight() {
        return SuperFrameCts.SF_HEIGHT_DEPTH;
    }

    public int getImgSize() {
        return getWidth() * getHeight();
    }
}
