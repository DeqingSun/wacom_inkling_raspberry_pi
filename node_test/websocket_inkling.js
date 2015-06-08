

var socketList = []


var websocketProcess = require('./inkling_websocket');
websocketProcess.startSocketServer(socketList);
var inklingProcess = require('./inkling_process');
inklingProcess.startProcess(socketList);

