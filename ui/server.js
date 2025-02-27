// server.js

// dependencies
const express = require('express')
const ws = require('express-ws')
const path = require('path')
const bodyParser = require('body-parser')

// create new HTTP server using express
const server = express()

// install WebSocket plugin
ws(server)

server.use(bodyParser.urlencoded({extended: false}));

// serve public files
server.use(express.static(path.join(__dirname, './public')));

// create WebSocket endpoint
server.ws('/data', function(ws, req) {

    ws.on("message", function(deviceID, payload) {
        const parsed = JSON.parse(payload)
        console.log("Received websocket message", parsed);
    })
});

// server start
server.listen(8080, function() {
    console.log("Server listening on port 8080");
})