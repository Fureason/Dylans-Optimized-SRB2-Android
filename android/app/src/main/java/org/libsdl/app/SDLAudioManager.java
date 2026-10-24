package org.libsdl.app;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Build;
import android.util.Log;

public class SDLAudioManager
{
    protected static final String TAG = "SDLAudio";

    protected static AudioTrack mAudioTrack;
    protected static AudioRecord mAudioRecord;

    public static void initialize() {
        mAudioTrack = null;
        mAudioRecord = null;
    }



    protected static String getAudioFormatString(int audioFormat) {
        switch (audioFormat) {
        case AudioFormat.ENCODING_PCM_8BIT:
            return "8-bit";
        case AudioFormat.ENCODING_PCM_16BIT:
            return "16-bit";
        case AudioFormat.ENCODING_PCM_FLOAT:
            return "float";
        default:
            return Integer.toString(audioFormat);
        }
    }

    protected static int[] open(boolean isCapture, int sampleRate, int audioFormat, int desiredChannels, int desiredFrames) {
        int channelConfig;
        int sampleSize;
        int frameSize;

        Log.v(TAG, "Opening " + (isCapture ? "capture" : "playback") + ", requested " + desiredFrames + " frames of " + desiredChannels + " channel " + getAudioFormatString(audioFormat) + " audio at " + sampleRate + " Hz");

         
        if (Build.VERSION.SDK_INT < 21) {
            if (desiredChannels > 2) {
                desiredChannels = 2;
            }
            if (sampleRate < 8000) {
                sampleRate = 8000;
            } else if (sampleRate > 48000) {
                sampleRate = 48000;
            }
        }

        if (audioFormat == AudioFormat.ENCODING_PCM_FLOAT) {
            int minSDKVersion = (isCapture ? 23 : 21);
            if (Build.VERSION.SDK_INT < minSDKVersion) {
                audioFormat = AudioFormat.ENCODING_PCM_16BIT;
            }
        }
        switch (audioFormat)
        {
        case AudioFormat.ENCODING_PCM_8BIT:
            sampleSize = 1;
            break;
        case AudioFormat.ENCODING_PCM_16BIT:
            sampleSize = 2;
            break;
        case AudioFormat.ENCODING_PCM_FLOAT:
            sampleSize = 4;
            break;
        default:
            Log.v(TAG, "Requested format " + audioFormat + ", getting ENCODING_PCM_16BIT");
            audioFormat = AudioFormat.ENCODING_PCM_16BIT;
            sampleSize = 2;
            break;
        }

        if (isCapture) {
            switch (desiredChannels) {
            case 1:
                channelConfig = AudioFormat.CHANNEL_IN_MONO;
                break;
            case 2:
                channelConfig = AudioFormat.CHANNEL_IN_STEREO;
                break;
            default:
                Log.v(TAG, "Requested " + desiredChannels + " channels, getting stereo");
                desiredChannels = 2;
                channelConfig = AudioFormat.CHANNEL_IN_STEREO;
                break;
            }
        } else {
            switch (desiredChannels) {
            case 1:
                channelConfig = AudioFormat.CHANNEL_OUT_MONO;
                break;
            case 2:
                channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
                break;
            case 3:
                channelConfig = AudioFormat.CHANNEL_OUT_STEREO | AudioFormat.CHANNEL_OUT_FRONT_CENTER;
                break;
            case 4:
                channelConfig = AudioFormat.CHANNEL_OUT_QUAD;
                break;
            case 5:
                channelConfig = AudioFormat.CHANNEL_OUT_QUAD | AudioFormat.CHANNEL_OUT_FRONT_CENTER;
                break;
            case 6:
                channelConfig = AudioFormat.CHANNEL_OUT_5POINT1;
                break;
            case 7:
                channelConfig = AudioFormat.CHANNEL_OUT_5POINT1 | AudioFormat.CHANNEL_OUT_BACK_CENTER;
                break;
            case 8:
                if (Build.VERSION.SDK_INT >= 23) {
                    channelConfig = AudioFormat.CHANNEL_OUT_7POINT1_SURROUND;
                } else {
                    Log.v(TAG, "Requested " + desiredChannels + " channels, getting 5.1 surround");
                    desiredChannels = 6;
                    channelConfig = AudioFormat.CHANNEL_OUT_5POINT1;
                }
                break;
            default:
                Log.v(TAG, "Requested " + desiredChannels + " channels, getting stereo");
                desiredChannels = 2;
                channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
                break;
            }

  
                                                                         

                                                    
                                                        
             
                                                    
                                                         
             
                                                    
                                                          
             
                                                    
                                                           
             
                                                    
                                                       
             
                                                    
                                                        
             
                                                    
                                                                  
             
                                                    
                                                                   
             
                                                    
                                                         
             
                                                    
                                                       
             
                                                    
                                                        
             
  
        }
        frameSize = (sampleSize * desiredChannels);




        int minBufferSize;
        if (isCapture) {
            minBufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat);
        } else {
            minBufferSize = AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat);
        }
        desiredFrames = Math.max(desiredFrames, (minBufferSize + frameSize - 1) / frameSize);

        int[] results = new int[4];

        if (isCapture) {
            if (mAudioRecord == null) {
                mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.DEFAULT, sampleRate,
                        channelConfig, audioFormat, desiredFrames * frameSize);


                if (mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
                    Log.e(TAG, "Failed during initialization of AudioRecord");
                    mAudioRecord.release();
                    mAudioRecord = null;
                    return null;
                }

                mAudioRecord.startRecording();
            }

            results[0] = mAudioRecord.getSampleRate();
            results[1] = mAudioRecord.getAudioFormat();
            results[2] = mAudioRecord.getChannelCount();

        } else {
            if (mAudioTrack == null) {
                mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate, channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);




                if (mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                     

                    Log.e(TAG, "Failed during initialization of Audio Track");
                    mAudioTrack.release();
                    mAudioTrack = null;
                    return null;
                }

                mAudioTrack.play();
            }

            results[0] = mAudioTrack.getSampleRate();
            results[1] = mAudioTrack.getAudioFormat();
            results[2] = mAudioTrack.getChannelCount();
        }
        results[3] = desiredFrames;

        Log.v(TAG, "Opening " + (isCapture ? "capture" : "playback") + ", got " + results[3] + " frames of " + results[2] + " channel " + getAudioFormatString(results[1]) + " audio at " + results[0] + " Hz");

        return results;
    }

       
                                              
       
    public static int[] audioOpen(int sampleRate, int audioFormat, int desiredChannels, int desiredFrames) {
        return open(false, sampleRate, audioFormat, desiredChannels, desiredFrames);
    }

       
                                              
       
    public static void audioWriteFloatBuffer(float[] buffer) {
        if (mAudioTrack == null) {
            Log.e(TAG, "Attempted to make audio call with uninitialized audio!");
            return;
        }

        for (int i = 0; i < buffer.length;) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i, AudioTrack.WRITE_BLOCKING);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {

                }
            } else {
                Log.w(TAG, "SDL audio: error return from write(float)");
                return;
            }
        }
    }

       
                                              
       
    public static void audioWriteShortBuffer(short[] buffer) {
        if (mAudioTrack == null) {
            Log.e(TAG, "Attempted to make audio call with uninitialized audio!");
            return;
        }

        for (int i = 0; i < buffer.length;) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {

                }
            } else {
                Log.w(TAG, "SDL audio: error return from write(short)");
                return;
            }
        }
    }

       
                                              
       
    public static void audioWriteByteBuffer(byte[] buffer) {
        if (mAudioTrack == null) {
            Log.e(TAG, "Attempted to make audio call with uninitialized audio!");
            return;
        }

        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {

                }
            } else {
                Log.w(TAG, "SDL audio: error return from write(byte)");
                return;
            }
        }
    }

       
                                              
       
    public static int[] captureOpen(int sampleRate, int audioFormat, int desiredChannels, int desiredFrames) {
        return open(true, sampleRate, audioFormat, desiredChannels, desiredFrames);
    }

     
    public static int captureReadFloatBuffer(float[] buffer, boolean blocking) {
        return mAudioRecord.read(buffer, 0, buffer.length, blocking ? AudioRecord.READ_BLOCKING : AudioRecord.READ_NON_BLOCKING);
    }

     
    public static int captureReadShortBuffer(short[] buffer, boolean blocking) {
        if (Build.VERSION.SDK_INT < 23) {
            return mAudioRecord.read(buffer, 0, buffer.length);
        } else {
            return mAudioRecord.read(buffer, 0, buffer.length, blocking ? AudioRecord.READ_BLOCKING : AudioRecord.READ_NON_BLOCKING);
        }
    }

     
    public static int captureReadByteBuffer(byte[] buffer, boolean blocking) {
        if (Build.VERSION.SDK_INT < 23) {
            return mAudioRecord.read(buffer, 0, buffer.length);
        } else {
            return mAudioRecord.read(buffer, 0, buffer.length, blocking ? AudioRecord.READ_BLOCKING : AudioRecord.READ_NON_BLOCKING);
        }
    }

     
    public static void audioClose() {
        if (mAudioTrack != null) {
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
    }

     
    public static void captureClose() {
        if (mAudioRecord != null) {
            mAudioRecord.stop();
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }

     
    public static void audioSetThreadPriority(boolean iscapture, int device_id) {
        try {

             
            if (iscapture) {
                Thread.currentThread().setName("SDLAudioC" + device_id);
            } else {
                Thread.currentThread().setName("SDLAudioP" + device_id);
            }

             
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_AUDIO);

        } catch (Exception e) {
            Log.v(TAG, "modify thread properties failed " + e.toString());
        }
    }

    public static native int nativeSetupJNI();
}
