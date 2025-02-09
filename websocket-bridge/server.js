const dgram = require("dgram");
const WebSocket = require("ws");
const express = require("express");
const cors = require("cors");

const UDP_SERVER_PORT = 8080; // Same as your C++ server
const UDP_CLIENT_PORT = 9090;
const UDP_SERVER_IP = "127.0.0.1";

const udpClient = dgram.createSocket("udp4");
const udpServer = dgram.createSocket("udp4");

// Start WebSocket Server
const wss = new WebSocket.Server({ port: 5000 });

// Enable CORS for HTTP routes (optional)
const app = express();
app.use(cors());
app.use(express.json());

app.listen(4000, () => {
    console.log("HTTP Server running on port 4000");
});

// Handle WebSocket Connections
wss.on("connection", (ws) => {
    console.log("WebSocket client connected.");

    ws.on("message", (message) => {
        const data = JSON.parse(message);

        if (data.type === "message") {
            console.log("Sending message:", data.text);
            udpClient.send(data.text, 0, data.text.length, UDP_SERVER_PORT, UDP_SERVER_IP);
        }
    });

    // Listen for UDP responses
    udpServer.on("message", (msg, rinfo) => {
        console.log(`Received: ${msg.toString()}`);
        ws.send(JSON.stringify({ type: "response", text: msg.toString() }));
    });
});

console.log("WebSocket server running on port 5000");
