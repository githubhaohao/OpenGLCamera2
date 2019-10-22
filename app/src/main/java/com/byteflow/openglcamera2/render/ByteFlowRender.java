package com.byteflow.openglcamera2.render;

public abstract class ByteFlowRender {
    public static final int GL_RENDER_TYPE = 0;
    public static final int CL_RENDER_TYPE = 1;

    public static final int PARAM_TYPE_SET_SHADER_INDEX = 201;

    static {
        System.loadLibrary("byteflow_render");
    }

    private long mNativeContextHandle;

    protected native void native_CreateContext(int renderType);

    protected native void native_DestroyContext();

    protected native int native_Init(int initType);

    protected native int native_UnInit();

    protected native void native_UpdateFrame(byte[] data, int width, int height);

    protected native void native_SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror);

    protected native void native_SetParamsInt(int paramType, int value);

    protected native int native_GetParamsInt(int paramType);

    protected native void native_OnSurfaceCreated();

    protected native void native_OnSurfaceChanged(int width, int height);

    protected native void native_OnDrawFrame();
}
