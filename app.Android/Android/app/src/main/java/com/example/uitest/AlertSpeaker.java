package com.example.uitest;

import android.support.v4.app.FragmentActivity;

public class AlertSpeaker {
	private static BaiduSpeaker mBaiduSpeaker;
	
	AlertSpeaker(FragmentActivity fragmentActivity){
		mBaiduSpeaker = new BaiduSpeaker(fragmentActivity);
	}
	
	public static void welcomeSpeaking()
	{
		mBaiduSpeaker.speak("欢迎使用车联网安全应用");
	}
	
	public static void closeAlertSpeaking(int vIndex)
	{
		mBaiduSpeaker.speak(vIndex + "号车辆过近,请注意!");
	}

	public static void frontBreakingSpeaking()
	{
		mBaiduSpeaker.speak("前车有急刹,请注意!");
	}
	
	public static void leftCollectionSpeaking()
	{
		mBaiduSpeaker.speak("左侧有车辆驶入，请注意！");
	}
	
	public static void rightCollectionSpeaking()
	{
		mBaiduSpeaker.speak("右侧有车辆驶入，请注意！");
	}
	
	public static void newUnicastMsg()
	{
		mBaiduSpeaker.speak("您有一条新消息！");
	}
	
	public static void stuckSpeaking()
	{
		mBaiduSpeaker.speak("小黑楼南侧有施工，请绕道！");
	}
	public static void  jamSpeaking()
	{
		mBaiduSpeaker.speak("前方红色区域有堵塞，请注意！");
	}
}
