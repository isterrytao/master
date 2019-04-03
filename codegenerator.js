module.exports = {

    /*
    ********************************
    modsFile
    ********************************
    module.exports = [
        {
            "name":"BatteryInfo",
            "dir":"applications/bcu",
            "preprocess": "batteryinfo.js",
            "templates": ["BatteryInfo_Lcfg.c.tmpl"]
        },
        ...
    ]
    */

    /*
    ********************************
    cfgsFile
    ********************************
    module.exports = {
        BatteryInfo: {

            type: "磷酸铁锂",
            nominal: {
                cap: 400.0,
                dischargecurrent: 100.0,
                totalvoltage: 115.2,
                dcchargecurrent: 80.0,
                acchargecurrent: 30.0,
            },
            serialnum: 12,
            parallelnum: 5,
            celltempnum: 8,
            heattempnum: 0,
            poletempnum: 0,
            initsoc: 80,
            initsoh: 100,
        },
    }
    */
    /**
     *
     * @param 模块文件 modsFile
     * @param 配置文件 cfgsFile
     * @param 生成代码的目标目录 outdir
     */

    generate: function (mods, cfgs, outdir) {
        var ejs = require('ejs');
        var fs = require('fs');
        var mkdir = require('mkdir-recursive');
        var path = require('path');
        var util = require('util');
        var child_process = require("child_process");

        var header = '/*\r\n' +
            ' * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n' +
            ' * Generate by template render, DONOT edit it\r\n' +
            ' * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n' +
            ' */\r\n\r\n';

        var globalInfo = {
            requires: {
                'child_process': child_process,
                'util': util,
            }
        }

        mods.forEach(function (mod) {
            var cfg;

            //console.log("MOD:" + JSON.stringify(mod));
            //console.log("GOL:" + JSON.stringify(globalInfo));

            if (mod.preprocess) {
                var ret = require(mod.dir + '/' + mod.preprocess).process(cfgs[mod.name], globalInfo);
                globalInfo[mod.name] = ret.globalInfo;
                cfg = ret.data;
            } else {
                globalInfo[mod.name] = cfgs[mod.name];
                cfg = cfgs[mod.name];
            }

            globalInfo.setReturn = function (val) { globalInfo[mod.name] = val; };

            if (mod.templates) {
                for (var tmpl of mod.templates) {
                    var dest = tmpl.replace(/\.tmpl$/g, '');
                    if (tmpl == dest) {
                        continue
                    }

                    var tmplContent = fs.readFileSync(path.join(mod.dir, tmpl)).toString();
                    try {
                        var content = ejs.render(tmplContent, { globalInfo: globalInfo, cfg: cfg });
                    } catch (e) {
                        var err = "Render " + path.join(mod.dir, tmpl) + " error:" + e;
                        // console.error("==================================================================");
                        console.error(err);
                        // console.error("------------------------------------------------------------------");
                        // console.error("GlobalInfo: " + JSON.stringify(globalInfo, null, "  "))
                        // console.error("------------------------------------------------------------------");
                        // console.error("Config: " + JSON.stringify(cfg, null, "  "))
                        // console.error("==================================================================");
                        throw(err);
                    }
                    dest = path.join(outdir, dest);
                    mkdir.mkdirSync(path.dirname(dest));
                    fs.writeFileSync(dest, header + content);
                }
            }
        });
        return globalInfo;
    }
}