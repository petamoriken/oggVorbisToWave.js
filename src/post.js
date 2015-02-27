// libogg and libvorbis function wrappers
var Vorbis = {

	sp_ov_start:	Module["cwrap"]('sp_ov_start', 'number', []),
	sp_ov_end:		Module["cwrap"]('sp_ov_end', 'void', ['number']),
	sp_ov_to_wave:	Module["cwrap"]('sp_ov_to_wave', 'number', ['number'])

};

var buffer = Module["buffer"], heapu32 = Module["HEAPU32"], isLittleEndian = !!(new Uint8Array((new Uint16Array([0x00ff])).buffer))[0];

// delete emscripten window.Module
delete window["Module"];

window["oggVorbisToWave"] = function(oggBuffer) {

	var ovFile, size, wavpc, ret;

	FS.createDataFile("/", "data.ogg", new Uint8Array(oggBuffer), 1, undef);

	ovFile = Vorbis.sp_ov_start();
	wavpc = Vorbis.sp_ov_to_wave(ovFile);

	FS.unlink("data.ogg");

	size = (isLittleEndian && wavpc % 4 === 0) ? heapu32[wavpc / 4 + 1] + 8 : (new DataView(buffer)).getUint32(wavpc + 4, true) + 8;
	ret = buffer.slice(wavpc, wavpc + size);

	Vorbis.sp_ov_end(ovFile);

	return ret;
};

})(this);


