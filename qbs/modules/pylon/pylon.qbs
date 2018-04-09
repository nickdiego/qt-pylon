import qbs
import qbs.Environment
import qbs.FileInfo
import qbs.PathTools
import qbs.ModUtils
import pylon

Module {
    property bool found: probe.found
    property string pylonConfigPath: FileInfo.joinPaths(pylonRootDir, "bin", "pylon-config")
    property string pylonRootDir: {
        var envVar = Environment.getEnv("PYLON_ROOT");
        if (envVar == undefined) {
            console.warn("PYLON_ROOT environment variable not set. Defaulting to '/opt/pylon5'!");
            return "/opt/pylon5";
        }
        return envVar;
    }

    Depends { name: "cpp" }

    property stringList libraryPaths: probe.libraryPaths

    cpp.includePaths: base.concat(probe.includePaths == undefined ? [] : probe.includePaths)
    cpp.libraryPaths: base.concat(probe.libraryPaths == undefined ? [] : probe.libraryPaths)
    cpp.dynamicLibraries: base.concat(probe.libraries == undefined ? [] : probe.libraries)

    pylon.PylonConfigProbe {
        id: probe
        name: "pylon"
        executable: pylonConfigPath
    }

    setupRunEnvironment: {
        if (product.pylon.found) {
            var env = Environment.currentEnv();
            if (product.qbs.hostOS.contains("unix") && product.qbs.targetOS.contains("unix")) {
                env["LD_LIBRARY_PATH"] = PathTools.prependOrSetPath(product.pylon.libraryPaths.join(product.qbs.pathListSeparator),
                                                                    env["LD_LIBRARY_PATH"], product.qbs.pathListSeparator);
            }
            for (var i in env) {
                var v = new ModUtils.EnvironmentVariable(i, qbs.pathListSeparator,
                product.qbs.hostOS.contains("windows"));
                v.value = env[i];
                v.set();
            }
        }
    }
}

