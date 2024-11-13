#ifndef __SPI_TZ_REG_REG_H__
#define __SPI_TZ_REG_REG_H__

//2019.5 test: only enable spi_clk_control for you debug, 
// please control clk just in kernel for any MP product!!!!!!!!!!!!!!!!!
//#define SPI_CLK_CONTROL
/* spi register define -------------------------------------
 * please choose the spi port that you want to use.
 */

/* spi  base address (physical address) */
#define SPI0_BASE_ADDR		(0x1100A000)
#define SPI1_BASE_ADDR		(0x11010000)
#define SPI2_BASE_ADDR		(0x11012000)
#define SPI3_BASE_ADDR		(0x11013000)
#define SPI4_BASE_ADDR		(0x11014000)
#define SPI5_BASE_ADDR		(0x11015000)

#define SPI_REG_PA_BASE		SPI5_BASE_ADDR
#define SPI_PORT			(5)

/*secure dirver map region (via MapPhys, driver will own virtual address)  */
extern unsigned char  *SpiBaseVA;
extern unsigned char  *SpiTxDmaVA;
extern unsigned char  *SpiRxDmaVA;

#define SPI_REG_VA_BASE      (SpiBaseVA)
#define SPI_TX_DMA_VA_BASE   (SpiTxDmaVA) 
#define SPI_RX_DMA_VA_BASE   (SpiRxDmaVA) 

/* spi register offset define */
#define SPI_REG_CFG0         (SPI_REG_VA_BASE + 0x00)
#define SPI_REG_CFG1         (SPI_REG_VA_BASE + 0x04)
#define SPI_REG_TX_SRC       (SPI_REG_VA_BASE + 0x08)
#define SPI_REG_RX_DST       (SPI_REG_VA_BASE + 0x0C)
#define SPI_REG_TX_DATA      (SPI_REG_VA_BASE + 0x10)
#define SPI_REG_RX_DATA      (SPI_REG_VA_BASE + 0x14)
#define SPI_REG_CMD          (SPI_REG_VA_BASE + 0x18)
#define SPI_REG_STATUS0      (SPI_REG_VA_BASE + 0x1C)
#define SPI_REG_STATUS1      (SPI_REG_VA_BASE + 0x20)
#define SPI_REG_PAD_SEL      (SPI_REG_VA_BASE + 0x24)
#define SPI_REG_CFG2         (SPI_REG_VA_BASE + 0x28)
#define SPI_REG_TX_SRC_64    (SPI_REG_VA_BASE + 0x2C)
#define SPI_REG_RX_DST_64    (SPI_REG_VA_BASE + 0x30)

/* spi register bit define */
#define SPI_CFG0_SCK_HIGH_OFFSET          0
#define SPI_CFG0_SCK_LOW_OFFSET           16
#define SPI_CFG0_CS_HOLD_OFFSET           0
#define SPI_CFG0_CS_SETUP_OFFSET          16

#define SPI_CFG0_SCK_HIGH_MASK            0xffff
#define SPI_CFG0_SCK_LOW_MASK             0xffff0000
#define SPI_CFG0_CS_HOLD_MASK             0xffff
#define SPI_CFG0_CS_SETUP_MASK            0xffff0000

#define SPI_CFG1_CS_IDLE_OFFSET           0
#define SPI_CFG1_PACKET_LOOP_OFFSET       8
#define SPI_CFG1_PACKET_LENGTH_OFFSET     16
#define SPI_CFG1_GET_TICK_DLY_OFFSET      29

#define SPI_CFG1_CS_IDLE_MASK             0xff
#define SPI_CFG1_PACKET_LOOP_MASK         0xff00
#define SPI_CFG1_PACKET_LENGTH_MASK       0x3fff0000
#define SPI_CFG1_GET_TICK_DLY_MASK        0xe0000000

#define SPI_CMD_ACT_OFFSET                0
#define SPI_CMD_RESUME_OFFSET             1
#define SPI_CMD_RST_OFFSET                2
#define SPI_CMD_PAUSE_EN_OFFSET           4
#define SPI_CMD_DEASSERT_OFFSET           5
#define SPI_CMD_SAMPLE_SEL_OFFSET         6
#define SPI_CMD_CS_POL_OFFSET             7
#define SPI_CMD_CPHA_OFFSET               8
#define SPI_CMD_CPOL_OFFSET               9 
#define SPI_CMD_RX_DMA_OFFSET             10
#define SPI_CMD_TX_DMA_OFFSET             11
#define SPI_CMD_TXMSBF_OFFSET             12
#define SPI_CMD_RXMSBF_OFFSET             13
#define SPI_CMD_RX_ENDIAN_OFFSET          14
#define SPI_CMD_TX_ENDIAN_OFFSET          15
#define SPI_CMD_FINISH_IE_OFFSET          16
#define SPI_CMD_PAUSE_IE_OFFSET           17

#define SPI_CMD_ACT_MASK                  0x1
#define SPI_CMD_RESUME_MASK               0x2
#define SPI_CMD_RST_MASK                  0x4
#define SPI_CMD_PAUSE_EN_MASK             0x10
#define SPI_CMD_DEASSERT_MASK             0x20
#define SPI_CMD_CPHA_MASK                 0x100
#define SPI_CMD_CPOL_MASK                 0x200
#define SPI_CMD_RX_DMA_MASK               0x400
#define SPI_CMD_TX_DMA_MASK               0x800
#define SPI_CMD_TXMSBF_MASK               0x1000
#define SPI_CMD_RXMSBF_MASK               0x2000
#define SPI_CMD_RX_ENDIAN_MASK            0x4000
#define SPI_CMD_TX_ENDIAN_MASK            0x8000
#define SPI_CMD_FINISH_IE_MASK            0x10000
#define SPI_CMD_PAUSE_IE_MASK             0x20000

#define SPI_ULTRA_HIGH_EN_OFFSET          0
#define SPI_ULTRA_HIGH_THRESH_OFFSET      16

#define SPI_ULTRA_HIGH_EN_MASK            0x1
#define SPI_ULTRA_HIGH_THRESH_MASK        0xffff0000

#define MTK_SPI_32BIS_MASK  (0xFFFFFFFF)
#define MTK_SPI_32BIS_SHIFT (32)

/* spi clock control define ----------------------------------------
 * please don't control spi clock in tee evnirment,
 * we hardly suggest that control the clock via kernel,
 * the API has bee provided by kernel spi
 */

#define SPI_PDN_PA_BASE       (0x10001000)

/*secure dirver map region (via MapPhys, driver will own virtual address)  */
extern unsigned char  *SpiPdnBaseVA;

#define SPI_PDN_VA_BASE		(SpiPdnBaseVA) 

#define SPI0_PDN_SET		(SPI_PDN_VA_BASE + 0x88) 
#define SPI0_PDN_CLR		(SPI_PDN_VA_BASE + 0x8c)
#define SPI0_PDN_STA		(SPI_PDN_VA_BASE + 0x94)

#define SPI12345_PDN_SET	(SPI_PDN_VA_BASE + 0xA4) 
#define SPI12345_PDN_CLR	(SPI_PDN_VA_BASE + 0xA8)
#define SPI12345_PDN_STA	(SPI_PDN_VA_BASE + 0xAC)

#define SPI0_PDN_OFFSET		1
#define SPI1_PDN_OFFSET		6
#define SPI2_PDN_OFFSET		9
#define SPI3_PDN_OFFSET		10
#define SPI4_PDN_OFFSET		25
#define SPI5_PDN_OFFSET		26

/* READ/WRITE  register  API ----------------------------------------
 * please don't hard code set any register in tee ,
 * it may lead to issue difficult to debug
 */
#define SPI_REG_GET32(addr)        (*(volatile unsigned int*)(addr))
#define SPI_REG_SET32(addr, val)   (*(volatile unsigned int*)(addr) = val)

#define SPI_READ(addr)             SPI_REG_GET32(addr)
#define SPI_WRITE(addr, val)       SPI_REG_SET32(addr, val) 


#endif // __SPI_TZ_REG_REG_H__

