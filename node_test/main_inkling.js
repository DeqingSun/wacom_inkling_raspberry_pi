

var socketList = []

var express = require('express');
var app = express();
var path = require("path");

app.get('/', function (req, res) {
    res.sendFile(path.join(__dirname + '/index.html'));
});

app.use('/static', express.static('static'));

var server = app.listen(8080, function () {

  var host = server.address().address;
  var port = server.address().port;

  console.log('Example app listening at http://%s:%s', host, port);

});

var websocketProcess = require('./inkling_websocket');
websocketProcess.startSocketServer(socketList);
var inklingProcess = require('./inkling_process');
inklingProcess.startProcess(socketList);

