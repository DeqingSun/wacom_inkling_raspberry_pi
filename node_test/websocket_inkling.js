var WebSocketServer = require('ws').Server;
var wss = new WebSocketServer({port: 8080});

var socketList = []

wss.on('connection', function(ws) {
    
    console.log('New conn %s', ws.upgradeReq.connection.remoteAddress);
    if (socketList.indexOf(ws)==-1) socketList.push(ws);
    console.log('We have %d connections', socketList.length);
    
    ws.send('something');
    
    ws.on('message', function(data, flags) {
        console.log('received: %s', data);
        if (data=="broadcast") {
            console.log('Do broadcast');
            for (var singleSocket of socketList){
                singleSocket.send('broadcast to all client');
            }
        }
    });
    
    /*ws.on('open', function() {
        console.log('Socket opened');
    });*/
    
    ws.on('close', function(code, message)  {
        console.log('Socket closed');
        var index = socketList.indexOf(ws);
        if (index > -1) {
            socketList.splice(index, 1);
        }
        console.log('We have %d connections', socketList.length);
    });
    

});

var spawn = require('child_process').spawn,
    ls    = spawn('./count.sh', []);

ls.stdout.on('data', function (data) {
    //console.log('stdout: ' + data);
    //console.log(socketList);
    for (var singleSocket of socketList){
        //console.log('send to %s', singleSocket.upgradeReq.connection.remoteAddress);
        singleSocket.send(String(data));
    }
});

ls.stderr.on('data', function (data) {
  console.log('stderr: ' + data);
});

ls.on('close', function (code) {
  console.log('child process exited with code ' + code);
});



