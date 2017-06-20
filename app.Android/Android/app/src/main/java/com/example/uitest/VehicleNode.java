package com.example.uitest;

import java.util.LinkedList;

public class VehicleNode {
	
	public int vIndex;
	public double latitude;
	public double longitude;
	public double speed;
	public double heading;
	public double altitude;
	public String localTime;
	public int	  distance;
	public static int count;
	
	public static LinkedList<VehicleNode> vList = new LinkedList<VehicleNode>();
	
	public static int byte1ToInt(byte[] bytes){
    	int target = (bytes[0] & 0xff) & 0x000000ff;
    	return target;
    }
    
    public static int byte2ToInt(byte[] bytes){
    	int target = (bytes[0] & 0xff) | ((bytes[1] << 8 ) & 0xff00) & 0x0000ffff;
    	return target;
    }
    
    public static int byte4ToInt(byte[] bytes){
    	int target = (bytes[0] & 0xff) | ((bytes[1] << 8) & 0xff00)
    			| ((bytes[2] << 24) >>> 8) | (bytes[3] << 24);
    	return target;
    }
    
    public static byte[] subBytes(byte[] bytes, int begin, int length){
    	byte[] temp = new byte[length];
    	for(int i=0; i<length; i++)
    		temp[i] = bytes[i+begin];
    	return temp;
    }
    
    public static void printVehicleList(LinkedList<VehicleNode> vList){
    	for(VehicleNode Vnode: vList){
    		System.out.println("Index: " + vList.indexOf(Vnode));
    		System.out.println("Car No." + Vnode.vIndex);
    		System.out.println("latitude: " + Vnode.latitude);
    		System.out.println("longitude: " + Vnode.longitude);
    		System.out.println("speed: " + Vnode.speed);
    		System.out.println("heading: " + Vnode.heading);
    		System.out.println("altitude: " + Vnode.altitude);
    		System.out.println("time: "+ Vnode.localTime);
    	}
    }
    
    public static double convertFormatLat(int latitude)
    {
    	return ((double)(latitude >> 3)) / 1000000f;
    }
    
    public static double convertFormatLong(int longitude)
    {
    	return ((double)(longitude >> 3)) / 1000000f;
    }
    
    public static double convertFormatSpd(int spd)
    {
    	return (double)(spd / 100f) * 3.6f;
    }
    
    public static double convertFormatHead(int heading)
    {
    	return (double)(heading / 100f);
    }
    
    public static double convertFormatAlt(int altitude)
    {
    	return (double)(altitude / 10.f);
    }
  
}
