## FF01 - SB - Serial transfer data (R/W)
8 Bits of data to be read/written

## FF02 - SC - Serial Transfer Control (R/W)
  Bit 7 - Transfer Start Flag (0=No Transfer, 1=Start)

  Bit 1 - Clock Speed (0=Normal, 1=Fast) ** CGB Mode Only **

  Bit 0 - Shift Clock (0=External Clock, 1=Internal Clock)

The clock signal specifies the rate at which the eight data bits in SB (FF01) are transferred. 

When the gameboy is communicating with another gameboy then either one must supply internal clock, and the other one must use external clock.

## Internal Clock
In Non-CGB Mode the gameboy supplies an internal clock of 8192Hz only. 

In CGB Mode four internal clock rates are available, depending on Bit 1 of the SC register, and on whether the CGB Double Speed Mode is used:

| Freq.  | Speed                 | Mode   |
|--------|-----------------------|--------|
| 8192Hz | 1KB/s - Bit 1 cleared | Normal |
| 16384Hz | 2KB/s - Bit 1 cleared | Double Speed Mode |
| 262144Hz | 32KB/s - Bit 1 set | Normal |
| 524288Hz | 64KB/s - Bit 1 set | Double Speed Mode |

## External Clock
The external clock is typically supplied by another gameboy. It isn't required that the clock pulses are sent at an regular interval.

## Timeouts
When using external clock then the transfer will not complete until the last bit is received. 

For this reason the transfer procedure should use a timeout counter, and abort the communication if no response has been received during the timeout interval.

## Delays and Synchronization
The gameboy that is using internal clock should always execute a small delay between each transfer, in order to ensure that the opponent gameboy has enough time to prepare itself for the next transfer, ie. the gameboy with external clock must have set its transfer start bit before the gameboy with internal clock starts the transfer. 

Two gameboys could switch between internal and external clock for each transferred byte to ensure synchronization.

Transfer is initiated by setting the Transfer Start Flag (**SC Bit 7**). This bit is automatically set to 0 at the end of Transfer. Reading this bit can be used to determine if the transfer is still active.

## INT 58 - Serial Interrupt
When a transfer has completed, an interrupt is requested by setting Bit 3 of the IF Register (FF0F). When that interrupt is enabled, then the Serial Interrupt vector at 0058 is called.

## Notes

Transmitting and receiving serial data is done simultaneously. The received data is automatically stored in SB.

During a transfer, a byte is shifted in at the same time that a byte is shifted out. The rate of the shift is determined by whether the clock source is internal or external.

The most significant bit is shifted in and out first.

When the internal clock is selected, it drives the clock pin on the game link port and it stays high when not used. During a transfer it will go low eight times to clock in/out each bit.

The state of the last bit shifted out determines the state of the output line until another transfer takes place.

If a serial transfer with internal clock is performed and no external GameBoy is present, a value of $FF will be received in the transfer.

The following code causes $75 to be shifted out the serial port and a byte to be shifted into $FF01:

    ld   a,$75
    ld  ($FF01),a
    ld   a,$81
    ld  ($FF02),a
