package com.wind;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.app.Activity;

import android.widget.Button;
import android.widget.TextView;
import android.view.View;
import android.net.VpnService;

import android.content.Intent;


public class MainActivity extends Activity implements View.OnClickListener  {

    private TextView mServerAddress;
    private TextView mServerPort;
    private TextView mMTU;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mServerAddress = (TextView)findViewById(R.id.address);
        mServerPort = (TextView)findViewById(R.id.port);
        mMTU = (TextView)findViewById(R.id.mtu);

        findViewById(R.id.connect).setOnClickListener(this);
        findViewById(R.id.stop).setOnClickListener(this);

        SharedPreferences pref = getSharedPreferences("setting", Context.MODE_PRIVATE);
        String serverip = pref.getString("serverip", "");
        String serverport = pref.getString("serverport", "2200");
        String mtu = pref.getString("mtu", "547");

        mServerAddress.setText(serverip);
        mServerPort.setText(serverport);
        mMTU.setText(mtu);
    }

    private void SaveConfig()
    {
        SharedPreferences pref = getSharedPreferences("setting", Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = pref.edit();
        editor.putString("serverip", mServerAddress.getText().toString());
        editor.putString("serverport", mServerPort.getText().toString());
        editor.putString("mtu", mMTU.getText().toString());
        editor.commit();
    }

    @Override
    public void onClick(View v)
    {
        if (v.getId() == R.id.stop)
        {
            Intent intent = new Intent(this, ToyVpnService.class);
            stopService(intent) ;
            return;
        }
        else if(v.getId() == R.id.connect)
        {
            Intent intent = VpnService.prepare(this);
            if (intent != null) {
                startActivityForResult(intent, 0);
            } else {
                onActivityResult(0, RESULT_OK, null);
            }
        }

    }

    @Override
    protected void onActivityResult(int request, int result, Intent data)
    {
        if(result == RESULT_OK)
        {
            SaveConfig();

            String prefix = getPackageName();
            Intent intent = new Intent(this, ToyVpnService.class);
            intent.putExtra(prefix + ".ADDRESS", mServerAddress.getText().toString());
            intent.putExtra(prefix + ".PORT", mServerPort.getText().toString());
            intent.putExtra(prefix + ".MTU", mMTU.getText().toString());
            startService(intent);
        }
    }

}
