Module["preRun"] = Module["preRun"] || [];

const filesystemSetup = () => {
	// Have to add run dependency because syncfs is asynchronous
	addRunDependency("syncfs");

	FS.mkdir("/ecwolf");
	FS.mount(IDBFS, {}, "/ecwolf");

	FS.syncfs(true, (err) => {
		if (!err) console.log("Mounted settings directory");
		else console.error("Could not create settings directory: ", err);

		removeRunDependency("syncfs");
	});
};

Module["preRun"].push(filesystemSetup);
