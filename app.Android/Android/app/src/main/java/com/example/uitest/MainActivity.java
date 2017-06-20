package com.example.uitest;

import static android.content.Context.WIFI_SERVICE;

import java.util.ArrayList;
import java.util.List;

import com.example.uitest.UDPSocketService.MyBinder;

import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import static com.example.uitest.UDPSocketService.*;

public class MainActivity extends FragmentActivity implements OnClickListener {
	static public String ssid;
	private ViewPager mViewPager;
	private FragmentPagerAdapter mAdapter;
	private List<Fragment> mFragments = new ArrayList<Fragment>();	
    private TabGroup mapLayout;
    private TabGroup detailLayout;
    private UDPSocketService bindService;
    private ServiceConnection conn = new ServiceConnection() {
        
        @Override
        public void onServiceDisconnected(ComponentName name) {
            // TODO Auto-generated method stub
            
        }
        
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            // TODO Auto-generated method stub
            MyBinder binder = (MyBinder)service;
            bindService = binder.getService();
        }
    };
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(MainActivity.this,UDPSocketService.class);

		ssid = getConnectWifiSsid();
		
	    bindService(intent, conn, Context.BIND_AUTO_CREATE);
	    
	    //this.startService(new Intent(this,UDPSocketService.class));
		
	    setContentView(R.layout.activity_main);
		mViewPager = (ViewPager) findViewById(R.id.viewpager);
	    
		initView();
    	
		mapLayout.setTextViewText("实时路况");
		mapLayout.setImageResource(R.drawable.chekuang_pressed);
	    detailLayout.setTextViewText("车辆信息");
		detailLayout.setImageResource(R.drawable.dingwei_nomal);	
		
		mAdapter = new FragmentPagerAdapter(getSupportFragmentManager()) {
			@Override
			public Fragment getItem(int arg0) {
				// TODO Auto-generated method stub
				return mFragments.get(arg0);
			}

			@Override
			public int getCount() {
				// TODO Auto-generated method stub
				return mFragments.size();
			}
		};
		
		mViewPager.setAdapter(mAdapter);
		mViewPager.setOnPageChangeListener(new OnPageChangeListener() {
			@Override
			public void onPageSelected(int position) {
				// TODO Auto-generated method stub
				resetTab();
				switch (position) {
				case 0:
					((TabGroup) mapLayout
							.findViewById(R.id.map))
							.setImageResource(R.drawable.chekuang_pressed);
					break;
				case 1:
					((TabGroup) detailLayout
							.findViewById(R.id.detail))
							.setImageResource(R.drawable.dingwei_pressed);
					break;
				}
			}
			
			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
				// TODO Auto-generated method stub
			}
			
			@Override
			public void onPageScrollStateChanged(int arg0) {
				// TODO Auto-generated method stub
			}
		});
		
	}
	
	protected void resetTab() {
		((TabGroup) mapLayout.findViewById(R.id.map))
				.setImageResource(R.drawable.chekuang_nomal);
		((TabGroup) detailLayout.findViewById(R.id.detail))
				.setImageResource(R.drawable.dingwei_nomal);
	}
	
	private void  initView() {
		mapLayout = (TabGroup) findViewById(R.id.map);
		detailLayout = (TabGroup) findViewById(R.id.detail);
		mapLayout.setOnClickListener(this);
		detailLayout.setOnClickListener(this);
		MapFragment map_fragment = new MapFragment();
		DetailFragment detail_fragment = new DetailFragment();
		mFragments.add(map_fragment);
		mFragments.add(detail_fragment);
	}
	
	public void onClick(View view) {
		resetTab();
		switch (view.getId()) {
		case R.id.map: 
			mViewPager.setCurrentItem(0);
			break;
		case R.id.detail:
			mViewPager.setCurrentItem(1);
			break;
		default:
			break;
		}
	}
    
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if(item.getItemId() == R.id.offlineMap){
			Intent intent = new Intent(MainActivity.this, OfflineMap.class);
			startActivity(intent);
		}
		if(item.getItemId() == R.id.jam){
			jamStart = true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	private String getConnectWifiSsid(){  
        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);  
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();  
        return wifiInfo.getSSID();  
	}  
	
    public boolean onKeyDown(int keyCode, KeyEvent event) {
   		if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {

   			AlertDialog alertDialog = new AlertDialog.Builder(
   				MainActivity.this).setTitle("退出程序").setMessage("是否退出程序")
   				.setPositiveButton("确定",
   						new DialogInterface.OnClickListener() {

   							public void onClick(DialogInterface dialog,
   									int which) {
   									   bindService.onDestroy();
   								       android.os.Process.killProcess(
   								    		   android.os.Process.myPid());
   							}

   						})
   				.setNegativeButton("取消",
       				new DialogInterface.OnClickListener() {
       					public void onClick(DialogInterface dialog, int which) {
       						return;
       					}
       				}).create(); // 创建对话框

   		   alertDialog.show(); // 显示对话框
   		   
   			return false;
   		}
   		
   		return false;
   	}
}
