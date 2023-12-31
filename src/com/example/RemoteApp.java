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
    private JLabel status;

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
        JButton connectButton = new JButton("Connect");
        deviceNames = new DefaultComboBoxModel<>();
        deviceComboBox = new JComboBox<>(deviceNames);
        status = new JLabel("Ready");

        Bluetooth bluetooth;
        try {
            bluetooth = new Bluetooth();
            searchButton.addActionListener(e -> {
                status.setText("Searching...");
                ((JButton) e.getSource()).setEnabled(false);

                try {
                    bluetooth.searchDevices(devices -> {
                        deviceNames.removeAllElements();
                        deviceNames.addAll(devices);
                        // ((JButton) e.getSource()).setText("Search Devices");
                        status.setText(devices.size()+" found");

                        ((JButton) e.getSource()).setEnabled(true);

                    });
                } catch (BluetoothStateException e1) {
                    e1.printStackTrace();
                }

            });
            connectButton.addActionListener(e -> {
                status.setText("Connecting...");
                ((JButton) e.getSource()).setEnabled(false);
                try {
                    bluetooth.connectToDevice(deviceComboBox.getSelectedIndex(), message -> {
                        ((JButton) e.getSource()).setText("Disconnect");
                        ((JButton) e.getSource()).setEnabled(true);
                                                        status.setText(message);

                    },
                            message -> {
                                ((JButton) e.getSource()).setText("Connect");
                                ((JButton) e.getSource()).setEnabled(true);
                                status.setText(message);

                            });
                } catch (BluetoothStateException e1) {
                    e1.printStackTrace();
                }
            });
        } catch (BluetoothStateException e) {
            e.printStackTrace();
        }

        JPanel bottomPanel = new JPanel(new GridLayout(2, 1, 10, 10));
        JPanel bluetoothPanel = new JPanel(new GridLayout(1, 3, 10, 10));
        bluetoothPanel.add(searchButton);
        bluetoothPanel.add(deviceComboBox);
        bluetoothPanel.add(connectButton);
        bottomPanel.add(bluetoothPanel);

        bottomPanel.add(status);

        JPanel contentPane = new JPanel(new BorderLayout(10, 10));
        contentPane.add(canvas, BorderLayout.CENTER);
        contentPane.add(bottomPanel, BorderLayout.SOUTH);

        setContentPane(contentPane);
    }
}
