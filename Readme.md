### MAX30003 Driver Library

#### Description
This library provides an interface to configure and use the MAX30003, an analog front-end (AFE) designed for ECG measurement. It includes functions for initialization, ECG data acquisition, and register configuration using the SPI interface.

---

#### **Requirements**
- STM32 microcontroller with HAL enabled.
- Configured SPI communication.
- GPIO connection for the MAX30003 CS pin.
- Support files:
  - `max30003.h`: Contains register definitions and function prototypes.
  - `max30003.c`: Contains function implementations.

---

#### **Hardware Setup**
1. **Power Supply:**
   - Connect `AVDD` and `DVDD` to the required voltage level (typically 1.8V or 2.0V).
   - Connect `OVDD` to the logic level of the SPI communication (e.g., 3.3V).
2. **SPI Signals:**
   - `SCLK`, `SDI`, `SDO`, `CSB`: Connect to the SPI pins of the microcontroller.
3. **Capacitors:**
   - Between `CAPP` and `CAPN`: 1 µF for the high-pass filter.
   - Between `VBG`, `VCM`, `VREF`, and `AGND`: Appropriate capacitors for filtering (refer to the datasheet for specifications).
4. **GPIO:**
   - Configure a pin for `INTB` for interrupts.

---

### **Library Usage**

#### 1. **Initialization**
The `MAX30003_Init()` function must be called once at program startup to configure the device:
```c
SPI_HandleTypeDef hspi1; // Handle for SPI peripheral
MAX30003_Init(&hspi1, GPIOC, GPIO_PIN_1);
```
- **Parameters:**
  - `hspi`: SPI handle configured for the MAX30003.
  - `CS_GPIO_Port`: GPIO port for the chip select pin.
  - `CS_Pin`: GPIO pin number for `CS`.

The initialization sequence includes:
- Software reset.
- General and calibration configuration.
- Interrupt enablement.
- FIFO reset.

---

#### 2. **Reading ECG Data**
To read data from the MAX30003 FIFO, use the `MAX30003_ReadECG()` function:
```c
uint8_t ecg_data[49] = {0};
MAX30003_ReadECG(ecg_data);
```
- The `ecg_data` buffer will contain the data, with each sample represented by 3 bytes.

---

#### 3. **Writing to Registers**
To configure the device by writing to a register:
```c
uint8_t data[3] = {0x08, 0x10, 0x07}; // Example value
MAX30003_WriteRegister(CNFG_GEN, data);
```
- **Parameters:**
  - `reg_address`: Register address (e.g., `CNFG_GEN`).
  - `data`: Buffer containing the 3 bytes to write.

---

#### 4. **Reading from Registers**
To read a specific register:
```c
uint8_t data[3];
MAX30003_ReadRegister(CNFG_GEN, data);
```
- **Output:** The `data` buffer will contain the register value.

---

### **Register Configuration**
These custom configuration uses the following values:

| **Register**   | **Value**         | **Description**                               |
|----------------|-------------------|-----------------------------------------------|
| `CNFG_GEN`     | `0x08, 0x10, 0x07`| General configuration.                        |
| `CNFG_CAL`     | `0x70, 0x00, 0x00`| Calibration settings for self-test.           |
| `CNFG_EMUX`    | `0x0B, 0x00, 0x00`| Input multiplexer configuration.              |
| `CNFG_ECG`     | `0x00, 0x50, 0x00`| Filter and gain settings for ECG acquisition. |
| `EN_INT`       | `0x80, 0x00, 0x01`| Interrupt enable configuration.               |
| `MNGR_INT`     | `0x78, 0x00, 0x04`| FIFO interrupt management.                    |

**Explanation:**
1. **`CNFG_GEN` (General Configuration):**
   - Enables ECG channel (`EN_ECG`).
   - Configures lead-off detection current sources and thresholds.
   - Internal lead bias resistors for common-mode input range.

2. **`CNFG_CAL` (Calibration Settings):**
   - Configures calibration signal amplitude and frequency.
   - Useful for end-to-end testing of the signal path.

3. **`CNFG_EMUX` (Input Multiplexer):**
   - Controls the signal routing for ECG inputs and calibration voltages.
   - Allows the selection of single-ended or differential input modes.

4. **`CNFG_ECG` (ECG Configuration):**
   - Configures high-pass and low-pass filters.
   - Sets programmable gain for the ECG signal (20x to 160x).

5. **`EN_INT` (Interrupt Enable):**
   - Enables specific interrupts for events like FIFO overflow, data ready, and lead-off detection.

6. **`MNGR_INT` (Interrupt Management):**
   - Configures FIFO thresholds for efficient data transfer and system wake-ups.

In order to change the configuration read **MAX30003 Datasheet**.

### **Operation Sequence**
1. **Startup:**
   - Initialize SPI and GPIO peripherals.
   - Call `MAX30003_Init()` to configure the device.
2. **ECG Measurement:**
   - Wait for an interrupt (`INTB`) or poll for data readiness.
   - Use `MAX30003_ReadECG()` to acquire data.
   - Process and transmit the data via UART or another interface.
3. **Serial Data Handling:**
   - On Linux, monitor the serial port at `/dev/ttyACM0`.
   - Use the command: `sudo minicom -b 115200 -o -D /dev/ttyACM0` to check data output.

---

### **References**
- **MAX30003 Datasheet:** Detailed register and configuration specifications.
- **STM32 HAL Manual:** Guidance for SPI interface setup.
