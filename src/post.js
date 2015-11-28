// libogg and libvorbis function wrappers
var Vorbis = {

	sp_ov_start:	Module["cwrap"]('sp_ov_start', 'number', []),
	sp_ov_end:		Module["cwrap"]('sp_ov_end', 'void', ['number']),
	sp_ov_to_wave:	Module["cwrap"]('sp_ov_to_wave', 'number', ['number', "number"])

};

var buffer = Module["buffer"], heapu32 = Module["HEAPU32"], dataview = new DataView(buffer), isLittleEndian = (new Uint8Array((new Uint16Array([1])).buffer))[0];
var ENVIRONMENT_IS_REQUIRE = (typeof module !== "undefined" && module["exports"]);

function oggVorbisToWave(oggBuffer) {

	var ovFile, size, wavpc, ret;

	FS.createDataFile("/", "data.ogg", new Uint8Array(oggBuffer), 1);

	ovFile = Vorbis.sp_ov_start();
	wavpc = Vorbis.sp_ov_to_wave(ovFile, isLittleEndian);

	FS.unlink("data.ogg");

	size = ( (isLittleEndian && wavpc % 4 === 0) ? heapu32[wavpc / 4 + 1] : dataview.getUint32(wavpc + 4, true) ) + 8;
	ret = buffer.slice(wavpc, wavpc + size);

	Vorbis.sp_ov_end(ovFile);

	return ret;
	
}

// export
if(ENVIRONMENT_IS_REQUIRE) {
	module["exports"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WEB) {
	window["oggVorbisToWave"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WORKER) {
	onmessage = function(e) {
		postMessage(oggVorbisToWave(e.data));
	}
}

})();