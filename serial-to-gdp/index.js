var os = require('os');
var gdpjs = require('@terraswarm/gdp');
var SerialPort = require('serialport');

var gdpd_addr = "gdp-01.eecs.berkeley.edu"; // need to specify this, as it seems that the GDP node module doesn't read ~/.ep_adm_params/gdp
var logdxname = os.hostname();
var gcl_append = true;
var recsrc = 0;
var recarray = [ "355\t357\t464", "358\t354\t462" ];
var recarray_out = [];

var port = new SerialPort('/dev/tty.usbmodem1411131', {
  parser: SerialPort.parsers.readline('\n')
});

port.on('data', function (data) {
  console.log('Data: ' + data);
  gdpjs.write_gcl_records(gdpd_addr, "edu.berkeley.eecs.bid.mellis.arduino101", logdxname, gcl_append, recsrc, [ data ], true, recarray_out);
});

