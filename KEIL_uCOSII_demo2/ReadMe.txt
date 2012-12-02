uCOS-II移植目标：搭建针对板级的系统支持，使用同一块学习板(本人使用FL2440)编写不同应用程序时，只需复制Demo即可，Demo中不包含任何无用信息，仅包含最基本的支持以避免
文件说明：
1.CommenFlies里面是和应用程序无关代码，每一个新的应用程序可以含有相同的CommonFlies(FL2440+uCOSIIV2.90)
2.error:Symbol *** multiply defined(by uCOS_II.o and ***)
 改正方式：将uCOS_II.c从工程中移除
3.未使用os_dbg_r.c，禁止DeBug。
4.在OS_core.c中添加extern void OSStartHighRdy(void);和extern void OSIntCtxSw(void);
2010/8/15
阿呆
阿呆的游乐园 河北大学
于 保定飞凌嵌入式

使用MDK J-Link调试，需要将Debug>Option>Utilities中的Update Target before Debuggingq取消
（使用RAM内部调试，思想参看DebugInRam.ini中的存储器安排）
2010/8/16

uCOS-II测试代码已运行，初步判定移植完成，在RAM中测试通过，下载到Nor中测试通过，未下载到NAND中测试，有时间再测试，现在还不是很清楚如何使用J-Link向NAND中烧写代码。（程序默认从NAND中搬运64K代码到SRAM中，如代码大小超过64K，可手动修改启动代码NAND搬运地方相应参数）
2010//8/20


目录安排：
1.CommonFlies存放以FL2440为目标板，uCOS-II为系统的基本代码，用户编写应用程序基本不用修改
2.Driver存放驱动程序代码，一般驱动在App_cfg.h中添加Enable/Disable宏,在App.h中添加基于上述宏的包含代码(以LED_test.h为例，参考程序代码)
3.App_Main用户主程序代码,存放功能性代码

目录安排纯属阿呆嗜好，您老可以根据自己胃口修改之^_^