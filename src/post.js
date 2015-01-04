// libogg and libvorbis function wrappers

var Vorbis = {

	sp_ov_start:	Module["cwrap"]('sp_ov_start', 'number', []),
	sp_ov_end:		Module["cwrap"]('sp_ov_end', 'void', ['number']),
	sp_ov_to_wave:	Module["cwrap"]('sp_ov_to_wave', 'number', ['number'])

};

window["oggVorbisToWave"] = function(oggBuffer) {

	var ovFile, size, wavpc, ret, heap8, heapu32;

	heap8 = Module["HEAP8"];
	heapu32 = Module["HEAPU32"];

	FS.createDataFile("/", "data.ogg", new Uint8Array(oggBuffer), 1, void 0);

	ovFile = Vorbis.sp_ov_start();
	wavpc = Vorbis.sp_ov_to_wave(ovFile);

	FS.unlink("data.ogg");

	size = heapu32[wavpc / 4 + 1] + 8;
	
	ret = new Uint8Array(size);
	for(var i=0; i<size; ++i) {
		ret[i] = heap8[wavpc + i];
	}

	Vorbis.sp_ov_end(ovFile);

	return ret.buffer;
};

})();


