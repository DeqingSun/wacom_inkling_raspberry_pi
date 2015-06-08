var localSocketList=[];

function startProcess(socketList) {
    localSocketList=socketList;
    var spawn = require('child_process').spawn,
        inklingProcess    = spawn('./read_inkling', []);
    var inklingBuffer = "";
    inklingProcess.stdout.on('data', function (data) {
        var i = 0, piece = '', offset = 0;
        inklingBuffer += String(data);
        while ( (i = inklingBuffer.indexOf('\n', offset)) !== -1) {
            piece = inklingBuffer.substr(offset, i - offset);
            offset = i + 1;
            for (var singleSocket of localSocketList){
                singleSocket.send(piece);    
            }
        }
        inklingBuffer = inklingBuffer.substr(offset);    
    });
    
    inklingProcess.stderr.on('data', function (data) {
      console.log('stderr: ' + data);
    });
    
    inklingProcess.on('close', function (code) {
      console.log('child process exited with code ' + code);
    }); 
}
exports.startProcess = startProcess;
