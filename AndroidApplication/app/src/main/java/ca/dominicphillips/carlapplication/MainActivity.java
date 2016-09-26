package ca.dominicphillips.carlapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_layout);

    }

    public void showController(View view) {
        Intent intent = new Intent(this,ControllerActivity.class);
        startActivity(intent);
    }

    public void showActions(View view) {

    }
}
