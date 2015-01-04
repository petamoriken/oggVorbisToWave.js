// libogg and libvorbis function wrappers

var Vorbis = {

	sp_ov_start:	Module["cwrap"]('sp_ov_start', 'number', []),
	sp_ov_end:		Module["cwrap"]('sp_ov_end', 'void', ['number']),
	sp_ov_to_wave:	Module["cwrap"]('sp_ov_to_wave', 'number', ['number'])

};

var heap8 = Module["HEAP8"], heapu32 = Module["HEAPU32"], isLittleEndian = !!(new Uint8Array((new Uint16Array([0x00ff])).buffer))[0];

window["oggVorbisToWave"] = function(oggBuffer) {

	var ovFile, size, wavpc, ret;

	FS.createDataFile("/", "data.ogg", new Uint8Array(oggBuffer), 1, undef);

	ovFile = Vorbis.sp_ov_start();
	wavpc = Vorbis.sp_ov_to_wave(ovFile);

	FS.unlink("data.ogg");

	if(isLittleEndian)
		size = heapu32[wavpc / 4 + 1] + 8;
	else 
		size = (new DataView(Module["buffer"])).getUint32(wavpc + 4, true) + 8;

	ret = new Uint8Array(size);
	for(var i=0; i<size; ++i) {
		ret[i] = heap8[wavpc + i];
	}

	Vorbis.sp_ov_end(ovFile);

	return ret.buffer;
};

})(this);


