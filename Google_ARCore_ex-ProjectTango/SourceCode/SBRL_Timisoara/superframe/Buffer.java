package com.sbrl.superframe;

/**
 * Created by radhoo on 26/04/14.
 */
public class Buffer {
    byte buffer[];

    int     bpp,
            width,          // buffer frame width
            startLine,      // start line in the buffer
            linesLength;    // length of this buffer in lines

    public Buffer(byte data[], int width, int startLine, int linesLength, int bpp) {
        this.width = width;
        this.startLine = startLine;
        this.linesLength = linesLength;
        this.bpp = bpp;
        buffer = new byte[ getSize() ];
        System.arraycopy(data, getStartIndex(), buffer, 0, getSize());
    }

    public int getStartLine() {
        return startLine;
    }

    public int getLinesLength() {
        return linesLength;
    }

    public int getStartIndex() {
        return width * startLine;
    }

    public int getSize() {
        return  bpp * width * linesLength / 8;
    }

    public byte[] getData() {
        return buffer;
    }

}
