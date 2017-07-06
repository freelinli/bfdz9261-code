#ifndef COMMON_H
#define COMMON_H

//定义通讯用常量
#define  CMD_HEAD0             0xa5
#define  CMD_HEAD1             0x5a
#define  CMD_END               0xbf

//通信协议里面的指令
#define  CMD_READ_CAIJIQI      0x03
#define  CMD_READ_HUGANQI      0x02
#define  CMD_CONTROL_RELAY     0x05
#define  CMD_ADJUSTTIME        0x07     //对网关校时
#define  CMD_RELOADPARAM       0x08
#define  CMD_DEL_CURRDATA      0x09
#define  CMD_DEL_PARAM         0x0A
#define  CMD_READ_VER          0x0B
#define  CMD_SET_VER           0x0C     //设置版本号
#define  CMD_SET_MENXIAN       0x0D     //设置参数
#define  CMD_GET_MENXIAN       0x0E     //得到大棚门限

#define  CMD_SET_DPINFO        0x0F     //设置采集器信息
#define  CMD_GET_DPINFO        0x10     //得到采集器信息
#define  CMD_DEL_DPINFO        0x13     //删除采集器信息

#define  CMD_SET_DPCONTROL     0x11     //设置控制器类信息  继电器及互感器
#define  CMD_GET_DPCONTROL     0x12     //得到控制器类信息
#define  CMD_GET_DPCONTROLV3   0x22     //获取设置的设备信息 20170212添加
#define  CMD_DEL_DPCONTROL     0x14     //删除控制器类型
#define  CMD_SET_DEVINFO       0x19     //设置设备信息20160511
#define  CMD_SET_DPCONTROLV3   0x20     //设置控制器类信息  继电器及互感器 采用xml格式
#define  CMD_DEL_DPCONTROLV3   0x21     //删除控制器信息,采用xml格式

#define  CMD_RELAODPARAM       0x15
#define  CMD_QUIT              0x16     //退出软件

#define  CMD_SET_IPANDPORT     0x17     //设置IP及端口
#define  CMD_GET_IPANDPORT     0x18     //得到ip及端口
#define  CMD_CLEAR_ALLDEV      0x23     //清除所有的设备
#define  CMD_GET_MENXIANV3     0x24     //获取指定采集器编号的门限，通过xml格式
#define  CMD_GET_SQLITE_DATA     0x25     //获取指定采集器编号的门限，通过xml格式



#define PARAMCODE_START 0xee        //开
#define PARAMCODE_CLOSE 0x11        //关
#define PARAMCODE_PAUSE  0x0e       //停
#define PARAMCODE_DIANCIFA_JIETONG 0x22   //电磁阀类型接通
#define PARAMCODE_DIANCIFA_DUANKAI 0x33   //电磁阀类型断开


//错误代码
#define ERROR_SUCCESS              0

#define ERROR_NOT_XML              1000     //非xml格式
#define ERROR_NOT_FOUND_NODE       1001     //没有找到对应的xml节点
#define ERROR_LOADXML              1002     //加载配置文件出错
#define ERROR_CTRLID_TOOBIG        1003     //控制器编号不在范围内
#define ERROR_DEVCOUNT_TOOBIG      1004     //配置的设备数据太多，需删除后重新配置
#define ERROR_SAVE_CONFIG          1005     //保存配置文件失败
#define ERROR_NO_MESSAGE           1006     //没有消息体
#define ERROR_NOT_CTRLID           1007     //无控制器编号 获取控制参数时，下发的指令中没有控制器编号或者控制器编号不在范围内
#define ERROR_NOT_FOUND_CTRLID     1008     //获取控制器参数时，在配置文件中没有找到对应的控制器编号。

#define ERROR_DEL_NOT_FOUND_CTRLID 0xfe     //删除设备参数时，没有找到可删除的CTRLID
#define ERROR_MESSAGE_TOOLONG      1009     //消息体太长
#define ERROR_MESSAGE_LENGTH       1010     //消息体长度不对




#endif // #ifndef COMMON_H
