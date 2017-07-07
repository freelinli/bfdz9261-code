#ifndef COMMON_H
#define COMMON_H

//����ͨѶ�ó���
#define  CMD_HEAD0             0xa5
#define  CMD_HEAD1             0x5a
#define  CMD_END               0xbf

//ͨ��Э�������ָ��
#define  CMD_READ_CAIJIQI      0x03
#define  CMD_READ_HUGANQI      0x02
#define  CMD_CONTROL_RELAY     0x05
#define  CMD_ADJUSTTIME        0x07     //������Уʱ
#define  CMD_RELOADPARAM       0x08
#define  CMD_DEL_CURRDATA      0x09
#define  CMD_DEL_PARAM         0x0A
#define  CMD_READ_VER          0x0B
#define  CMD_SET_VER           0x0C     //���ð汾��
#define  CMD_SET_MENXIAN       0x0D     //���ò���
#define  CMD_GET_MENXIAN       0x0E     //�õ���������

#define  CMD_SET_DPINFO        0x0F     //���òɼ�����Ϣ
#define  CMD_GET_DPINFO        0x10     //�õ��ɼ�����Ϣ
#define  CMD_DEL_DPINFO        0x13     //ɾ���ɼ�����Ϣ

#define  CMD_SET_DPCONTROL     0x11     //���ÿ���������Ϣ  �̵�����������
#define  CMD_GET_DPCONTROL     0x12     //�õ�����������Ϣ
#define  CMD_GET_DPCONTROLV3   0x22     //��ȡ���õ��豸��Ϣ 20170212���
#define  CMD_DEL_DPCONTROL     0x14     //ɾ������������
#define  CMD_SET_DEVINFO       0x19     //�����豸��Ϣ20160511
#define  CMD_SET_DPCONTROLV3   0x20     //���ÿ���������Ϣ  �̵����������� ����xml��ʽ
#define  CMD_DEL_DPCONTROLV3   0x21     //ɾ����������Ϣ,����xml��ʽ

#define  CMD_RELAODPARAM       0x15
#define  CMD_QUIT              0x16     //�˳����

#define  CMD_SET_IPANDPORT     0x17     //����IP���˿�
#define  CMD_GET_IPANDPORT     0x18     //�õ�ip���˿�
#define  CMD_CLEAR_ALLDEV      0x23     //������е��豸
#define  CMD_GET_MENXIANV3     0x24     //��ȡָ���ɼ�����ŵ����ޣ�ͨ��xml��ʽ
#define  CMD_GET_SQLITE_DATA     0x25     //��ȡָ���ɼ�����ŵ����ޣ�ͨ��xml��ʽ



#define PARAMCODE_START 0xee        //��
#define PARAMCODE_CLOSE 0x11        //��
#define PARAMCODE_PAUSE  0x0e       //ͣ
#define PARAMCODE_DIANCIFA_JIETONG 0x22   //��ŷ����ͽ�ͨ
#define PARAMCODE_DIANCIFA_DUANKAI 0x33   //��ŷ����ͶϿ�


//�������
#define ERROR_SUCCESS              0

#define ERROR_NOT_XML              1000     //��xml��ʽ
#define ERROR_NOT_FOUND_NODE       1001     //û���ҵ���Ӧ��xml�ڵ�
#define ERROR_LOADXML              1002     //���������ļ�����
#define ERROR_CTRLID_TOOBIG        1003     //��������Ų��ڷ�Χ��
#define ERROR_DEVCOUNT_TOOBIG      1004     //���õ��豸����̫�࣬��ɾ������������
#define ERROR_SAVE_CONFIG          1005     //���������ļ�ʧ��
#define ERROR_NO_MESSAGE           1006     //û����Ϣ��
#define ERROR_NOT_CTRLID           1007     //�޿�������� ��ȡ���Ʋ���ʱ���·���ָ����û�п�������Ż��߿�������Ų��ڷ�Χ��
#define ERROR_NOT_FOUND_CTRLID     1008     //��ȡ����������ʱ���������ļ���û���ҵ���Ӧ�Ŀ�������š�

#define ERROR_DEL_NOT_FOUND_CTRLID 0xfe     //ɾ���豸����ʱ��û���ҵ���ɾ����CTRLID
#define ERROR_MESSAGE_TOOLONG      1009     //��Ϣ��̫��
#define ERROR_MESSAGE_LENGTH       1010     //��Ϣ�峤�Ȳ���




#endif // #ifndef COMMON_H
