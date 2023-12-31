package com.example;
import javax.bluetooth.*;
import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class RemoteApp extends JFrame {

    private double lastX, lastY;

    private List<RemoteDevice> bluetoothDevices;
    private DefaultComboBoxModel<String> deviceNames;
    private JComboBox<String> deviceComboBox;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new RemoteApp().setVisible(true));
    }

    public RemoteApp() {
        setTitle("Remote App");
        setSize(500, 500);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        Canvas canvas = new Canvas();
        canvas.setPreferredSize(new Dimension(400, 400));
        canvas.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                lastX = evt.getX();
                lastY = evt.getY();
                System.out.println(lastX);
            }
        });
        canvas.addMouseMotionListener(new java.awt.event.MouseMotionAdapter() {
            public void mouseDragged(java.awt.event.MouseEvent evt) {
                Graphics g = canvas.getGraphics();
                g.setColor(Color.BLACK);
                g.drawLine((int) lastX, (int) lastY, evt.getX(), evt.getY());
                lastX = evt.getX();
                lastY = evt.getY();
            }
        });

        JButton searchButton = new JButton("Search Devices");
        searchButton.addActionListener(e -> searchBluetoothDevices());

        deviceNames = new DefaultComboBoxModel<>();
        deviceComboBox = new JComboBox<>(deviceNames);

        JButton connectButton = new JButton("Connect");
        connectButton.addActionListener(e -> connectToDevice());

        JPanel bluetoothPanel = new JPanel();
        bluetoothPanel.add(searchButton);
        bluetoothPanel.add(deviceComboBox);
        bluetoothPanel.add(connectButton);

        JPanel contentPane = new JPanel(new BorderLayout());
        contentPane.add(canvas, BorderLayout.CENTER);
        contentPane.add(bluetoothPanel, BorderLayout.SOUTH);

        setContentPane(contentPane);
    }

    private void searchBluetoothDevices() {
        try {
            bluetoothDevices = new ArrayList<>();
            deviceNames.removeAllElements();

            LocalDevice localDevice = LocalDevice.getLocalDevice();
            DiscoveryAgent agent = localDevice.getDiscoveryAgent();

            agent.startInquiry(DiscoveryAgent.GIAC, new RemoteDeviceDiscovery());

            System.out.println("Searching for devices...");
            Thread.sleep(5000);

            for (RemoteDevice device : bluetoothDevices) {
                deviceNames.addElement(device.getBluetoothAddress());
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void connectToDevice() {
        int selectedIndex = deviceComboBox.getSelectedIndex();
        if (selectedIndex >= 0 && selectedIndex < bluetoothDevices.size()) {
            RemoteDevice selectedDevice = bluetoothDevices.get(selectedIndex);
            System.out.println("Connecting to device: " + selectedDevice.getBluetoothAddress());
    
            try {
                LocalDevice localDevice = LocalDevice.getLocalDevice();
                DiscoveryAgent agent = localDevice.getDiscoveryAgent();
    
                UUID uuid = new UUID("0000110100001000800000805F9B34FB", false); // SPP UUID
    
                // Start service discovery with null attrSet
                agent.searchServices(null, new UUID[] { uuid }, selectedDevice, new DiscoveryListener() {
                    public void deviceDiscovered(RemoteDevice btDevice, DeviceClass cod) {
                    }
    
                    public void servicesDiscovered(int transID, ServiceRecord[] servRecord) {
                        for (ServiceRecord record : servRecord) {
                            String url = record.getConnectionURL(ServiceRecord.NOAUTHENTICATE_NOENCRYPT, false);
                            if (url != null) {
                                try {
                                    StreamConnection streamConnection = (StreamConnection) Connector.open(url);
                                    // Now you can use the streamConnection to communicate with the device
                                    // Implement your communication logic here
                                    System.out.println("Connected to: " + url);
                                    // Don't forget to close the streamConnection when done
                                    streamConnection.close();
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }
    
                    public void serviceSearchCompleted(int transID, int respCode) {
                    }
    
                    public void inquiryCompleted(int discType) {
                    }
                });
    
                // Sleep to allow time for service discovery
                Thread.sleep(5000);
    
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
            }
        } else {
            System.out.println("Please select a device before connecting.");
        }
    }
    

    private class RemoteDeviceDiscovery implements DiscoveryListener {

        @Override
        public void deviceDiscovered(RemoteDevice btDevice, DeviceClass cod) {
            System.out.println("Device discovered: " + btDevice.getBluetoothAddress());
            bluetoothDevices.add(btDevice);
            try{
                //deviceNames.addElement(btDevice.getFriendlyName(true));
                deviceNames.addElement(btDevice.getBluetoothAddress());

            }
            catch(Exception e)
            {
                deviceNames.addElement(btDevice.getBluetoothAddress());
            }
        }

        @Override
        public void inquiryCompleted(int discType) {
            if (discType == DiscoveryListener.INQUIRY_COMPLETED) {
                System.out.println("Device inquiry completed.");
            } else {
                System.out.println("Device inquiry failed.");
            }
        }

        @Override
        public void servicesDiscovered(int transID, ServiceRecord[] servRecord) {
            // Not used in this example
        }

        @Override
        public void serviceSearchCompleted(int transID, int respCode) {
            // Not used in this example
        }
    }
}
