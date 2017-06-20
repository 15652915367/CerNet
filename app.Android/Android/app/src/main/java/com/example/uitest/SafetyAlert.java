package com.example.uitest;

import static com.example.uitest.VehicleNode.*;
import static com.example.uitest.CalDirection.*;

public class SafetyAlert {
	static double distanceAlertPoint = 5;
	static double frontBrakeAlertPoint = 25;
	//debug
	static double collectinoAlertPoint =5 ;

	
	public static int isClosedAlert(){
		double distance;
		double min = Double.MAX_VALUE;
		int vIndex = -1;
		
		for(int i=1; i<vList.size(); i++){
			distance = calDistance(vList.get(i).latitude, vList.get(i).longitude);
			if(distance < min){
				min = distance;
				vIndex = vList.get(i).vIndex;
			}
		}
		if(min <= distanceAlertPoint)
			return vIndex;
		else
			return -1;
	}
	
	public static int isFrontBrakeAlert(){
		double max = Double.MIN_VALUE;
		double accelt = 0;
		int vIndex = -1;
		
		for (int i=1; i<vList.size(); i++){
			accelt = vList.get(i).altitude;
			if((accelt > max)&&(isFront(vList.getFirst().heading,vList.getFirst().latitude,vList.getFirst().longitude,
					vList.get(i).heading,vList.get(i).latitude,vList.get(i).longitude))){
				max = accelt;
				vIndex = vList.get(i).vIndex;
			}
		}
		if(max >= frontBrakeAlertPoint)
			return vIndex;
		else
			return -1;
	}
	
	public static int isCollectionAlert(){
		int direction;
		System.out.println("vList.size="+vList.size());
		for (int i=1; i<vList.size(); i++){
			if(vList.get(i).speed >= collectinoAlertPoint)
				if ((direction=judgeLeftOrRight(vList.getFirst().heading,vList.getFirst().latitude,vList.getFirst().longitude,
						vList.get(i).heading,vList.get(i).latitude,vList.get(i).longitude)) !=-1)
				{	System.out.println("testset");
					return direction;				
				}
		}
		return -1;		
	}
	
	public static int calDistance(double latitude, double longitude)
	{
		double distance;
		double earthRadius = 6378.137;
		double radLat1 = radian(vList.getFirst().latitude);
	    double radLat2 = radian(latitude);
	    double a = radLat1 - radLat2;
	    double b = radian(vList.getFirst().longitude) - radian(longitude);
	    distance = 2 * Math.asin((Math.sqrt(Math.pow(Math.sin(a / 2), 2) 
	    		+ Math.cos(radLat1) * Math.cos(radLat2) * Math.pow(Math.sin(b / 2), 2) )));
	    distance = distance * earthRadius;
	    distance= Math.round(distance * 10000) / 10000.0 * 1000;
	    
	    return (int) distance;
	}
	
	private static double radian(double d)
    {
		return d * Math.PI / 180.0;
    }
}
