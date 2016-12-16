#!/usr/bin/env node

var WebSocket = require('ws');
var cliParser = require('minimist');

var url = "http://localhost:8895/echo";
if (process.argv.length > 2)
    url = process.argv[2];

var ws = new WebSocket(url);

ws.on('open', function open() {
    ws.send('something');
});

ws.on('message', function(data, flags) {
    console.log("message arrived ", flags + ", data = ", data);
  // flags.binary will be set if a binary data is received.
  // flags.masked will be set if the data was masked.
  ws.close();
});

console.log("Exiting");
