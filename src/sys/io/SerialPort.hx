package sys.io;

import haxe.io.Bytes;

using StringTools;

//TODO
/*
@:enum abstract BaudRate(Int) to Int {
	var _115200 = 115200;
	var _57600 = 57600;
	var _38400 = 38400;
	var _19200 = 19200;
	var _9600 = 9600;
	var _4800 = 4800;
	var _2400 = 2400;
	var _1800 = 1800;
	var _1200 = 1200;
	var _600 = 600;
	var _300 = 300;
	var _200 = 200;
	var _150 = 150;
	var _134 = 134;
	var _110 = 110;
	var _75 = 75;
	var _50 = 50;
}
*/

typedef SerialPortInfo = {
	var path: String;
	var ?manufacturer: String;
	var ?serialNumber: String;
	var ?pnpId: String;
	var ?locationId: String;
	var ?vendorId: String;
	var ?productId: String;
}

@:enum abstract BaudRate(Int) to Int {
	var B9600 = 13; //0x0D;
	var B115200 = 4098;
}

class SerialPort {

	public var path(default,null) : String;

	var fd : Null<Int>;

	function new( path : String, fd : Int ) {
		this.path = path;
		this.fd = fd;
	}

	public function close() {
		serial_close_port( fd );
		/*
		var r = serial_close_port( fd );
		trace(r);
		if( r == -1 ) {
			fd = null;
			throw 'failed to close serialport $fd';
		}
		*/
		fd = null;
	}

	public function readBytes( bytes : Bytes, size : Int ) : Int {
		var r = serial_read( fd, bytes.getData(), size );
		trace(r);
		return r;
	}

	public function write( bytes : Bytes, pos = 0, ? len : Int ) {
		if( len == null ) len = bytes.length;
		var c = serial_write( fd, bytes.getData(), pos, len );
		trace(c);
		//if( c != 0 ) throw 'failed to write serial $fd';
	}
	
	public function flush() {
		if( !serial_flush( fd ) ) {
			throw haxe.io.Error.Custom("flush() failure");
		}
	}

	/**
		Waits until all output data has been transmitted to the serial port.
	**/
	public function drain() {
		if( !serial_drain( fd ) ) {
			throw haxe.io.Error.Custom("drain() failure");
		}
	}

	public static function open( path : String, baudRate : BaudRate ) : SerialPort {
		var fd = serial_open_port( @:privateAccess path.bytes, baudRate );
		if( fd <= 0 ) throw fd;
		return new SerialPort( path, fd );
	}

	@:hlNative("serialport","open_port")
	static function serial_open_port( path : hl.Bytes, baudRate : Int ) : Int { return 0; }

	@:hlNative("serialport","close_port")
	static function serial_close_port( fd : Int ) : Void {} // { return 0; }
	
	@:hlNative("serialport","set_baudrate")
	static function serial_set_baudrate( fd : Int, braudRRate : BaudRate ) : Int { return 0; }

	@:hlNative("serialport","read")
	static function serial_read( fd : Int, buf : hl.Bytes, size : Int ) : Int { return 0; }

	@:hlNative("serialport","read_char")
	static function serial_read_char( fd : Int ) : Int { return 0; }

	@:hlNative("serialport","write")
	static function serial_write( fd : Int, bytes : hl.Bytes, pos : Int, len : Int ) : Int { return 0; }

	@:hlNative("serialport","flush")
	static function serial_flush( fd : Int ) : Bool return false;

	@:hlNative("serialport","drain")
	static function serial_drain( fd : Int ) : Bool return false;

	public static function list() : Array<SerialPortInfo> {
		var ports = new Array<SerialPortInfo>();
		var p = new sys.io.Process( "udevadm", ["info","-e"] );
		var port : SerialPortInfo = cast {};
		var skip = false;
		while( true ) {
			var line = try p.stdout.readLine() catch(e:haxe.io.Eof) {
				break;
			}
			var type = line.substr(0,1);
			var data = line.substr(3);
			if( type == "P" ) {
				port = {
					manufacturer: null,
					serialNumber: null,
					pnpId: null,
					locationId: null,
					vendorId: null,
					productId: null,
					path: null
				}
				skip = false;
				continue;
			}
			if( skip )
				continue;
			if( type == "N" ) {
				if( ~/(tty(S|WCH|ACM|USB|AMA|MFD|O|XRUSB)|rfcomm)/.match( line ) ) {
					ports.push( port );
				} else {
					skip = true;
				}
			}
			if( type == "E" ) {
				var e = ~/^(.+)=(.*)/;
				if( !e.match( data ) )
					continue;
				var k = e.matched(1);
				var v = e.matched(2);
				var prop = switch k.toUpperCase() {
				case 'DEVNAME': 'path';
				case 'ID_VENDOR_ENC': 'manufacturer';
				case 'ID_SERIAL_SHORT': 'serialNumber';
				case 'ID_VENDOR_ID': 'vendorId';
				case 'ID_MODEL_ID': 'productId';
				case 'DEVLINKS': 'pnpId';
				case _: continue;
				}
				if( prop == null )
					continue;
				Reflect.setField( port, prop, v );
			}
		}
		var code = p.exitCode( true );
		p.close();
		if( code != 0 )
			throw 'failed to list serial devices';
		return ports;
		
	}
	
}
