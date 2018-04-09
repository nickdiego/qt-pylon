/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qbs.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms and
** conditions see http://www.qt.io/terms-conditions. For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************
** Slightly modified version of Qbs's PkgConfigProbe. Needed because
** pylon-config option for getting 'version' is not compatible with
** pkg-config's one.
**
** author: Nick Yamane <nick@diegoyam.com>
****************************************************************************/

import qbs 1.0
import qbs.Process
import qbs.FileInfo

Probe {
    // Inputs
    property string executable: 'pylon-config'
    property string name
    property stringList packageNames: [name]
    property string minVersion
    property string exactVersion
    property string maxVersion
    property string pathListSeparator: qbs.pathListSeparator

    // Output
    property stringList cflags // Unmodified --cflags output
    property stringList libs   // Unmodified --libs output

    property stringList defines
    property stringList libraries
    property stringList libraryPaths
    property stringList includePaths
    property stringList compilerFlags
    property stringList linkerFlags
    property string version

    configure: {
        if (!packageNames || packageNames.length === 0)
            throw 'PylonConfigProbe.packageNames must be specified.';
        var p = new Process();
        try {
            var args = packageNames;
            if (minVersion !== undefined)
                args.unshift("--atleast-version=" + minVersion);
            if (exactVersion !== undefined)
                args.unshift("--exact-version=" + exactVersion);
            if (maxVersion !== undefined)
                args.unshift("--max-version=" + maxVersion);
            if (p.exec(executable, args.concat([ '--cflags' ])) === 0) {
                cflags = p.readStdOut().trim();
                cflags = cflags ? cflags.split(/\s/) : [];
                if (p.exec(executable, args.concat(["--libs"])) === 0) {
                    libs = p.readStdOut().trim();
                    libs = libs ? libs.split(/\s/) : [];
                    if (p.exec(executable, [packageNames[0]].concat([ '--version' ])) === 0) {
                        version = p.readStdOut().trim();
                        found = true;
                        includePaths = [];
                        defines = []
                        compilerFlags = [];
                        for (var i = 0; i < cflags.length; ++i) {
                            var flag = cflags[i];
                            if (flag.startsWith("-I"))
                                includePaths.push(flag.slice(2));
                            else if (flag.startsWith("-D"))
                                defines.push(flag.slice(2));
                            else
                                compilerFlags.push(flag);
                        }
                        libraries = [];
                        libraryPaths = [];
                        linkerFlags = [];
                        for (i = 0; i < libs.length; ++i) {
                            flag = libs[i];
                            if (flag.startsWith("-l"))
                                libraries.push(flag.slice(2));
                            else if (flag.startsWith("-L"))
                                libraryPaths.push(flag.slice(2));
                            else
                                linkerFlags.push(flag);
                        }
                        console.debug("PylonConfigProbe: found packages " + packageNames);
                        return;
                    }
                }
            }
            found = false;
            cflags = undefined;
            libs = undefined;
        } finally {
            p.close();
        }
    }
}
