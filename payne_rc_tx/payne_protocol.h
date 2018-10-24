#include "iface_nrf24l01.h"
#include "payne_rc.h"

enum {
  TX_INIT2 = 0,
  TX_INIT2_NO_BIND,
  TX_BIND1,
  TX_BIND2,
  TX_DATA
};

enum {
  // flags going to byte 14
  FLAG_CAMERA = 0x01, // also automatic Missile Launcher and Hoist in one direction
  FLAG_VIDEO  = 0x02, // also Sprayer, Bubbler, Missile Launcher(1), and Hoist in the other dir.
  FLAG_FLIP   = 0x04,
  FLAG_UNK9   = 0x08,
  FLAG_LED    = 0x10,
  FLAG_UNK10  = 0x20,
  FLAG_BIND   = 0xC0,
  // flags going to byte 10
  FLAG_HEADLESS  = 0x0200,
  FLAG_MAG_CAL_X = 0x0800,
  FLAG_MAG_CAL_Y = 0x2000
};

const uint8_t freq_hopping[][16] = {
  { 0x27, 0x1B, 0x39, 0x28, 0x24, 0x22, 0x2E, 0x36,
    0x19, 0x21, 0x29, 0x14, 0x1E, 0x12, 0x2D, 0x18 }, //  00
  { 0x2E, 0x33, 0x25, 0x38, 0x19, 0x12, 0x18, 0x16,
    0x2A, 0x1C, 0x1F, 0x37, 0x2F, 0x23, 0x34, 0x10 }, //  01
  { 0x11, 0x1A, 0x35, 0x24, 0x28, 0x18, 0x25, 0x2A,
    0x32, 0x2C, 0x14, 0x27, 0x36, 0x34, 0x1C, 0x17 }, //  02
  { 0x22, 0x27, 0x17, 0x39, 0x34, 0x28, 0x2B, 0x1D,
    0x18, 0x2A, 0x21, 0x38, 0x10, 0x26, 0x20, 0x1F }  //  03
};
uint8_t rf_channels[16];

// Packet ack status values
enum {
  PKT_PENDING = 0,
  PKT_ACKED,
  PKT_TIMEOUT
};

static uint8_t packet_ack()
{
  switch (NRF24L01_ReadReg(NRF24L01_07_STATUS) & (_BV(NRF24L01_07_TX_DS) | _BV(NRF24L01_07_MAX_RT))) {
  case _BV(NRF24L01_07_TX_DS):
    return PKT_ACKED;
  case _BV(NRF24L01_07_MAX_RT):
    return PKT_TIMEOUT;
  }
  return PKT_PENDING;
}

#define PAYLOADSIZE 16

#define BIND_COUNT 1000

// Timeout for callback in uSec, 4ms=4000us for TX
#define PACKET_PERIOD 4000
// Time to wait for packet to be sent (no ACK, so very short)
#define PACKET_CHKTIME  100

u8 packet[PAYLOADSIZE];
static u8 packet_sent;
static u8 tx_id[3+2];
static u8 rf_ch_num;
static u16 bind_counter;
static u8 tx_power;
static u16 tx_flags;
static u8 tx_phase;

static uint8_t rx_tx_addr[] = {0x66, 0x88, 0x68, 0x68, 0x78};
static uint8_t rx_p1_addr[] = {0x88, 0x66, 0x86, 0x86, 0x87};

static void tx_init()
{

  NRF24L01_Initialize();

  // 2-bytes CRC, radio off
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, _BV(NRF24L01_00_EN_CRC) | _BV(NRF24L01_00_CRCO)); 
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);      // No Auto Acknoledgement
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x3F);  // Enable all data pipes
  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);   // 5-byte RX/TX address
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0xFF); // 4ms retransmit t/o, 15 tries
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, 0x08);      // Channel 8
  NRF24L01_SetBitrate(NRF24L01_BR_250K);
  NRF24L01_SetPower(TXPOWER_100mW);
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);     // Clear data ready, data sent, and retransmit
  //    NRF24L01_WriteReg(NRF24L01_08_OBSERVE_TX, 0x00); // no write bits in this field
  //    NRF24L01_WriteReg(NRF24L01_00_CD, 0x00);         // same
  NRF24L01_WriteReg(NRF24L01_0C_RX_ADDR_P2, 0xC3); // LSB byte of pipe 2 receive address
  NRF24L01_WriteReg(NRF24L01_0D_RX_ADDR_P3, 0xC4);
  NRF24L01_WriteReg(NRF24L01_0E_RX_ADDR_P4, 0xC5);
  NRF24L01_WriteReg(NRF24L01_0F_RX_ADDR_P5, 0xC6);
  NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, PAYLOADSIZE);   // bytes of data payload for pipe 1
  NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, PAYLOADSIZE);
  NRF24L01_WriteReg(NRF24L01_13_RX_PW_P2, PAYLOADSIZE);
  NRF24L01_WriteReg(NRF24L01_14_RX_PW_P3, PAYLOADSIZE);
  NRF24L01_WriteReg(NRF24L01_15_RX_PW_P4, PAYLOADSIZE);
  NRF24L01_WriteReg(NRF24L01_16_RX_PW_P5, PAYLOADSIZE);
  NRF24L01_WriteReg(NRF24L01_17_FIFO_STATUS, 0x00); // Just in case, no real bits to write here

  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, rx_tx_addr, 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0B_RX_ADDR_P1, rx_p1_addr, 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR, rx_tx_addr, 5);
  
  // Check for Beken BK2421/BK2423 chip
  // It is done by using Beken specific activate code, 0x53
  // and checking that status register changed appropriately
  // There is no harm to run it on nRF24L01 because following
  // closing activate command changes state back even if it
  // does something on nRF24L01

  // Implicit delay in callback
  delay(50);
}

static void TX_init2()
{
  uint8_t config;
  NRF24L01_FlushTx();
  packet_sent = 0;
  rf_ch_num = 0;

  // Turn radio power on
  NRF24L01_SetTxRxMode(TX_EN);
  config = _BV(NRF24L01_00_EN_CRC) | _BV(NRF24L01_00_CRCO) | _BV(NRF24L01_00_PWR_UP);
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, config);
  // Implicit delay in callback
  delayMicroseconds(150);
}

static void set_tx_id(u32 id)
{
  uint8_t i, sum, increment, val;
  uint8_t *fh_row;


  tx_id[0] = (id >> 16) & 0xFF;
  tx_id[1] = (id >> 8) & 0xFF;
  tx_id[2] = (id >> 0) & 0xFF;

  sum = tx_id[0] + tx_id[1] + tx_id[2];
    tx_id[3] = sum;
    tx_id[4] = 0x33; // extend to 5 as address

  // Base row is defined by lowest 2 bits
  fh_row = freq_hopping[sum & 0x03];
  // Higher 3 bits define increment to corresponding row
  increment = (sum & 0x1e) >> 2;
  for (i = 0; i < 16; ++i) {
    val = fh_row[i] + increment;
    // Strange avoidance of channels divisible by 16
    rf_channels[i] = (val & 0x0f) ? val : val - 3;
  }
}

static void add_pkt_checksum()
{
  uint8_t i, sum = 0;
  for (i = 0; i < 15;  ++i) sum += packet[i];
  packet[15] = sum;
}



const uint8_t mask[8+1] = {0,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

static void buildDataPakage() {
    /* 8ch data (each 11 bits) will fill 11 byte package (0 to 10) */
  uint8_t Current_Channel = 0;
  uint8_t Current_Channel_Bit = 0;
  uint8_t Current_Packet_Bit = 0;
  uint8_t Packet_Position = 0;
    uint8_t Current_Packet_Rest_Bits;
    uint8_t Current_Channel_Rest_Bits;
  uint8_t i;
  uint16_t value;


  for(i=0; i< 11; i++) {
     packet[i] = 0;
  }

    while( Current_Channel < CHAN_SIZE ) {
      Current_Packet_Rest_Bits = 8 - Current_Packet_Bit;
      Current_Channel_Rest_Bits = 11 - Current_Channel_Bit;

    noInterrupts();
    value = ppms[Current_Channel]-500;
    interrupts();

      if ( Current_Packet_Rest_Bits < Current_Channel_Rest_Bits) {
          packet[Packet_Position] |=  (((value>>Current_Channel_Bit)& mask[Current_Packet_Rest_Bits] ) << Current_Packet_Bit);

          Current_Channel_Bit += Current_Packet_Rest_Bits;
          Packet_Position++;
          Current_Packet_Bit = 0;
      } else if ( Current_Packet_Rest_Bits > Current_Channel_Rest_Bits) {
          packet[Packet_Position] |=  (((value>>Current_Channel_Bit)& mask[Current_Channel_Rest_Bits] ) << Current_Packet_Bit);

         Current_Packet_Bit +=Current_Channel_Rest_Bits;
         Current_Channel++;
         Current_Channel_Bit = 0;
      } else {
          packet[Packet_Position] |=  (((value>>Current_Channel_Bit)& mask[Current_Packet_Rest_Bits] ) << Current_Packet_Bit);
         Current_Channel++;
         Current_Channel_Bit = 0;
          Packet_Position++;
          Current_Packet_Bit = 0;         
      }
  }
}

static void send_packet(uint8_t bind)
{
  uint8_t rf_ch;
  if (bind) {
    tx_flags  = FLAG_BIND;
        packet[0] = tx_id[0];
        packet[1] = tx_id[1];
        packet[2] = tx_id[2];

  } else {
    NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR, tx_id, 5);
    buildDataPakage();
  }

  packet[13] = tx_flags >> 8;
  packet[14] = tx_flags & 0xff;
  add_pkt_checksum();

  packet_sent = 0;

  // Each packet is repeated twice on the same
  // channel, hence >> 1
  // We're not strictly repeating them, rather we
  // send new packet on the same frequency, so the
  // receiver gets the freshest command. As receiver
  // hops to a new frequency as soon as valid packet
  // received it does not matter that the packet is
  // not the same one repeated twice - nobody checks this
  rf_ch = rf_channels[rf_ch_num];
  rf_ch_num = (rf_ch_num + 1) & 0x0F;

  //  Serial.print(rf_ch); Serial.write("\n");
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, rf_ch);
  NRF24L01_FlushTx();
  NRF24L01_WritePayload(packet, sizeof(packet));
  packet_sent = 1;
  //    radio.ce(HIGH);
  //    delayMicroseconds(15);
  // It saves power to turn off radio after the transmission,
  // so as long as we have pins to do so, it is wise to turn
  // it back.
  //    radio.ce(LOW);

  // Check and adjust transmission power. We do this after
  // transmission to not bother with timeout after power
  // settings change -  we have plenty of time until next
  // packet.

  /*
  if (! rf_ch_num && tx_power != Model.tx_power) {
    //Keep transmit power updated
    tx_power = Model.tx_power;
    NRF24L01_SetPower(tx_power);
  }
  */
}

static uint16_t tx_run()
{
  switch (tx_phase) {
  case TX_INIT2:
    TX_init2();
    //MUSIC_Play(MUSIC_TELEMALARM1);
    //        tx_phase = TX_BIND1;
    tx_phase = TX_BIND2;
    return 150;
    break;
  case TX_INIT2_NO_BIND:
    TX_init2();
    tx_phase = TX_DATA;
    return 150;
    break;
  case TX_BIND1:
    /*
    send_packet(1);
    if (throttle >= 240) tx_phase = TX_BIND2;
    */
    break;
  case TX_BIND2:
    if (packet_sent && packet_ack() != PKT_ACKED) {
      //            printf("Packet not sent yet\n");
      return PACKET_CHKTIME;
    }
    send_packet(1);
    //        if (throttle == 0) {
    if (--bind_counter == 0) {
      Red_LED_ON
      tx_phase = TX_DATA;
      tx_flags = 0;
      //PROTOCOL_SetBindState(0);
      //MUSIC_Play(MUSIC_DONE_BINDING);
    } else {
      if (bitRead(bind_counter, 4) == 1) {
        Red_LED_ON;
      }
      else {
        Red_LED_OFF;
      }
    }
    break;
  case TX_DATA:

    if (packet_sent && packet_ack() != PKT_ACKED) {
      return PACKET_CHKTIME;
    }
    send_packet(0);
    break;
  }
  // Packet every 4ms
  return PACKET_PERIOD;
}

static void init_tx_id()
{
 
  set_tx_id(TX_ID);
}

static void tx_init(uint8_t bind)
{
  tx_init();
  tx_phase = bind ? TX_INIT2 : TX_INIT2_NO_BIND;
  if (bind) {
    bind_counter = BIND_COUNT/10;  // 1000/10 reduce bind count to 100
  }
  init_tx_id();
}
