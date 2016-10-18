package ca.dominicphillips.carlapplication;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    WifiManager manager;
    WifiInfo info;

    @Override
    public void onBackPressed() {
        finishAffinity();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);

        manager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        info = manager.getConnectionInfo();
        ((TextView) findViewById(R.id.text_update)).setText("Build " + new Date(BuildConfig.TIMESTAMP).toString().replaceAll("GMT\\+\\d+:\\d+", ""));
        updateTitle();

    }

    public void showController(View view) {
        startActivity(new Intent(this, ControllerActivity.class));
    }

    public void showActions(View view) {
        startActivity(new Intent(this, ActionActivity.class));
    }

    public void showRobotList(View view) {
        Intent intent = new Intent(this, RobotListActivity.class);
//        intent.putExtra("title",)
        startActivity(intent);
    }

    public void updateTitle() {
        getSupportActionBar().setTitle("Connected to " + info.getSSID());
    }

}

