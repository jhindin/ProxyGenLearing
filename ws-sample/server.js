#!/usr/bin/env node

var express = require("express");

app = express();
app.use(express.static('public'));

var port = "8895";

if (process.argv.length > 2)
    port = process.argv[2];

console.log(port);
server = app.listen(port);
