/* MTK copyright
  * luhua Xu   2018.9.22
  *luhua.xu@mediatek.com
  */
#include "spi.h"


int spiTest()
{
	SpiInit();
	int testData = 0x12345678;
	int recvData = 0;
	int len = 4;
	int i = 0;
	int ret = 0;

	ret = SpiSend(&testData, &recvData, len, 0, 1);
	if (ret)
		dprintf(INFO, "SpiSend failed,ret:%d", ret);

	for (i = 0; i < len/4; i++)
	{
		if (recvData != testData) {
			dprintf(INFO,"SpiRecv Check fail,rx:0x%x while tx:0x%x", recvData, testData);
			ret++;
		}
	}
	if (ret == 0)
		dprintf(INFO, "SpiSend succeed,ret:%d", ret);
	
}



