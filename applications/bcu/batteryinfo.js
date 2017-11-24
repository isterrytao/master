module.exports = {
    process: function(cfgData, info) {
        var cfg = JSON.parse(JSON.stringify(cfgData));

        //console.log("============== batteryinfo =============")
        //console.log(cfg);

        var typeToDescMap = [
            ["APP_BATTERY_TYPE_LEAD_ACID", "铅酸"],
            ["APP_BATTERY_TYPE_NIMH", "镍氢"],
            ["APP_BATTERY_TYPE_LIFEPO4", "磷酸铁锂"],
            ["APP_BATTERY_TYPE_LIMN2O4", "锰酸锂"],
            ["APP_BATTERY_TYPE_LICOO2", "钴酸锂"],
            ["APP_BATTERY_TYPE_TERNARY", "三元材料"],
            ["APP_BATTERY_TYPE_POLYMER_LI_LON", "聚合物锂离子"],
            ["APP_BATTERY_TYPE_LI4O4TI", "钛酸锂"],
            ["APP_BATTERY_TYPE_OTHERS", "其他"],
        ];

        var type = "APP_BATTERY_TYPE_UNKNOWN";
        for (var t of typeToDescMap) {
            if (t[1] == cfg.type) {
                type = t[0];
                break;
            }
        }

        if (cfg.serialnum > 400) {
            throw "Dose not support serialnum > 400";
        }

        if (cfg.celltempnum + cfg.heattempnum + cfg.poletempnum > 200) {
            throw "Dose not support celltempnum + heattempnum + poletempnum > 200";
        }

        var mdata = {
            batttype: type,
            capAH: cfg.nominal.cap,
            dischargecurrent: cfg.nominal.dischargecurrent,
            acchargecurrent: cfg.nominal.acchargecurrent,
            dcchargecurrent: cfg.nominal.dcchargecurrent,
            totalvoltage: cfg.nominal.totalvoltage,
            serialnum: cfg.serialnum,
            parallelnum: cfg.parallelnum,
            celltempnum: cfg.celltempnum,
            heattempnum: cfg.heattempnum,
            poletempnum: cfg.poletempnum,
            initsoc: cfg.initsoc,
            initsoh: cfg.initsoh,
        };

        info.render("BatteryInfo_Lcfg.c", "BatteryInfo_Lcfg.c.tmpl", mdata);

        return {
            battnum: cfg.serialnum,
            celltempnum: cfg.celltempnum,
            heattempnum: cfg.heattempnum,
            poletempnum: cfg.poletempnum,
            totalCap: cfg.nominal.cap,
            initCap: cfg.nominal.cap * cfg.initsoc / 100,
            acchargecurrent: cfg.nominal.acchargecurrent,
            dcchargecurrent: cfg.nominal.dcchargecurrent
        }
    }
}
