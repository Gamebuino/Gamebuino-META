/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Sorunome
*/

#include "SdSpi.h"

#include "../../../Adafruit_ZeroDMA.h"

Gamebuino_Meta::Adafruit_ZeroDMA txDMA;
Gamebuino_Meta::Adafruit_ZeroDMA rxDMA;
DmacDescriptor* txDesc;
DmacDescriptor* rxDesc;
// are we done yet?
volatile bool transfer_rx_done = false;
volatile bool transfer_tx_done = false;
// If you like, a callback can be used
void dma_callback_rx(Gamebuino_Meta::Adafruit_ZeroDMA *dma) {
	transfer_rx_done = true;
}
void dma_callback_tx(Gamebuino_Meta::Adafruit_ZeroDMA *dma) {
	transfer_tx_done = true;
}

static SPISettings sdFatSPISettings;

uint8_t chipSelectPin;


void initRxDMA() {
	rxDMA.setTrigger(SERCOM4_DMAC_ID_RX);
	rxDMA.setAction(Gamebuino_Meta::DMA_TRIGGER_ACTON_BEAT);
	rxDMA.allocate();
	rxDesc = rxDMA.addDescriptor(
		(void *)(&SERCOM4->SPI.DATA.reg), // from here
		0, // to here
		0, // this many
		Gamebuino_Meta::DMA_BEAT_SIZE_BYTE, // 8 bits
		false, // increment source addr?
		true // increment dest addr?
	);
	rxDMA.setCallback(dma_callback_rx);
}

void initTxDMA() {
	txDMA.setTrigger(SERCOM4_DMAC_ID_TX);
	txDMA.setAction(Gamebuino_Meta::DMA_TRIGGER_ACTON_BEAT);
	txDMA.allocate();
	txDesc = txDMA.addDescriptor(
		0, // from here
		(void *)(&SERCOM4->SPI.DATA.reg), // to here
		0, // this many
		Gamebuino_Meta::DMA_BEAT_SIZE_BYTE, // 8 bits
		true, // increment source addr?
		false // increment dest addr?
	);
	txDMA.setCallback(dma_callback_tx);
}

void SdSpi::begin(uint8_t _chipSelectPin) {
	chipSelectPin = _chipSelectPin;
	pinMode(chipSelectPin, OUTPUT);
	digitalWrite(chipSelectPin, HIGH);
	SPI.begin();
	sdFatSPISettings = SPISettings(12000000, MSBFIRST, SPI_MODE0);
	initRxDMA();
	initTxDMA();
}

void SdSpi::beginTransaction(uint8_t divisor) {
#if ENABLE_SPI_TRANSACTIONS
	SPI.beginTransaction(sdFatSPISettings);
#endif  // ENABLE_SPI_TRANSACTIONS
#ifndef SPI_CLOCK_DIV128
	SPI.setClockDivider(divisor);
#else  // SPI_CLOCK_DIV128
	int v;
	if (divisor <= 2) {
		v = SPI_CLOCK_DIV2;
	} else  if (divisor <= 4) {
		v = SPI_CLOCK_DIV4;
	} else  if (divisor <= 8) {
		v = SPI_CLOCK_DIV8;
	} else  if (divisor <= 16) {
		v = SPI_CLOCK_DIV16;
	} else  if (divisor <= 32) {
		v = SPI_CLOCK_DIV32;
	} else  if (divisor <= 64) {
		v = SPI_CLOCK_DIV64;
	} else {
		v = SPI_CLOCK_DIV128;
	}
	SPI.setClockDivider(v);
#endif  // SPI_CLOCK_DIV128
}

void SdSpi::endTransaction() {
#if ENABLE_SPI_TRANSACTIONS
	SPI.endTransaction();
#endif  // ENABLE_SPI_TRANSACTIONS
}

// no DMA needed here
uint8_t SdSpi::receive() {
	return SPI.transfer(0XFF);
}

uint8_t SdSpi::receive(uint8_t* buf, size_t n) {
	uint8_t b = 0xFF;
	txDesc->BTCTRL.bit.SRCINC = false;
	txDMA.changeDescriptor(txDesc, &b, NULL, n);
	
	rxDMA.changeDescriptor(rxDesc, NULL, buf, n);
	
	transfer_tx_done = transfer_rx_done = false;
	rxDMA.startJob();
	txDMA.startJob();
	while (!(transfer_tx_done && transfer_rx_done)); // chill
	return 0; // we just assume this worked.... (probably not too good an idea)
	// TODO: check for stuff
}

void SdSpi::send(uint8_t b) {
	SPI.transfer(b);
}

void SdSpi::send(const uint8_t* buf, size_t n) {
	txDesc->BTCTRL.bit.SRCINC = true;
	txDMA.changeDescriptor(txDesc, (uint8_t*)buf, NULL, n);
	
	transfer_tx_done = false;
	txDMA.startJob();
	while (!transfer_tx_done); // chill
}
