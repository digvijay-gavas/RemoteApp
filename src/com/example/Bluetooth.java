package com.example;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

public class Bluetooth {

    private List<RemoteDevice> bluetoothDevices;
    StreamConnection streamConnection;
    RemoteDevice selectedDevice;
    private InputStream input;
    BufferedReader bufferedReader;
    private OutputStream output;
    private DiscoveryAgent agent;
    private boolean isDeviceConnected = false;

    Bluetooth() throws BluetoothStateException {
        bluetoothDevices = new ArrayList<>();
        LocalDevice localDevice = LocalDevice.getLocalDevice();
        agent = localDevice.getDiscoveryAgent();

    }

    interface SeachComplete {
        void apply(List<String> devices);
    }

    public void searchDevices(SeachComplete searchComplete) throws BluetoothStateException {
        System.out.println("Searching for devices...");
        bluetoothDevices.clear();
        agent.startInquiry(DiscoveryAgent.GIAC, new DiscoveryListener() {
            @Override
            public void deviceDiscovered(RemoteDevice btDevice, DeviceClass arg1) {
                bluetoothDevices.add(btDevice);
            }

            @Override
            public void inquiryCompleted(int arg0) {
                searchComplete.apply(bluetoothDevices.stream().map(device -> device.getBluetoothAddress()).toList());
            }

            @Override
            public void serviceSearchCompleted(int arg0, int arg1) {
            }

            @Override
            public void servicesDiscovered(int arg0, ServiceRecord[] arg1) {
            }
        });
    }

    interface Complete {
        void apply(String message);
    }

    interface Failed {
        void apply(String message);
    }

    public void connectToDevice(int selectedIndex, Complete connectionComplete,
            Failed connectionFailed) throws BluetoothStateException {
        if (selectedIndex >= 0 && selectedIndex < bluetoothDevices.size()) {
            selectedDevice = bluetoothDevices.get(selectedIndex);
            System.out.println("Connecting to device: " + selectedDevice.getBluetoothAddress());

            UUID uuid = new UUID("0000110100001000800000805F9B34FB", false);
            agent.searchServices(null, new UUID[] { uuid }, selectedDevice, new DiscoveryListener() {

                @Override
                public void deviceDiscovered(RemoteDevice arg0, DeviceClass arg1) {
                }

                @Override
                public void inquiryCompleted(int arg0) {
                }

                @Override
                public void serviceSearchCompleted(int arg0, int arg1) {
                    if (arg1 != 1)
                        connectionFailed.apply("Connection failed with code: " + arg1);
                }

                @Override
                public void servicesDiscovered(int arg0, ServiceRecord[] servRecord) {
                    System.out.println("servicesDiscovered");
                    for (ServiceRecord record : servRecord) {
                        String url = record.getConnectionURL(ServiceRecord.NOAUTHENTICATE_NOENCRYPT, false);
                        if (url != null) {
                            try {
                                streamConnection = (StreamConnection) Connector.open(url);
                                System.out.println("Connected to: " + url);
                                input = streamConnection.openInputStream();
                                output = streamConnection.openOutputStream();
                                bufferedReader = new BufferedReader(new InputStreamReader(input));
                                connectionComplete.apply(
                                        "Connected to: " + selectedDevice.getBluetoothAddress() + "(" + url + ")");
                                isDeviceConnected = true;

                            } catch (IOException e) {
                                connectionFailed.apply(e.getMessage());
                                e.printStackTrace();
                            }
                        } else {
                            connectionFailed.apply("Not Connected");
                        }
                    }
                }

            });
        } else {
            System.out.println("Please select a device before connecting.");
        }
    }

    public void disconnect(Complete disconnectionComplete, Failed disconnectionFailed) {
        try {
            streamConnection.close();
            disconnectionComplete.apply("Disconnected " + selectedDevice.getBluetoothAddress());
        } catch (IOException e) {
            disconnectionFailed.apply("Failed to disconnect " + selectedDevice.getBluetoothAddress());
            e.printStackTrace();
        }
    }

    public boolean isConnected() {
        return isDeviceConnected;
    }

    public void send(String message) throws IOException {
        output.write(message.getBytes());
        output.flush();
    }
    public void send(byte message) throws IOException {
        output.write(message);
        output.flush();
    }

    public String readLine() throws IOException {
        return bufferedReader.readLine();
    }

    public String readPacket() throws IOException {
        StringBuffer strBuffer = new StringBuffer();

        while (input.available() > 0) {

            byte readByte=input.readNBytes(1)[0];
            strBuffer.append((char)readByte);
            if(readByte==10)
                break;
        }

        return strBuffer.toString();
    }

    public String read() throws IOException {
        StringBuffer strBuffer = new StringBuffer();
        while (input.available() > 0) {
            if (input.available() > 10) {
                strBuffer.append(new String(input.readNBytes(10)));
            } else {
                strBuffer.append(new String(input.readNBytes(1)));
            }
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        return strBuffer.toString();
    }
}
