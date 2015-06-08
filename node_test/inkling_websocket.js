var WebSocketServer = require('ws').Server;
var wss = new WebSocketServer({port: 8080});

var localSocketList=[];

function startSocketServer(socketList) {
    localSocketList=socketList;
    
    wss.on('connection', function(ws) {
        
        console.log('New conn %s', ws.upgradeReq.connection.remoteAddress);
        if (localSocketList.indexOf(ws)==-1) localSocketList.push(ws);
        console.log('We have %d connections', localSocketList.length);
        
        //ws.send('something');
        
        ws.on('message', function(data, flags) {
            console.log('received: %s', data);
            if (data=="broadcast") {
                console.log('Do broadcast');
                for (var singleSocket of localSocketList){
                    singleSocket.send('broadcast to all client');
                }
            }
        });
        
        /*ws.on('open', function() {
            console.log('Socket opened');
        });*/
        
        ws.on('close', function(code, message)  {
            console.log('Socket closed');
            var index = localSocketList.indexOf(ws);
            if (index > -1) {
                localSocketList.splice(index, 1);
            }
            console.log('We have %d connections', localSocketList.length);
        });
        
    
    });
}
exports.startSocketServer = startSocketServer;
 
