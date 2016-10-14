package ca.dominicphillips.carlapplication;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by s401321 on 14/10/2016.
 */
public class RobotListActivity extends Activity {
    ListView listView;
    ArrayAdapter<String> adapter;
    WifiManager manager;
    WifiInfo info;
    ArrayList<String> list;

    @Override
    public void onBackPressed() {
        startActivity(new Intent(this,MainActivity.class));
        finish();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.robotlist_layout);
        manager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        info = manager.getConnectionInfo();

        listView = (ListView) findViewById(R.id.list_robots);
        list = new ArrayList<String>();

        final BroadcastReceiver mWifiScanReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context c, Intent intent) {
                if (intent.getAction().equals(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
                    List<ScanResult> results = manager.getScanResults();
                    list.clear();
                    for (ScanResult res : results) {
                        if (res.SSID.contains("Carl"))
                        list.add(res.SSID);
                    }
                    adapter.notifyDataSetChanged();
                    // add your logic here
                }
            }
        };

        registerReceiver(mWifiScanReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                manager.startScan();
            }
        }, 100, 10000);

        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, list);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Toast.makeText(view.getContext() ,"Connecting to " +  adapter.getItem(position-1).toString()  , Toast.LENGTH_SHORT).show();
                WifiConfiguration conf = new WifiConfiguration();
                conf.SSID = "\"" + adapter.getItem(position-1).toString() + "\"";
                conf.preSharedKey = "\"CarlRobot\"";
                int netID = manager.addNetwork(conf);
                manager.disconnect();
                manager.enableNetwork(netID,true);
                manager.reconnect();
                setResult(1);
                startActivity(new Intent(view.getContext(),MainActivity.class));
                finish();
            }
        });
        TextView header = new TextView(this);
        header.setText("Available Robots");
        header.setTextSize(32);
        header.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
        header.setTextColor(Color.RED);
        listView.addHeaderView(header);


    }
}
