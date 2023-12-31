package com.example;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;

public class Bluetooth {

    private List<RemoteDevice> bluetoothDevices;
    private InputStream input;
    private OutputStream output;
    private DiscoveryAgent agent;

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

    interface ConnectionComplete {
        void apply(String message);
    }

    interface ConnectionFailed {
        void apply(String message);
    }

    public void connectToDevice(int selectedIndex, ConnectionComplete connectionComplete,
            ConnectionFailed connectionFailed) throws BluetoothStateException {
        if (selectedIndex >= 0 && selectedIndex < bluetoothDevices.size()) {
            RemoteDevice selectedDevice = bluetoothDevices.get(selectedIndex);
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
                                StreamConnection streamConnection = (StreamConnection) Connector.open(url);
                                System.out.println("Connected to: " + url);
                                input = streamConnection.openInputStream();
                                output = streamConnection.openOutputStream();
                                connectionComplete.apply(
                                        "Connected to: " + selectedDevice.getBluetoothAddress() + "(" + url + ")");

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

    public void send(String message) throws IOException {
        output.write(message.getBytes());
    }

    public String read() throws IOException {
        return input.readAllBytes().toString();
    }
}
