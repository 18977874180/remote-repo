/* MTK copyright
  * luhua Xu   2018.9.22
  *luhua.xu@mediatek.com
  */

#include "spi_reg.h"
int a = 123;
// git add 123
#include "spi_reg.h"  12345 
#include "spi.h"


#define SPI_DEBUG
#define SPI_TRANSFER_POLLING

//TODO: add your printf API here
#define SPI_ERR(fmt, args...)  do { dprintf(INFO,fmt,##args); } while(0)
#ifdef SPI_DEBUG
#define SPI_DBG(fmt, args...)  do { dprintf(INFO,fmt,##args); } while(0)
#else
#define SPI_DBG(fmt, args...)
#endif


//TODO: please check you SPI pin mode in DWS, if it is 'SPIx_A_MISO', set  SPI_PAD=0, if it is 'SPIx_B_MISO'
// set SPI_PAD=1, if it is just 'SPIx_MISO',  set SPI_PAD=0.
#define  SPI_PAD  0	

//TODO: add your local os define  for a  semaphore;
//static sem_t wait_semaphore;

//if youre DMA addr is reserved, these para record the PH addr and size
uint32_t spiTXDmaPHAddr, spiRXDmaPHAddr;

/*base address, mapped form physical address */
unsigned char  *SpiBaseVA = NULL;
unsigned char  *SpiTxDmaVA = NULL;
unsigned char  *SpiRxDmaVA = NULL;
unsigned char  *SpiPdnBaseVA = NULL;

/* default chip config */
static struct mt_chip_conf mt_chip_conf_def = {
	.setuptime = 10,
	.holdtime = 10,
	.high_time = 20,
	.low_time = 20,
	.cs_idletime = 20,
	.ulthgh_thrsh = 0,
	.cpol = 0,
	.cpha = 1,
	.rx_mlsb = 1, 
	.tx_mlsb = 1,
	.tx_endian = 0,
	.rx_endian = 0,
	.com_mod = DMA_TRANSFER,
	.pause = 1,
	.finish_intr = 1,
	.deassert = 0,
	.ulthigh = 0,
	.tckdly = 0
};

/* save the chip config as global, just for debug */
struct mt_chip_conf *g_chip_config;

/* save the latest xfer */
static struct spi_transfer *g_xfer = NULL;	

/* this flag to sync every data transfer */
static int g_irq_flag = IRQ_IDLE;

/*  spi status */
static int g_spi_running = 0;


/*whether is last xfer, check to clear pause bit*/
unsigned char is_last_xfer = 1;

/*brief   get the information of current chip configuration
 * return 	mt_chip_config
 */
struct mt_chip_conf* GetChipConfig(void)
{
    return g_chip_config;
}

/*brief	config the transfer parameter
 * param 	ptr : HW config setting from User
 * return 	none
 */
void SaveChipConfig(struct mt_chip_conf *ptr)
{
	if (!ptr)
		return;
	g_chip_config = ptr;
}

/*brief   get spi transfer struct
 * return 	none
 */
struct spi_transfer* GetSpiTransfer(void)
{
    return g_xfer;
}

/*brief	save the current pointer of transfer struct
 * param 	ptr : data structure from User
 * return 	none
 */
void SaveSpiTransfer(struct spi_transfer* ptr)
{
	if(NULL == ptr)
		SPI_ERR("spi_transfer is NULL !!!\n");
	else
		g_xfer = ptr;
}

/*brief	save the current irq flag
 * return 	none
 */
int GetIrqFlag(void)
{
    return g_irq_flag;
}

/*brief	save the current irq flag
 * return 	none
 */
void SaveIrqFlag(enum spi_irq_flag flag)
{
    g_irq_flag = flag;
}

/*brief   get current status of spi controller
 * return 	g_spi_running
 */
int GetPauseStatus(void)
{
    return g_spi_running;
}

/*brief   set current status of spi controller
 * param 	 now status
 * return 	g_spi_running
 */
void SavePauseStatus(int status)
{
    g_spi_running = status;
}

/*brief   out the current information about chip configuration
  * @return 	none
 */
void DumpChipConfig(struct mt_chip_conf *chip_config)
{
	if (chip_config == NULL) {
		SPI_DBG("dump chip_config is NULL !!\n");
		return;
	}

	SPI_DBG("setuptime=%d\n",chip_config->setuptime);
	SPI_DBG("holdtime=%d\n",chip_config->holdtime);
	SPI_DBG("high_time=%d\n",chip_config->high_time);
	SPI_DBG("low_time=%d\n",chip_config->low_time);
	SPI_DBG("cs_idletime=%d\n",chip_config->cs_idletime);
	SPI_DBG("ulthgh_thrsh=%d\n",chip_config->ulthgh_thrsh);
	SPI_DBG("cpol=%d\n",chip_config->cpol);
	SPI_DBG("cpha=%d\n",chip_config->cpha);
	SPI_DBG("tx_mlsb=%d\n",chip_config->tx_mlsb);
	SPI_DBG("rx_mlsb=%d\n",chip_config->rx_mlsb);
	SPI_DBG("tx_endian=%d\n",chip_config->tx_endian);
	SPI_DBG("rx_endian=%d\n",chip_config->rx_endian);
	SPI_DBG("com_mod=%d\n",chip_config->com_mod);
	SPI_DBG("pause=%d\n",chip_config->pause);
	SPI_DBG("finish_intr=%d\n",chip_config->finish_intr);
	SPI_DBG("deassert=%d\n",chip_config->deassert);
	SPI_DBG("ulthigh=%d\n",chip_config->ulthigh);
	SPI_DBG("tckdly=%d\n",chip_config->tckdly);
}

/*brief   out the information of spi register
  * @return 	none
 */
void DumpReg(void)
{
	SPI_DBG("SPI_REG_CFG0=0x%08x\n",SPI_READ(SPI_REG_CFG0));
	SPI_DBG("SPI_REG_CFG1=0x%08x\n",SPI_READ(SPI_REG_CFG1));
	SPI_DBG("SPI_REG_TX_SRC=0x%08x\n",SPI_READ(SPI_REG_TX_SRC));
	SPI_DBG("SPI_REG_RX_DST=0x%08x\n",SPI_READ(SPI_REG_RX_DST));
	SPI_DBG("SPI_REG_CMD=0x%08x\n",SPI_READ(SPI_REG_CMD));
	SPI_DBG("SPI_REG_STATUS0=0x%08x\n",SPI_READ(SPI_REG_STATUS0));
	SPI_DBG("SPI_REG_STATUS1=0x%08x\n",SPI_READ(SPI_REG_STATUS1));
	SPI_DBG("SPI_REG_PAD_SEL=0x%08x\n",SPI_READ(SPI_REG_PAD_SEL));
	SPI_DBG("SPI_REG_CFG2=0x%08x\n",SPI_READ(SPI_REG_CFG2));
}

/*brief   calculate the current bus speed, the source clock is 109.2 M for mt6765
 * @return 	none
 */
int GetSpiSpeed(struct mt_chip_conf *chip_config)
{
	int speed;
	if (chip_config == NULL) {
		SPI_DBG("dump chip_config is NULL !!\n");
		return ERR_NULL_PARAMETER;
	}

	speed = 109200000/((chip_config->high_time+1)+(chip_config->low_time+1));

	return speed;
}

#ifdef SPI_CLK_CONTROL
/* brief	enable the spi clock
 * return 	none
 */
void SpiEnableClk(void)
{
	int value = 0;

	SPI_DBG("before enable spi clk: PDN sta=0x%08x\n", SPI_READ(SPI12345_PDN_STA));
	value = 1<<SPI5_PDN_OFFSET;
	SPI_WRITE(SPI12345_PDN_CLR, value);

	SPI_DBG("after enable spi clk: PDN sta=0x%08x\n", SPI_READ(SPI12345_PDN_STA));
}

/* brief	disable the spi clock
 * return 	none
 */
void SpiDisableClk(void)
{
	int value = 0;

	SPI_DBG("before dis spi clk: PDN sta=0x%08x\n", SPI_READ(SPI12345_PDN_STA));
	value = 1<<SPI5_PDN_OFFSET;
	SPI_WRITE(SPI12345_PDN_SET, value);

	SPI_DBG("after dis spi clk: PDN sta=0x%08x\n", SPI_READ(SPI12345_PDN_STA));
}
#endif

/* brief	virtual DMA address mapping
 * return 	none
 */
static void SpiDMAMapping(uint32_t len)
{
	//TODO: if for your OS, DMA addr is runtime alloced. use this fun to get DMA physical addr and virtual addr.
 
	//spiTXDmaPHAddr ---->SpiTxDmaVA, len
	//spiRXDmaPHAddr ---->SpiRxDmaVA, len
	//ret = vm_create_mem(spiRXDmaPHAddr, SpiTxDmaVA, len);
}

/* brief	delete virtual DMA mapping space
 * return 	none
 */
static void SpiDMAUnMapping(void)
{
}

/**
 * @brief	get interrupt status
 * @param 	current interrupt status
 * @return 	none
 */
static unsigned int IsInterruptEnable(void)
{
    unsigned int cmd;
    cmd = SPI_READ(SPI_REG_CMD);
    return (cmd >> SPI_CMD_FINISH_IE_OFFSET) & 1;
}

/**
 * @brief	set interrupt status
 * @param 	none
 * @return 	none
 */
static void ClearPauseBit(void)
{
    unsigned int reg_val;

    reg_val = SPI_READ(SPI_REG_CMD);
    reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
    SPI_WRITE(SPI_REG_CMD, reg_val);
}

/**
 * @brief	enable pause mode
 * @param 	none
 * @return 	none
 */
static void SetPauseBit(void)
{
    unsigned int reg_val;

    reg_val = SPI_READ(SPI_REG_CMD);
    reg_val |= 1 << SPI_CMD_PAUSE_EN_OFFSET;
    SPI_WRITE(SPI_REG_CMD, reg_val);
    return;
}

/**
 * @brief	clear resume bit
 * @param 	none
 * @return 	none
 */
static void ClearResumeBit(void)
{
    unsigned int reg_val;

    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~SPI_CMD_RESUME_MASK;
    SPI_WRITE ( SPI_REG_CMD, reg_val );

    return;
}

/**
 * @brief	used to define per data length and loop count
 * @param 	ptr : data structure from User
 * @return 	none
 */
static void SpiSetupPacket(struct spi_transfer *ptr)
{
    unsigned int  packet_size, packet_loop, cfg1;

	/*set transfer packet and loop*/
	if (ptr->len < PACKET_SIZE) {
		packet_size = ptr->len;
	} else {
		packet_size = PACKET_SIZE;
	}

	if (ptr->len % packet_size) {
		SPI_ERR("The lens are not a multiple of %d, your len %u",PACKET_SIZE, ptr->len);
	}

	packet_loop = (ptr->len) / packet_size;

    cfg1 = SPI_READ(SPI_REG_CFG1);
    cfg1 &= ~(SPI_CFG1_PACKET_LENGTH_MASK + SPI_CFG1_PACKET_LOOP_MASK);
    cfg1 |= (packet_size - 1)<<SPI_CFG1_PACKET_LENGTH_OFFSET;
    cfg1 |= (packet_loop - 1)<<SPI_CFG1_PACKET_LOOP_OFFSET;
    SPI_WRITE ( SPI_REG_CFG1, cfg1 );
}

/**
 * @brief	disable dma transfer
 * @param 	none
 * @return 	none
 */
static void SpiDisableDma(void)
{
    unsigned int  cmd;

    cmd = SPI_READ( SPI_REG_CMD);
    cmd &= ~SPI_CMD_TX_DMA_MASK;
    cmd &= ~SPI_CMD_RX_DMA_MASK;
    SPI_WRITE(SPI_REG_CMD, cmd);

    return ;
}

/**
 * @brief	enable dma transfer
 * @param 	none
 * @return 	none
 */
#define SPI_4B_ALIGN 0x4
static void SpiEnableDma(struct spi_transfer *xfer,unsigned int mode)
{
	unsigned int  cmd;
	unsigned int  temp_tx_dma;
	unsigned int  temp_rx_dma;

	//TODO: if for your os, DMA is runtime mapped,call SpiDMAMapping
	SpiDMAMapping(xfer->len);

	cmd = SPI_READ(SPI_REG_CMD);

	xfer->tx_dma = spiTXDmaPHAddr;
	if((xfer->tx_buf != NULL)|| ((xfer->tx_dma != INVALID_DMA_ADDRESS) && (xfer->tx_dma != 0))){
		if(xfer->tx_dma & (SPI_4B_ALIGN - 1))
			SPI_ERR("Warning!Tx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",xfer->tx_buf,xfer->tx_dma);
			
		SPI_WRITE(SPI_REG_TX_SRC,(xfer->tx_dma));
		cmd |= 1 << SPI_CMD_TX_DMA_OFFSET;
	}

	xfer->rx_dma = spiRXDmaPHAddr;
	if((xfer->rx_buf != NULL)|| ((xfer->rx_dma != INVALID_DMA_ADDRESS) && (xfer->rx_dma != 0))){
		if(xfer->rx_dma & (SPI_4B_ALIGN - 1))
			SPI_ERR("Warning!Rx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",xfer->rx_buf,xfer->rx_dma);

		SPI_WRITE(SPI_REG_RX_DST,(xfer->rx_dma));
		cmd |= 1 << SPI_CMD_RX_DMA_OFFSET;
	}
	
	SPI_WRITE(SPI_REG_CMD, cmd);

	SPI_DBG("DMA PA addr in reg, tx: 0x%08x 0x%08x rx: 0x%08x 0x%08x cmd reg: 0x%08x\n",
		SPI_READ(SPI_REG_TX_SRC_64), SPI_READ(SPI_REG_TX_SRC),
		SPI_READ(SPI_REG_RX_DST_64), SPI_READ(SPI_REG_RX_DST),
		SPI_READ(SPI_REG_CMD));

    return;
}

/**
 * @brief	start to transmit data to slave device
 * @param 	none
 * @return 	none
 */
static void SpiStartTransfer(void)
{
    unsigned int reg_val;
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val |= 1 << SPI_CMD_ACT_OFFSET;

    SPI_WRITE( SPI_REG_CMD, reg_val);

    return;
}

/**
 * @brief	 spi resume 
 * @return 
 */
static void SpiResumeTransfer(void)
{
    unsigned int reg_val;

    reg_val = SPI_READ(SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RESUME_MASK;
    reg_val |= 1 << SPI_CMD_RESUME_OFFSET;
    SPI_WRITE( SPI_REG_CMD, reg_val );

    return;
}

/**
 * @brief	 sw reset SPI. 
 * @return 	0  set configuration success
 */
static void ResetSpi(void)
{
    unsigned int reg_val;

    /*set the software reset bit in SPI_REG_CMD.*/
    reg_val=SPI_READ(SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RST_MASK;
    reg_val |= 1 << SPI_CMD_RST_OFFSET;
    SPI_WRITE(SPI_REG_CMD, reg_val);

    reg_val = SPI_READ(SPI_REG_CMD);
    reg_val &= ~SPI_CMD_RST_MASK;
    SPI_WRITE(SPI_REG_CMD, reg_val);

    return;
}

/**
 * @brief	Write chip configuration to HW register
 * @param 	chip_config	  parameter used when do a transmition
 * @return 	0  set configuration success
 */
static int SpiSetup(struct mt_chip_conf *chip_config)
{
	unsigned int reg_val;
	if(chip_config == NULL) {
		SPI_ERR("SpiSetup chip_config is NULL !!");
		SaveIrqFlag(IRQ_IDLE);
		return ERR_NULL_PARAMETER;
	}

	SPI_DBG("SpiSetup now frst RW spi reg.\n");
	reg_val = SPI_READ( SPI_REG_CFG0 );
	reg_val &= ~ ( SPI_CFG0_CS_HOLD_MASK |SPI_CFG0_CS_SETUP_MASK );
	reg_val |= ( (chip_config->holdtime-1) << SPI_CFG0_CS_HOLD_OFFSET );
	reg_val |= ( (chip_config->setuptime-1) << SPI_CFG0_CS_SETUP_OFFSET );
	SPI_WRITE( SPI_REG_CFG0, reg_val );

    reg_val = SPI_READ ( SPI_REG_CFG1 );
    reg_val &= ~(SPI_CFG1_CS_IDLE_MASK);
    reg_val |= ( (chip_config->cs_idletime-1 ) << SPI_CFG1_CS_IDLE_OFFSET );
    reg_val &= ~(SPI_CFG1_GET_TICK_DLY_MASK);
    reg_val |= ( ( chip_config->tckdly ) << SPI_CFG1_GET_TICK_DLY_OFFSET );
    SPI_WRITE( SPI_REG_CFG1, reg_val );

	reg_val = SPI_READ ( SPI_REG_CFG1 );
	reg_val &= ~(0x7<<26);
	SPI_WRITE( SPI_REG_CFG1, reg_val );

	reg_val = SPI_READ ( SPI_REG_CFG2 );
	reg_val &= ~ ( SPI_CFG0_SCK_HIGH_MASK |SPI_CFG0_SCK_LOW_MASK );
	reg_val |= ( (chip_config->high_time-1) << SPI_CFG0_SCK_HIGH_OFFSET );
	reg_val |= ( (chip_config->low_time-1) << SPI_CFG0_SCK_LOW_OFFSET );
	SPI_WRITE ( SPI_REG_CFG2, reg_val );

    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~ ( SPI_CMD_TX_ENDIAN_MASK | SPI_CMD_RX_ENDIAN_MASK );
    reg_val &= ~ ( SPI_CMD_TXMSBF_MASK| SPI_CMD_RXMSBF_MASK );
    reg_val &= ~ ( SPI_CMD_CPHA_MASK | SPI_CMD_CPOL_MASK );
    reg_val |= ( chip_config->tx_mlsb << SPI_CMD_TXMSBF_OFFSET );
    reg_val |= ( chip_config->rx_mlsb << SPI_CMD_RXMSBF_OFFSET );
    reg_val |= (chip_config->tx_endian << SPI_CMD_TX_ENDIAN_OFFSET );
    reg_val |= (chip_config->rx_endian << SPI_CMD_RX_ENDIAN_OFFSET );
	// reg_val |= (chip_config->sample_sel << SPI_CMD_SAMPLE_SEL_OFFSET );
	// reg_val |= (chip_config->cs_pol << SPI_CMD_CS_POL_OFFSET );
    reg_val |= (chip_config->cpha << SPI_CMD_CPHA_OFFSET );
    reg_val |= (chip_config->cpol << SPI_CMD_CPOL_OFFSET );
    SPI_WRITE(SPI_REG_CMD, reg_val);

#ifdef SPI_TRANSFER_POLLING
	/* disable pause IE in polling mode */
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
    reg_val &= ~ SPI_CMD_PAUSE_IE_MASK;
    reg_val |= ( chip_config->pause << SPI_CMD_PAUSE_EN_OFFSET );
    SPI_WRITE( SPI_REG_CMD, reg_val );
#else
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
    reg_val &= ~SPI_CMD_PAUSE_IE_MASK;
    reg_val |= ( chip_config->pause << SPI_CMD_PAUSE_EN_OFFSET );
    reg_val |= ( chip_config->pause << SPI_CMD_PAUSE_IE_OFFSET );
    SPI_WRITE( SPI_REG_CMD, reg_val );
#endif

#ifdef SPI_TRANSFER_POLLING
	/* disable finish IE in polling mode */
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~ SPI_CMD_FINISH_IE_MASK;
    SPI_WRITE ( SPI_REG_CMD, reg_val );
#else
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~ SPI_CMD_FINISH_IE_MASK;
    reg_val |= ( 1 << SPI_CMD_FINISH_IE_OFFSET );
    SPI_WRITE ( SPI_REG_CMD, reg_val );
#endif

    /*set the communication of mode*/
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~ SPI_CMD_TX_DMA_MASK;
    reg_val &= ~ SPI_CMD_RX_DMA_MASK;
    SPI_WRITE ( SPI_REG_CMD, reg_val );

    /*set deassert mode*/
    reg_val = SPI_READ ( SPI_REG_CMD );
    reg_val &= ~SPI_CMD_DEASSERT_MASK;
    reg_val |= ( chip_config->deassert << SPI_CMD_DEASSERT_OFFSET );
    SPI_WRITE ( SPI_REG_CMD, reg_val );

    SPI_WRITE ( SPI_REG_PAD_SEL, SPI_PAD );

    return 0;
}

/**
 * @brief  handle SPI interrupt
 * @return 	none
 */
static int SpiHandleIRQ(void)
{
	int ret = 0;
	struct spi_transfer *xfer;
	struct mt_chip_conf *chip_config;

	unsigned int reg_val, cnt;
	unsigned int i;

	xfer = GetSpiTransfer();
	chip_config = GetChipConfig();

	if (NULL == chip_config) {
		SPI_ERR("chip_config is NULL!!\n");
		ret = ERR_NULL_PARAMETER;
		return ret;
	}

	/*pause mode*/
	if (chip_config->pause) {
		if ( GetPauseStatus() == INPROGRESS)
			SavePauseStatus(PAUSED);
		else {
			SPI_ERR("Wrong spi pause status.\n" );
			ret = ERR_INVALID_STATUS;
			return ret;
		}
	} else
		SavePauseStatus(IDLE);

    if ((chip_config->com_mod == FIFO_TRANSFER) && xfer->rx_buf) {
        cnt = (xfer->len%4)?(xfer->len/4 + 1):(xfer->len/4);
        for(i = 0; i < cnt; i++) {
            reg_val = SPI_READ(SPI_REG_RX_DATA);
            *((unsigned int *)xfer->rx_buf + i) = reg_val;
			SPI_DBG("SPI_RX_DATA_REG[%d]:0x%x\n", i, reg_val);
        }
    }
    else if ((chip_config->com_mod == DMA_TRANSFER) && xfer->rx_buf) {
        cnt = (xfer->len%4)?(xfer->len/4 + 1):(xfer->len/4);
		//TODO: notice to cache invalid.
		//ache_inv_data((unsigned long)SPI_RX_DMA_VA_BASE, (unsigned long)(SPI_RX_DMA_VA_BASE + xfer->len));	/*inv cache*/
        for(i = 0; i < cnt; i++)
        {
			//reg_val =  *( ( unsigned int * ) SPI_RX_DMA_VA_BASE + i);
			reg_val = SPI_READ(SPI_RX_DMA_VA_BASE + 4*i);
			*((unsigned int *)xfer->rx_buf + i) = reg_val;
			SPI_DBG("SPI_RX_DATA_DMA[%d]:0x%x\n", i,reg_val);
        }
	}

	if (is_last_xfer == 1 && xfer->is_transfer_end == 1)
    {
		SavePauseStatus(IDLE);
#if 0
        if(chip_config->com_mod == DMA_TRANSFER) {
              SPI_MSG("SpiDMAUnMapping\n");
              SpiDMAUnMapping();
        }
#endif
		ResetSpi();
    }
    SaveIrqFlag(IRQ_IDLE);

	//TODO: remember to post the semaphore
	//sem_post(&wait_semaphore))
 
    return ret;
}

/**
 * @brief  SPI ISR
 * @return 	none
 */
static int SpiIrqHandler(void)
{
#ifdef SPI_TRANSFER_POLLING
	unsigned int irqStatus;
	do {
		irqStatus = SPI_READ(SPI_REG_STATUS1);
	} while (irqStatus == 0);

#else
    unsigned int ret_val;

    ret_val = SPI_READ(SPI_REG_STATUS0);
     if((ret_val & 0x00000003) == 0)
     {
		 ret = ERR_INVALID_IRQ;
		 SPI_ERR("error spi interrupt status, SPI_REG_STATUS0 %x\n", ret_val );
		 return ret;
	 }
#endif
    ret = SpiHandleIRQ();
	return ret;
}

/**
 * @brief  data transfer from spi user
 * @return 	none
 */
static int SpiNextXfer(struct spi_transfer *xfer)
{
    unsigned int mode, cnt, speed, i;
    int ret = 0;
    struct mt_chip_conf *chip_config = xfer->chip_config;

    if(chip_config == NULL) {
      SPI_ERR("SpiNextXfer get chip_config is NULL!");
      ret = ERR_NULL_PARAMETER;
	  goto fail;
    }

#ifndef SPI_TRANSFER_POLLING
    if((!IsInterruptEnable())){
        SPI_ERR("interrupt is disable!");
        ret = ERR_IRQ_DISABLE;
        goto fail;
    }
#endif

    mode = chip_config->com_mod;
    if((mode == FIFO_TRANSFER)){
        if(xfer->len > FIFO_MAX_LEN ){
            ret = ERR_UNSUPPORT_MODE;
            SPI_ERR("xfer len is invalid over fifo size !");
            goto fail;
        }
    }

	/*
	 * cannot 1K align & FIFO->DMA need used pause mode
       * this is to clear pause bit (CS turn to idle after data transfer done)
       */
	if (mode == DMA_TRANSFER) {
		if ((is_last_xfer == 1) && (xfer->is_transfer_end == 1))
			ClearPauseBit();
	} else if (mode == FIFO_TRANSFER) {
		if (xfer->is_transfer_end == 1)
			ClearPauseBit();
	} else {
		ret = ERR_UNSUPPORT_MODE;
		SPI_ERR("xfer mode is invalid !");
		goto fail;
	}

	SpiDisableDma();

	SpiSetupPacket(xfer);

	SPI_DBG("SpiNextXfer, reg dump before activate/resume transfer, before EnableDMA\n");
	DumpReg();

    if (mode == FIFO_TRANSFER) {
		cnt = ((xfer->len) %4)?((xfer->len)/4 + 1):((xfer->len)/4);
		for(i = 0; i < cnt; i++) {
			SPI_WRITE(SPI_REG_TX_DATA, *((unsigned int *)xfer->tx_buf + i));
            SPI_DBG("tx_buf data[%d]:0x%x, addr is:%p\n", i, *((unsigned int *)xfer->tx_buf + i),
            	(unsigned int *)xfer->tx_buf + i);
        }
    }

    if (mode == DMA_TRANSFER) {
        SpiEnableDma(xfer, mode);

		SPI_DBG("xfer tx_dma:%08x rx_dma:%08x\n", xfer->tx_dma, xfer->rx_dma);

		//TODO: if cache is enable for your os, please remember to flush cache for SPI DMA memory.
		//SPI_TX_DMA_VA_BASE and SPI_RX_DMA_VA_BASE
		
		cnt = ( xfer->len%4)?(xfer->len/4 + 1):(xfer->len/4);
		for(i = 0; i < cnt; i++) {
            *((unsigned int * ) SPI_TX_DMA_VA_BASE + i)= *((unsigned int *)xfer->tx_buf + i);
			SPI_DBG("tx_buf data[%d] is:%x", i, *((unsigned int *)xfer->tx_buf + i));
            SPI_DBG("tx_buf addr is:%p", (unsigned int *)xfer->tx_buf + i);
        }

		//TODO:remember to flush cache
		//cache_flush_data(SPI_TX_DMA_VA_BASE, (SPI_TX_DMA_VA_BASE + xfer->len));
    }
    if (GetPauseStatus() == PAUSED) {
        SavePauseStatus(INPROGRESS);
        SpiResumeTransfer();
    } else if (GetPauseStatus() == IDLE) {
        if ((chip_config->pause))
            SetPauseBit();

        SavePauseStatus(INPROGRESS);
        SpiStartTransfer();
    }else {
        ret = ERR_INVALID_STATUS;
        goto fail;
    }

	speed = GetSpiSpeed(chip_config);
	if (speed == 0) {
		SPI_ERR("speed is 0, error!");
		ret = ERR_INVALID_STATUS;
		goto fail;
	}
	
	cnt = (xfer->len * 50 * 1000)/(speed);
	//TODO: we can use a timeout semaphore to wait for spi complete. wait  cnt  ms.
	//ret = sem_timedwait (&wait_semaphore, &ts);

	if(ret) {
		SPI_ERR ("wait interrupt timeout,ret=%d", ret);
		goto fail;
	}

	
/**************************************************************************************/
#ifdef SPI_TRANSFER_POLLING
    ret = SpiIrqHandler(NULL);
#endif

    if(GetPauseStatus() == PAUSED && is_last_xfer==1){
        ClearResumeBit();
    }

    return 0;

fail:
    SavePauseStatus(IDLE);
    SaveIrqFlag(IRQ_IDLE);
	ResetSpi();
	if (mode == DMA_TRANSFER)
		SpiDMAUnMapping();
	return ret;
}

/**
 * @brief  data transfer from spi user
 * @return 	none
 */
static int SpiTransfer(struct spi_transfer *xfer)
{
	int ret = 0;
	int i = 0;
	struct mt_chip_conf *chip_config;

	/* wait intrrupt had been clear */
	while(IRQ_BUSY == GetIrqFlag()) {
		if (i >= 500) {
			SPI_ERR("Has already waited IRQFLAG for %d ms\n", i);
			SaveIrqFlag(IRQ_IDLE);
			return ERR_BUS_BUSY;
		}
		i++;
		//TODO, add your sleep fucntionm, sleep 1ms;
	}

    /*set flag to block next transfer*/
    SaveIrqFlag(IRQ_BUSY);

    DumpReg();

    if((NULL == xfer)){
      	SPI_ERR("the message is NULL.\n" );
		return ERR_NULL_PARAMETER;
	}

	if (!((xfer->tx_buf || xfer->rx_buf) && xfer->len))
	{
		SPI_ERR("missing tx %p or rx %p buf, len%d\n", xfer->tx_buf, xfer->rx_buf, xfer->len);
		return ERR_NULL_PARAMETER;
	}

	chip_config = xfer->chip_config;
	SPI_DBG("spi xfer chipconfig dump before reg setup.\n");
	DumpChipConfig(chip_config);

	ret = SpiSetup(chip_config);

	if (!ret)
		ret = SpiNextXfer(xfer);

	return ret;
}

/**
 * @brief  data transfer from spi user
 * @param 	txbuf: spi data TX buffer pointer
			rxbuf: spi data RX buffer pointer
			len: bytes number read from spi slave device or write to it
			chip_conf: MTK SPI HW configuration
			flag: if used pause mode, set this flag to clear pause mode bit
 * @return 	none
 */
int SpiSend(const void *tx_buf,
			  void*rx_buf,
			  unsigned int len,
			  struct mt_chip_conf *chip_conf,
			  unsigned int flag)
{
	struct spi_transfer spiData;
	int  ret = 0;
	unsigned int packet_loop, rest_size;

	SPI_DBG("SpiSend enter, len:%d,line:%d\n", len, __LINE__);

	spiData.tx_buf = tx_buf;
	spiData.rx_buf = rx_buf;
	spiData.len = len;

	if (chip_conf)
		spiData.chip_config = chip_conf;
	else
		spiData.chip_config = &mt_chip_conf_def;

	if(len > FIFO_MAX_LEN) {
		spiData.is_dma_used = 1;
		spiData.chip_config->com_mod = DMA_TRANSFER;
	}
	else {
		spiData.is_dma_used = 0;
		spiData.chip_config->com_mod = FIFO_TRANSFER;
	}

	spiData.is_transfer_end = flag;

	SaveChipConfig(spiData.chip_config);
	SaveSpiTransfer(&spiData);

	packet_loop = spiData.len/1024;
	rest_size = spiData.len % 1024;

#ifdef SPI_CLK_CONTROL
	SpiEnableClk();
#endif

	if (spiData.len <= 1024 || rest_size == 0) {
		SPI_DBG("SpiSend,one transfer, spiData.len:%d\n", spiData.len);
		ret = SpiTransfer(&spiData);
		goto done;
	}
	
	/* first transfer 1024*packet_loop */
	g_chip_config->pause = 1;
	is_last_xfer = 0;
	spiData.len = 1024 * packet_loop;
	SPI_DBG("SpiSend,1st of 2 transfer, spiData.len:%d\n", spiData.len);
	ret = SpiTransfer(&spiData);
	if (ret)
		goto done;

	/* then transfer reset_size byte */
	if (1 == flag){
		spiData.is_transfer_end = 0;
	} else {
		spiData.is_transfer_end = 1;
	}
	is_last_xfer = 1;
	spiData.is_transfer_end = 1;
	spiData.tx_buf = (tx_buf + 1024*packet_loop);
	spiData.rx_buf = (rx_buf + 1024*packet_loop);
	spiData.len = rest_size;
	SPI_DBG("SpiSend,2nd of 2 transfer, spiData.len:%d\n", spiData.len);
	ret = SpiTransfer(&spiData);

done:
#ifdef SPI_CLK_CONTROL
	SpiDisableClk();
#endif

	return ret;
}

/** spiMapRegister
 * detail: 
 */
int SpiMapRegister(void)
{
	int ret = 0;

	//TODO: map spi register, including:  SPI_REG_PA_BASE --> SpiBaseVA, length: 0x1000,
	//SPI_PDN_PA_BASE --->SpiPdnBaseVA. length:0x1000,  The flowing is just for build pass.
	SpiBaseVA = SPI_REG_PA_BASE;
	SpiPdnBaseVA = SPI_PDN_PA_BASE;

    return ret;
}

/** SpiDMAAddrGetAndMap
 * detail: Is it needed for your TEE os to do physical register addr mapping? 
 */
int SpiDMAAddrGetAndMap(void)
{
	int ret = 0;

	//TODO: if your DMA addr is reserved we should first get DMA physical addr, 
	spiTXDmaPHAddr = 0;
	spiRXDmaPHAddr = 0;

	//TODO: then DMA addr mapping. spiTXDmaPHAddr ---> SpiTxDmaVA,  and spiRXDmaPHAddr--->SpiRxDmaVA.  
	//for reserved DMA memory, the length is also specified.
	//the flowing is just for build pass	
	SpiTxDmaVA = spiTXDmaPHAddr;
	SpiRxDmaVA = spiRXDmaPHAddr;

    return ret;
}

/* *spiInit().
 *  detail: call thif function when tee boot,and secure mem is ready for:
 *    (1) Is your DMA addr is reserved, you need to do this, if you dma can be  created runtime,ignore this.
 *    (2) mapregister, 
 *    to get spiDmaMemAddr  and register spi interrupt.
 *    get an semaphore to do spi sync.
 */
int SpiInit()
{
	int ret;

	ret = SpiDMAAddrGetAndMap;
	if (ret) {
		SPI_ERR("spiInit,spi get and map DMA addr fail,ret:%d", ret);
		return ret;
	}

	ret = SpiMapRegister;
	if (ret) {
		SPI_ERR("spiInit,spi map reg fail,ret:%d", ret);
		return ret;
	}
	
	//TODO: register spi interrupt, SPI interrupt number, please ref to kernel mt6765.dtsi, the ISR is "SpiIrqHandler"
	//TODO: init a global semaphore , named   'wait_semaphore'.

	SetIrqFlag(IRQ_IDLE);
	ResetSpi();
	return ret;	
}


