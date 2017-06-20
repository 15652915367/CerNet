package com.example.uitest;

import java.net.UnknownHostException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.AbsListView;
import android.widget.ArrayAdapter;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import static com.example.uitest.VehicleNode.*;
import static com.example.uitest.UDPSocketService.*;
import static com.example.uitest.MainActivity.*;
@SuppressLint("InflateParams")
public class DetailFragment extends Fragment 
{
	static public String targetCarID;
	static public String sourceCarID;
	static public boolean isToSend = false;
	static public String unicastMsg;
	
	private TextView  name;
	private TextView  message; 	
	private ExpandableListView list;
    private List<String> group; 
    private List<String> child;
    private ExpandableListAdapter adapter;
	DecimalFormat df6=new DecimalFormat("0.######");
	DecimalFormat df2=new DecimalFormat("0.##");
	private BroadcastReceiverFromService broadcastReceiverFromService;  
	private Spinner mySpinner;    
	private Button 	sendBtn;
	private EditText  sendText;
	private TextView  chatText;
	private List<String> all;
	private ArrayAdapter<String> adapterSpinner;

	public DetailFragment()
    {
        initdatas();
    } 
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);

		adapter = new BaseExpandableListAdapter()
		{
		   @Override
	       public Object getChild(int groupPosition, int childPosition) 
		   {
			   return child.get(childPosition);
		   }
		   @Override
		   public long getChildId(int groupPosition, int childPosition) 
		   {
			   return childPosition;
		   }
		   @Override
		   public View getChildView(int groupPosition, int childPosition,
				   boolean isLastChild, View convertView, ViewGroup parent) 
		   {
		        if (null == convertView || !(convertView instanceof LinearLayout)) 
		        {
		              convertView = LayoutInflater.from(getActivity()).inflate(
		                      R.layout.detail_item, null);
		        }  
		        message = (TextView)convertView.findViewById(R.id.message);
		        name=(TextView)convertView.findViewById(R.id.name);
		
		        name.setText(child.get(childPosition).toString());
		        if(vList.size() == group.size()){
			    	if (getGroup(groupPosition).toString()=="MY CAR")
			    	{ 
			    		switch(name.getText().toString())
			    		{
				    		case "Latitude:": message.setText(String.valueOf(df6.format(vList.getFirst().latitude)) + "°");break;
							case "Longitude:": message.setText(String.valueOf(df6.format(vList.getFirst().longitude)) + "°");break;
							case "Speed:": message.setText(String.valueOf(df2.format(vList.getFirst().speed)) + "km/h");break;
							case "Heading:": message.setText(String.valueOf(df2.format(vList.getFirst().heading)) + "°");break;
							case "Accelt:": message.setText(String.valueOf(df2.format(vList.getFirst().altitude)) + "");break;
							case "Count/Distance:":message.setText(sourceCarID);break;
							default: message.setText(""); break;
						}
					}
			    	for (int i=1; i<vList.size(); i++){
			    		if(getGroup(groupPosition).toString()==group.get(i))
				    		switch(name.getText().toString()){
					    		case "Latitude:": message.setText(String.valueOf(df6.format(vList.get(i).latitude)) + "°");break;
								case "Longitude:": message.setText(String.valueOf(df6.format(vList.get(i).longitude)) + "°");break;
								case "Speed:": message.setText(String.valueOf(df2.format(vList.get(i).speed)) + "km/h");break;
								case "Heading:": message.setText(String.valueOf(df2.format(vList.get(i).heading)) + "°");break;
								case "Accelt:": message.setText(String.valueOf(df2.format(vList.get(i).altitude)) + "");break;
								case "Count/Distance:": message.setText(String.valueOf(vList.get(i).distance) + "m");break;
								default: message.setText(""); break;
							}
			    			
			    	}
		        }
		    	
		    	return convertView;
		    	
		    }
		   
		   @Override
		   public int getChildrenCount(int groupPosition) 
		   {
			   return child.size();
		   }
		   @Override
		   public Object getGroup(int groupPosition) 
		   {
			   return group.get(groupPosition);
		   }
		   @Override
		   public int getGroupCount() 
		   {
			   return group.size();
		   }
		   @Override
		   public long getGroupId(int groupPosition) 
		   {
			   return groupPosition;
		   }
		   
		   @Override
		   public View getGroupView(int groupPosition, boolean isExpanded,   
		                  View convertView, ViewGroup parent) 
		   {
			   LinearLayout ll = new LinearLayout(getActivity());
			   TextView textView = getTextView();
			   textView.setText(getGroup(groupPosition).toString());
			   ll.addView(textView);
			   return ll;
		   }

		   public TextView getTextView()
		   {
		       AbsListView.LayoutParams lp = new AbsListView.LayoutParams(   
		    		   ViewGroup.LayoutParams.MATCH_PARENT,120);        
		       TextView textView = new TextView(getActivity());   
		       textView.setLayoutParams(lp);   
		       textView.setGravity(Gravity.CENTER_VERTICAL | Gravity.LEFT);   
		       textView.setPadding(40, 0, 0, 0);
		       textView.setTextSize(25);
		       textView.setTextColor(Color.RED);
		       return textView;   
		      }
		      
		   @Override
		   public boolean hasStableIds() 
		   {
			   return true;
		   }
		   @Override
		   public boolean isChildSelectable(int groupPosition, int childPosition) 
		   {
			   return true;
		   }
		};
	}  
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View details = inflater.inflate(R.layout.activity_details, container, false);
		broadcastReceiverFromService=new BroadcastReceiverFromService();  
		IntentFilter filter = new IntentFilter();  
		filter.addAction("com.example.uitest.detailFragment");  
		getActivity().registerReceiver(broadcastReceiverFromService, filter); 
		return details;
	}
	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);
		list = (ExpandableListView)view.findViewById(R.id.detail);

		all = new ArrayList<>();
		adapterSpinner = new ArrayAdapter<>(getActivity().getBaseContext(), R.layout.myspinner, all);
		mySpinner = (Spinner)view.findViewById(R.id.spinner);
		mySpinner.setAdapter(adapterSpinner);

		sendBtn = (Button)view.findViewById(R.id.buttonSend);
		sendText = (EditText)view.findViewById(R.id.editText1);
		chatText = (TextView)view.findViewById(R.id.textView1);
		sendBtn.setOnClickListener(new View.OnClickListener() {  
			    public void onClick(View v) {  
			        // TODO Auto-generated method stub  
			    	targetCarID = mySpinner.getSelectedItem().toString();
			    	switch (targetCarID.substring(4 , 5)){
			    		case "1":targetCarID = "A";break;
			    		case "2":targetCarID = "B";break;
			    		case "3":targetCarID = "C";break;
			    		case "4":targetCarID = "D";break;
			    		case "5":targetCarID = "E";break;
						case "6":targetCarID = "F";break;
						case "7":targetCarID = "G";break;
						case "8":targetCarID = "H";break;
						case "9":targetCarID = "I";break;
						case "10":targetCarID = "J";break;
			    		default:targetCarID = "N";break;
			    	}
			    	switch (ssid){
			    		case "\"obu1\"":sourceCarID = "A";break;
			    		case "\"obu2\"":sourceCarID = "B";break;
			    		case "\"obu3\"":sourceCarID = "C";break;
			    		case "\"obu4\"":sourceCarID = "D";break;
			    		case "\"obu5\"":sourceCarID = "E";break;
						case "\"obu6\"":sourceCarID = "F";break;
						case "\"obu7\"":sourceCarID = "G";break;
						case "\"obu8\"":sourceCarID = "H";break;
						case "\"obu9\"":sourceCarID = "I";break;
						case "\"obu10\"":sourceCarID = "J";break;
			    		default: sourceCarID = "N";break;
			    	}
					chatText.setText("dsad");
					isToSend = true;	
					unicastMsg = sendText.getText().toString().trim();
					chatText.append("\n"+"我的车： "+unicastMsg);
					sendText.setText(null);
			    }  
		});
		
		list.setAdapter(adapter);
	}
	private void updateSpinner(){
		String car = "CAR";
		adapterSpinner.clear();
		if(vList.size() > 1)
		{
			for(int i=1; i<vList.size(); i++)
				adapterSpinner.add(car + " " + vList.get(i).vIndex);
		}
	}
	public class BroadcastReceiverFromService extends BroadcastReceiver  
	{  
	    @Override  
	    public void onReceive(Context context, Intent intent)   
	    {
			updateSpinner();
	    	clearGroupName();
	    	addGroupName();
	    	if(!group.isEmpty())
	    		((BaseExpandableListAdapter) adapter).notifyDataSetChanged();
	    	if(unicastRecvFlag)
	    	{
	    		chatText.append("\n"+ unicastRecvID +"： "+unicastRecvMsg);
	    		unicastRecvFlag = false;
	    		unicastRecvMsg = null;
	    	}
	    }          
	}  
	private void initdatas() 
	{
		group= new ArrayList<>();
		child= new ArrayList<>();
		add(group,new String[]{"MY CAR"});//pl
		add(child,new String[]{"Latitude:","Longitude:","Speed:","Heading:","Accelt:"});
	}
	public void add(List<String> list,String[] c)
	{
		for(int i=0;i<c.length;i++)
			list.add(c[i]);
	}
	private void clearGroupName()
	{
		group.clear();
	}
	private void addGroupName()
	{
		String car = "CAR";
		String myCar = "MY CAR";
		if(!vList.isEmpty())
		{
		for(int i=0; i<vList.size(); i++)
			if(i == 0)
				group.add(myCar);
			else
				group.add(car + " " + vList.get(i).vIndex);
	
		}
	}
}