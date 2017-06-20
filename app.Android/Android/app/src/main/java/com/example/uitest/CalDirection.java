package com.example.uitest;

import static com.example.uitest.SafetyAlert.*;
public class CalDirection {
	public static boolean isFront(double myHeading,double myLat,double myLng,double heading,double lat,double lng)
	{
		if ((judgeDirection(myHeading) == 1)&&(lng > myLng)&&(judgeDirection(heading)==1))
			return true;
		else if ((judgeDirection(myHeading) == 2)&&(lat < myLat)&&(judgeDirection(heading)==2))
			return true;
		else if ((judgeDirection(myHeading) == 3)&&(lng < myLng)&&(judgeDirection(heading)==3))
			return true;
		else if ((judgeDirection(myHeading) == 4)&&(lat > myLat)&&(judgeDirection(heading)==4))
			return true;
		else 
			return false;
	}
	
	public static int judgeDirection(double heading)
	{
		if((45 <= heading)&&(heading < 135))         //dong
			return 1;
		else if((135 <= heading)&&(heading < 225))   //nan
			return 2;
		else if((225 <= heading)&&(heading < 315))	 //xi
			return 3;
		else if((315 <= heading)||(heading < 45))	 //bei
			return 4;
		else {
			//debug
			System.out.println("ttttttttt");
			return -1;
		}
	}
	
	public static int judgeLeftOrRight(double myHeading,double myLat,double myLng,double heading,double lat,double lng)
	{
		
		if(calDistance(lat, lng) > 40)
			return -1;
			
		// 0 left
		// 1 right
		System.out.println("judgeDirection"+judgeDirection(myHeading)+"dsad"+judgeDirection(heading));
		
		if ((judgeDirection(myHeading) == 1)&&(lat < myLat)&&(judgeDirection(heading)==4))
			return 1;
		else if ((judgeDirection(myHeading) == 1)&&(lat > myLat)&&(judgeDirection(heading)==2))
			return 0;
		else if ((judgeDirection(myHeading) == 2)&&(lng < myLng)&&(judgeDirection(heading)==1))
			return 1;
		else if ((judgeDirection(myHeading) == 2)&&(lng > myLng)&&(judgeDirection(heading)==3))
			return 0;
		else if ((judgeDirection(myHeading) == 3)&&(lat < myLat)&&(judgeDirection(heading)==4))
			return 0;
		else if ((judgeDirection(myHeading) == 3)&&(lat > myLat)&&(judgeDirection(heading)==2))
			return 1;
		else if ((judgeDirection(myHeading) == 4)&&(lng < myLng)&&(judgeDirection(heading)==1))
			return 0;
		else if ((judgeDirection(myHeading) == 4)&&(lng > myLng)&&(judgeDirection(heading)==3))
			return 1;
		else {
			//debug
			System.out.println("trret");
			return -1;
		}
	}
}
