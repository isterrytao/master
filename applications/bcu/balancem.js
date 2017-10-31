module.exports = {
    process: function(cfgData, info) {
        /*
         * 增加代码，验证配置参数
         */

        var mdata = {
            Charging: cfgData.Charging,
            Static: cfgData.Static,
        };

        info.render("BalanceM_LCfg.c", "BalanceM_LCfg.c.tmpl", mdata);

        return {}
    }
}
