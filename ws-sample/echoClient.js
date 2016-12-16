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

var help="<url>\n" +
    "\topen websocket on given URL, by default http://localhost:80/echo";

var url = "http://localhost:8895/echo";

parsedArgs = cliParser(process.argv.slice(2),
                       { boolean: ["h"], stopEarly: true,
                         unknown: optUnknown });


if (parsedArgs.h || parsedArgs.help) {
    console.log(help);
    process.exit(0);
}

if (parsedArgs._.length > 1) {
    console.log("Extra args");
    console.log(help);
    process.exit(0);
}

if (process.argv.length == 1)
    url = parsedArgs._[0];

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

