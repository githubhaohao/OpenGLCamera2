package com.byteflow.openglcamera2.render;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.util.Size;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLByteFlowRender extends ByteFlowRender implements GLSurfaceView.Renderer {
    private static final String TAG = "GLByteFlowRender";
    private GLSurfaceView mGLSurfaceView;
    public volatile boolean mReadPixels = false;
    private Size mCurrentImgSize;
    private String mImagePath;
    private Callback mCallback;

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

    public void setRenderFrame(int format, byte[] data, int width, int height) {
        Log.d(TAG, "setRenderFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
        native_UpdateFrame(format, data, width, height);
    }

    public void setParamsInt(int paramType, int param) {
        native_SetParamsInt(paramType, param);
    }

    public int getParamsInt(int paramType) {
        return native_GetParamsInt(paramType);
    }

    public void loadLutImage(int index, int format, int width, int height, byte[] bytes) {
        native_LoadFilterData(index, format, width, height, bytes);
    }

    public void loadShaderFromAssetsFile(int shaderIndex, Resources r) {
        String result = null;
        try {
            InputStream in = r.getAssets().open("shaders/fshader_" + shaderIndex + ".glsl");
            int ch = 0;
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            while ((ch = in.read()) != -1) {
                baos.write(ch);
            }
            byte[] buff = baos.toByteArray();
            baos.close();
            in.close();
            result = new String(buff, "UTF-8");
            result = result.replaceAll("\\r\\n", "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (result != null) {
            native_LoadShaderScript(shaderIndex, result);
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

        if (mReadPixels) {
            saveToLocal(createBitmapFromGLSurface(0, 0, mCurrentImgSize.getWidth(), mCurrentImgSize.getHeight()), mImagePath);
            mReadPixels = false;
        }
    }

    public void unInit() {
        native_UnInit();
        native_DestroyContext();
    }

    public void addCallback(Callback callback) {
        mCallback = callback;
    }

    private Bitmap createBitmapFromGLSurface(int x, int y, int w, int h) {
        ByteBuffer buffer = ByteBuffer.allocate(w * h * 4);
        GLES20.glReadPixels(x, y, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
        Bitmap bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(buffer);
        Matrix matrix = new Matrix();
        matrix.setRotate(180);
        matrix.postScale(-1, 1);
        Bitmap newBM = Bitmap.createBitmap(bitmap, 0, 0, w, h, matrix, false);
        return newBM;
    }

    public void readPixels(Size size, String imagePath)
    {
        mCurrentImgSize = new Size(size.getWidth(), size.getHeight());
        mImagePath = imagePath;
        mReadPixels = true;
    }

    private void saveToLocal(Bitmap bitmap, String imgPath) {
        File file = new File(imgPath);
        if (file.exists()) {
            file.delete();
        }
        FileOutputStream out;
        try {
            out = new FileOutputStream(file);
            if (bitmap.compress(Bitmap.CompressFormat.JPEG, 90, out)) {
                out.flush();
                out.close();
                if(mCallback != null) mCallback.onReadPixelsSaveToLocal(file.getAbsolutePath());
            }
            bitmap.recycle();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public interface Callback {
        void onReadPixelsSaveToLocal(String imgPath);
    }
}
