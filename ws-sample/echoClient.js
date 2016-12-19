#!/usr/bin/env node

var WebSocket = require('ws');
var cliParser = require('minimist');

function optUnknown(a)
{
    c = a.charAt(0);
    if (c == '-') {
	console.log("Unknown options", a)
	return false;
    }
    return true;
}

var help="echoClient [-m <message>] [<url>]\n" +
    "\t-m <message> message to send, by default 'test message'\n" +
    "open websocket on given URL, by default http://localhost/echo";

var url = "http://localhost/echo";
var message = "test message";

parsedArgs = cliParser(process.argv.slice(2),
                       { string: ["m"],
			 boolean: ["h"],
			 stopEarly: true,
                         unknown: optUnknown });


if (parsedArgs.h || parsedArgs.help) {
    console.log(help);
    process.exit(0);
}

if (parsedArgs.m)
    message = parsedArgs.m;

if (parsedArgs._.length > 1) {
    console.log("Extra args");
    console.log(help);
    process.exit(0);
}

if (parsedArgs._.length == 1) {
    url = parsedArgs._[0];
}

var ws = new WebSocket(url);

ws.on('message', function(data, flags) {
    console.log("message arrived ", flags + ", data = ", data);
  // flags.binary will be set if a binary data is received.
  // flags.masked will be set if the data was masked.

    ws.close();
});

ws.on('open', function open() {
    console.log("Sending ", message);
    ws.send(message);
});
