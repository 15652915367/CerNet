package com.example.uitest;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class TabGroup extends LinearLayout {
	private ImageView icon;
	private TextView function;

	public TabGroup(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public TabGroup(Context context, AttributeSet attrs) {
		super(context,attrs);
		LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		inflater.inflate(R.layout.bottom_tab, this);
		icon = (ImageView) findViewById(R.id.icon);
		function = (TextView) findViewById(R.id.function);
	}
	
	//设置显示图标
		public void setImageResource(int resId) {
			icon.setImageResource(resId);
		}
	
	//设置显示属性的文字
	public void setTextViewText(String text) {
		function.setText(text);
	}
}
