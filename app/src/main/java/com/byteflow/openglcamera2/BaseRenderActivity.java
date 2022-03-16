package com.byteflow.openglcamera2;

import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.camera2.CameraCharacteristics;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Size;
import android.view.MotionEvent;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.byteflow.openglcamera2.camera.CameraUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;
import com.byteflow.openglcamera2.render.GLByteFlowRender;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import static com.byteflow.openglcamera2.render.ByteFlowRender.IMAGE_FORMAT_RGBA;

public abstract class BaseRenderActivity extends AppCompatActivity implements MyGestureListener.SimpleGestureListener {
    private static final String TAG = "BaseRenderActivity";
    protected static final int SHADER_NUM = 34;
    protected static final int LUT_A_SHADER_INDEX = 19;
    protected static final int LUT_B_SHADER_INDEX = 20;
    protected static final int LUT_C_SHADER_INDEX = 21;
    protected static final int LUT_D_SHADER_INDEX = 22;
    protected static final int ASCII_SHADER_INDEX = 29;
    protected GLByteFlowRender mByteFlowRender;
    protected GLSurfaceView mGLSurfaceView;
    protected MyGestureListener mGestureDetector;
    //protected int mCurrentShaderIndex = SHADER_NUM - 1;
    protected int mCurrentShaderIndex = 23;
    protected Size mRootViewSize, mScreenSize;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mGLSurfaceView = new GLSurfaceView(this);
        mGestureDetector = new MyGestureListener(this, this);
        mByteFlowRender = new GLByteFlowRender();

    }

    @Override
    protected void onResume() {
        super.onResume();
        Toast.makeText(this, "左滑或右滑切换滤镜", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mGestureDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    public void updateTransformMatrix(String cameraId) {
        if (Integer.valueOf(cameraId) == CameraCharacteristics.LENS_FACING_FRONT) {
            mByteFlowRender.setTransformMatrix(90, 0);
        } else {
            mByteFlowRender.setTransformMatrix(90, 1);
        }

    }

    public void updateGLSurfaceViewSize(Size previewSize) {
        Size fitSize = null;
        fitSize = CameraUtil.getFitInScreenSize(previewSize.getWidth(), previewSize.getHeight(), getScreenSize().getWidth(), getScreenSize().getHeight());
        RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) mGLSurfaceView
                .getLayoutParams();
        params.width = fitSize.getWidth();
        params.height = fitSize.getHeight();
        params.addRule(RelativeLayout.ALIGN_PARENT_TOP | RelativeLayout.CENTER_HORIZONTAL);

        mGLSurfaceView.setLayoutParams(params);
    }

    public Size getScreenSize() {
        if (mScreenSize == null) {
            DisplayMetrics displayMetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
            mScreenSize = new Size(displayMetrics.widthPixels, displayMetrics.heightPixels);
        }
        return mScreenSize;
    }

    public void loadRGBAImage(int resId, int index) {
        InputStream is = this.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                mByteFlowRender.loadLutImage(index, IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);
            }
        } finally {
            try {
                is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
