#include "route.h"

u16 SystemParameter[SystemParaNum];//ϵͳ����

u16 RouteStationNum[RouteNum]; 		//ÿ��·������վ����
u16 NowRouteInfor[StationNum][StaiionInfor];   //��ǰ·����Ϣ
u16 StationInfor[StaiionInfor]; 
u16 HmiRouteNumFlag = 0; //��һ��·���ţ��ж�·���仯��
u16 HmiRouteCopiedNumTemp=0;//������·������ʱ����


u16 ProcessStepNum[ProcessNum]; 		   //ÿ�����̰�������
u16 NowProcessInfor[StepNum][StepInfor];  //��ǰ������Ϣ
//u16 NowStepInfor[StepInfor];              //����������Ϣ

u16 HmiProcessNumFlag = 0; //��һ�����̺ţ��ж����̱仯��
u16 HmiProcessCopiedNumTemp=0;//���������̺���ʱ����


//
//---------------ϵͳ------------//
//
//��ϵͳ��ȡһ������ num:����λ��
u16 GetOneParameterFromSystem(u16 num)
{
	u16 buf[1];
	//��ȡϵͳ����	
	W25QXX_Read_16(buf,num,1);
	return buf[0];
}

//��ȡ����ϵͳ������������Ĭ������SystemParameter
void GetAllParameterFromSystem(void)
{
	//��ȡϵͳ����	
	W25QXX_Read_16(SystemParameter,0,SystemParaNum);
}

//����һ��������ϵͳ value:����ֵ num:�������
void SetOneParameterToSystem(u16 value,u16 num)
{
	u16 buf[1];
	buf[0] = value;
	W25QXX_Write_16(buf,num,1);
}

//�������в�����ϵͳ ��SystemParameter��������ݴ���Flash
void SetAllParameterToSystem()
{
	W25QXX_Write_16(SystemParameter,0,SystemParaNum);
}



//
//---------------·��------------//
//
//��ȡ·������վ������ num:·����
u16 GetRouteStationNum(u16 num)
{
	//��ȡ
	W25QXX_Read_16(&RouteStationNum[num-1],RouteStationNumAdress(num),1);	
	
	//������Χ����
	if(RouteStationNum[num-1]>StationNum)
		RouteStationNum[num-1] = 0;
	return RouteStationNum[num-1];
}

//��������·��������վ����
void SetAllStationNum()
{
	W25QXX_Write_16(RouteStationNum,RouteStationNumAdress(1),RouteNum);
}
//��ȡ·����Ϣ,num:·����
void GetRouteData(u16 num)
{
	u16 i;
	//��ȡվ������
	HmiStationNum = GetRouteStationNum(num);		//���µ���������
	
	//��ȡվ����Ϣ
	for(i=0;i<RouteStationNum[num-1];i++)
	{
		W25QXX_Read_16(&NowRouteInfor[i][0],NowStationInforAdress(num,i),StaiionInfor);
	}
	

}

//����·������վ����
void SetRouteStationNum(void)
{
	//����·������վ����
	RouteStationNum[HmiRouteNum-1] = HmiStationNum;
	//����
	W25QXX_Write_16(&RouteStationNum[HmiRouteNum-1],RouteStationNumAdress(HmiRouteNum),1);	
}

//����վ����Ϣ,num:·����,nվ���
void SetStationData(u16 num,u16 n)
{
	//����վ�������Ϣ
	W25QXX_Write_16(&NowRouteInfor[n-1][0],NowStationInforAdress(num,n-1),StaiionInfor);	
}

//����·����Ϣ,num:·����
void SetRouteData(u16 num)
{
	u16 i;
	//����·������վ����
	RouteStationNum[HmiRouteNum-1] = HmiStationNum;
	//����
	W25QXX_Write_16(&RouteStationNum[HmiRouteNum-1],RouteStationNumAdress(HmiRouteNum),1);	

	
	//����·��վ�������Ϣ
	for(i=0;i<RouteStationNum[num-1];i++)
	{
		W25QXX_Write_16(&NowRouteInfor[i][0],NowStationInforAdress(num,i),StaiionInfor);	
//		osdelay_ms(10);
	}
}


//����·����ǰվ�㶯�������ݵ�����
void UpdataStationToHmi()
{
		HmiRoadData0  = NowRouteInfor[HmiStationSerialNum-1][0];
		HmiRoadData1  = NowRouteInfor[HmiStationSerialNum-1][1];
		HmiRoadData2  = NowRouteInfor[HmiStationSerialNum-1][2];
		HmiRoadData3  = NowRouteInfor[HmiStationSerialNum-1][3];
		HmiRoadData4  = NowRouteInfor[HmiStationSerialNum-1][4];
		HmiRoadData5  = NowRouteInfor[HmiStationSerialNum-1][5];
		HmiRoadData6  = NowRouteInfor[HmiStationSerialNum-1][6];
		HmiRoadData7  = NowRouteInfor[HmiStationSerialNum-1][7];
		HmiRoadData8  = NowRouteInfor[HmiStationSerialNum-1][8];
		HmiRoadData9  = NowRouteInfor[HmiStationSerialNum-1][9];
		HmiRoadData10 = NowRouteInfor[HmiStationSerialNum-1][10];
		HmiRoadData11 = NowRouteInfor[HmiStationSerialNum-1][11];
		HmiRoadData12 = NowRouteInfor[HmiStationSerialNum-1][12];
		HmiRoadData13 = NowRouteInfor[HmiStationSerialNum-1][13];
		HmiRoadData14 = NowRouteInfor[HmiStationSerialNum-1][14];
		HmiRoadData15 = NowRouteInfor[HmiStationSerialNum-1][15];
		HmiRoadData16 = NowRouteInfor[HmiStationSerialNum-1][16];
		HmiRoadData17 = NowRouteInfor[HmiStationSerialNum-1][17];
		HmiRoadData18 = NowRouteInfor[HmiStationSerialNum-1][18];
		HmiRoadData19 = NowRouteInfor[HmiStationSerialNum-1][19];
		
}

//�ӽ������·��վ�����ݵ���ǰ
void UpdataStationToNow()
{
		NowRouteInfor[HmiStationSerialNum-1][0]  = HmiRoadData0 ;
		NowRouteInfor[HmiStationSerialNum-1][1]  = HmiRoadData1 ;
		NowRouteInfor[HmiStationSerialNum-1][2]  = HmiRoadData2 ;
		NowRouteInfor[HmiStationSerialNum-1][3]  = HmiRoadData3 ;
		NowRouteInfor[HmiStationSerialNum-1][4]  = HmiRoadData4 ;
		NowRouteInfor[HmiStationSerialNum-1][5]  = HmiRoadData5 ;
		NowRouteInfor[HmiStationSerialNum-1][6]  = HmiRoadData6 ;
		NowRouteInfor[HmiStationSerialNum-1][7]  = HmiRoadData7 ;
		NowRouteInfor[HmiStationSerialNum-1][8]  = HmiRoadData8 ;
		NowRouteInfor[HmiStationSerialNum-1][9]  = HmiRoadData9 ;
		NowRouteInfor[HmiStationSerialNum-1][10] = HmiRoadData10;
		NowRouteInfor[HmiStationSerialNum-1][11] = HmiRoadData11;
		NowRouteInfor[HmiStationSerialNum-1][12] = HmiRoadData12;
		NowRouteInfor[HmiStationSerialNum-1][13] = HmiRoadData13;
		NowRouteInfor[HmiStationSerialNum-1][14] = HmiRoadData14;
		NowRouteInfor[HmiStationSerialNum-1][15] = HmiRoadData15;
		NowRouteInfor[HmiStationSerialNum-1][16] = HmiRoadData16;
		NowRouteInfor[HmiStationSerialNum-1][17] = HmiRoadData17;
		NowRouteInfor[HmiStationSerialNum-1][18] = HmiRoadData18;
		NowRouteInfor[HmiStationSerialNum-1][19] = HmiRoadData19;
}




//��������·��վ�����ݵ�����
void ClearStationToHmi()
{
	//Ĭ��վ����Ϣ
	HmiRoadData0  = 0;	//
	HmiRoadData1  = 0;	//
	HmiRoadData2  = 0;	//
	HmiRoadData3  = 0;	//	
	HmiRoadData4  = 0;	//  	       
	HmiRoadData5  = 666;	//�ر�Ĭ��
	HmiRoadData6  = 0;	//
	HmiRoadData7  = 0;	//
	HmiRoadData8  = 0;	//	//¯��
	HmiRoadData9  = 0;	//
	HmiRoadData10 = 0;	//	//δʹ��
	HmiRoadData11 = 1;  //
	HmiRoadData12 = 1;  //	//δʹ��
	HmiRoadData13 = 1;  //
	HmiRoadData14 = 1;  //	//δʹ��
	HmiRoadData15 = 0;  //0:��ͨ�ر� 1:�ֲ�ر�
	HmiRoadData16 = 0;  //����ر겻����
	HmiRoadData17 = 1;  //����1��
	HmiRoadData18 = 1;  //�ر����� 1:ʵ��ر� 0:����ر�
	HmiRoadData19 = 0;  //���̺��� Ĭ��Ϊ0 0������1����2����

}




//
//---------------����------------//
//
//���漰�����������ݣ���ǰ������������Ϣ��
void	SetProcessData()
{
	u16 i = 0;
	//���̲���
	
	ProcessStepNum[HmiProcessNum-1] = HmiStepNum;
	//����
	W25QXX_Write_16(&ProcessStepNum[HmiProcessNum-1],ProcessStepNumAdress(HmiProcessNum),1);	

	
	//������Ϣ
	//���µ�ǰ��������ʼ·����
	NowProcessInfor[HmiProcessSerialNum-1][0] = HmiProcessStartNum;
	//���µ�ǰ�����Ķ���
	NowProcessInfor[HmiProcessSerialNum-1][1] = HmiProcessAction;
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	NowProcessInfor[HmiProcessSerialNum-1][2] = HmiProcessStopNum;
	
	//����
	for(i=0;i<HmiStepNum;i++)
		W25QXX_Write_16(&NowProcessInfor[i][0],NowStepInforAdress(HmiProcessNum,i),StepInfor);	
}


//��ȡ���̰�������
u16	GetProcessStepNum(u16 num)
{
	//��ȡ
	W25QXX_Read_16(&ProcessStepNum[num-1],ProcessStepNumAdress(num),1);	
	
	return ProcessStepNum[num-1];
}

//�����������̰�������
void SetAllStepNum()
{
	W25QXX_Write_16(ProcessStepNum,ProcessStepNumAdress(1),ProcessNum);
}

//��ȡ�������������ݣ���ǰ������������Ϣ��
void	GetProcessData()
{
	u16 i = 0;
	//���̲���
	
	//��ȡ
	W25QXX_Read_16(&ProcessStepNum[HmiProcessNum-1],ProcessStepNumAdress(HmiProcessNum),1);	
	//����
	HmiStepNum = ProcessStepNum[HmiProcessNum-1];
	
	HmiProcessSerialNum = 1;
	
	//���̲��������趨���ֵ��ֱ����0
	if(HmiStepNum>StepNum)
	{
		//����������Ϊ0
		HmiStepNum = 0;
		//�������Ϊ0
		HmiProcessSerialNum = 0;
	}
	
	//������Ϣ
	//��ȡ
	for(i=0;i<HmiStepNum;i++)
		W25QXX_Read_16(&NowProcessInfor[i][0],NowStepInforAdress(HmiProcessNum,i),StepInfor);	
	
	//���µ�ǰ��������ʼ·����
	HmiProcessStartNum = 	NowProcessInfor[HmiProcessSerialNum-1][0];
	//���µ�ǰ�����Ķ���
	HmiProcessAction = 	NowProcessInfor[HmiProcessSerialNum-1][1];
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	HmiProcessStopNum = 	NowProcessInfor[HmiProcessSerialNum-1][2];
}


//��ȡ������ָ���������ݣ�ָ����������Ϣ��
void	GetProcessDataFrom(u16 num)
{
	u16 i = 0;
	//���̲���
	
	//��ȡ
	W25QXX_Read_16(&ProcessStepNum[num-1],ProcessStepNumAdress(num),1);	
	//����
	HmiStepNum = ProcessStepNum[num-1];
	
	HmiProcessSerialNum = 1;
	
	//���̲��������趨���ֵ��ֱ����0
	if(HmiStepNum>StepNum)
	{
		//����������Ϊ0
		HmiStepNum = 0;
		//�������Ϊ0
		HmiProcessSerialNum = 0;
	}
	
	//������Ϣ
	//��ȡ
	for(i=0;i<HmiStepNum;i++)
		W25QXX_Read_16(&NowProcessInfor[i][0],NowStepInforAdress(num,i),StepInfor);	
	
	//���µ�ǰ��������ʼ·����
	HmiProcessStartNum = 	NowProcessInfor[HmiProcessSerialNum-1][0];
	//���µ�ǰ�����Ķ���
	HmiProcessAction = 	NowProcessInfor[HmiProcessSerialNum-1][1];
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	HmiProcessStopNum = 	NowProcessInfor[HmiProcessSerialNum-1][2];
}



//�����������ݵ�����
void UpdataProcessToHmi()
{
	//���µ�ǰ��������ʼ·����
	HmiProcessStartNum = 	NowProcessInfor[HmiProcessSerialNum-1][0];
	//���µ�ǰ�����Ķ���
	HmiProcessAction = 	NowProcessInfor[HmiProcessSerialNum-1][1];
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	HmiProcessStopNum = 	NowProcessInfor[HmiProcessSerialNum-1][2];
}

//�����������ݵ�����
void ClearStepToHmi()
{
	//���µ�ǰ��������ʼ·����
	HmiProcessStartNum = 1;
	//���µ�ǰ�����Ķ���
	HmiProcessAction = 0;
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	HmiProcessStopNum = 0;
}

//���뼰�����������ݣ���ǰ������������Ϣ��
void	InsertProcessData()
{
	u16 i,j;
	//���̲���
	
	ProcessStepNum[HmiProcessNum-1] = HmiStepNum;
	//����
	W25QXX_Write_16(&ProcessStepNum[HmiProcessNum-1],ProcessStepNumAdress(HmiProcessNum),1);	

	
	//������Ϣ
	//����ǰ����������������ƶ�һλ
	for(i=HmiStepNum;i>=HmiProcessSerialNum;i--)
	{
		for(j=0;j<StepInfor;j++)
		{
			NowProcessInfor[i-1][j] = NowProcessInfor[i-1-1][j];
		}
	}
	
	//�����²�������
	
	//���µ�ǰ��������ʼ·����
	NowProcessInfor[HmiProcessSerialNum-1][0] = HmiProcessStartNum;
	//���µ�ǰ�����Ķ���
	NowProcessInfor[HmiProcessSerialNum-1][1] = HmiProcessAction;
	//���µ�ǰ�����Ľ���·���Ż��ظ�ִ�д���
	NowProcessInfor[HmiProcessSerialNum-1][2] = HmiProcessStopNum;

		
	//����
	for(i=0;i<HmiStepNum;i++)
		W25QXX_Write_16(&NowProcessInfor[i][0],NowStepInforAdress(HmiProcessNum,i),StepInfor);	
}


//ɾ���������������ݣ���ǰ������������Ϣ��
void	DeleteProcessData()
{
	u16 i,j;
	//���̲���
	if(HmiStepNum>0)
	{
		//��������������ǰ�ƶ�һλ
		if((HmiStepNum>0)&&(HmiStepNum!=HmiProcessSerialNum))
		{
			for(i=HmiProcessSerialNum;i<HmiStepNum;i++)
			{
				for(j=0;j<StepInfor;j++)
				{
					NowProcessInfor[i-1][j] = NowProcessInfor[i][j];
				}
			}
		}
		
		HmiStepNum--;
		if(HmiStepNum==0)
			HmiProcessSerialNum = 0;
		else
			if((HmiStepNum+1)==HmiProcessSerialNum)
				HmiProcessSerialNum--;
		
		//����
		for(i=0;i<HmiStepNum;i++)
			W25QXX_Write_16(&NowProcessInfor[i][0],NowStepInforAdress(HmiProcessNum,i),StepInfor);	
		
		ProcessStepNum[HmiProcessNum-1] = HmiStepNum;
		//����
		W25QXX_Write_16(&ProcessStepNum[HmiProcessNum-1],ProcessStepNumAdress(HmiProcessNum),1);	

		//���½���
		UpdataProcessToHmi();	
	}
}







//�Զ�������ʾ��Ϣ����
void HmiAutoReload()
{
	//�������ִ�����̣�������·����
	if(HmiTask == 2)					//��Ļ����: ������0 ִ��·��1 ִ������2
	{
		//��ȡ������Ϣ
		GetProcessData();
		HmiRouteNum = NowProcessInfor[0][0];
	}
	//����վ����
	//��ȡ·������վ����
	HmiStationNum = GetRouteStationNum(HmiRouteNum);
	//��ǰ�ر�����
	HmiRfidNum = 0;
	
	//��ȡ��ǰ·����Ϣ
	GetRouteData(HmiRouteNum);	
	//������һ�ر�
	HmiNextRfidNum = NowRouteInfor[0][5];
}













