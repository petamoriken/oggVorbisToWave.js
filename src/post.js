var dataview = new DataView(buffer), isLittleEndian = (new Uint8Array((new Uint16Array([1])).buffer))[0];
var ENVIRONMENT_IS_REQUIRE = (typeof module !== "undefined" && module["exports"]);

var asap =	typeof setImmediate === "function" && setImmediate ||
			typeof Promise === "function" && function(fn) { new Promise(function(resolve) { fn(); resolve() }) } ||
			function(fn) { setTimeout(fn, 0) };

function oggVorbisToWave(oggBuffer) {

	var oggpc, oggsize, wavpc, wavsize, ret;

	// set Ogg Buffer to memory
	oggsize = oggBuffer.byteLength;
	oggpc = _malloc(oggsize);

	//writeArrayToMemory(new Uint8Array(oggBuffer), oggpc);
	HEAPU8.set(new Uint8Array(oggBuffer), oggpc);

	// Ogg Buffer -> Wave Buffer
	wavpc = _sp_ov_to_wave(oggpc, oggsize, isLittleEndian);

	wavsize = ( (isLittleEndian && wavpc % 4 === 0) ? HEAPU32[wavpc / 4 + 1] : dataview.getUint32(wavpc + 4, true) ) + 8;
	ret = buffer.slice(wavpc, wavpc + wavsize);

	asap(function() {
		_free(oggpc);
		_free(wavpc);
	});

	return ret;

}

// export
if(ENVIRONMENT_IS_REQUIRE) {
	module["exports"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WEB) {
	self["oggVorbisToWave"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WORKER) {
	self["onmessage"] = function(e) {
		postMessage(oggVorbisToWave(e.data));
	}
}

})();