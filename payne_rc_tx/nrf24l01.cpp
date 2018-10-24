#include "iface_nrf24l01.h"
#include <SPI.h> 

#define PROTOSPI_xfer  SPI.transfer
#define usleep       delayMicroseconds

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

 // NRF24L01
#define CSN_pin   2               
#define CSN_port  PORTB
#define CSN_ddr   DDRB
#define CSN_output  NRF_CSN_ddr  |=  _BV(CSN_pin);
#define CSN_on    CSN_port |=  _BV(CSN_pin);
#define CSN_off   CSN_port &= ~_BV(CSN_pin);
#define CE_on
#define CE_off

static u8 rf_setup;

void NRF24L01_Initialize()
{
    rf_setup = 0x0F;

  // Initialize SPI
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
 CE_off
 CSN_on
}


u8 NRF24L01_WriteReg(u8 reg, u8 d)
{
    u8 res;
    CSN_off
    res = PROTOSPI_xfer(W_REGISTER | (REGISTER_MASK & reg));
    PROTOSPI_xfer(d);
    CSN_on
    return res;
}

u8 NRF24L01_WriteRegisterMulti(u8 reg, u8 d[], u8 len)
{
    u8 i, res;
    CSN_off
    res = PROTOSPI_xfer(W_REGISTER | ( REGISTER_MASK & reg));
    for (i = 0; i < len; i++)
    {
        PROTOSPI_xfer(d[i]);
    }
    CSN_on
    return res;
}

u8 NRF24L01_WritePayload(u8 *d, u8 len)
{
    u8 i, res;
    CSN_off
    res = PROTOSPI_xfer(W_TX_PAYLOAD);
    for (i = 0; i < len; i++)
    {
        PROTOSPI_xfer(d[i]);
    }
    CSN_on
    return res;
}

u8 NRF24L01_ReadReg(u8 reg)
{
    u8 d;
    CSN_off
    PROTOSPI_xfer(R_REGISTER | (REGISTER_MASK & reg));
    d = PROTOSPI_xfer(0xFF);
    CSN_on
    return d;
}

u8 NRF24L01_ReadRegisterMulti(u8 reg, u8 d[], u8 len)
{
    u8 res, i;
    CSN_off
    res = PROTOSPI_xfer(R_REGISTER | (REGISTER_MASK & reg));
    for(i = 0; i < len; i++)
    {
        d[i] = PROTOSPI_xfer(0xFF);
    }
    CSN_on
    return res;
}

u8 NRF24L01_ReadPayload(u8 *d, u8 len)
{
  u8 i, res;
    CSN_off
    res = PROTOSPI_xfer(R_RX_PAYLOAD);
    for(i = 0; i < len; i++)
    {
        d[i] = PROTOSPI_xfer(0xFF);
    }
    CSN_on
    return res;
}

u8 Strobe(u8 state)
{
  u8 res;
    CSN_off
    res = PROTOSPI_xfer(state);
    CSN_on
    return res;
}

u8 NRF24L01_FlushTx()
{
    return Strobe(FLUSH_TX);
}

u8 NRF24L01_FlushRx()
{
    return Strobe(FLUSH_RX);
}

u8 NRF24L01_Activate(u8 c)
{
  u8 res;
    CSN_off
    res = PROTOSPI_xfer(ACTIVATE);
    PROTOSPI_xfer(c);
    CSN_on
    return res;
}

u8 NRF24L01_SetBitrate(u8 bitrate)
{
    // Note that bitrate 250kbps (and bit RF_DR_LOW) is valid only
    // for nRF24L01+. There is no way to programmatically tell it from
    // older version, nRF24L01, but the older is practically phased out
    // by Nordic, so we assume that we deal with with modern version.

    // Bit 0 goes to RF_DR_HIGH, bit 1 - to RF_DR_LOW
    rf_setup = (rf_setup & 0xD7) | ((bitrate & 0x02) << 4) | ((bitrate & 0x01) << 3);
    return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, rf_setup);
}

// Power setting is 0..3 for nRF24L01
// Claimed power amp for nRF24L01 from eBay is 20dBm. 
//      Raw            w 20dBm PA
// 0 : -18dBm  (16uW)   2dBm (1.6mW)
// 1 : -12dBm  (60uW)   8dBm   (6mW)
// 2 :  -6dBm (250uW)  14dBm  (25mW)
// 3 :   0dBm   (1mW)  20dBm (100mW)
// So it maps to Deviation as follows
/*
TXPOWER_100uW  = -10dBm
TXPOWER_300uW  = -5dBm
TXPOWER_1mW    = 0dBm
TXPOWER_3mW    = 5dBm
TXPOWER_10mW   = 10dBm
TXPOWER_30mW   = 15dBm
TXPOWER_100mW  = 20dBm
TXPOWER_150mW  = 22dBm
*/
u8 NRF24L01_SetPower(u8 power)
{
    u8 nrf_power = 0;
    switch(power) {
        case TXPOWER_100uW: nrf_power = 0; break;
        case TXPOWER_300uW: nrf_power = 0; break;
        case TXPOWER_1mW:   nrf_power = 0; break;
        case TXPOWER_3mW:   nrf_power = 1; break;
        case TXPOWER_10mW:  nrf_power = 1; break;
        case TXPOWER_30mW:  nrf_power = 2; break;
        case TXPOWER_100mW: nrf_power = 3; break;
        case TXPOWER_150mW: nrf_power = 3; break;
        default:            nrf_power = 0; break;
    };
    // Power is in range 0..3 for nRF24L01
    rf_setup = (rf_setup & 0xF9) | ((nrf_power & 0x03) << 1);
    return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, rf_setup);
}


void NRF24L01_SetTxRxMode(enum TXRX_State mode)
{
    if(mode == TX_EN) {
        CE_off
        NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                                            | (1 << NRF24L01_07_TX_DS)
                                            | (1 << NRF24L01_07_MAX_RT));
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to TX mode
                                            | (1 << NRF24L01_00_CRCO)
                                            | (1 << NRF24L01_00_PWR_UP));
        usleep(130);
        CE_on
    } else if (mode == RX_EN) {
        CE_off
        NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);        // reset the flag(s)
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F);        // switch to RX mode
        NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                                            | (1 << NRF24L01_07_TX_DS)
                                            | (1 << NRF24L01_07_MAX_RT));
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to RX mode
                                            | (1 << NRF24L01_00_CRCO)
                                            | (1 << NRF24L01_00_PWR_UP)
                                            | (1 << NRF24L01_00_PRIM_RX));
        usleep(130);
        CE_on
    } else {
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)); //PowerDown
        CE_off
    }
}

int NRF24L01_Reset()
{
  u8 status1, status2; 
    NRF24L01_FlushTx();
    NRF24L01_FlushRx();
    status1 = Strobe(NOP);
    status2 = NRF24L01_ReadReg(0x07);
    NRF24L01_SetTxRxMode(TXRX_OFF);
    return (status1 == status2 && (status1 & 0x0f) == 0x0e);
}

