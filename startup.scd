// foward OSC from TidalCycles
var addr = NetAddr.new("127.0.0.1", 3333);
OSCFunc({ |msg, time, tidalAddr|
	var latency = time - Main.elapsedTime;
    if (msg.indexOf('orbit').isNil) {
        msg = msg ++ ["orbit", 0];
    };
    msg = msg ++ ["time", time, "latency", latency, "zz"];
	msg.postln;
	addr.sendBundle(latency, msg)
}, '/play2').fix;
