package com.byteflow.openglcamera2;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.util.Size;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.byteflow.openglcamera2.frame.ByteFlowFrame;
import com.byteflow.openglcamera2.frame.FrameUtil;
import com.byteflow.openglcamera2.gesture.MyGestureListener;
import com.byteflow.openglcamera2.render.GLByteFlowRender;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.GregorianCalendar;
import java.util.Locale;

import static com.byteflow.openglcamera2.render.ByteFlowRender.IMAGE_FORMAT_I420;

public class ImageActivity extends BaseRenderActivity implements GLByteFlowRender.Callback {
    private static final String TAG = "ImageActivity";
    private static final SimpleDateFormat DateTime_FORMAT = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss", Locale.US);
    private static final String RESULT_IMG_DIR = "OpenGLCamera2";
    private RelativeLayout mSurfaceViewRoot;
    private ByteFlowFrame mByteFlowFrame;
    private volatile boolean mReadPixelsReady = true;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_image);

        mSurfaceViewRoot = (RelativeLayout) findViewById(R.id.surface_root);

        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        mSurfaceViewRoot.addView(mGLSurfaceView, p);

        mByteFlowRender.init(mGLSurfaceView);
        mByteFlowRender.addCallback(this);
        mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
        String path = getIntent().getStringExtra("img_path");
        String camera_id = getIntent().getStringExtra("img_ort");
        Log.d(TAG, "onCreate() called with: path = [" + path + "]");
        mByteFlowFrame = FrameUtil.decodeFrame(path);
        updateTransformMatrix(camera_id);

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mByteFlowFrame != null) {
            mByteFlowRender.setRenderFrame(IMAGE_FORMAT_I420, mByteFlowFrame.getData(), mByteFlowFrame.getWidth(), mByteFlowFrame.getHeight());
            updateGLSurfaceViewSize(new Size(mByteFlowFrame.getWidth(), mByteFlowFrame.getHeight()));
            mByteFlowRender.requestRender();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mByteFlowRender.unInit();
    }

    @Override
    public void onSwipe(MyGestureListener.SwipeDirection direction) {
        Log.d(TAG, "onSwipe() called with: direction = [" + direction + "]");
        switch (direction) {
            case SWIPE_UP:
                break;
            case SWIPE_DOWN:
                break;
            case SWIPE_RIGHT:
                mCurrentShaderIndex++;
                mCurrentShaderIndex = mCurrentShaderIndex % SHADER_NUM;
                switch (mCurrentShaderIndex) {
                    case LUT_A_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_a, 0);
                        break;
                    case LUT_B_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_b, 0);
                        break;
                    case LUT_C_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_c, 0);
                        break;
                    case LUT_D_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_d, 0);
                        break;
                    case ASCII_SHADER_INDEX:
                        loadRGBAImage(R.drawable.ascii_mapping, ASCII_SHADER_INDEX);
                        break;
                    default:
                }

                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }

                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                if(mReadPixelsReady) {
                    mReadPixelsReady = false;
                    mByteFlowRender.readPixels(new Size(mByteFlowFrame.getHeight(),mByteFlowFrame.getWidth()), getResultImgFile(".jpg").getPath());
                }
                mByteFlowRender.requestRender();
                break;
            case SWIPE_LEFT:
                mCurrentShaderIndex--;
                if (mCurrentShaderIndex < 0) {
                    mCurrentShaderIndex += SHADER_NUM;
                }
                switch (mCurrentShaderIndex) {
                    case LUT_A_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_a, 0);
                        break;
                    case LUT_B_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_b, 0);
                        break;
                    case LUT_C_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_c, 0);
                        break;
                    case LUT_D_SHADER_INDEX:
                        loadRGBAImage(R.drawable.lut_d, 0);
                        break;
                    case ASCII_SHADER_INDEX:
                        loadRGBAImage(R.drawable.ascii_mapping, ASCII_SHADER_INDEX);
                        break;
                    default:
                }
                if (LUT_A_SHADER_INDEX <= mCurrentShaderIndex && mCurrentShaderIndex <= LUT_D_SHADER_INDEX) {
                    mByteFlowRender.loadShaderFromAssetsFile(LUT_A_SHADER_INDEX, getResources());
                } else {
                    mByteFlowRender.loadShaderFromAssetsFile(mCurrentShaderIndex, getResources());
                }
                //mByteFlowRender.setParamsInt(PARAM_TYPE_SET_SHADER_INDEX, mCurrentShaderIndex);
                if(mReadPixelsReady) {
                    mReadPixelsReady = false;
                    mByteFlowRender.readPixels(new Size(mByteFlowFrame.getHeight(),mByteFlowFrame.getWidth()), getResultImgFile(".jpg").getPath());
                }
                mByteFlowRender.requestRender();
                break;
            default:
                break;
        }
    }

    public static final File getResultImgFile(final String ext) {
        final File dir = new File(Environment.getExternalStorageDirectory(), RESULT_IMG_DIR);
        Log.d(TAG, "path=" + dir.toString());
        dir.mkdirs();
        if (dir.canWrite()) {
            return new File(dir, getDateTimeString() + ext);
        }
        return null;
    }

    private static final String getDateTimeString() {
        final GregorianCalendar now = new GregorianCalendar();
        return DateTime_FORMAT.format(now.getTime());
    }

    @Override
    public void onReadPixelsSaveToLocal(final String imgPath) {
        mReadPixelsReady = true;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(ImageActivity.this, "Save result image to path:" + imgPath, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
