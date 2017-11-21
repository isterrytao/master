module.exports = {
    process: function(cfgData, info) {
        var temperatureSensors = [
            {
                id: 'Shiheng_CWF4B_103F_3435B',
                define: 0,
                desc: '时恒10K',
                start: -40,
                // cat SHIHENG_CWF4B103F3435B.temperature_table.txt  | awk -F' ' '{ printf("/*%s*/%s, ",$1,$3); }'
                resistors: [/*-40*/197.590, /*-39*/184.878, /*-38*/173.564, /*-37*/163.389, /*-36*/154.154, /*-35*/145.703, /*-34*/137.914, /*-33*/130.691, /*-32*/123.959, /*-31*/117.657, /*-30*/111.738, /*-29*/106.163, /*-28*/100.900, /*-27*/95.924, /*-26*/91.213, /*-25*/86.750, /*-24*/82.517, /*-23*/78.503, /*-22*/74.696, /*-21*/71.083, /*-20*/67.657, /*-19*/64.406, /*-18*/61.324, /*-17*/58.401, /*-16*/55.629, /*-15*/53.003, /*-14*/50.513, /*-13*/48.154, /*-12*/45.918, /*-11*/43.800, /*-10*/41.794, /*-9*/39.892, /*-8*/38.091, /*-7*/36.383, /*-6*/34.765, /*-5*/33.231, /*-4*/31.777, /*-3*/30.398, /*-2*/29.089, /*-1*/27.847, /*0*/26.838, /*1*/25.549, /*2*/24.486, /*3*/23.475, /*4*/22.514, /*5*/21.600, /*6*/20.730, /*7*/19.902, /*8*/19.113, /*9*/18.361, /*10*/17.750, /*11*/16.960, /*12*/16.307, /*13*/15.683, /*14*/15.088, /*15*/14.518, /*16*/13.974, /*17*/13.453, /*18*/12.954, /*19*/12.477, /*20*/12.019, /*21*/11.581, /*22*/11.161, /*23*/10.758, /*24*/10.371, /*25*/10.000, /*26*/9.643, /*27*/9.301, /*28*/8.972, /*29*/8.655, /*30*/8.351, /*31*/8.059, /*32*/7.778, /*33*/7.508, /*34*/7.247, /*35*/6.997, /*36*/6.756, /*37*/6.523, /*38*/6.300, /*39*/6.084, /*40*/5.877, /*41*/5.677, /*42*/5.485, /*43*/5.299, /*44*/5.120, /*45*/4.948, /*46*/4.782, /*47*/4.621, /*48*/4.467, /*49*/4.318, /*50*/4.175, /*51*/4.036, /*52*/3.903, /*53*/3.774, /*54*/3.650, /*55*/3.530, /*56*/3.414, /*57*/3.303, /*58*/3.196, /*59*/3.092, /*60*/2.992, /*61*/2.896, /*62*/2.802, /*63*/2.713, /*64*/2.626, /*65*/2.543, /*66*/2.462, /*67*/2.384, /*68*/2.309, /*69*/2.237, /*70*/2.167, /*71*/2.099, /*72*/2.034, /*73*/1.971, /*74*/1.911, /*75*/1.852, /*76*/1.796, /*77*/1.741, /*78*/1.689, /*79*/1.638, /*80*/1.589, /*81*/1.541, /*82*/1.495, /*83*/1.451, /*84*/1.409, /*85*/1.385, /*86*/1.327, /*87*/1.289, /*88*/1.252, /*89*/1.216, /*90*/1.181, /*91*/1.148, /*92*/1.115, /*93*/1.084, /*94*/1.053, /*95*/1.024, /*96*/0.996, /*97*/0.968, /*98*/0.942, /*99*/0.916, /*100*/0.906, /*101*/0.867, /*102*/0.844, /*103*/0.821, /*104*/0.800, /*105*/0.778, /*106*/0.758, /*107*/0.738, /*108*/0.719, /*109*/0.701, /*110*/0.683, /*111*/0.665, /*112*/0.648, /*113*/0.632, /*114*/0.616, /*115*/0.600, /*116*/0.586, /*117*/0.571, /*118*/0.557, /*119*/0.543, /*120*/0.530, /*121*/0.517, /*122*/0.505, /*123*/0.493, /*124*/0.481, /*125*/0.470, ]
            }, {
                id: 'Soarwhale_10K3435B',
                define: 1,
                desc: '由甲申田10K',
                start: -40,
                resistors: [/*-40*/188.2, /*-39*/178.3, /*-38*/169, /*-37*/160.2, /*-36*/151.9, /*-35*/144.1, /*-34*/136.7, /*-33*/129.8, /*-32*/123.2, /*-31*/117, /*-30*/111.2, /*-29*/105.7, /*-28*/100.5, /*-27*/95.53, /*-26*/90.87, /*-25*/86.46, /*-24*/82.29, /*-23*/78.34, /*-22*/74.61, /*-21*/71.07, /*-20*/67.72, /*-19*/64.55, /*-18*/61.54, /*-17*/58.69, /*-16*/55.98, /*-15*/53.42, /*-14*/50.98, /*-13*/48.67, /*-12*/46.48, /*-11*/44.4, /*-10*/42.42, /*-9*/40.54, /*-8*/38.75, /*-7*/37.05, /*-6*/35.44, /*-5*/33.9, /*-4*/32.44, /*-3*/31.05, /*-2*/29.73, /*-1*/28.47, /*0*/27.27, /*1*/26.12, /*2*/25.03, /*3*/24, /*4*/23.01, /*5*/22.06, /*6*/21.16, /*7*/20.3, /*8*/19.48, /*9*/18.7, /*10*/17.95, /*11*/17.24, /*12*/16.56, /*13*/15.91, /*14*/15.29, /*15*/14.69, /*16*/14.13, /*17*/13.58, /*18*/13.06, /*19*/12.57, /*20*/12.09, /*21*/11.64, /*22*/11.2, /*23*/10.78, /*24*/10.38, /*25*/10, /*26*/9.633, /*27*/9.282, /*28*/8.945, /*29*/8.622, /*30*/8.313, /*31*/8.016, /*32*/7.731, /*33*/7.458, /*34*/7.196, /*35*/6.944, /*36*/6.702, /*37*/6.47, /*38*/6.247, /*39*/6.033, /*40*/5.828, /*41*/5.63, /*42*/5.44, /*43*/5.258, /*44*/5.082, /*45*/4.913, /*46*/4.751, /*47*/4.595, /*48*/4.444, /*49*/4.3, /*50*/4.16, /*51*/4.026, /*52*/3.897, /*53*/3.773, /*54*/3.653, /*55*/3.538, /*56*/3.427, /*57*/3.32, /*58*/3.216, /*59*/3.117, /*60*/3.021, /*61*/2.928, /*62*/2.839, /*63*/2.753, /*64*/2.67, /*65*/2.59, /*66*/2.512, /*67*/2.438, /*68*/2.365, /*69*/2.296, /*70*/2.229, /*71*/2.164, /*72*/2.101, /*73*/2.04, /*74*/1.982, /*75*/1.925, /*76*/1.87, /*77*/1.817, /*78*/1.766, /*79*/1.716, /*80*/1.669, /*81*/1.622, /*82*/1.577, /*83*/1.534, /*84*/1.492, /*85*/1.451, /*86*/1.412, /*87*/1.374, /*88*/1.337, /*89*/1.301, /*90*/1.267, /*91*/1.233, /*92*/1.201, /*93*/1.169, /*94*/1.139, /*95*/1.109, /*96*/1.08, /*97*/1.052, /*98*/1.026, /*99*/0.9993, /*100*/0.974, /*101*/0.9493, /*102*/0.9254, /*103*/0.9023, /*104*/0.8798, /*105*/0.8579, /*106*/0.8367, /*107*/0.8161, /*108*/0.7961, /*109*/0.7767, /*110*/0.7579, /*111*/0.7395, /*112*/0.7217, /*113*/0.7044, /*114*/0.6876, /*115*/0.6713, /*116*/0.6554, /*117*/0.64, /*118*/0.625, /*119*/0.6104, /*120*/0.5962, /*121*/0.5807, /*122*/0.5673, /*123*/0.5544, /*124*/0.5418, /*125*/0.5295, /*126*/0.5175, /*127*/0.5059, /*128*/0.4946, /*129*/0.4836, /*130*/0.4729, ]
            }, {
                id: 'Kemit_CWF110KF3435',
                define: 2,
                desc: '科敏10K',
                start: -40,
                resistors: [/*-40*/204.7, /*-39*/193.4, /*-38*/182.7, /*-37*/172.7, /*-36*/163.3, /*-35*/154.4, /*-34*/146.1, /*-33*/138.4, /*-32*/131.0, /*-31*/124.1, /*-30*/117.6, /*-29*/111.5, /*-28*/105.8, /*-27*/100.4, /*-26*/95.25, /*-25*/90.44, /*-24*/85.90, /*-23*/81.61, /*-22*/77.57, /*-21*/73.75, /*-20*/70.15, /*-19*/66.74, /*-18*/63.52, /*-17*/60.48, /*-16*/57.60, /*-15*/54.87, /*-14*/52.29, /*-13*/49.85, /*-12*/47.54, /*-11*/45.35, /*-10*/43.27, /*-9*/41.30, /*-8*/39.44, /*-7*/37.67, /*-6*/35.99, /*-5*/34.39, /*-4*/32.88, /*-3*/31.44, /*-2*/30.07, /*-1*/28.77, /*0*/27.53, /*1*/26.36, /*2*/25.24, /*3*/24.18, /*4*/23.16, /*5*/22.2, /*6*/21.28, /*7*/20.41, /*8*/19.57, /*9*/18.78, /*10*/18.02, /*11*/17.3, /*12*/16.61, /*13*/15.95, /*14*/15.32, /*15*/14.72, /*16*/14.15, /*17*/13.6, /*18*/13.08, /*19*/12.58, /*20*/12.1, /*21*/11.64, /*22*/11.2, /*23*/10.78, /*24*/10.38, /*25*/10, /*26*/9.633, /*27*/9.281, /*28*/8.944, /*29*/8.621, /*30*/8.311, /*31*/8.015, /*32*/7.73, /*33*/7.457, /*34*/7.195, /*35*/6.944, /*36*/6.703, /*37*/6.471, /*38*/6.249, /*39*/6.035, /*40*/5.83, /*41*/5.633, /*42*/5.443, /*43*/5.261, /*44*/5.086, /*45*/4.918, /*46*/4.756, /*47*/4.6, /*48*/4.45, /*49*/4.305, /*50*/4.166, /*51*/4.033, /*52*/3.904, /*53*/3.78, /*54*/3.66, /*55*/3.545, /*56*/3.434, /*57*/3.327, /*58*/3.224, /*59*/3.124, /*60*/3.028, /*61*/2.935, /*62*/2.846, /*63*/2.76, /*64*/2.677, /*65*/2.596, /*66*/2.519, /*67*/2.444, /*68*/2.371, /*69*/2.301, /*70*/2.234, /*71*/2.169, /*72*/2.106, /*73*/2.045, /*74*/1.986, /*75*/1.929, /*76*/1.874, /*77*/1.82, /*78*/1.769, /*79*/1.719, /*80*/1.671, /*81*/1.624, /*82*/1.579, /*83*/1.535, /*84*/1.492, /*85*/1.451, /*86*/1.412, /*87*/1.373, /*88*/1.336, /*89*/1.3, /*90*/1.265, /*91*/1.231, /*92*/1.198, /*93*/1.166, /*94*/1.135, /*95*/1.105, /*96*/1.076, /*97*/1.048, /*98*/1.02, /*99*/0.9936, /*100*/0.9679, /*101*/0.9429, /*102*/0.9186, /*103*/0.8950, /*104*/0.8722, /*105*/0.85, /*106*/0.8284, /*107*/0.8075, /*108*/0.7872, /*109*/0.7675, /*110*/0.7483, /*111*/0.7297, /*112*/0.7116, /*113*/0.694, /*114*/0.6769, /*115*/0.6603, /*116*/0.6442, /*117*/0.6285, /*118*/0.6132, /*119*/0.5984, /*120*/0.584, /*121*/0.5699, /*122*/0.5563, /*123*/0.5431, /*124*/0.5302, /*125*/0.5176, /*126*/0.5054, /*127*/0.4935, /*128*/0.482, /*129*/0.4707, /*130*/0.4598, ]
            }, {
                id: 'Yonggui_YG691_51_00_50',
                define: 3,
                desc: '永贵10K',
                start: -40,
                resistors: [/*-40*/173.07, /*-39*/163.86, /*-38*/155.22, /*-37*/147.12, /*-36*/139.52, /*-35*/132.38, /*-34*/125.67, /*-33*/119.36, /*-32*/113.42, /*-31*/107.81, /*-30*/102.52, /*-29*/97.518, /*-28*/92.792, /*-27*/88.321, /*-26*/84.088, /*-25*/80.079, /*-24*/76.282, /*-23*/72.682, /*-22*/69.271, /*-21*/66.037, /*-20*/62.971, /*-19*/60.064, /*-18*/57.307, /*-17*/54.694, /*-16*/52.215, /*-15*/49.865, /*-14*/47.636, /*-13*/45.522, /*-12*/43.517, /*-11*/41.615, /*-10*/39.810, /*-9*/38.096, /*-8*/36.469, /*-7*/34.924, /*-6*/33.456, /*-5*/32.061, /*-4*/30.734, /*-3*/29.472, /*-2*/28.271, /*-1*/27.127, /*0*/26.037, /*1*/24.999, /*2*/24.008, /*3*/23.063, /*4*/22.160, /*5*/21.299, /*6*/20.475, /*7*/19.687, /*8*/18.934, /*9*/18.213, /*10*/17.523, /*11*/16.863, /*12*/16.229, /*13*/15.623, /*14*/15.041, /*15*/14.483, /*16*/13.948, /*17*/13.435, /*18*/12.942, /*19*/12.469, /*20*/12.015, /*21*/11.579, /*22*/11.160, /*23*/10.758, /*24*/10.371, /*25*/10.000, /*26*/9.6433, /*27*/9.3004, /*28*/8.9710, /*29*/8.6543, /*30*/8.3500, /*31*/8.0574, /*32*/7.7762, /*33*/7.5058, /*34*/7.2458, /*35*/6.9958, /*36*/6.7555, /*37*/6.5243, /*38*/6.3019, /*39*/6.0881, /*40*/5.8824, /*41*/5.6845, /*42*/5.4940, /*43*/5.3108, /*44*/5.1345, /*45*/4.9648, /*46*/4.8014, /*47*/4.6441, /*48*/4.4927, /*49*/4.3469, /*50*/4.2064, /*51*/4.0711, /*52*/3.9408, /*53*/3.8152, /*54*/3.6941, /*55*/3.5775, /*56*/3.4650, /*57*/3.3565, /*58*/3.2519, /*59*/3.1510, /*60*/3.0537, /*61*/2.9598, /*62*/2.8692, /*63*/2.7817, /*64*/2.6972, /*65*/2.6157, /*66*/2.5369, /*67*/2.4608, /*68*/2.3873, /*69*/2.3163, /*70*/2.2477, /*71*/2.1813, /*72*/2.1172, /*73*/2.0552, /*74*/1.9953, /*75*/1.9373, /*76*/1.8812, /*77*/1.8270, /*78*/1.7745, /*79*/1.7237, /*80*/1.6745, /*81*/1.6269, /*82*/1.5809, /*83*/1.5363, /*84*/1.4931, /*85*/1.4513, /*86*/1.4109, /*87*/1.3717, /*88*/1.3337, /*89*/1.2969, /*90*/1.2613, /*91*/1.2268, /*92*/1.1933, /*93*/1.1609, /*94*/1.1295, /*95*/1.0991, /*96*/1.0696, /*97*/1.0410, /*98*/1.0133, /*99*/0.98648, /*100*/0.96045, /*101*/0.93522, /*102*/0.91075, /*103*/0.88703, /*104*/0.86403, /*105*/0.84173, /*106*/0.82010, /*107*/0.79913, /*108*/0.77879, /*109*/0.75907, /*110*/0.73993, /*111*/0.72137, /*112*/0.70337, /*113*/0.68591, /*114*/0.66897, /*115*/0.65253, /*116*/0.63658, /*117*/0.62110, /*118*/0.60608, /*119*/0.59151, /*120*/0.57736, /*121*/0.56363, /*122*/0.55030, /*123*/0.53736, /*124*/0.52480, /*125*/0.51260, /*126*/0.50075, /*127*/0.48925, /*128*/0.47808, /*129*/0.46723, /*130*/0.45669, /*131*/0.44645, /*132*/0.43650, /*133*/0.42683, /*134*/0.41743, /*135*/0.40830, /*136*/0.39942, /*137*/0.39079, /*138*/0.38239, /*139*/0.37423, /*140*/0.36629, /*141*/0.35857, /*142*/0.35106, /*143*/0.34375, /*144*/0.33664, /*145*/0.32971, /*146*/0.32297, /*147*/0.31641, /*148*/0.31002, /*149*/0.30380, /*150*/0.29774, ]
            }, {
                id: 'Shiheng_CWF4B_104F_3950',
                define: 4,
                desc: '时恒100K',
                start: -40,
                resistors: [/*-40*/2971, /*-39*/2793.89, /*-38*/2627.18, /*-37*/2470.4, /*-36*/2323.09, /*-35*/2184.77, /*-34*/2054.98, /*-33*/1933.24, /*-32*/1819.11, /*-31*/1712.14, /*-30*/1611.9, /*-29*/1517.98, /*-28*/1430, /*-27*/1347.57, /*-26*/1270.35, /*-25*/1198, /*-24*/1130.19, /*-23*/1066.65, /*-22*/1007.07, /*-21*/951.217, /*-20*/898.82, /*-19*/849.657, /*-18*/803.514, /*-17*/760.189, /*-16*/719.495, /*-15*/681.256, /*-14*/645.311, /*-13*/611.507, /*-12*/579.703, /*-11*/549.768, /*-10*/521.579, /*-9*/495.024, /*-8*/469.995, /*-7*/446.396, /*-6*/424.134, /*-5*/403.123, /*-4*/383.286, /*-3*/364.548, /*-2*/346.840, /*-1*/330.099, /*0*/315.68, /*1*/299.283, /*2*/285.101, /*3*/271.671, /*4*/258.947, /*5*/246.889, /*6*/235.457, /*7*/224.614, /*8*/214.326, /*9*/204.561, /*10*/195.29, /*11*/186.484, /*12*/178.117, /*13*/170.165, /*14*/162.605, /*15*/155.416, /*16*/148.577, /*17*/142.068, /*18*/135.874, /*19*/129.976, /*20*/124.358, /*21*/119.008, /*22*/113.909, /*23*/109.049, /*24*/104.417, /*25*/100, /*26*/95.786, /*27*/91.767, /*28*/87.932, /*29*/84.272, /*30*/80.779, /*31*/77.443, /*32*/74.258, /*33*/71.215, /*34*/68.309, /*35*/65.532, /*36*/62.878, /*37*/60.341, /*38*/57.916, /*39*/55.597, /*40*/53.380, /*41*/51.259, /*42*/49.230, /*43*/47.289, /*44*/45.432, /*45*/43.654, /*46*/41.952, /*47*/40.323, /*48*/38.764, /*49*/37.27, /*50*/35.84, /*51*/34.469, /*52*/33.156, /*53*/31.898, /*54*/30.693, /*55*/29.538, /*56*/28.43, /*57*/27.368, /*58*/26.35, /*59*/25.374, /*60*/24.437, /*61*/23.539, /*62*/22.678, /*63*/21.851, /*64*/21.057, /*65*/20.296, /*66*/19.565, /*67*/18.863, /*68*/18.189, /*69*/17.542, /*70*/16.921, /*71*/16.324, /*72*/15.751, /*73*/15.2, /*74*/14.671, /*75*/14.162, /*76*/13.673, /*77*/13.203, /*78*/12.751, /*79*/12.316, /*80*/11.898, /*81*/11.496, /*82*/11.109, /*83*/10.737, /*84*/10.379, /*85*/10.035, /*86*/9.703, /*87*/9.383, /*88*/9.076, /*89*/8.780, /*90*/8.495, /*91*/8.22, /*92*/7.956, /*93*/7.701, /*94*/7.455, /*95*/7.219, /*96*/6.99, /*97*/6.77, /*98*/6.558, /*99*/6.354, /*100*/6.173, /*101*/5.966, /*102*/5.783, /*103*/5.606, /*104*/5.435, /*105*/5.27, /*106*/5.111, /*107*/4.957, /*108*/4.809, /*109*/4.666, /*110*/4.527, /*111*/4.394, /*112*/4.264, /*113*/4.14, /*114*/4.019, /*115*/3.903, /*116*/3.79, /*117*/3.681, /*118*/3.576, /*119*/3.474, /*120*/3.376, /*121*/3.28, /*122*/3.188, /*123*/3.099, /*124*/3.013, /*125*/2.929, ]
            }, {
                id: 'MTG2_420F103H',
                define: 5,
                desc: 'MTG2_10K',
                start: -50,
                resistors: [/*-50.0*/882.3669, /*-49.0*/819.0741, /*-48.0*/760.6220, /*-47.0*/706.6228, /*-46.0*/656.7206, /*-45.0*/610.5887, /*-44.0*/567.9277, /*-43.0*/528.4627, /*-42.0*/491.9414, /*-41.0*/458.1320, /*-40.0*/426.8220, /*-39.0*/397.8160, /*-38.0*/370.9347, /*-37.0*/346.0131, /*-36.0*/322.8999, /*-35.0*/301.4557, /*-34.0*/281.5526, /*-33.0*/263.0729, /*-32.0*/245.9082, /*-31.0*/229.9589, /*-30.0*/215.1333, /*-29.0*/201.3468, /*-28.0*/188.5219, /*-27.0*/176.5867, /*-26.0*/165.4753, /*-25.0*/155.1269, /*-24.0*/145.4853, /*-23.0*/136.4988, /*-22.0*/128.1197, /*-21.0*/120.3038, /*-20.0*/113.01060, /*-19.0*/106.2024, /*-18.0*/99.8445, /*-17.0*/93.9049, /*-16.0*/88.3539, /*-15.0*/83.1642, /*-14.0*/78.3103, /*-13.0*/73.7687, /*-12.0*/69.5179, /*-11.0*/65.5376, /*-10.0*/61.8092, /*-9.0*/58.3154, /*-8.0*/55.0404, /*-7.0*/51.9692, /*-6.0*/49.0880, /*-5.0*/46.3842, /*-4.0*/43.8458, /*-3.0*/41.4618, /*-2.0*/39.2221, /*-1.0*/37.1171, /*0.0*/35.1380, /*1.0*/33.2767, /*2.0*/31.5254, /*3.0*/29.8770, /*4.0*/28.3251, /*5.0*/26.8633, /*6.0*/25.4861, /*7.0*/24.1880, /*8.0*/22.9640, /*9.0*/21.8096, /*10.0*/20.7204, /*11.0*/19.6923, /*12.0*/18.7216, /*13.0*/17.8049, /*14.0*/16.9387, /*15.0*/16.1566, /*16.0*/15.3800, /*17.0*/14.6451, /*18.0*/13.9494, /*19.0*/13.2905, /*20.0*/12.6664, /*21.0*/12.0749, /*22.0*/11.5143, /*23.0*/10.9827, /*24.0*/10.4784, /*25.0*/10.0000, /*26.0*/9.5459, /*27.0*/9.1149, /*28.0*/8.7055, /*29.0*/8.3167, /*30.0*/7.9472, /*31.0*/7.5960, /*32.0*/7.2621, /*33.0*/6.9446, /*34.0*/6.6426, /*35.0*/6.3552, /*36.0*/6.0817, /*37.0*/5.8213, /*38.0*/5.5734, /*39.0*/5.3372, /*40.0*/5.1073, /*41.0*/4.8932, /*42.0*/4.6892, /*43.0*/4.4947, /*44.0*/4.3093, /*45.0*/4.1325, /*46.0*/3.9638, /*47.0*/3.8029, /*48.0*/3.6493, /*49.0*/3.5028, /*50.0*/3.3628, /*51.0*/3.2292, /*52.0*/3.1015, /*53.0*/2.9795, /*54.0*/2.8630, /*55.0*/2.7516, /*56.0*/2.6451, /*57.0*/2.5432, /*58.0*/2.4458, /*59.0*/2.3526, /*60.0*/2.2634, /*61.0*/2.1780, /*62.0*/2.0963, /*63.0*/2.0181, /*64.0*/1.9431, /*65.0*/1.8713, /*66.0*/1.8025, /*67.0*/1.7366, /*68.0*/1.6734, /*69.0*/1.6129, /*70.0*/1.5548, /*71.0*/1.4991, /*72.0*/1.4456, /*73.0*/1.3944, /*74.0*/1.3452, /*75.0*/1.2941, /*76.0*/1.2491, /*77.0*/1.2059, /*78.0*/1.1644, /*79.0*/1.1246, /*80.0*/1.0864, /*81.0*/1.0497, /*82.0*/1.0145, /*83.0*/0.9806, /*84.0*/0.9481, /*85.0*/0.9168, /*86.0*/0.8868, /*87.0*/0.8579, /*88.0*/0.8301, /*89.0*/0.8034, /*90.0*/0.7783, /*91.0*/0.7536, /*92.0*/0.7298, /*93.0*/0.7069, /*94.0*/0.6848, /*95.0*/0.6635, /*96.0*/0.6430, /*97.0*/0.6231, /*98.0*/0.6040, /*99.0*/0.5856, /*100.0*/0.5678, /*101.0*/0.5507, /*102.0*/0.5341, /*103.0*/0.5181, /*104.0*/0.5027, /*105.0*/0.4878, /*106.0*/0.4734, /*107.0*/0.4595, /*108.0*/0.4460, /*109.0*/0.4331, /*110.0*/0.4205, /*111.0*/0.4084, /*112.0*/0.3967, /*113.0*/0.3853, /*114.0*/0.3744, /*115.0*/0.3632, /*116.0*/0.3529, /*117.0*/0.3431, /*118.0*/0.3335, /*119.0*/0.3243, /*120.0*/0.3154, /*121.0*/0.3067, /*122.0*/0.2984, /*123.0*/0.2903, /*124.0*/0.2824, /*125.0*/0.2749, ]
            }, {
                id: 'Kemit_CWF110KF4150',
                define: 6,
                desc: '科敏10K',
                start: -50,
                resistors: [/*-50*/836.539, /*-49*/777.2219, /*-48*/722.3932, /*-47*/671.6966, /*-46*/624.8055, /*-45*/581.4195, /*-44*/541.2631, /*-43*/504.0831, /*-42*/469.6469, /*-41*/437.7406, /*-40*/408.1681, /*-39*/380.7487, /*-38*/355.3165, /*-37*/331.7188, /*-36*/309.8153, /*-35*/289.4768, /*-34*/270.5846, /*-33*/253.0291, /*-32*/236.7098, /*-31*/221.5339, /*-30*/207.4159, /*-29*/194.2772, /*-28*/182.0451, /*-27*/170.6528, /*-26*/160.0386, /*-25*/150.1455, /*-24*/140.9212, /*-23*/132.317, /*-22*/124.2883, /*-21*/116.7936, /*-20*/109.7949, /*-19*/103.2568, /*-18*/97.1466, /*-17*/91.4342, /*-16*/86.0917, /*-15*/81.0933, /*-14*/76.4149, /*-13*/72.0345, /*-12*/67.9315, /*-11*/64.087, /*-10*/60.4833, /*-9*/57.1041, /*-8*/53.9342, /*-7*/50.9595, /*-6*/48.167, /*-5*/45.5446, /*-4*/43.081, /*-3*/40.7657, /*-2*/38.5891, /*-1*/36.5421, /*0*/34.6162, /*1*/32.8038, /*2*/31.0974, /*3*/29.4903, /*4*/27.9762, /*5*/26.5402, /*6*/25.2038, /*7*/23.935, /*8*/22.7379, /*9*/21.6081, /*10*/20.5414, /*11*/19.5341, /*12*/18.5824, /*13*/17.683, /*14*/16.8327, /*15*/16.0645, /*16*/15.3014, /*17*/14.5788, /*18*/13.8942, /*19*/13.2456, /*20*/12.6308, /*21*/12.0479, /*22*/11.495, /*23*/10.9704, /*24*/10.4726, /*25*/10, /*26*/9.5512, /*27*/9.1249, /*28*/8.7199, /*29*/8.3349, /*30*/7.9689, /*31*/7.6209, /*32*/7.2898, /*33*/6.9748, /*34*/6.675, /*35*/6.3896, /*36*/6.1178, /*37*/5.8589, /*38*/5.6123, /*39*/5.3772, /*40*/5.1483, /*41*/4.935, /*42*/5.7316, /*43*/4.5377, /*44*/4.3527, /*45*/4.1762, /*46*/4.0077, /*47*/3.8469, /*48*/3.6934, /*49*/3.5468, /*50*/3.4067, /*51*/3.2729, /*52*/3.145, /*53*/3.0228, /*54*/2.9059, /*55*/2.7942, /*56*/2.6873, /*57*/2.585, /*58*/2.4871, /*59*/2.3935, /*60*/2.3038, /*61*/2.2179, /*62*/2.1357, /*63*/2.0569, /*64*/1.9814, /*65*/1.909, /*66*/1.8397, /*67*/1.7732, /*68*/1.7094, /*69*/1.6483, /*70*/1.5896, /*71*/1.5333, /*72*/1.4793, /*73*/1.4275, /*74*/1.3777, /*75*/1.326, /*76*/1.2804, /*77*/1.2366, /*78*/1.1946, /*79*/1.1543, /*80*/1.1155, /*81*/1.0783, /*82*/1.0425, /*83*/1.0081, /*84*/0.9751, /*85*/0.9433, /*86*/0.9127, /*87*/0.8833, /*88*/0.855, /*89*/0.8278, /*90*/0.8024, /*91*/0.7772, /*92*/0.7529, /*93*/0.7296, /*94*/0.707, /*95*/0.6853, /*96*/0.6643, /*97*/0.6441, /*98*/0.6246, /*99*/0.6057, /*100*/0.5876, /*101*/0.57, /*102*/0.5531, /*103*/0.5367, /*104*/0.5209, /*105*/0.5056, /*106*/0.4909, /*107*/0.4766, /*108*/0.4629, /*109*/0.4496, /*110*/0.4367, /*111*/0.4242, /*112*/0.4122, /*113*/0.4006, /*114*/0.3893, /*115*/0.3778, /*116*/0.3673, /*117*/0.3571, /*118*/0.3473, /*119*/0.3378, /*120*/0.3286, /*121*/0.3197, /*122*/0.3111, /*123*/0.3028, /*124*/0.2947, /*125*/0.2869, /*126*/0.2793, /*127*/0.272, /*128*/0.2649, /*129*/0.258, /*130*/0.2513, ]
            }
        ];

        if (temperatureSensors.findIndex(x=>x.id==cfgData.cellTempType) < 0) {
            throw "Unsupport cell temp sensor type: " + cfgData.cellTempType;
        }

        if (temperatureSensors.findIndex(x=>x.id==cfgData.chgSckTempType) < 0) {
            throw "Unsupport charge stack temp sensor type: " + cfgData.chgSckTempType;
        }

        var resToVolMapper = (res) => Math.round(4096.0 * res / (10 + res));
        for (var i = 0; i < temperatureSensors.length; i++) {
            temperatureSensors[i]['vols'] = temperatureSensors[i].resistors.map(resToVolMapper);
        }


        var chgSckAdcChannel = ['FCH_T1', 'FCH_T2', 'SCH_T1', 'SCH_T2', 'SCH_T3'];
        var chgSckTempChannel = ['DC_POSITIVE', 'DC_NEGATIVE', 'AC_AL', 'AC_BN', 'AC_C'];

        var chgSckTempEnable = {};
        var chgSckAdcToTempChannleMap = {};
        for (var tmp in cfgData.chgSckTempMap) {
            var k = chgSckTempChannel.indexOf(tmp);
            if (k < 0) {
                continue;
            }
            var v = cfgData.chgSckTempMap[tmp];

            if (v == 'OFF' || v == undefined) {
                chgSckTempEnable[tmp] = 'STD_OFF';
                continue;
            }

            if (chgSckAdcChannel.indexOf(v) < 0) {
                throw "Unsupport sample channel:" +  v
            }

            if (chgSckAdcToTempChannleMap.hasOwnProperty(v)) {
                throw v + "used by muti temperature channel"

            }

            chgSckAdcToTempChannleMap[v] = tmp;
            chgSckTempEnable[tmp] = 'STD_ON';
        }

        var mdata = {
            cellTempType: cfgData.cellTempType,
            chgSckTempType: cfgData.chgSckTempType,
            chgSckTempEnable: chgSckTempEnable,
            chgSckAdcToTempChannleMap: chgSckAdcToTempChannleMap,
            temperatureSensors: temperatureSensors,
        };

        //console.log(JSON.stringify(mdata,  null, "  "));
        info.render("NTC_Lcfg.c", "NTC_Lcfg.c.tmpl", mdata);
        info.render("NTC_Lcfg.h", "NTC_Lcfg.h.tmpl", mdata);

        return {}
    }
}
