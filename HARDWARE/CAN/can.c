#include "can.h" 

u8 g_Get_RFID_buff[16];

u8 g_cinavi1_RXbuff[16];  //前磁导航
u8 g_cinavi2_RXbuff[16];	
u8 g_cinavi3_RXbuff[16];  
u8 g_cinavi4_RXbuff[16];	//后磁导航
u16 g_u16_val[5];

u16 g_AGV_RFID_ID;//地标  //
u8  g_flag_RFID_beep;






//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode) //PA11 PA12
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12
	
	  //引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//关闭--禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
#if CAN1_RX0_INT_ENABLE
	
	 CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
} 

u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode) //PB5  PB6
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN2_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB5,PB6
	
	  //引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); //GPIOB5复用为CAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2); //GPIOB6复用为CAN2
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//关闭--禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//由队列顺序决定--优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //过滤器14
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器14
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
#if CAN2_RX0_INT_ENABLE
	
	 CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}
 

int g_Ct_chugui[5];
u16 g_CT_bit[5];


#if CAN1_RX0_INT_ENABLE	//使能RX0中断   
//中断服务函数			    
void CAN1_RX0_IRQHandler(void)			//磁导航采集
{
	CanRxMsg RxMessage;	

	int i=0;

 
#if SYSTEM_SUPPORT_OS 	
	OSIntEnter();    
#endif
	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	if(RxMessage.StdId == 22)		//遥控器ID:22
	{
		for(i=0;i<RxMessage.DLC;i++)
		{
//			if(RxMessage.Data[i] > 100)
//			{
//				g_AGV_yaokong.g_SD_buff[i] = RxMessage.Data[i] ; 
//			}
//			else
//			{
				g_AGV_yaokong.g_SD_buff[i] = RxMessage.Data[i] ; 
//			}
			
		}	
		YaoKong_control(&g_AGV_yaokong);			//手持摇杆遥控器  //10ms一次
	}
	else if(RxMessage.StdId == 16)					//地标传感器ID: 16
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_Get_RFID_buff[i] = RxMessage.Data[i]; 
		g_AGV_RFID_ID = g_Get_RFID_buff[1]<<8|g_Get_RFID_buff[0];
		g_AGV_Sta.shiti_RFID = g_Get_RFID_buff[1]<<8|g_Get_RFID_buff[0];
		g_AGV_Sta.juedui_RFID = g_AGV_Sta.shiti_RFID; 
		g_shoudong_screen_RFID = g_AGV_Sta.juedui_RFID;
		
		g_AGV_Sta.RFID_buff[0] = g_AGV_Sta.RFID_buff[1] ;
		g_AGV_Sta.RFID_buff[1] = g_AGV_Sta.shiti_RFID;
		
		g_flag_RFID_beep = 1;	
	}  	
	else if(RxMessage.StdId == 1)					//前1磁导航
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_cinavi1_RXbuff[i]=RxMessage.Data[i];
		g_u16_val[1] = u8_to_u16(g_cinavi1_RXbuff+1,g_cinavi1_RXbuff+2);
		g_CT_bit[1] = bit16_huan(g_u16_val[1]);
		fencha_1to2_qian(g_AGV_Sta.Car_fencha_dir,&g_AGV_Sta.Car_fencha_dir_qian);
		g_CDH8_qian_1 = FindSpace(g_CT_bit[1],15,g_AGV_Sta.Car_fencha_dir_qian); 
		if(g_CDH8_qian_1.Distance)	//非0存			//-4  --  0  --  4
		{
			g_Ct_chugui[1] = g_CDH8_qian_1.Distance;	
		} 										
		BEEP = 0;													
	}   
	else if(RxMessage.StdId == 2)				//前2磁导航
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_cinavi2_RXbuff[i]=RxMessage.Data[i]; 
		g_u16_val[2] = u8_to_u16(g_cinavi2_RXbuff+1,g_cinavi2_RXbuff+2);
		fencha_1to2_qian(g_AGV_Sta.Car_fencha_dir,&g_AGV_Sta.Car_fencha_dir_qian);
		g_CDH8_qian_2 = FindSpace(g_u16_val[2],15,g_AGV_Sta.Car_fencha_dir_qian);
		if(g_CDH8_qian_2.Distance)	//非0存
		{
			g_Ct_chugui[2] = g_CDH8_qian_2.Distance;
		}
	}
	else if(RxMessage.StdId == 3)				//后1磁导航
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_cinavi3_RXbuff[i]=RxMessage.Data[i]; 
		g_u16_val[3] = u8_to_u16(g_cinavi3_RXbuff+1,g_cinavi3_RXbuff+2);
		g_CT_bit[3] = bit16_huan(g_u16_val[3]);
		fencha_1to2_hou(g_AGV_Sta.Car_fencha_dir,&g_AGV_Sta.Car_fencha_dir_hou);
		g_CDH8_hou_1 = FindSpace(g_CT_bit[3],15,g_AGV_Sta.Car_fencha_dir_hou);
		if(g_CDH8_hou_1.Distance)	//非0存***
		{
			g_Ct_chugui[3] = g_CDH8_hou_1.Distance;
		}//00111100
	}
	else if(RxMessage.StdId == 4)				//后2磁导航
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_cinavi4_RXbuff[i]=RxMessage.Data[i]; 
		g_u16_val[4] = u8_to_u16(g_cinavi4_RXbuff+1,g_cinavi4_RXbuff+2);
		fencha_1to2_hou(g_AGV_Sta.Car_fencha_dir,&g_AGV_Sta.Car_fencha_dir_hou);
		g_CDH8_hou_2 = FindSpace(g_u16_val[4],15,g_AGV_Sta.Car_fencha_dir_hou);
		if(g_CDH8_hou_2.Distance)	//非0存***
		{
			g_Ct_chugui[4] = g_CDH8_hou_2.Distance;
		}
	}	
	
#if SYSTEM_SUPPORT_OS 	
	OSIntExit();  											 
#endif	
	
}
#endif


u8 g_succese_Motec_init[5];
u16 g_CANOPEN_RX_buff[5][9];
CanRxMsg CANOPEN_RxMessage;


#if CAN2_RX0_INT_ENABLE	//使能RX0中断    
//中断服务函数			    
void CAN2_RX0_IRQHandler(void)  					//CAN2电机驱动
{
  	CanRxMsg RxMessage;
	int i=0;
	
#if SYSTEM_SUPPORT_OS 	
	OSIntEnter();    
#endif	
	
	
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
//	
	if(RxMessage.StdId == 0x701)			//
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_CANOPEN_RX_buff[1][i]=RxMessage.Data[i]; 
		
		if(g_CANOPEN_RX_buff[1][0] == 0x05)
		{
			//g_succese_Motec_init[1] = 1;
			g_motec_init.zijian_enable = g_motec_init.init_delay_val; 
		}

		g_motec_init.zijian_shangdian = g_motec_init.init_delay_val;
		

		
	}  	
	if(RxMessage.StdId == 0x702)			//
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_CANOPEN_RX_buff[2][i]=RxMessage.Data[i]; 
		
		if(g_CANOPEN_RX_buff[2][0] == 0x05)
		{
			//g_succese_Motec_init[2] = 1;
			g_motec_init.zijian_enable = g_motec_init.init_delay_val;
		}

			g_motec_init.zijian_shangdian = g_motec_init.init_delay_val;
		

	}  
	if(RxMessage.StdId == 0x703)			//
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_CANOPEN_RX_buff[3][i]=RxMessage.Data[i]; 
		
		if(g_CANOPEN_RX_buff[3][0] == 0x05)
		{
			//g_succese_Motec_init[3] = 1;
			g_motec_init.zijian_enable = g_motec_init.init_delay_val;
		}

			g_motec_init.zijian_shangdian = g_motec_init.init_delay_val;
		
		
		
		
	}  
	if(RxMessage.StdId == 0x704)			//
	{
		for(i=0;i<RxMessage.DLC;i++)
		g_CANOPEN_RX_buff[4][i]=RxMessage.Data[i]; 
		

		if(g_CANOPEN_RX_buff[4][0] == 0x05)
		{
			//g_succese_Motec_init[4] = 1;
			g_motec_init.zijian_enable = g_motec_init.init_delay_val;
		}
//		else if(g_CANOPEN_RX_buff[4][0] == 0x7F)
//		{
			g_motec_init.zijian_shangdian = g_motec_init.init_delay_val;
//		}
		
		
		
	}


#if SYSTEM_SUPPORT_OS 	
	OSIntExit();  											 
#endif  	
	
}
#endif
	





//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u16 ID,char* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=ID;	 // 标准标识符为0
  TxMessage.ExtId=ID;	 // 设置扩展标示符（29位）
  TxMessage.IDE=0;		  // 使用扩展标识符
  TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;							 // 发送两帧信息
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // 第一帧信息          
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Ok)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;
  return 0;		

}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
//u8 CAN1_Receive_Msg(void)
//{		   		   
// 	u32 i;
//	CanRxMsg RxMessage;
//    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
//	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
//	
//	if(RxMessage.StdId == 5)
//	{
//		for(i=0;i<RxMessage.DLC;i++)
//		g_cinavi5_RXbuff[i]=RxMessage.Data[i];
//		
//	}   
//	if(RxMessage.StdId == 6)
//	{
//		for(i=0;i<RxMessage.DLC;i++)
//		g_cinavi6_RXbuff[i]=RxMessage.Data[i]; 
//	}
//	
//	return RxMessage.DLC;	
//}

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN2_Send_Msg(u16 ID,char* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=ID;	 // 标准标识符为0
  TxMessage.ExtId=ID;	 // 设置扩展标示符（29位）
  TxMessage.IDE=0;		  // 使用扩展标识符
  TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;							 // 发送两帧信息
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // 第一帧信息          
  mbox= CAN_Transmit(CAN2, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN2, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;
  return 0;		


}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
	

	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	//LED4 = !LED4;
	return RxMessage.DLC;	
}

u16 bit16_huan(u16 temp_bit)   
{
	u8 bit_num = 16;						//16位的数转换
	u8 temp_i=0;
	u16 temp_out=0;
	
	for(temp_i=0;temp_i<bit_num;temp_i++)
	{
		temp_out |= ((temp_bit>>temp_i)&0x01)<<((bit_num-1)-temp_i);
	}
	return temp_out;
}




