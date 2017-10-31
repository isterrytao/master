module.exports = {
    process: function(cfgData, info) {
        var cfg = JSON.parse(JSON.stringify(cfgData));
        var child_process = require("child_process");

        var srcCommitID;
        try {
            srcCommitID = child_process.execSync('git rev-parse HEAD', {cwd: __dirname}).toString().trim();
        } catch(err) {
            srcCommitID = "0000000000000000000000000000000000000000";
        }
        try {
            child_process.execSync('git diff --quiet', {cwd: __dirname});
            child_process.execSync('git diff --cached --quiet', {cwd: __dirname});
        } catch(err) {
            srcCommitID = srcCommitID.slice(0, 20);
        }

        var fwAddrSpace = ""

        try {
            fwAddrSpace = process.env.COPY;
        } catch (err) {
        }

        if (fwAddrSpace == "B") {
            fwAddrSpace = "Boot_FWCopyBAddrSpace"
        } else {
            fwAddrSpace = "Boot_FWCopyAAddrSpace"
        }

        compatibleHWList = ["P2", "P3"];

        const compaiableMap = {
            "P1": 0x01,
            "P2": 0x02,
            "P3": 0x04,
        };

        const isValidOid = (oid) => {
            var ymd = oid.match(/^U[DO](\d{4})(\d{2})(\d{2})\d{3}$/)
            if (!ymd) return false;
            var isoString = ymd.slice(1).join('-');
            var d;
            if (!((d = new Date(isoString)) | 0)) return false;
            return d.toISOString().slice(0, 10) == isoString;
        }

        // const splitMasterVersion = (ver) => {
        //     var v = ver.match(/(\d+)\.(\d+)\.(\d+)\.(\d)+/)
        //     if (!v) null
        //     return v.slice(1);
        // }

        if (cfg.releaseType != "T" && cfg.releaseType != "R") throw "AppInfo.releaseType has wrong format";

        var releaseName;
        try {
            releaseName = info.SystemConnection.BcuType;
        } catch(e) {
            releaseName = cfg.releaseName;
        }

        if (!releaseName) {
            throw "What's the releaseName?!"
        }

        var appVersion = "";

        if (cfg.pid != "" || cfg.requireVer != "") {
            if (!/^C\d{3}\.\d{3}\.\d{3}\.\d{2}$/.test(cfg.pid)) throw "AppInfo.pid has wrong format";
            if (!/^\d+\.\d+$/.test(cfg.requireVer)) throw "AppInfo.requireVer has wrong format";

            releaseName += "_" + cfg.pid;
            //releaseName += "_" + cfg.oid;
            releaseName += "_" + cfg.releaseType;
            releaseName += cfg.requireVer + "." + cfg.buildVer;
            appVersion = "{" + (cfg.requireVer + "." + cfg.buildVer).split(".").map(x=>x+'U').join() + "}";
        } else {
            releaseName += "_" + cfg.releaseType;
            releaseName += info.LibInfo.masterVersion;
        }

        var compatibleHW = compatibleHWList.reduce((sum, x)=>sum | compaiableMap[x], 0x00);
        //var masterVersion = cfg.masterVersion;
        //var splitedMasterVersion = splitMasterVersion(masterVersion);
        //if (!splitedMasterVersion) throw "AppInfo.masterVersion  has wrong format"

        var buildTime = new Date();
        var buildTimeStr =
            buildTime.getFullYear().toString()
            + ("0" + (buildTime.getMonth() + 1).toString()).slice(-2)
            + ("0" + buildTime.getDate().toString()).slice(-2)
            + ("0" + buildTime.getHours().toString()).slice(-2)
            + ("0" + buildTime.getMinutes().toString()).slice(-2)
            + ("0" + buildTime.getSeconds().toString()).slice(-2);


        var releaseFileName = releaseName + "_" + buildTimeStr;
        releaseFileName = releaseFileName.slice(0, releaseFileName.length - 2)

        var mdata = {
            isTestVersion: cfg.releaseType == 'T' ? 1 : 0,
            fwAddrSpace: fwAddrSpace,
            buildTime: buildTime,
            buildTimeStr: buildTimeStr,
            compatibleHW: compatibleHW,
            releaseName: releaseName,
            srcCommitID: srcCommitID,
            // masterVersion: masterVersion,
            // masterMajor: splitedMasterVersion[0],
            // masterMinor: splitedMasterVersion[1],
            // masterRevision: splitedMasterVersion[2],
            // masterBuild: splitedMasterVersion[3],
            releaseFileName: releaseFileName,
            appVersion: appVersion,
        }

        info.render("AppInfo.h", "AppInfo.h.tmpl", mdata);
        info.render("AppInfo.c", "AppInfo.c.tmpl", mdata);

        return {
            compatibleHW: compatibleHW,
            releaseName: releaseName,
            releaseFileName: releaseFileName,
        }
    }
}
