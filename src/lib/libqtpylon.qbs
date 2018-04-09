import qbs

StaticLibrary {

    name: "libqtpylon"

    Depends { name: "cpp" }
    Depends { name: "pylon" }
    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "multimedia"
        ]
    }

    cpp.cxxLanguageVersion: "c++11"
    cpp.includePaths: [ product.sourceDirectory ]

    files: ["pyloncamera.cpp", "pyloncamera.h"]

    Export {
        Depends { name: "cpp" }
        Depends { name: "pylon" }
        cpp.includePaths: [ product.sourceDirectory ]
    }
}
