/**********************************************************************
include
***********************************************************************/
#include "VarMalloc.h"
#include "DMG80480C043_01WTC.h"

//---------------------------------------------------------------
//变量定义
TEMP_UNI            temp_uni;
PT_SYSTEM_STR       pSYSTEM;
//---------------------------------------------------------------
/****************************************************************
 * @fun     VarMalloc      
 * @brief   
 * @note    
 * @param   无
 * @return  无
 ***************************************************************/
signed char VarMalloc(void)
{
    uint32_t MemUse;
    uint32_t l;
    
    /*********************************************************************/
    //系统
    l=sizeof(SYSTEM_STR);
    pSYSTEM = (PT_SYSTEM_STR)malloc(l);
    if(pSYSTEM == NULL)return 4;
    memset(pSYSTEM,0,l);
    MemUse += l;
    
    /*********************************************************************/
    //串口屏
    if(SCREENVarMalloc())
    {
        return 2;
    }
    
    #ifdef PC_PRINTF
    printf("\r\nmalloc is ok,New Card inf size is %d.",MemUse);
    #endif
    
    return 0;
}

/*************************************************************************
 * @fun     EndianSwap      
 * @brief   大端小端转换
 * @note    
 * @param   无
 * @return  无
 *************************************************************************/
void EndianSwap(uint8_t *ptv)
{
    uint8_t temp;
    temp = ptv[0];
    ptv[0] = ptv[3];
    ptv[3] = temp;
    temp = ptv[1];
    ptv[1] = ptv[2];
    ptv[2] = temp;
}


/*************************************************************************
 * @fun     SelectMinKey     
 * @brief   在数组arr中，共有n个数组，从第s个数据开始查询至最后一个，得到最小值的索引值
 * @note    
 * @param   无
 * @return  最小值的索引值
 *************************************************************************/
uint8_t SelectMinKey(uint8_t arr[],uint8_t num,uint8_t s)
{
    uint8_t t;
    uint8_t k = s;
    for(t=s+1; t<num; t++)
    {
        if(arr[k] > arr[t])k = t;
    }
    return k;
}

/*************************************************************************
 * @fun     SelectSort     
 * @brief   排序
 * @note    
 * @param   无
 * @return  无
 *************************************************************************/
void SelectSort(uint8_t arr[],uint8_t num)
{
    uint8_t key;
    uint8_t tmp;
    uint8_t t;
    for(t=0; t<num; t++)
    {
        key = SelectMinKey(arr,num,t);
        if(key != t)
        {
            tmp = arr[t];
            arr[t] = arr[key];
            arr[key] = tmp;
        }
    }
}

/**************************************************************************
 * @fun     ArrCompare
 * @brief   两组数组内容进行比较
 * @note    
 * @param   
 * @return  0相同,1不相同
 *************************************************************************/
uint8_t ArrCompare(uint8_t *pbuf1,uint8_t *pbuf2,uint8_t len)
{
    uint8_t t;
    for(t=0;t<len;t++)if(pbuf1[t]!=pbuf2[t])return 1;
    return 0;
}


/*************************************************************************
 * @fun     IEC_FLOAT_TO_ASCII
 * @brief   将IEC浮点数转换成ASCII码
 * @note    f>0且小数点最大有2位
 * @param   buf:存放结果的缓冲区,buf[8]:符号位置,buf[0]:最低位
			f:IEC浮点数
 * @return  无
 ************************************************************************/
void IEC_FLOAT_TO_ASCII(uint8_t *buf, float f)
{	    
	uint8_t t;
	uint32_t n;
    
	if(f<0) return;
    
    //float转ASCII
	n=f*100;
	for(t=6;t>0;t--)
	{
        if(t==4)
        {
            buf[t-1]='.';
            t --;
        }
		buf[t-1]=n%10+0x30;
		n=n/10;
	}
    
    //清除无效零
    for(t=0;t<2;t++)
    {
        if(buf[t]==0x30)buf[t]=0x20;
        else break;
    }
}

/*************************************************************************
 * @fun     IEC_FLOAT_4_TO_ASCII
 * @brief   将IEC浮点数转换成ASCII码
 * @note    f>0且小数点最大有4位
 * @param   buf:存放结果的缓冲区,buf[8]:符号位置,buf[0]:最低位
			f:IEC浮点数
 * @return  无
 ************************************************************************/
void IEC_FLOAT_4_TO_ASCII(uint8_t *buf, float f)
{	    
	uint8_t t;
	uint32_t n;
    
	if(f<0) return;
    
    //float转ASCII
	n=f*10000;
	for(t=6;t>0;t--)
	{
        if(t==2)
        {
            buf[t-1]='.';
            t --;
        }
		buf[t-1]=n%10+0x30;
		n=n/10;
	}
}

