package sys.io;

import haxe.io.Bytes;

using StringTools;

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

@:enum abstract BaudRate(Int) to Int {
	var B9600 = 13; //0x0D;
	var B115200 = 4098;
}

class Serial {

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

	public static function open( path : String, baudRate : BaudRate ) : Serial {
		var fd = serial_open_port( @:privateAccess path.bytes, baudRate );
		if( fd <= 0 ) throw fd;
		return new Serial( path, fd );
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

	/*
	public static function list() {
		var p = new sys.io.Process("ls",["-CF","/sys/class/tty"]);
		var code = p.exitCode( true );
		trace(code);
		switch code {
			case 0:
				var str = p.stdout.readAll().toString();
				var ttys = ~/[, ]/g.split(str).map( s -> return s.trim() );
				for( t in ttys ) trace(t);
				default:
					trace( p.stderr.readAll().toString() );
				}
				p.close();
				
			}
	*/
}
