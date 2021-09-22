package com.byteflow.openglcamera2.render;

public abstract class ByteFlowRender {
    public static final int GL_RENDER_TYPE = 0;
    public static final int CL_RENDER_TYPE = 1;

    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;

    public static final int PARAM_TYPE_SET_SHADER_INDEX = 201;
    public static final int PARAM_TYPE_SET_EXAMPLE      = 202;

    public static final int EXAMPLE_TYPE = 300;
    public static final int EXAMPLE_TYPE_KEY_CONVEYOR_BELT = EXAMPLE_TYPE;
    public static final int EXAMPLE_TYPE_KEY_BLUELINE_CHALLENGE = EXAMPLE_TYPE + 1;

    static {
        System.loadLibrary("byteflow_render");
    }

    private long mNativeContextHandle;

    protected native void native_CreateContext(int renderType);

    protected native void native_DestroyContext();

    protected native int native_Init(int initType);

    protected native int native_UnInit();

    protected native void native_UpdateFrame(int format, byte[] data, int width, int height);

    protected native void native_LoadFilterData(int index, int format, int width, int height, byte[] bytes);

    protected native void native_LoadShaderScript(int shaderIndex, String scriptStr);

    protected native void native_SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror);

    protected native void native_SetParamsInt(int paramType, int value);

    protected native int native_GetParamsInt(int paramType);

    protected native void native_OnSurfaceCreated();

    protected native void native_OnSurfaceChanged(int width, int height);

    protected native void native_OnDrawFrame();
}
