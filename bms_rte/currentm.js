module.exports = {
    process: function(cfgData, info) {
        var cfg = JSON.parse(JSON.stringify(cfgData));

        //console.log("================= currentm ==============")
        //console.log(JSON.stringify(cfg, null, "  "));

        var hallparams = {
            DHAB_S124_1: {name:"DHAB_S124大量程通道", offset: -2500,  mul: 5, div: 2, voltMin: 240, voltMax: 4800, supplyVolAdj: true, comments: "[0.5V, 4.5V] <=> [-500A, 500A]" },
            DHAB_S124_2: {name:"DHAB_S124小量程通道", offset: -2500,  mul: 3, div: 8, voltMin: 240, voltMax: 4800, supplyVolAdj: true, lowerHallMaxValue: 75, comments: "[0.5V, 4.5V] <=> [-75A, 75A]" },
            DHAB_S118_1: {name:"DHAB_S118大量程通道", offset: -2500,  mul: 7, div: 4, voltMin: 240, voltMax: 4800, supplyVolAdj: true, comments: "[0.5V, 4.5V] <=> [-350A, 350A]" },
            DHAB_S118_2: {name:"DHAB_S118小量程通道", offset: -2500,  mul: 3, div: 20, voltMin: 240, voltMax: 4800, supplyVolAdj: true, lowerHallMaxValue: 30, comments: "[0.5V, 4.5V] <=> [-30A, 30A]" },
            FS300E2T: {name:"飞轩FS300E2T,300A量程", offset: -2500,  mul: 3, div: 2, voltMin: 240, voltMax: 4800, supplyVolAdj: false, comments: "[0.5V, 4.5V] <=> [-300A, 300A]" },
            FS500E2T: {name:"飞轩FS500E2T,500A量程", offset: -2500,  mul: 5, div: 2, voltMin: 240, voltMax: 4800, supplyVolAdj: false, comments: "[0.5V, 4.5V] <=> [-500A, 500A]" },
            WHK30_350DHAB5S2L_1: {name:"启东双赢30/350A大量程", offset: -2500,  mul: 7, div: 4, voltMin: 50, voltMax: 5200, supplyVolAdj: true, comments: "[0.5V, 4.5V] <=> [-350A, 350A]" },
            WHK30_350DHAB5S2L_2: {name:"启东双赢30/350A小量程", offset: -2500,  mul: 3, div: 20, voltMin: 50, voltMax: 5200, supplyVolAdj: true, lowerHallMaxValue: 30, comments: "[0.5V, 4.5V] <=> [-30A, 30A]" },
        };

        var shuntparams = {
            WSBM8518L1000: {name: "WSBM8518,500A, 50mv", maxVol: 60000, resistor: 100, tempDrift: 0},
            FL2300A50mV: {name:"福来2型,300A,50mV", maxVol: 60000, resistor: 166.7, tempDrift: 0},
            FL2400A50mV: {name:"福来2型,400A,50mV", maxVol: 60000, resistor: 125, tempDrift: 0},
            FL2500A50mV: {name:"福来2型,500A,50mV", maxVol: 60000, resistor: 100, tempDrift: 0},
            FL230A75mV: {name:"福来2型,30A,75mV", maxVol: 100000, resistor: 2500, tempDrift: 0},
            FL2200A75mV: {name:"福来2型,200A,75mV", maxVol: 100000, resistor: 375, tempDrift: 0},
            FL2300A75mV: {name:"福来2型,300A,75mV", maxVol: 100000, resistor: 250, tempDrift: 0},
            FL2400A75mV: {name:"福来2型,400A,75mV", maxVol: 100000, resistor: 187.5, tempDrift: 0},
            FL2500A75mV: {name:"福来2型,500A,75mV", maxVol: 100000, resistor: 150, tempDrift: 0},
        };

        if (!hallparams.hasOwnProperty(cfg.hall1.model)) {
            throw "Hall model not support for channel 1: " + cfg.hall1.model;
        }

        if (!hallparams.hasOwnProperty(cfg.hall2.model)) {
            throw "Hall model not support for channel 2: " + cfg.hall2.model;
        }

        if (!shuntparams.hasOwnProperty(cfg.shunt.model)) {
            throw "Shunt model not support: " + cfg.shunt.model
        }

        var shuntsensor = cfg.shunt;
        for (var x in shuntparams[cfg.shunt.model]) {
            shuntsensor[x] = shuntparams[cfg.shunt.model][x];
        }

        var hall1sensor = cfg.hall1;
        for (var x in hallparams[cfg.hall1.model]) {
            hall1sensor[x] = hallparams[cfg.hall1.model][x];
        }

        var hall2sensor = cfg.hall2;
        for (var x in hallparams[cfg.hall2.model]) {
            hall2sensor[x] = hallparams[cfg.hall2.model][x];
        }

        var isDualRange = false;
        var lowerHallMaxValue = 0;
        var h1model = cfg.hall1.model.match(/(.*)_1$/);
        var h2model = cfg.hall2.model.match(/(.*)_2$/);
        if (h1model && h1model && h1model[1] == h2model[1]) {
            isDualRange = true;
            lowerHallMaxValue = cfg.hall2.lowerHallMaxValue * 10;
        }

        var mainIndex = ['hall1', 'hall2', 'shunt'].indexOf(cfg.main.channel);
        var mainRedundantIndex = ['hall1', 'hall2', 'shunt'].indexOf(cfg.main.redundant);
        var heaterIndex = ['hall1', 'hall2', 'shunt'].indexOf(cfg.heater.channel);
        var heaterRedundantIndex = ['hall1', 'hall2', 'shunt'].indexOf(cfg.heater.redundant);

        if (mainIndex < 0) throw 'Current channel main must be one of "hall1", "hall2" or "shunt"';
        if (mainIndex == mainRedundantIndex || mainIndex == heaterIndex || mainIndex == heaterRedundantIndex) throw cfg.main.channel + " selected by multi";
        if (mainRedundantIndex >=0 && (mainRedundantIndex == heaterIndex || mainRedundantIndex == heaterRedundantIndex)) throw cfg.main.redundant + " selected by multi";
        if (heaterIndex >= 0 && heaterIndex == heaterRedundantIndex)   throw cfg.heater.channel + " selected by multi";

        var currentmcfg = {
            isDualRange: isDualRange,
            lowerHallMaxValue: lowerHallMaxValue,
            main: {
                enable: mainIndex >= 0,
                channel: cfg.main.channel.toUpperCase(),
                redundancyEnable: mainRedundantIndex >= 0,
                redundancyChannel: cfg.main.redundant.toUpperCase(),
            },
            heater: {
                enable: heaterIndex >= 0,
                channel: cfg.heater.channel.toUpperCase(),
                redundancyEnable: heaterRedundantIndex >= 0,
                redundancyChannel: cfg.heater.redundant.toUpperCase(),
            },
            autoCalibLimit: {
                hall1: cfg.hall1.autoCalibLimit * 10,
                hall2: cfg.hall2.autoCalibLimit * 10,
                shunt: cfg.shunt.autoCalibLimit * 10,
            },
            zeroFilter: {
                hall1: cfg.hall1.zeroFilterValue * 10,
                hall2: cfg.hall2.zeroFilterValue * 10,
                shunt: cfg.shunt.zeroFilterValue * 10,
            },
        };

        var mdata = {
            hall1sensor: hall1sensor,
            hall2sensor: hall2sensor,
            shuntsensor: shuntsensor,
            currentmcfg: currentmcfg,
        };

        //console.log("================= currentm ==============")
        //console.log(JSON.stringify(mdata, null, "  "));

        info.render("CurrentM_Lcfg.c", "CurrentM_Lcfg.c.tmpl", mdata);
    }
}
