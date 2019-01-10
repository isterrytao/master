#!/usr/bin/env node

modules_makelib = [
    {
        name: 'LibInfo',
        dir: './',
        templates: ['LibInfo.h.tmpl']
    },
    {
        name: 'Diagnosis',
        dir: './bms_rte',
        templates: ['Diagnosis_Cfg.h.tmpl']
    },
];
// 模块定义
modules = [
    {name: "LibInfo"},
    {
        name: 'BatteryInfo',
        dir: './applications/bcu',
        templates: ['BatteryInfo_Lcfg.c.tmpl']
    },
    {
        name: 'SystemConnection',
        dir: './applications/bcu',
        templates: ['SystemConnection_Lcfg.h.tmpl', 'SystemConnection_Lcfg.c.tmpl']
    },
    {
        name: 'AppInfo',
        dir: './applications',
        templates: ['AppInfo.h.tmpl']
    },
    {
        name: 'BalanceM',
        dir: './applications/bcu',
        templates: ['BalanceM_LCfg.c.tmpl']
    },
    {
        name: 'CanTp',
        dir: './com/CanTp',
        templates: ['CanTp_PBcfg.c.tmpl']
    },
    {
        name: 'DtuTp'
    },
    {
        name: 'LtcTp'
    },
    {
        name: 'InnerTp',
        dir: './com/InnerTp',
        templates: ['InnerTp_Lcfg.c.tmpl']
    },
    {
        name: 'J1939Tp',
        dir: './com/J1939Tp',
        templates: ['J1939Tp_Lcfg.h.tmpl', 'J1939Tp_Lcfg.c.tmpl']
    },
    {
        name: 'CanIf',
        dir: './com/CanIf',
        templates: ['CanIf_Lcfg.h.tmpl', 'CanIf_Lcfg.c.tmpl']
    },
    {
        name: 'CanDrv',
        dir: './arch/hcs12x',
        templates: ['Can_Lcfg.c.tmpl']
    },
    {
        name: 'PwmCapture',
        dir: './arch/hcs12x',
        templates: ['PwmCapture_Lcfg.h.tmpl', 'PwmCapture_Lcfg.c.tmpl']
    },
    {
        name: 'PduR',
        dir: './com/PduR',
        templates: ['PduR_Lcfg.h.tmpl', 'PduR_Lcfg.c.tmpl', 'PduR_Types.h.tmpl', 'PduR_Upper.c.tmpl', 'PduR_Middle.c.tmpl']
    },
    {
        name: 'GB32960',
        dir: './com/DtuTp',
        templates: ['GB32960_Lcfg.c.tmpl']
    },
    {
        name: 'Diagnosis',
        dir: './bms_rte',
        templates: ['Diagnosis_Cfg.h.tmpl', 'Diagnosis_Lcfg.c.tmpl']
    },
    {
        name: 'ParameterM',
        dir: './bms_rte',
        templates: ['ParameterM_Lcfg.c.tmpl']

    },
    {
        name: 'CurrentM',
        dir: './bms_rte',
        templates: ['CurrentM_Lcfg.h.tmpl', 'CurrentM_Lcfg.c.tmpl']

    },
    {
        name: 'RelayM',
        dir: './bms_rte',
        templates: ['RelayM_Lcfg.h.tmpl', 'RelayM_Lcfg.c.tmpl']
    },
    {
        name: 'Charge',
        dir: './applications/bcu',
        templates: ['Charge_Lcfg.c.tmpl']
    },
    {
        name: 'RuntimeM',
        dir: './bms_rte',
        templates: ['RuntimeM_Cfg.h.tmpl', 'RuntimeM_Lcfg.c.tmpl']
    },
    {
        name: 'Ntc',
        dir: './bms_rte',
        templates: ['Ntc_Lcfg.h.tmpl', 'Ntc_Lcfg.c.tmpl']
     },
];

// 导入使用到的模块
var ejs = require('ejs');
var fs = require('fs');
var mkdir = require('mkdir-recursive');
var path = require('path');
var program = require('commander');
var codeg = require('./codegenerator');

var libconfig = {};
try {
    libconfig = require('./libconfig.js')
} catch (err) {
}


var getJsonPath = (obj, path) => {
    return path.split('.').reduce((v, k) => {
        var ks = k.match(/(.+)\[(\d*)\]/);
        if (ks == null) return v[k];
        return v[ks[1]][parseInt(ks[2])];
    }, obj);
}

var defcalfile = path.join(__dirname, 'applications/bcu/calibrate.json');
var defcfgfile = path.join(__dirname, 'applications/bcu/config.js');
var defoutdir = path.join(__dirname, 'config');

// 合并配置文件与标定文件
var appendCalibrate = (cfgfile, calfile) => {
    var cfg = require(cfgfile);
    if (!cfg.hasOwnProperty('ParameterM')) return cfg;
    cfgParamterM = cfg.ParameterM;
    cfg.ParameterM = require(calfile);
    cfg.ParameterM.Diagnosis = cfg.ParameterM.Diagnosis.concat(cfgParamterM.Diagnosis);
    return cfg;
}

// 定义命令行参数
program
    .version('0.0.1')
    .option('-o, --outdir [dir]', 'The output directory, default [' + defoutdir + ']', defoutdir)
    .option('-c, --config [file]', 'The config file, default [' + defcfgfile + ']', defcfgfile)
    .option('--calibrate [calibratefile]', 'Calibrate JSON file, default [' + defcalfile + ']', defcalfile);

// 定义getcfg子命令
program.command('getcfg [key]').action(function (key) {
    var value = appendCalibrate(program.config, program.calibrate);
    if (key) value = getJsonPath(value, key);
    console.log(JSON.stringify(value));
    process.exit(0);
});

// 解析参数
program.parse(process.argv);

// 合并传入的配置文件和标定文件
var config = appendCalibrate(program.config, program.calibrate);
if (libconfig.hasOwnProperty('LibInfo')) {
    config['LibInfo'] = libconfig['LibInfo'];
}

process.chdir(__dirname);
if (process.env.hasOwnProperty('MAKELIB') && process.env.MAKELIB != '0') {
    var info = codeg.generate(modules_makelib, config, program.outdir)
}else {
    var info = codeg.generate(modules, config, program.outdir)
    // 输出生成项目的名称, 使用的PCBA和BCU的类型
    console.log(info.AppInfo.releaseFileName + ';' + info.SystemConnection.PCBA + ';' + info.SystemConnection.BcuType);
}


