#include "dianweiqi.h"

u16 g_AD_Value[ADC_BUFF_LEN][3];
u16 g_After_filter[3];		//0 - 4096
double g_ADC_dianweiqi[2];		//电位器电压值 0 - 3.3V

dianweiqi g_DWQ;


void ADC1_Configuration(void)   
{   
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	//先初始化ADC1通道
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;//PA5 通道5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化  

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE); //ADC1 复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE); //复位结束
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 3;
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_480Cycles);     
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_480Cycles); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_480Cycles);
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
}
		
void  DMA2_Stream0_IRQHandler(void)		//DMA--DAC //ADC1的前三个通道,现在用的是PC2 3 
{
	u32  sum = 0;
	u8 count=0;
	u8 i=0;
#if SYSTEM_SUPPORT_OS 	
	OSIntEnter();    
#endif
	
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0) != RESET)
	{

	   for(i=0;i<3;i++)		//用的通道1和2
	   {
			for ( count=0;count<ADC_BUFF_LEN;count++)
			{
				sum += g_AD_Value[count][i];
			}
			g_After_filter[i]=sum/ADC_BUFF_LEN / 11.37;		//把4096分成360度
			//g_ADC_dianweiqi[i-1] = g_After_filter[i]*3.3/4096.0;	
			sum=0;
	   }
		
	  DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
	}
	
#if SYSTEM_SUPPORT_OS 	
	OSIntExit();  											 
#endif	
}

void DMA_adc1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;  
    /* DMA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
      
    DMA_Cmd(DMA2_Stream0, DISABLE);
    DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Addr;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_AD_Value;		//数据buff
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
    DMA_InitStructure.DMA_BufferSize = ADC_BUFF_LEN*3;							//数据buff长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; 
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;	
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);	
    DMA_Cmd(DMA2_Stream0, ENABLE);   
}












