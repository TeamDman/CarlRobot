package ca.dominicphillips.carlapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.util.Date;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);
        ((TextView) findViewById(R.id.text_update)).setText("Build " + new Date(BuildConfig.TIMESTAMP).toString().replaceAll("GMT\\+\\d+:\\d+","dada"));
    }

    public void showController(View view) {
        startActivity(new Intent(this,ControllerActivity.class));
    }

    public void showActions(View view) {
        startActivity(new Intent(this,ActionActivity.class));
    }
}
