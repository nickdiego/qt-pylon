import qbs
import qbs.FileInfo

QtGuiApplication {

    name: "pylondemo"
    condition: project.withDemo

    Depends { name: "libqtpylon" }
    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "multimedia",
            "quickcontrols2"
        ]
    }

    cpp.includePaths: [ "../.." ]
    files: ["*.cpp", "*.h", "*.qrc"]
}
