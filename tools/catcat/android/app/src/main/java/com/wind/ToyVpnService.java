package com.wind;

import android.net.VpnService;
import android.os.Handler;
import android.content.Intent;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;

import android.app.PendingIntent;

import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import android.widget.Toast;

public class ToyVpnService extends VpnService implements Runnable, Handler.Callback {

    private static final String TAG = "ToyVpnService";
    private static final int HEADER_LEN = 1;
    private static final int BUFF_SIZE = 2000;
    private static final int MAX_RECV_SIZE = BUFF_SIZE - HEADER_LEN;

    private static final byte XOR_MAGIC_1 = (byte)220;
    private static final byte XOR_MAGIC_2 = (byte)171;


    private Handler mHandler;
    private Thread mThread;

    private String mServerAddress;
    private String mServerPort;
    private int     mMTU;

    //private PendingIntent mConfigureIntent;

    private ParcelFileDescriptor mTun;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        if (mHandler == null) {
            mHandler = new Handler(this);
        }

        if (mThread != null)
        {
            mThread.interrupt();
        }

        String prefix = getPackageName();
        mServerAddress = intent.getStringExtra(prefix + ".ADDRESS");
        mServerPort = intent.getStringExtra(prefix + ".PORT");
        mMTU = Integer.parseInt(intent.getStringExtra(prefix + ".MTU"));
        if (mMTU < 1 )
            mMTU = 1470;

        mThread = new Thread(this, "ToyVpnThread");
        mThread.start();

        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        try {
            if (mThread != null) {
                mThread.interrupt();
            }

            if(mTun != null)
            {

                    mTun.close();

                mTun = null;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public boolean handleMessage(Message message) {
        if (message != null && message.obj != null) {
            Toast.makeText(this, (String)message.obj, Toast.LENGTH_LONG).show();
        }
        return true;
    }

    private void openTun()
    {
        if (mTun != null)
        {
            Log.i(TAG, "Using the previous interface");
            return;
        }

        Builder builder = new Builder();
        //builder.setMtu(1500);//1472,548
        builder.setMtu(mMTU);
        builder.addAddress("192.168.0.1", 32);
        builder.addRoute("0.0.0.0", 0);
        builder.addDnsServer("8.8.8.8");


        mTun = builder.setSession(mServerAddress)
                .establish();
    }

    private void xorData(byte[] inOutBuff, int sz, byte magic1, byte magic2)
    {
        for(int i = 0; i < sz; ++i)
        {
            inOutBuff[i] ^= magic1;
            inOutBuff[i] ^= magic2;
        }
    }

    private byte xorHead(byte[] buff, int pos, int sz)
    {
        byte c = 0;

        for(int i = pos; i < sz + pos; ++i)
        {
            c ^= buff[i];
        }

        return c;
    }

    @Override
    public synchronized void run() {
        Log.i(TAG, "VPN thread starting");
        String setpRecord = "";
        DatagramChannel socket = null;
        try {

            int port = Integer.parseInt(mServerPort);

            InetSocketAddress server = new InetSocketAddress(
                    mServerAddress, port);

            socket = DatagramChannel.open();
            if(!protect(socket.socket()))
            {
                throw new IllegalStateException("Cannot protect the tunnel");
            }
            setpRecord = "build socket success";
            socket.connect(server);
            socket.configureBlocking(false);

            openTun();
            setpRecord = "open tun success";
            FileInputStream in = new FileInputStream(mTun.getFileDescriptor());
            FileOutputStream out = new FileOutputStream(mTun.getFileDescriptor());

            ByteBuffer packet = ByteBuffer.allocate(BUFF_SIZE);
            ByteBuffer outPacket = ByteBuffer.allocate(BUFF_SIZE);

            while (true) {

                boolean idle = true;

                int length = in.read(packet.array(), 0, MAX_RECV_SIZE);
                if (length > 0)
                {
                    idle = false;
                    byte[] packetArray = packet.array();
                    byte[] outPacketArray = outPacket.array();

                    outPacketArray[0] = xorHead(packetArray,0, length);
                    System.arraycopy(packetArray, 0, outPacketArray, 1, length);
                    ++length;

//                    if (length > 1400)
//                        Log.i(TAG, "send data Len:" + length);

                    xorData(outPacketArray, length, XOR_MAGIC_1, XOR_MAGIC_2);
                    outPacket.limit(length);
                    setpRecord = "Begin tun to net write";
                    socket.write(outPacket);
                    packet.clear();
                    outPacket.clear();
                }

                length = socket.read(packet);
                if (length > 0)
                {
                    idle = false;
                    byte[] packetArray = packet.array();
                    xorData(packetArray, length, XOR_MAGIC_2, XOR_MAGIC_1);
                    --length;
                    if(length > 0)
                    {
                        if(packetArray[0] == xorHead(packetArray , 1, length)) {
//                            if (length > 1400)
//                                Log.i(TAG, "write tun Len:" + length);

                            setpRecord = "Begin net to tun write";
                            out.write(packetArray, 1, length);
                        }
                        else
                        {
                            Log.i(TAG, "Data Decrpty error!");
                        }
                    }
                    packet.clear();
                }

                if (idle)
                {
                    Thread.sleep(100);
                }
            }
        }
        catch (Exception e)
        {
            Message msg = mHandler.obtainMessage();
            msg.obj = setpRecord + " Exce:" + e.toString();
            mHandler.sendMessage(msg);
        }
        finally
        {
            if(socket != null)
            {
                try {
                    socket.close();
                }catch (Exception e)
                {

                }
                socket = null;
            }
        }
    }
}
