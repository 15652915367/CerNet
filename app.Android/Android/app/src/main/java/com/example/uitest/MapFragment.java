package com.example.uitest;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.SynchronousQueue;

import com.baidu.mapapi.SDKInitializer;
import com.baidu.mapapi.map.BaiduMap;
import com.baidu.mapapi.map.BitmapDescriptor;
import com.baidu.mapapi.map.BitmapDescriptorFactory;
import com.baidu.mapapi.map.CircleOptions;
import com.baidu.mapapi.map.InfoWindow;
import com.baidu.mapapi.map.MapStatus;
import com.baidu.mapapi.map.MapStatusUpdate;
import com.baidu.mapapi.map.MapStatusUpdateFactory;
import com.baidu.mapapi.map.MapView;
import com.baidu.mapapi.map.Marker;
import com.baidu.mapapi.map.MarkerOptions;
import com.baidu.mapapi.map.OverlayOptions;
import com.baidu.mapapi.map.PolylineOptions;
import com.baidu.mapapi.map.Stroke;
import com.baidu.mapapi.map.TextOptions;
import com.baidu.mapapi.model.LatLng;
import com.baidu.mapapi.utils.CoordinateConverter;
import com.baidu.mapapi.utils.CoordinateConverter.CoordType;

import android.R.integer;
import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import static com.example.uitest.UDPSocketService.jamStart;
import static com.example.uitest.VehicleNode.*;
import static com.example.uitest.SafetyAlert.*;
import static com.example.uitest.AlertSpeaker.*;

class VBdNode {
    int vIndex;
    LatLng ll;
    LatLng llText;
    LatLng llWarning;
    int distance = 30;

    VBdNode(double lat, double lng, int vIndex, int distance) {
        this.ll = new LatLng(lat, lng);
        this.vIndex = vIndex;
        this.distance = distance;
    }
}

public class MapFragment extends Fragment {
    static public boolean warningFlag;
    static public int warningCount = 0;

    private int carNumber = 15;

    private static final String TAG = "ServiceDemo";
    //private TextGroup time;
    //private TextGroup oil;
    //private TextGroup mile;
    //private TextGroup speed;

    private MapView mMapView;
    private BaiduMap mBaiduMap;
    //private Button btAlert;
    //private Button btWSMPorC2C;
    private View overlayView;
    private boolean flagFollow;
    private boolean flagAlert;
    private boolean flagFrontBrake;
    private boolean flagLZY;
    private TextView testNum;
    // 初始化全局 bitmap 信息，不用时及时 recycle
    BitmapDescriptor bdMyCar;
    BitmapDescriptor bdWarning;
    BitmapDescriptor bdOtherCars;
    BitmapDescriptor bdCar1;
    BitmapDescriptor bdCar2;
    BitmapDescriptor bdCar3;
    BitmapDescriptor bdCar4;
    private Timer mTimer;
    private TimerTask mTimerTask;
    static Handler mHandler;
    private String tempSelf = new String();
    private String tempElse = new String();
    DecimalFormat df2 = new DecimalFormat("0.##");
    DecimalFormat df0 = new DecimalFormat("0");

    LinkedList<VBdNode> llList;
    ArrayList<BitmapDescriptor> gifList;
    CoordinateConverter converter;
    VBdNode bdNode;
    OverlayOptions ooCar;
    OverlayOptions ooWarning;
    OverlayOptions textOption;
    MapStatus mMapStatus;
    MapStatusUpdate mMapStatusUpdate;

    private DatagramSocket socketUDP = null;
    private int portRemoteNum;
    private int portLocalNum;
    private String addressIP;
    private Thread thread = null;
    private String protocolFlag = null;

    private BroadcastReceiverFromService broadcastReceiverFromService;

    public class BroadcastReceiverFromService extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            //debug
            resetOverlay();
            findMyLocation(overlayView);
            startAlertMoudle();//pl
        }
    }

    private void startAlertMoudle() {
        int dstIndex;
        int frontIndex;
        int direction;

        if ((frontIndex = isFrontBrakeAlert()) != -1) {
            frontBreakingSpeaking();
            warningFlag = true;
        }

        direction = isCollectionAlert();
        System.out.println("direction=" + direction);
        if (direction == 0) {
            leftCollectionSpeaking();
            warningFlag = true;
        } else if (direction == 1) {
            rightCollectionSpeaking();
            warningFlag = true;
        }

        //if((dstIndex=isClosedAlert()) != -1)
        //closeAlertSpeaking(dstIndex);
    }

    @Override
    public void onStop() {
        getActivity().unregisterReceiver(broadcastReceiverFromService);
        super.onStop();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        SDKInitializer.initialize(getActivity().getApplicationContext());
        super.onCreate(savedInstanceState);

        llList = new LinkedList<VBdNode>();
        gifList = new ArrayList<BitmapDescriptor>();

        bdMyCar = BitmapDescriptorFactory.fromResource(R.drawable.mycar);
        bdWarning = BitmapDescriptorFactory.fromResource(R.drawable.warning);
        gifList.add(bdMyCar);
        for (int i = 1; i <= carNumber; i++)
            gifList.add(i, BitmapDescriptorFactory.fromResource(2130837510 + i));

        converter = new CoordinateConverter();
        converter.from(CoordType.GPS);

        broadcastReceiverFromService = new BroadcastReceiverFromService();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.example.uitest.mapFragment");
        getActivity().registerReceiver(broadcastReceiverFromService, filter);

        new AlertSpeaker(getActivity());
        welcomeSpeaking();
    }

    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        overlayView = inflater.inflate(R.layout.map_fragment, container, false);
        return overlayView;
    }

    @SuppressLint("HandlerLeak")
    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mMapView = (MapView) view.findViewById(R.id.bmapView);
        mBaiduMap = mMapView.getMap();
        flagFollow = true;
        flagAlert = false;
        flagFrontBrake = false;
        flagLZY = false;
        LatLng cenpt = new LatLng(40.002495, 116.339374);
        //if(!vList.isEmpty())
        //	cenpt = new LatLng(vList.getFirst().latitude,vList.getFirst().longitude);

        mMapStatus = new MapStatus.Builder()
                .target(cenpt)
                .zoom(20)
                .build();
        mBaiduMap.setMapStatus(MapStatusUpdateFactory.newMapStatus(mMapStatus));
        mBaiduMap.setOnMapLongClickListener(new BaiduMap.OnMapLongClickListener() {
            //地图长按事件监听回调函数
            public void onMapLongClick(LatLng point) {
                Button button = new Button(getActivity().getBaseContext());
                InfoWindow mInfoWindow = new InfoWindow(button, point, -47);
                button.setText("拥塞扩散!");
                mBaiduMap.showInfoWindow(mInfoWindow);
                button.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        // TODO Auto-generated method stub
                        Log.i("TEST", "button onClick");
                        jamStart = true;
                        mBaiduMap.hideInfoWindow();
                    }
                });
            }
        });

        // time = (TextGroup) view.findViewById(R.id.time);
        //oil = (TextGroup) view.findViewById(R.id.oil);
        //mile = (TextGroup) view.findViewById(R.id.mile);
        //speed = (TextGroup) view.findViewById(R.id.speed);
        //time.setPropertyText("Time");
        //oil.setPropertyText("Oil Mass");
        //mile.setPropertyText("Mileage");
        //speed.setPropertyText("Speed");
        //time.setDataText("null");
        //oil.setDataText("null");
        //mile.setDataText("null");
        //speed.setDataText("null");

        //btAlert = (Button) view.findViewById(R.id.alert);
        //btWSMPorC2C = (Button) view.findViewById(R.id.WSMPorC2C);

        addressIP = "10.1.1.1";
        portRemoteNum = 12345;
        try {
            socketUDP = new DatagramSocket();
        } catch (SocketException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        findMyLocation(overlayView);
        initOverlay();

        thread = new Thread(revMsg);
        thread.start();

		/*btAlert.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (btAlert.getText().equals("DisAlert")) {
					flagAlert = false;
					flagFrontBrake = true;
					btAlert.setText("BrakeAlert");
				}
				else if (btAlert.getText().equals("BrakeAlert")){
					flagAlert = false;
					flagFrontBrake = false;
					btAlert.setText("No Alert");
				}
				else if (btAlert.getText().equals("No Alert")){
					flagAlert = true;
					flagFrontBrake = false;
					btAlert.setText("DisAlert");
				}	
//				else if (bt_disAlert.getText().equals("LZY")) {
//					flagAlert = false;
//					flagLZY = true;
//					bt_disAlert.setText("Stop");
//				}
				
			}
		});*/
		
		/*btWSMPorC2C.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				if (btWSMPorC2C.getText().equals("WSMP/C2C")) {
					protocolFlag = "W";
					btWSMPorC2C.setText("WSMP");
				}
				else if (btWSMPorC2C.getText().equals("WSMP")){
					protocolFlag = "C";
					btWSMPorC2C.setText("C2C");
				}
				else if (btWSMPorC2C.getText().equals("C2C")){
					protocolFlag = "W";
					btWSMPorC2C.setText("WSMP");
				}
				if (btWSMPorC2C.getText().equals(" Uploaded")) {
					protocolFlag = "Y";
					btWSMPorC2C.setText("Done");
				}
				else if(btWSMPorC2C.getText().equals("Done")){
					protocolFlag = null;
					btWSMPorC2C.setText(" Uploaded");
				}				
			}
		});*/


        mTimer = new Timer();
        mTimerTask = new DataTask();
        mTimer.schedule(mTimerTask, 1000, 100);
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {

            }
        };
    }

    public void DrawCircle() {
        LatLng llCircle = new LatLng(39.968133, 116.365296);

        OverlayOptions ooCircle = new CircleOptions().fillColor(0xAAFF0000)

                .center(llCircle).stroke(new Stroke(5, 0xAAFF0000))

                .radius(50).zIndex(20);

        mBaiduMap.addOverlay(ooCircle);
    }

    public void DrawLines() {

        double startlats = 40.030718;
        double startlongs = 116.304648;
        double latline = 40.027362;
        double lngline = 116.305008;
        LatLng p1 = new LatLng(startlats, startlongs);
        LatLng p2 = new LatLng(latline, lngline);
        List<LatLng> points = new ArrayList<LatLng>();
        points.add(p1);
        points.add(p2);
        OverlayOptions ooPolyline = new PolylineOptions().width(10).color(0xAAFF0000).points(points);

        mBaiduMap.addOverlay(ooPolyline);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    public void initOverlay() {
        //DrawCircle();
        for (VehicleNode vNode : vList) {
            if (vList.indexOf(vNode) == 0)
                bdNode = new VBdNode(vNode.latitude, vNode.longitude, 0, 0);
            else
                bdNode = new VBdNode(vNode.latitude, vNode.longitude, vNode.vIndex, vNode.distance);
            llList.add(bdNode);
        }

        while (!llList.isEmpty()) {
            bdNode = llList.pop();
            converter.coord(bdNode.ll);
            bdNode.ll = converter.convert();

            ooCar = new MarkerOptions().position(bdNode.ll).icon(gifList.get(bdNode.vIndex))
                    .zIndex(7);
            mBaiduMap.addOverlay(ooCar);

            if (bdNode.vIndex != 0) {
                bdNode.llText = new LatLng(bdNode.ll.latitude - (0.001 / mBaiduMap.getMapStatus().zoom),
                        bdNode.ll.longitude);
                textOption = new TextOptions()
                        .fontSize(24)
                        //				.fontColor(0xFFFF00FF)
                        .text(String.valueOf(bdNode.distance + "m"))
                        .position(bdNode.llText);
                mBaiduMap.addOverlay(textOption);
            }
            System.out.println(mBaiduMap.getMapStatus().zoom);

            if ((bdNode.vIndex == 0) && (warningFlag == true)) {
/*				bdNode.llWarning = new LatLng(bdNode.ll.latitude - 0.001, bdNode.ll.longitude);
				ooWarning = new MarkerOptions().position(bdNode.llWarning).icon(bdWarning)
						.zIndex(1);
				mBaiduMap.addOverlay(ooWarning);*/
                DrawLines();
                warningCount++;
            }
            if (warningCount >= 10) {
                warningFlag = false;
                warningCount = 0;
            }

        }
    }

    public void clearOverlay() {
        mBaiduMap.clear();
    }

    public void resetOverlay() {
        clearOverlay();
        initOverlay();
    }

    public void findMyLocation(View view) {
        if (!vList.isEmpty()) {
            LatLng cenpt = new LatLng(vList.getFirst().latitude, vList.getFirst().longitude);
            converter.coord(cenpt);
            cenpt = converter.convert();
            MapStatus mMapStatus = new MapStatus.Builder()
                    .target(cenpt)
                    .build();
            mBaiduMap.setMapStatus(MapStatusUpdateFactory.newMapStatus(mMapStatus));
        }
    }

    @Override
    public void onPause() {
        // MapView的生命周期与Activity同步，当activity挂起时需调用MapView.onPause()
        mMapView.onPause();
        super.onPause();
    }

    @Override
    public void onResume() {
        // MapView的生命周期与Activity同步，当activity恢复时需调用MapView.onResume()
        mMapView.onResume();
        super.onResume();
    }

    @Override
    public void onDestroy() {
        // MapView的生命周期与Activity同步，当activity销毁时需调用MapView.destroy()
        mMapView.onDestroy();
        super.onDestroy();
        // 回收 bitmap 资源
        bdMyCar.recycle();
//		bdCar1.recycle();
//		bdCar2.recycle();
//		bdCar3.recycle();
    }

    class DataTask extends TimerTask {
        @Override
        public void run() {
            mHandler.sendEmptyMessage(0);
        }
    }

    private Runnable revMsg = new Runnable() {
        @Override
        public void run() {
            try {
                while (true) {
					    	/*if (protocolFlag != null){
							    String sendData = "ProtocolFlag:"+protocolFlag+","+"Data:Hello VANET!";
							    byte data [] = sendData.getBytes(); 
							    DatagramPacket packetS = new DatagramPacket(data,
							    		data.length,InetAddress.getByName(addressIP),portRemoteNum);	
						    	socketUDP.send(packetS);
						    	Thread.sleep(1000);
					    	}*/

                    if (protocolFlag != null) {
                        String sendData = "jam!";
                        byte data[] = sendData.getBytes();
                        DatagramPacket packetS = new DatagramPacket(data,
                                data.length, InetAddress.getByName(addressIP), portRemoteNum);
                        socketUDP.send(packetS);
                        Thread.sleep(3000);
                    }
                }

            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        }
    };
}
