module.exports = {
    process: function(cfgData, info) {
        //console.log(info.CanIf.canhohs);
        var cref = 0;
        var crefToHw = [];
        var hwToCref = [];
        var hohs = [];
        var hwcfgs = [];

        var idToRegisterValue = (id) => {
            var ret;
            if (id & 0x80000000) {
                id = id & (~0x80000000);
                ret = (id<<3) & 0xFFE00000;
                ret |= 0x00180000;
                ret |= (id<<1) & 0x7FFFE;
                return [ret, ret];

            }
            ret = (id & 0x7FF)<<21;
            return [ret | 0x0007FFFF, ret];
        };

        var idFilterReduce = (result, id) => {
            if (!result) {
                result = {allOne: 0xFFFFFFFF, allZero: 0x00000000};
            }


            reg = idToRegisterValue(id);
            result.allOne = result.allOne & reg[0];
            result.allZero = result.allZero | reg[1];
            return result;
        };


        var oneZeroToAcceptMask = (oneZero)=> {
            var mask = oneZero.allOne ^ oneZero.allZero;
            var accept = oneZero.allOne & oneZero.allZero;

            return {
                mask: mask,
                accept: accept,
            }
        }


        for (var i = 0; i < info.CanIf.canhohs.length; i++) {
            hoh = info.CanIf.canhohs[i];
            var eol = i >= info.CanIf.canhohs.length-1 || hoh.hw != info.CanIf.canhohs[i+1].hw;
            hohs.push({
                index: i,
                controllerRef: cref,
                CanObjectType: hoh.dir == 'tx' ? 'CAN_OBJECT_TYPE_TRANSMIT' : 'CAN_OBJECT_TYPE_RECEIVE',
                Can_EOL:  eol ? 1 : 0,
            });

            if (hwcfgs.findIndex(cfg=>cfg.hw == hoh.hw) < 0) {
                var hw = cfgData.find(x=>x.hw == hoh.hw);
                hwcfgs.push({
                    hw: hoh.hw,
                    hohOffset: i,
                    baud: (hw && hw.hasOwnProperty('baud')) ? hw.baud : 250000,
                });
            }
            if (hoh.dir == "rx") {
                var cfg = hwcfgs.find(cfg=>cfg.hw == hoh.hw)
                var oneZero = hoh.ids.reduce(idFilterReduce, null);
                var acceptMask = oneZeroToAcceptMask(oneZero);
                //console.log("oneZero:" + uint32.toHex(oneZero.allOne)+","+uint32.toHex(oneZero.allZero));
                //console.log("acceptMask:" + uint32.toHex(acceptMask.accept)+","+uint32.toHex(acceptMask.mask));
                cfg.accept = acceptMask.accept;
                cfg.mask = acceptMask.mask;
            }

            if (eol) {
                crefToHw[cref] = 'CAN_CTRL_' + hoh.hw;
                hwToCref[hoh.hw] = cref;
                cref++;
            }
        }

        var mdata = {
            hohs: hohs,
            hwcfgs: hwcfgs,
            crefToHw: crefToHw,
            hwToCref: hwToCref,
        };

        //console.log(JSON.stringify(mdata, null, "  "));
        info.render("Can_Lcfg.c",  "Can_Lcfg.c.tmpl", mdata);
    }
}
