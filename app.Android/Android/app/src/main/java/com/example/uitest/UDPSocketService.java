package com.example.uitest;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import static com.example.uitest.VehicleNode.*;
import static com.example.uitest.SafetyAlert.*;
import static com.example.uitest.DetailFragment.*;
import static com.example.uitest.AlertSpeaker.*;
import static com.example.uitest.MapFragment.*;

public class UDPSocketService extends Service {
    static public String unicastRecvMsg;
    static public boolean unicastRecvFlag;
    static public String unicastRecvID;
    static public boolean jamStart;

    private int vNodeSize = 28;
    //private int vTimeSize = 4;
    private int vIndexSize = 1;
    private int vLatSize = 4;
    private int vLongSize = 4;
    private int vSpdSize = 2;
    private int vHeadSize = 2;
    private int vAltSize = 2;

    private int vIndexOffset = 9;
    //private int vTimeOffset = 10;
    private int vLatOffset = 14;
    private int vLongOffset = 18;
    private int vSpdOffset = 22;
    private int vHeadOffset = 24;
    private int vAltOffset = 26;

    public static final String ACTION = "com.example.uitest.UDPSocketService";
    private DatagramSocket socket = null;
    private DatagramSocket socketUnicast = null;
    private Thread thread = null;
    private Thread threadUnicast = null;
    private int port = 50001;
    private int portUnicast = 50002;
    @SuppressLint("SimpleDateFormat")
    SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

    Intent intent = new Intent();

    @Override
    public void onCreate() {
        init();
        VehicleNode vvNode = new VehicleNode();

        vvNode.vIndex = 1;
        vvNode.latitude = 39.968133;
        vvNode.longitude = 116.365296;
        vvNode.speed = 0.05;
        vvNode.heading = 71;
        vvNode.altitude = 0;
        vList.add(0, vvNode);//pl

    	/*vvNode = new VehicleNode();
        vvNode.vIndex = 2;
    	vvNode.latitude = 29.536949;
    	vvNode.longitude = 106.511488;
    	vvNode.speed = 0.23;
    	vvNode.heading = 85;
    	vvNode.altitude = 0;
    	vList.add(1,vvNode);*/
    	/*
    	vvNode = new VehicleNode();
		vvNode.vIndex = 3;
    	vvNode.latitude = 39.960280;
    	vvNode.longitude = 116.349322;
    	vvNode.speed = 0.31;
    	vvNode.heading = 175;
    	vvNode.altitude = 0;
    	vList.add(2,vvNode);*/
        super.onCreate();
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return myBinder;
    }

    public class MyBinder extends Binder {
        public UDPSocketService getService() {
            return UDPSocketService.this;
        }
    }

    private MyBinder myBinder = new MyBinder();

    @Override
    public boolean onUnbind(Intent intent) {
        onDestroy();
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void init() {
        try {
            socket = new DatagramSocket(port);
            socketUnicast = new DatagramSocket(portUnicast);
            thread = new Thread(revMsgSelf);
            threadUnicast = new Thread(revMsgUnicast);
            thread.start();
            threadUnicast.start();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private VehicleNode dumpPacket(DatagramPacket packetR, int i) {
        VehicleNode vNode = new VehicleNode();
        vNode.vIndex = byte1ToInt(subBytes(packetR.getData(), vIndexOffset + i * vNodeSize, vIndexSize));
        vNode.latitude = convertFormatLat(byte4ToInt(
                subBytes(packetR.getData(), vLatOffset + i * vNodeSize, vLatSize)));
        vNode.longitude = convertFormatLong(byte4ToInt(
                subBytes(packetR.getData(), vLongOffset + i * vNodeSize, vLongSize)));
        vNode.speed = convertFormatSpd(byte2ToInt(
                subBytes(packetR.getData(), vSpdOffset + i * vNodeSize, vSpdSize)));
        vNode.heading = convertFormatHead(byte2ToInt(
                subBytes(packetR.getData(), vHeadOffset + i * vNodeSize, vHeadSize)));
        vNode.altitude = convertFormatAlt(byte2ToInt(
                subBytes(packetR.getData(), vAltOffset + i * vNodeSize, vAltSize)));
        //vNode.altitude = 30;
        vNode.localTime = df.format(new Date());
        return vNode;
    }

    private void getDistance() {
        for (int i = 1; i < vList.size(); i++)
            vList.get(i).distance = calDistance(vList.get(i).latitude, vList.get(i).longitude);
    }

    private void sendIntent(String intentValue, String actionName) {
        intent.putExtra("updateVList", intentValue);
        intent.setAction(actionName);
        sendBroadcast(intent);
    }

    private void sortVList() {
        int minValue = Integer.MAX_VALUE;
        int minSubScript = 1;
        VehicleNode tmpNode;

        for (int j = 1; j < vList.size(); j++) {
            for (int i = j; i < vList.size(); i++)
                if (vList.get(i).vIndex < minValue) {
                    minValue = vList.get(i).vIndex;
                    minSubScript = i;
                }
            tmpNode = vList.get(minSubScript);
            vList.remove(minSubScript);
            vList.add(j, tmpNode);
            minValue = Integer.MAX_VALUE;
        }
    }

    private void sendObu(String type, String unicastMsg, String targetCarID, String sourceCarID) throws IOException {
        String serverIP = "10.1.1.1";
        String str = targetCarID + "," + sourceCarID + "," + type + "msg:" + unicastMsg;
        byte data[] = str.getBytes();
        try {
            InetAddress serverAddress = InetAddress.getByName(serverIP);
            DatagramPacket packetS = new DatagramPacket(data,
                    data.length, serverAddress, portUnicast);
            //从本地端口给指定IP的远程端口发数据包
            socketUnicast.send(packetS);
            unicastMsg = null;
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        isToSend = false;
    }

    private Runnable revMsgSelf = new Runnable() {
        @Override
        public void run() {
            while (true) {
                byte data[] = new byte[1024];
                DatagramPacket packetR = new DatagramPacket(data, data.length);
                try {
                    socket.receive(packetR);

                    if (!vList.isEmpty())
                        vList.clear();

                    int vNumber = VehicleNode.byte2ToInt(VehicleNode.subBytes(packetR.getData(), 0, 2));
                    for (int i = 0; i < vNumber; i++)
                        vList.add(dumpPacket(packetR, i));

                    new VehicleNode().count++;
                    sortVList();
                    getDistance();

                    if (!vList.isEmpty()) {
                        sendIntent("updateMap", "com.example.uitest.mapFragment");
                        sendIntent("updateDetail", "com.example.uitest.detailFragment");
                    }

                    //printVehicleList(vList);
                    //System.out.println(targetCarID);
                    if (isToSend)
                        sendObu("unicast", unicastMsg, targetCarID, sourceCarID);
                    if (jamStart) {
                        sendObu("S", "S", "S", "S");
                        jamStart = false;
                    }
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
    };

    private Runnable revMsgUnicast = new Runnable() {
        @Override
        public void run() {
            while (true) {
                byte data[] = new byte[1024];
                DatagramPacket packetR = new DatagramPacket(data, data.length);
                try {
                    socketUnicast.receive(packetR);
                    unicastRecvMsg = new String(packetR.getData(),
                            packetR.getOffset(), packetR.getLength());

                    switch (unicastRecvMsg.substring(2, 3)) {
                        case "A":
                            unicastRecvID = "CAR 1";
                            break;
                        case "B":
                            unicastRecvID = "CAR 2";
                            break;
                        case "C":
                            unicastRecvID = "CAR 3";
                            break;
                        case "D":
                            unicastRecvID = "CAR 4";
                            break;
                        case "E":
                            unicastRecvID = "CAR 5";
                            break;
                        case "F":
                            unicastRecvID = "CAR 6";
                            break;
                        case "G":
                            unicastRecvID = "CAR 7";
                            break;
                        case "H":
                            unicastRecvID = "CAR 8";
                            break;
                        case "I":
                            unicastRecvID = "CAR 9";
                            break;
                        case "J":
                            unicastRecvID = "CAR 10";
                            break;
                        case "S":
                            unicastRecvID = "S";
                            break;
                        default:
                            unicastRecvID = "N";
                            break;
                    }
                    if (unicastRecvID == "S") {
                        jamSpeaking();
                        warningFlag = true;
                        continue;
                    }
                    if (unicastRecvID == "N") {
                        stuckSpeaking();
                        warningFlag = true;
                        continue;
                    }

                    unicastRecvMsg = unicastRecvMsg.substring(unicastRecvMsg.indexOf("msg:") + 4);
                    unicastRecvFlag = true;
                    newUnicastMsg();

                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
    };
}