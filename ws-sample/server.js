#!/usr/bin/env node

var express = require("express");
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

var port = "8895";

var help="server [-h] [-v] [port]\n" +
    "\t-h help\n" +
    "\t-v verbose\n" +
    "run server on port, by default 8895";

parsedArgs = cliParser(process.argv.slice(2),
                       {
			 boolean: ["h", "v"],
			 stopEarly: true,
                         unknown: optUnknown });

if (parsedArgs.h || parsedArgs.help) {
    console.log(help);
    process.exit(0);
}

if (parsedArgs._.length == 1) {
    var port = parsedArgs._[0];
    if (isNan(port)) {
	console.log("Port should be a number");
	console.log(help);
	process.exit(0);
    }
}

app = express();

if (parsedArgs.v) {
    function report(req, resp, next) {
	console.log(req.url);
	next();
    }

    app.use(report);
}

app.use(express.static('public'));




var expressWs = require('express-ws')(app);

app.ws('/echo', function(ws, req) {
    ws.on('message', function(msg) {
        if (parsedArgs.v)
            console.log("Received ", msg);
        ws.send(msg);
    });
});

app.post("/htmlForm", function(request, response) {
    response.set( {'Content-type': 'text/html; charset=utf-8' })
    response.write("<!DOCTYPE html>\n")
    response.write("<html><head><title>HTML form</title></head><body><h1>HTML form</h1></body></html>\n");
    response.end();
});



console.log("Running on ", port);
server = app.listen(port);
