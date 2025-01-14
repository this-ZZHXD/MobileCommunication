import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.*;
import java.net.*;
import java.util.*;

public class ChatServer {
    private static final int PORT = 12345;
    private static HashSet<ClientHandler> clients = new HashSet<>();
    private static ArrayList<String> chatHistory = new ArrayList<>();
    private static HashMap<String, String> users = new HashMap<>(); // username -> password
    private static ArrayList<String> connectionHistory = new ArrayList<>();
    private static JFrame frame;
    private static JTable userTable;
    private static DefaultTableModel userTableModel;
    private static JTextArea historyArea;
    private static JTabbedPane tabbedPane;
    private static final String LOG_FILE = "connection_history.txt";
    private static JTextArea connectionArea;

    public static void main(String[] args) {
        createAndShowGUI();
        startServer();
    }

    private static void createAndShowGUI() {
        frame = new JFrame("Chat Server Management");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(600, 400);

        tabbedPane = new JTabbedPane();

        // User Management Panel
        JPanel userPanel = new JPanel(new BorderLayout());
        String[] columns = {"Username", "Password"};
        userTableModel = new DefaultTableModel(columns, 0);
        userTable = new JTable(userTableModel);
        userPanel.add(new JScrollPane(userTable), BorderLayout.CENTER);

        JPanel buttonPanel = new JPanel();
        JButton addButton = new JButton("Add User");
        JButton editButton = new JButton("Edit User");
        JButton deleteButton = new JButton("Delete User");
        buttonPanel.add(addButton);
        buttonPanel.add(editButton);
        buttonPanel.add(deleteButton);
        userPanel.add(buttonPanel, BorderLayout.SOUTH);

        // Connection History Panel
        JPanel connectionPanel = new JPanel(new BorderLayout());
        connectionArea = new JTextArea();
        connectionArea.setEditable(false);
        connectionPanel.add(new JScrollPane(connectionArea), BorderLayout.CENTER);
        
        JButton saveHistoryButton = new JButton("Save History");
        saveHistoryButton.addActionListener(e -> saveConnectionHistory());
        connectionPanel.add(saveHistoryButton, BorderLayout.SOUTH);

        // Chat History Panel
        JPanel chatPanel = new JPanel(new BorderLayout());
        historyArea = new JTextArea();
        historyArea.setEditable(false);
        chatPanel.add(new JScrollPane(historyArea), BorderLayout.CENTER);

        tabbedPane.addTab("User Management", userPanel);
        tabbedPane.addTab("Connection History", connectionPanel);
        tabbedPane.addTab("Chat History", chatPanel);

        frame.add(tabbedPane);

        // Add action listeners
        addButton.addActionListener(e -> {
            String username = JOptionPane.showInputDialog(frame, "Enter username:");
            String password = JOptionPane.showInputDialog(frame, "Enter password:");
            if (username != null && password != null) {
                users.put(username, password);
                updateUserTable();
            }
        });

        editButton.addActionListener(e -> {
            int row = userTable.getSelectedRow();
            if (row >= 0) {
                String username = (String) userTable.getValueAt(row, 0);
                String newPassword = JOptionPane.showInputDialog(frame, "Enter new password:");
                if (newPassword != null) {
                    users.put(username, newPassword);
                    updateUserTable();
                }
            }
        });

        deleteButton.addActionListener(e -> {
            int row = userTable.getSelectedRow();
            if (row >= 0) {
                String username = (String) userTable.getValueAt(row, 0);
                users.remove(username);
                updateUserTable();
            }
        });

        frame.setVisible(true);
    }

    private static void updateUserTable() {
        userTableModel.setRowCount(0);
        for (Map.Entry<String, String> entry : users.entrySet()) {
            userTableModel.addRow(new Object[]{entry.getKey(), entry.getValue()});
        }
    }

    private static void startServer() {
        try {
            ServerSocket serverSocket = new ServerSocket(PORT);
            logConnection("Server started on port " + PORT);

            while (true) {
                Socket socket = serverSocket.accept();
                String clientIP = socket.getInetAddress().getHostAddress();
                logConnection("New connection from " + clientIP);
                
                ClientHandler clientHandler = new ClientHandler(socket);
                clients.add(clientHandler);
                new Thread(clientHandler).start();
            }
        } catch (IOException e) {
            logConnection("Server error: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void saveConnectionHistory() {
        try (PrintWriter writer = new PrintWriter(new FileWriter(LOG_FILE, true))) {
            for (String entry : connectionHistory) {
                writer.println(entry);
            }
            JOptionPane.showMessageDialog(frame, "Connection history saved to " + LOG_FILE);
        } catch (IOException e) {
            JOptionPane.showMessageDialog(frame, "Error saving history: " + e.getMessage());
        }
    }

    private static void logConnection(String message) {
        String timestamp = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
        String logMessage = timestamp + " - " + message;
        connectionHistory.add(logMessage);
        
        SwingUtilities.invokeLater(() -> {
            connectionArea.append(logMessage + "\n");
            connectionArea.setCaretPosition(connectionArea.getDocument().getLength());
        });
        
        try (PrintWriter writer = new PrintWriter(new FileWriter(LOG_FILE, true))) {
            writer.println(logMessage);
        } catch (IOException e) {
            System.err.println("Error writing to log file: " + e.getMessage());
        }
    }

    static class ClientHandler implements Runnable {
        private Socket socket;
        private PrintWriter out;
        private BufferedReader in;
        private String username;
        private String clientIP;

        public ClientHandler(Socket socket) {
            this.socket = socket;
            this.clientIP = socket.getInetAddress().getHostAddress();
            try {
                out = new PrintWriter(socket.getOutputStream(), true);
                in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                logConnection("Client initialized from IP: " + clientIP);
            } catch (IOException e) {
                logConnection("Error initializing client connection from IP " + clientIP + ": " + e.getMessage());
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            try {
                System.out.println("Client handler started for IP: " + clientIP);
                while (true) {
                    String message = in.readLine();
                    if (message == null) {
                        System.out.println("Client disconnected from IP: " + clientIP);
                        break;
                    }
                    
                    System.out.println("Received message from " + (username != null ? username : "unknown") + 
                                     " (" + clientIP + "): " + message);
                    
                    if (message.startsWith("LOGIN ")) {
                        handleLogin(message);
                    } else if (message.startsWith("REGISTER ")) {
                        handleRegister(message);
                    } else if (message.startsWith("CHAT ")) {
                        handleChat(message);
                    } else if (message.equals("LOGOUT")) {
                        System.out.println("Client logging out: " + (username != null ? username : "unknown") + 
                                         " (" + clientIP + ")");
                        break;
                    }
                }
            } catch (IOException e) {
                logConnection("Connection error for client " + clientIP + ": " + e.getMessage());
            } finally {
                clients.remove(this);
                try {
                    if (socket != null && !socket.isClosed()) {
                        socket.close();
                        logConnection("Client disconnected: " + clientIP);
                    }
                } catch (IOException e) {
                    logConnection("Error closing socket for client " + clientIP + ": " + e.getMessage());
                }
                if (username != null) {
                    String disconnectMessage = username + " has disconnected";
                    System.out.println("Broadcasting disconnect message for user: " + username);
                    for (ClientHandler client : clients) {
                        client.out.println("MESSAGE " + disconnectMessage);
                    }
                    System.out.println(disconnectMessage);
                }
                System.out.println("Current active connections: " + clients.size());
            }
        }

        private void handleLogin(String message) {
            String[] parts = message.split(" ");
            String username = parts[1];
            String password = parts[2];
            
            logConnection("Login attempt from " + clientIP + " with username: " + username);
            
            if (users.containsKey(username) && users.get(username).equals(password)) {
                this.username = username;
                out.println("LOGIN_SUCCESS");
                System.out.println("Login successful for user: " + username);
                String loginMessage = username + " has joined the chat";
                for (ClientHandler client : clients) {
                    client.out.println("MESSAGE " + loginMessage);
                }
            } else {
                out.println("LOGIN_FAILED");
                System.out.println("Login failed for user: " + username);
            }
        }

        private void handleRegister(String message) {
            String[] parts = message.split(" ");
            String username = parts[1];
            String password = parts[2];
            
            logConnection("Registration attempt from " + clientIP + " with username: " + username);
            
            if (users.containsKey(username)) {
                out.println("REGISTER_FAILED");
                System.out.println("Registration failed - username already exists: " + username);
            } else {
                users.put(username, password);
                updateUserTable();
                out.println("REGISTER_SUCCESS");
                System.out.println("Registration successful for user: " + username);
            }
        }

        private void handleChat(String message) {
            if (username != null) {
                String chatMessage = username + ": " + message.substring(5);
                chatHistory.add(chatMessage);
                historyArea.append(chatMessage + "\n");
                System.out.println("Broadcasting message from " + username + ": " + message.substring(5));
                for (ClientHandler client : clients) {
                    client.out.println("MESSAGE " + chatMessage);
                }
            } else {
                System.out.println("Attempted to send message without login from IP: " + clientIP);
            }
        }
    }
} 