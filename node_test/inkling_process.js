var localSocketList=[];

function startProcess(socketList) {
    localSocketList=socketList;
    
    console.log('Kill old read_inkling');
    var exec = require('child_process').execFileSync;
    try {
        exec('pkill', ['read_inkling']);
    } catch(err) {
        console.log('No old read_inkling to kill');        
    }

    console.log('Start new read_inkling');
    var spawn = require('child_process').spawn,
        inklingProcess    = spawn('./read_inkling', []);
    var inklingBuffer = "";
    inklingProcess.stdout.on('data', function (data) {
        var i = 0, piece = '', offset = 0;
        inklingBuffer += String(data);
        while ( (i = inklingBuffer.indexOf('\n', offset)) !== -1) {
            piece = inklingBuffer.substr(offset, i - offset);
            offset = i + 1;
            if ((piece.charAt(0)=='d' || piece.charAt(0)=='u') && piece.charAt(1)==':' ) {
                for (var singleSocket of localSocketList){
                    singleSocket.send(piece);    
                }
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
