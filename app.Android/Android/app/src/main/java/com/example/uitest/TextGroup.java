package com.example.uitest;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.TextView;

public class TextGroup extends LinearLayout {
	private TextView tv_data;
	private TextView tv_property;

	public TextGroup(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public TextGroup(Context context, AttributeSet attrs) {
		super(context,attrs);
		LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		inflater.inflate(R.layout.top_custom, this);
		tv_data = (TextView) findViewById(R.id.data);
		tv_property = (TextView) findViewById(R.id.property);
	}
	
	//设置显示数据的内容
		public void setDataText(String text) {
			tv_data.setText(text);
		}
	
	//设置显示属性的文字
	public void setPropertyText(String text) {
		tv_property.setText(text);
	}

}
