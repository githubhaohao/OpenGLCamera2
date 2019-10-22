package com.byteflow.openglcamera2.render;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLByteFlowRender extends ByteFlowRender implements GLSurfaceView.Renderer {
    private static final String TAG = "GLByteFlowRender";
    private GLSurfaceView mGLSurfaceView;

    public GLByteFlowRender() {

    }

    public void init(GLSurfaceView surfaceView) {
        mGLSurfaceView = surfaceView;
        mGLSurfaceView.setEGLContextClientVersion(2);
        mGLSurfaceView.setRenderer(this);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        native_CreateContext(GL_RENDER_TYPE);
        native_Init(0);
    }

    public void requestRender() {
        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }
    }

    public void setTransformMatrix(int degree, int mirror) {
        Log.d(TAG, "setTransformMatrix() called with: degree = [" + degree + "], mirror = [" + mirror + "]");
        native_SetTransformMatrix(0, 0, 1, 1, degree, mirror);
    }

    public void setRenderFrame(byte[] data, int width, int height) {
        Log.d(TAG, "setRenderFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
        native_UpdateFrame(data, width, height);
    }

    public void setParamsInt(int paramType, int param) {
        native_SetParamsInt(paramType, param);
    }

    public int getParamsInt(int paramType) {
        return native_GetParamsInt(paramType);
    }

    public void setGLBeginLocX(int x) {
        setParamsInt(PARAM_TYPE_SET_GL_BEGIN_X, x);
    }

    public void setGLBeginLocY(int y) {
        setParamsInt(PARAM_TYPE_SET_GL_BEGIN_Y, y);
    }

    public void setMask(byte[] data, int width, int height) {
        if (data != null) {
            native_SetMask(data, width, height);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d(TAG, "onSurfaceCreated() called with: gl = [" + gl + "], config = [" + config + "]");
        native_OnSurfaceCreated();

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d(TAG, "onSurfaceChanged() called with: gl = [" + gl + "], width = [" + width + "], height = [" + height + "]");
        native_OnSurfaceChanged(width, height);

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        Log.d(TAG, "onDrawFrame() called with: gl = [" + gl + "]");
        native_OnDrawFrame();

    }

    public void unInit() {
        native_UnInit();
        native_DestroyContext();
    }
}
