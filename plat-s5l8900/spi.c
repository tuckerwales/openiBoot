#include "openiboot.h"
#include "arm/arm.h"
#include "spi.h"
#include "hardware/spi.h"
#include "util.h"
#include "clock.h"
#include "chipid.h"
#include "timer.h"
#include "interrupt.h"

static const SPIRegister SPIRegs[NUM_SPIPORTS] = {
	{SPI0 + CONTROL, SPI0 + SETUP, SPI0 + STATUS, SPI0 + PIN, SPI0 + TXDATA, SPI0 + RXDATA, SPI0 + CLKDIVIDER, SPI0 + SPCNT, SPI0 + SPIDD},
	{SPI1 + CONTROL, SPI1 + SETUP, SPI1 + STATUS, SPI1 + PIN, SPI1 + TXDATA, SPI1 + RXDATA, SPI1 + CLKDIVIDER, SPI1 + SPCNT, SPI1 + SPIDD},
	{SPI2 + CONTROL, SPI2 + SETUP, SPI2 + STATUS, SPI2 + PIN, SPI2 + TXDATA, SPI2 + RXDATA, SPI2 + CLKDIVIDER, SPI2 + SPCNT, SPI2 + SPIDD}
};

static SPIInfo spi_info[NUM_SPIPORTS];

int spi_setup() {
	clock_gate_switch(SPI0_CLOCKGATE, ON);
	clock_gate_switch(SPI1_CLOCKGATE, ON);
	clock_gate_switch(SPI2_CLOCKGATE, ON);

	memset(spi_info, 0, sizeof(SPIInfo) * NUM_SPIPORTS);

	int i;
	for(i = 0; i < NUM_SPIPORTS; i++) {
		spi_info[i].clockSource = NCLK;
		SET_REG(SPIRegs[i].control, 0);
	}

	// No interrupt setup — using polled I/O

	return 0;
}

static void spi_txdata(int port, const volatile void *buff, int from, int to)
{
	int i=from>>spi_info[port].wordSize;
	int j=to>>spi_info[port].wordSize;
	switch(spi_info[port].wordSize)
	{
		case 0: // 8-bytes
			{
				uint8_t *buf = (uint8_t*)buff;
				for(;i<j;i++)
				{
					SET_REG(SPIRegs[port].txData, buf[i]);
				}
			}
			break;

		case 1: // 16-bytes
			{
				uint16_t *buf = (uint16_t*)buff;
				for(;i<j;i++)
				{
					SET_REG(SPIRegs[port].txData, buf[i]);
				}
			}
			break;

		case 2: // 32-bytes
			{
				uint32_t *buf = (uint32_t*)buff;
				for(;i<j;i++)
				{
					SET_REG(SPIRegs[port].txData, buf[i]);
				}
			}
			break;
	}
}

static void spi_rxdata(int port, const volatile void *buff, int from, int to)
{
	int i=from>>spi_info[port].wordSize;
	int j=to>>spi_info[port].wordSize;
	switch(spi_info[port].wordSize)
	{
		case 0: // 8-bytes
			{
				uint8_t *buf = (uint8_t*)buff;
				for(;i<j;i++)
				{
					buf[i] = GET_REG(SPIRegs[port].rxData);
				}
			}
			break;

		case 1: // 16-bytes
			{
				uint16_t *buf = (uint16_t*)buff;
				for(;i<j;i++)
				{
					buf[i] = GET_REG(SPIRegs[port].rxData);
				}
			}
			break;

		case 2: // 32-bytes
			{
				uint32_t *buf = (uint32_t*)buff;
				for(;i<j;i++)
				{
					buf[i] = GET_REG(SPIRegs[port].rxData);
				}
			}
			break;
	}
}

int spi_tx(int port, const uint8_t* buffer, int len, int block, int unknown) {
	if(port > (NUM_SPIPORTS - 1)) {
		return -1;
	}

	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 2));
	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 3));

	int txCurrentLen;
	if(len > MAX_TX_BUFFER)
		txCurrentLen = MAX_TX_BUFFER;
	else
		txCurrentLen = len;

	if(unknown == 0) {
		SET_REG(SPIRegs[port].cnt, 0);
	}

	spi_txdata(port, buffer, 0, txCurrentLen);

	SET_REG(SPIRegs[port].control, 1);

	if(block) {
		// Polled TX: feed remaining data as FIFO drains
		while(txCurrentLen < len) {
			uint32_t status = GET_REG(SPIRegs[port].status);
			if(status & (1 << 1)) {
				int toTX = len - txCurrentLen;
				int canTX = (MAX_TX_BUFFER - GET_BITS(status, 4, 4)) << spi_info[port].wordSize;
				if(toTX > canTX)
					toTX = canTX;
				spi_txdata(port, buffer, txCurrentLen, txCurrentLen + toTX);
				txCurrentLen += toTX;
				SET_REG(SPIRegs[port].status, status);
			}
		}
		// Wait for TX FIFO to fully drain
		while(GET_BITS(GET_REG(SPIRegs[port].status), 4, 4) != 0) {
		}
		// Clear any pending status
		SET_REG(SPIRegs[port].status, GET_REG(SPIRegs[port].status));
		return len;
	} else {
		return 0;
	}
}

int spi_rx(int port, uint8_t* buffer, int len, int block, int noTransmitJunk) {
	if(port > (NUM_SPIPORTS - 1)) {
		return -1;
	}

	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 2));
	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 3));

	if(noTransmitJunk == 0) {
		SET_REG(SPIRegs[port].setup, GET_REG(SPIRegs[port].setup) | 1);
	}

	SET_REG(SPIRegs[port].cnt, (len + ((1<<spi_info[port].wordSize)-1)) >> spi_info[port].wordSize);
	SET_REG(SPIRegs[port].control, 1);

	if(block) {
		int rxCurrentLen = 0;
		uint64_t startTime = timer_get_system_microtime();

		// Polled RX: read data as it arrives in FIFO
		while(rxCurrentLen < len) {
			uint32_t status = GET_REG(SPIRegs[port].status);
			int canRX = GET_BITS(status, 8, 4) << spi_info[port].wordSize;
			if(canRX > 0) {
				int toRX = len - rxCurrentLen;
				if(toRX > canRX)
					toRX = canRX;
				spi_rxdata(port, buffer, rxCurrentLen, rxCurrentLen + toRX);
				rxCurrentLen += toRX;
				SET_REG(SPIRegs[port].status, status);
			}
			if(has_elapsed(startTime, 1000)) {
				if(noTransmitJunk == 0) {
					SET_REG(SPIRegs[port].setup, GET_REG(SPIRegs[port].setup) & ~1);
				}
				return -1;
			}
		}

		if(noTransmitJunk == 0) {
			SET_REG(SPIRegs[port].setup, GET_REG(SPIRegs[port].setup) & ~1);
		}
		// Clear any pending status
		SET_REG(SPIRegs[port].status, GET_REG(SPIRegs[port].status));
		return len;
	} else {
		return 0;
	}
}

int spi_txrx(int port, const uint8_t* outBuffer, int outLen, uint8_t* inBuffer, int inLen, int block)
{
	if(port > (NUM_SPIPORTS - 1)) {
		return -1;
	}

	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 2));
	SET_REG(SPIRegs[port].control, GET_REG(SPIRegs[port].control) | (1 << 3));

	int txCurrentLen;
	if(outLen > MAX_TX_BUFFER)
		txCurrentLen = MAX_TX_BUFFER;
	else
		txCurrentLen = outLen;

	spi_txdata(port, outBuffer, 0, txCurrentLen);

	SET_REG(SPIRegs[port].cnt, (inLen + ((1<<spi_info[port].wordSize)-1)) >> spi_info[port].wordSize);
	SET_REG(SPIRegs[port].control, 1);

	if(block) {
		int txDone = (txCurrentLen >= outLen) ? TRUE : FALSE;
		int rxCurrentLen = 0;

		// Polled TX+RX
		while(!txDone || rxCurrentLen < inLen) {
			uint32_t status = GET_REG(SPIRegs[port].status);

			// Handle TX
			if(!txDone && (status & (1 << 1))) {
				if(txCurrentLen < outLen) {
					int toTX = outLen - txCurrentLen;
					int canTX = (MAX_TX_BUFFER - GET_BITS(status, 4, 4)) << spi_info[port].wordSize;
					if(toTX > canTX)
						toTX = canTX;
					spi_txdata(port, outBuffer, txCurrentLen, txCurrentLen + toTX);
					txCurrentLen += toTX;
				}
				if(txCurrentLen >= outLen)
					txDone = TRUE;
			}

			// Handle RX
			if(rxCurrentLen < inLen) {
				int canRX = GET_BITS(status, 8, 4) << spi_info[port].wordSize;
				if(canRX > 0) {
					int toRX = inLen - rxCurrentLen;
					if(toRX > canRX)
						toRX = canRX;
					spi_rxdata(port, inBuffer, rxCurrentLen, rxCurrentLen + toRX);
					rxCurrentLen += toRX;
				}
			}

			SET_REG(SPIRegs[port].status, status);
		}

		// Wait for TX FIFO to drain
		while(GET_BITS(GET_REG(SPIRegs[port].status), 4, 4) != 0) {
		}
		// Clear any pending status
		SET_REG(SPIRegs[port].status, GET_REG(SPIRegs[port].status));
		return inLen;
	} else {
		return 0;
	}
}

void spi_set_baud(int port, int baud, SPIWordSize wordSize, int isMaster, int isActiveLow, int lastClockEdgeMissing) {
	if(port > (NUM_SPIPORTS - 1)) {
		return;
	}

	SET_REG(SPIRegs[port].control, 0);

	switch(wordSize) {
		case SPIWordSize8:
			spi_info[port].wordSize = 0;
			break;

		case SPIWordSize16:
			spi_info[port].wordSize = 1;
			break;

		case SPIWordSize32:
			spi_info[port].wordSize = 2;
			break;
	}

	spi_info[port].isActiveLow = isActiveLow;
	spi_info[port].lastClockEdgeMissing = lastClockEdgeMissing;

	uint32_t clockFrequency;

	if(spi_info[port].clockSource == PCLK) {
		clockFrequency = clock_get_frequency(FrequencyBasePeripheral);
	} else {
		clockFrequency = clock_get_frequency(FrequencyBaseFixed);
	}

	uint32_t divider;

	if(chipid_spi_clocktype() != 0) {
		divider = clockFrequency / baud;
		if(divider < 2)
			divider = 2;
	} else {
		divider = clockFrequency / (baud * 2 - 1);
	}

	if(divider > MAX_DIVIDER) {
		return;
	}

	SET_REG(SPIRegs[port].clkDivider, divider);
	spi_info[port].baud = baud;
	spi_info[port].isMaster = isMaster;

	uint32_t options = (lastClockEdgeMissing << 1)
			| (isActiveLow << 2)
			| ((isMaster ? 0x3 : 0) << 3)
			| ((spi_info[port].option5 ? 0x2 : 0x3D) << 5)
			| (spi_info[port].clockSource << CLOCK_SHIFT)
			| spi_info[port].wordSize << 13;

	SET_REG(SPIRegs[port].setup, options);
	SET_REG(SPIRegs[port].pin, 0);
	SET_REG(SPIRegs[port].control, 1);

}
