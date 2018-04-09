import qbs

Project {
    name: "qtpylon"
    property bool withDemo: true

    qbsSearchPaths: [ "qbs" ]
    minimumQbsVersion: "1.6"

    references: [
        "src/lib/libqtpylon.qbs",
        "src/demo/pylondemo.qbs"
    ]
}
