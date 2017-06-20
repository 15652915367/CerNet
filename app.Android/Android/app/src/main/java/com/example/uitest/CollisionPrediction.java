package com.example.uitest;

public class CollisionPrediction {
	private static Gps data1;
	private static Gps data2;

	public  CollisionPrediction(){};

	public CollisionPrediction(Gps data1, Gps data2){
		this.data1 = data1;
		this.data2 = data2;
	}
	public void setData1(Gps data1){
		this.data1 = data1;
	}

	public void setData2(Gps data2){
		this.data2 = data2;
	}
	
	
	public static int collisionDirection(){
		
		int intersectionThreshold = 10;
		int preCollisionTime = 15;
		
		double x1 = data1.getX();
		double y1 = data1.getY();
		double angle1 = data1.stdAngle(data1.getAngle());
		double radian1 = data1.angleToRadian(angle1);
		double v1 = data1.getSpeed();
		
		double x2 = data2.getX();
		double y2 = data2.getY();
		double angle2 = data2.stdAngle(data2.getAngle());
		double radian2 = data2.angleToRadian(angle2);
		double v2 = data2.getSpeed();
		
		double timeDifference = data2.getGpsTime() - data1.getGpsTime();
		
		//xi,yi insection point location
		double xi = 0;
		double yi = 0;
		//the time of smartphone/vehicle to the intersection point
		double t1 = 0;
		double t2 = 0;
		
		double k1 = Math.tan(radian1);
		double k2 = Math.tan(radian2);
		
		//y - y1 = k1 (x - x1)
		//y - y2 = k2 ��x - x2)
		//ת����
		//k1 * x - y = k1 * x1 - y1
		//k2 * x - y = k2 * x2 - y2
		//common solution of�� 
		//ax+by=e
		//cx+dy=f
		//a = k1, b = -1, e = k1 * x1 - y1
		//c = k2, d = -1, f = k2 * x2 - y2
		double a;
		double b;
		double e;
		double c;
		double d;
		double f;
		
		if (((radian1 == Math.PI/2) || (radian1 == Math.PI*3/2)) && ((radian2 == Math.PI/2) || (radian2 == Math.PI*3/2))){
			return 0;
		}else if (((radian1 == Math.PI/2) || (radian1 == Math.PI*3/2))){
			b = 0;
			a = 1;
			e = x1;
			c = k2;
			d = -1;
			f = (k2 * x2 - y2);
		}else if (((radian2 == Math.PI/2) || (radian2 == Math.PI*3/2))){  		
			d = 0;
			c = 1;
			f = x2;
			a = k1;
			b = -1;
			e = (k1 * x1 - y1);
		}else{
			a = k1;
			b = -1;
			e = (k1 * x1 - y1);
			c = k2;
			d = -1;
			f = (k2 * x2 - y2);
		}
		if ((a*d - b*c) == 0){
			return 0;
		}else{
			xi = (e*d - b*f)/(a*d - b*c);
			yi = (a*f - e*c)/(a*d - b*c);
		}
		
		if ((v1 == 0) || (v2 == 0)){
			return 0;
		}
		t1 = Math.sqrt((xi - x1)*(xi - x1) + (yi - y1)*(yi -y1))/v1;
		t2 = Math.sqrt((xi - x2)*(xi - x2) + (yi - y2)*(yi -y2))/v2;
		t2 = t2 + timeDifference;

		if ((t1 > preCollisionTime) && (t2 > preCollisionTime)){
			return 0;
		}

		if (Math.abs(t2-t1) >= intersectionThreshold) {
			return 0;	
		}else if((Math.cos(radian2)*(xi-x2) >= 0)&&(Math.sin(radian2)*(yi-y2) >= 0)
				&&(Math.cos(radian1)*(xi-x1) >= 0)&&(Math.sin(radian1)*(yi-y1) >= 0) ){
			//left:(0,pi)(-2pi,-pi);
			//right:(-pi,0)(pi,2pi)
			double radian_dif = xyToRadian(x1-x2,y1-y2)-radian2;
			if(((radian_dif >= 0) && (radian_dif <= Math.PI)||
					((radian_dif >= -2 * Math.PI) &&(radian_dif <= -Math.PI)))){
				return 2;
			}
			if(((radian_dif >= -Math.PI) && (radian_dif < 0)||
					((radian_dif > Math.PI) &&(radian_dif < 2*Math.PI)))){
				return 1;
			}	
		}else{
			return 0;
			}
		return -1;
	}
	
	public static double xyToRadian(double x, double y){
		double radian = 0;
		if(x >= 0){
			radian = Math.asin(y/Math.sqrt(x*x + y*y));
			if (radian < 0){
				radian = 2*Math.PI + radian;
			}
		}else{
			radian = Math.PI - Math.asin(y/Math.sqrt(x*x + y*y));
		}
		return radian;
	}
}