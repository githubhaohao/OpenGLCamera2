package com.byteflow.openglcamera2.frame;

public class ByteFlowFrame {
    byte[] mData;
    int mWidth;
    int mHeight;

    public ByteFlowFrame(byte[] mData, int mWidth, int mHeight) {
        this.mData = mData;
        this.mWidth = mWidth;
        this.mHeight = mHeight;
    }

    public byte[] getData() {
        return mData;
    }

    public int getWidth() {
        return mWidth;
    }

    public int getHeight() {
        return mHeight;
    }
}
