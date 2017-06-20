package com.example.uitest;

import com.baidu.speechsynthesizer.SpeechSynthesizer;
import com.baidu.speechsynthesizer.SpeechSynthesizerListener;
import com.baidu.speechsynthesizer.publicutility.SpeechError;
import com.baidu.speechsynthesizer.publicutility.SpeechLogger;

import android.content.Context;

public class BaiduSpeaker {
	//public BaiduSpeaker(Context con, String str, SpeechSynthesizerListener sp)
	
		private SpeechSynthesizer speechSynthesizer;


		public BaiduSpeaker(Context con)
		{
			
			speechSynthesizer =
	                SpeechSynthesizer.newInstance(SpeechSynthesizer.SYNTHESIZER_AUTO, con, "holder",
	                		mListener);
			
	        System.loadLibrary("gnustl_shared");

	        try {
	            System.loadLibrary("BDSpeechDecoder_V1");
	        } catch (UnsatisfiedLinkError e) {
	            SpeechLogger.logD("load BDSpeechDecoder_V1 failed, ignore");
	        }
	        System.loadLibrary("bd_etts");
	        System.loadLibrary("bds");
	        

	        speechSynthesizer.setApiKey("5jVwWapqZ85EvACmVOkO9Tz6", "aCuvdNPcVs3PgueDlbIRBuWtT9Fi4QIe");

	        speechSynthesizer.setAppId("6942621");

	        String ttsTextModelFilePath =
	                con.getApplicationInfo().dataDir + "/lib/libbd_etts_text.dat.so";
	        String ttsSpeechModelFilePath =
	                con.getApplicationInfo().dataDir + "/lib/libbd_etts_speech_female.dat.so";
	        speechSynthesizer.setParam(SpeechSynthesizer.EXTRA_CONNECTION_TIMEOUT, "1");
	        speechSynthesizer.setParam(SpeechSynthesizer.EXTRA_SO_TIMEOUT, "1");
	        speechSynthesizer.setParam(SpeechSynthesizer.PARAM_TTS_TEXT_MODEL_FILE, ttsTextModelFilePath);
	        speechSynthesizer.setParam(SpeechSynthesizer.PARAM_TTS_SPEECH_MODEL_FILE, ttsSpeechModelFilePath);
	        speechSynthesizer.initEngine();
		}
		public int speak(String str) {
			int ret = speechSynthesizer.speak(str);
			return ret;
		}
	    
	    
		private SpeechSynthesizerListener mListener = new SpeechSynthesizerListener() {
			
			@Override
			public void onSynthesizeFinish(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onStartWorking(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onSpeechStart(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onSpeechResume(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onSpeechProgressChanged(SpeechSynthesizer arg0, int arg1) {

				
			}
			
			@Override
			public void onSpeechPause(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onSpeechFinish(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onNewDataArrive(SpeechSynthesizer arg0, byte[] arg1,
					boolean arg2) {

				
			}
			
			@Override
			public void onError(SpeechSynthesizer arg0, SpeechError arg1) {

				
			}
			
			@Override
			public void onCancel(SpeechSynthesizer arg0) {

				
			}
			
			@Override
			public void onBufferProgressChanged(SpeechSynthesizer arg0, int arg1) {

				
			}
		};
		
	    private void setParams(){
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_SPEAKER, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_VOLUME, "5");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_SPEED, "5");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_PITCH, "5");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_AUDIO_ENCODE, "1");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_AUDIO_RATE, "4");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_LANGUAGE, "ZH");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_NUM_PRON, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_ENG_PRON, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_PUNC, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_BACKGROUND, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_STYLE, "0");
	        // speechSynthesizer.setParam(SpeechSynthesizer.PARAM_TERRITORY, "0");
	    }
}
