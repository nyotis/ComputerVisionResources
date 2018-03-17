package com.sbrl.superframe;

/**
 * Created by radhoo on 27/04/14.
 */
public class BufferBigRgb extends Buffer {

    public BufferBigRgb(byte[] data) {
        super(data, SuperFrameCts.SF_WIDTH, SuperFrameCts.SF_START_BIGRGB, SuperFrameCts.SF_LENGTH_BIGRGB, 8);
    }

    public int getWidth() {
        return SuperFrameCts.SF_WIDTH_BIGRGB;
    }

    public int getHeight() {
        return SuperFrameCts.SF_HEIGHT_BIGRGB;
    }
}
