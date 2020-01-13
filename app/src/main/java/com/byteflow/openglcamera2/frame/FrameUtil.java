package com.byteflow.openglcamera2.frame;

import android.os.Environment;
import android.util.Log;
import android.util.Size;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FrameUtil {
    private static final String TAG = "FrameUtil";
    private static String sBaseDirPath = null;

    public static String getBaseDirPath() {
        if (sBaseDirPath == null) {
            sBaseDirPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/ByteFlow";
            File file = new File(sBaseDirPath);
            if (!file.exists()) {
                if (!file.mkdir()) {
                    sBaseDirPath = null;
                }
            }
        }
        return sBaseDirPath;
    }

    public static ByteFlowFrame decodeFrame(String filePath) {
        File file = new File(filePath);
        if (file != null && getImageSize(filePath) != null) {
            FileInputStream in = null;
            filePath.split("[\\d]+[xX]{1}[\\d]+");
            try {
                in = new FileInputStream(file);
                byte[] data=new byte[in.available()];
                in.read(data);
                in.close();
                return new ByteFlowFrame(data, getImageSize(filePath).getWidth(), getImageSize(filePath).getHeight());
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    public static String encodeFrame(ByteFlowFrame flowFrame) {
        if (flowFrame.getData() != null) {
            StringBuilder stringBuilder = new StringBuilder("Img");
            stringBuilder.append("_").append(System.currentTimeMillis()).append("_")
                    .append(flowFrame.getWidth()).append("x").append(flowFrame.getHeight()).append(".I420");
            Log.d(TAG, "encodeFrame() called with: filePath = [" + stringBuilder.toString() + "]");
            String filePath = getBaseDirPath() + File.separator + stringBuilder.toString();
            File file = new File(filePath);
            try {
                FileOutputStream outputStream  =new FileOutputStream(file);
                outputStream.write(flowFrame.getData());
                outputStream.close();
                return filePath;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    public static Size getImageSize(String filePath) {
        Pattern pattern = Pattern.compile("[\\d]+[xX]{1}[\\d]+");
        Matcher matcher =pattern.matcher(filePath);
        if (matcher.find()) {
            Log.d(TAG, "getImageSize() called with: filePath = [" + matcher.group() + "]");
            String[] strs = matcher.group().split("[xX]");
            return new Size(Integer.valueOf(strs[0]), Integer.valueOf(strs[1]));
        }
        return null;
    }

}


