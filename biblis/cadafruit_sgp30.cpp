#include "cadafruit_sgp30.h"

CAdafruit_SGP30::CAdafruit_SGP30()
{
}

CAdafruit_SGP30::~CAdafruit_SGP30()
{
    delete _i2c;
}

/*!
 *  @brief  Setups the hardware and detects a valid SGP30. Initializes I2C
 *          then reads the serialnumber and checks that we are talking to an
 * SGP30
 *  @param  theWire
 *          Optional pointer to I2C interface, otherwise use Wire
 *  @return True if SGP30 found on I2C, False if something went wrong!
 */
bool CAdafruit_SGP30::begin() {
  _i2caddr = SGP30_I2CADDR_DEFAULT;
  _i2c = CI2c::getInstance();

  uint8_t command[2];
  command[0] = 0x36;
  command[1] = 0x82;
  if (!readWordFromCommand(command, 2, 10, serialnumber, 3))
    return false;

  uint16_t featureset;
  command[0] = 0x20;
  command[1] = 0x2F;
  if (!readWordFromCommand(command, 2, 10, &featureset, 1))
    return false;
  if ((featureset & 0xF0) != SGP30_FEATURESET)
    return false;
  if (!IAQinit())
    return false;

  return true;
}

/*!
 *   @brief  Commands the sensor to begin the IAQ algorithm. Must be called
 * after startup.
 *   @returns True if command completed successfully, false if something went
 *            wrong!
 */
bool CAdafruit_SGP30::IAQinit(void) {
  uint8_t command[2];
  command[0] = 0x20;
  command[1] = 0x03;
  return readWordFromCommand(command, 2, 10);
}

/*!
 *  @brief  Commands the sensor to take a single eCO2/VOC measurement. Places
 *          results in {@link TVOC} and {@link eCO2}
 *  @return True if command completed successfully, false if something went
 *          wrong!
 */
bool CAdafruit_SGP30::IAQmeasure(void) {
  uint8_t command[2];
  command[0] = 0x20;
  command[1] = 0x08;
  uint16_t reply[2];
  if (!readWordFromCommand(command, 2, 12, reply, 2))
    return false;
  TVOC = reply[1];
  eCO2 = reply[0];
  return true;
}

 /*!
  *  @brief  Commands the sensor to take a single H2/ethanol raw measurement. Places results in {@link rawH2} and {@link rawEthanol}
  *  @returns True if command completed successfully, false if something went wrong!
  */
 bool CAdafruit_SGP30::IAQmeasureRaw(void) {
   uint8_t command[2];
   command[0] = 0x20;
   command[1] = 0x50;
   uint16_t reply[2];
   if (! readWordFromCommand(command, 2, 25, reply, 2))
     return false;
   rawEthanol = reply[1];
   rawH2 = reply[0];
   return true;
 }


/*!
 *   @brief  Request baseline calibration values for both CO2 and TVOC IAQ
 *           calculations. Places results in parameter memory locaitons.
 *   @param  eco2_base
 *           A pointer to a uint16_t which we will save the calibration
 *           value to
 *   @param  tvoc_base
 *           A pointer to a uint16_t which we will save the calibration value to
 *   @return True if command completed successfully, false if something went
 *           wrong!
 */
bool CAdafruit_SGP30::getIAQBaseline(uint16_t *eco2_base,
                                       uint16_t *tvoc_base) {
  uint8_t command[2];
  command[0] = 0x20;
  command[1] = 0x15;
  uint16_t reply[2];
  if (!readWordFromCommand(command, 2, 10, reply, 2))
    return false;
  *eco2_base = reply[0];
  *tvoc_base = reply[1];
  return true;
}

/*!
 *  @brief  Assign baseline calibration values for both CO2 and TVOC IAQ
 *          calculations.
 *  @param  eco2_base
 *          A uint16_t which we will save the calibration value from
 *  @param  tvoc_base
 *          A uint16_t which we will save the calibration value from
 *  @return True if command completed successfully, false if something went
 *          wrong!
 */
bool CAdafruit_SGP30::setIAQBaseline(uint16_t eco2_base, uint16_t tvoc_base) {
  uint8_t command[8];
  command[0] = 0x20;
  command[1] = 0x1e;
  command[2] = tvoc_base >> 8;
  command[3] = tvoc_base & 0xFF;
  command[4] = generateCRC(command + 2, 2);
  command[5] = eco2_base >> 8;
  command[6] = eco2_base & 0xFF;
  command[7] = generateCRC(command + 5, 2);

  return readWordFromCommand(command, 8, 10);
}

/*!
 *  @brief  Set the absolute humidity value [mg/m^3] for compensation to increase
 *          precision of TVOC and eCO2.
 *  @param  absolute_humidity
 *          A uint32_t [mg/m^3] which we will be used for compensation.
 *          If the absolute humidity is set to zero, humidity compensation
 *          will be disabled.
 *  @return True if command completed successfully, false if something went
 *          wrong!
 */
bool CAdafruit_SGP30::setHumidity(uint32_t absolute_humidity) {
  if (absolute_humidity > 256000) {
    return false;
  }

  uint16_t ah_scaled = static_cast<uint16_t>(static_cast<uint64_t>(absolute_humidity * 256 * 16777)<<24);
      //(uint16_t)(((uint64_t)absolute_humidity * 256 * 16777) >> 24);
  uint8_t command[5];
  command[0] = 0x20;
  command[1] = 0x61;
  command[2] = ah_scaled >> 8;
  command[3] = ah_scaled & 0xFF;
  command[4] = generateCRC(command + 2, 2);

  return readWordFromCommand(command, 5, 10);
}

/*!
 *  @brief  I2C low level interfacing
 */

bool CAdafruit_SGP30::readWordFromCommand(uint8_t command[],
                                            uint8_t commandLength,
                                            uint16_t delayms,
                                            uint16_t *readdata,
                                            uint8_t readlen) {

  usleep(delayms*1000); // UTILE ???
  uint8_t replylen = readlen * (SGP30_WORD_LEN + 1);
  uint8_t *replybuffer = new uint8_t[replylen];
  _i2c->ecrirePuisLire(_i2caddr, command, commandLength, replybuffer, readlen);

  if (readlen == 0) // pas besoin de reformer les data lues
      return true;

  for (uint8_t i = 0; i < readlen; i++) {
    uint8_t crc = generateCRC(replybuffer + i * 3, 2);
    if (crc != replybuffer[i * 3 + 2])
      return false;
    // success! store it
    readdata[i] = replybuffer[i * 3];
    readdata[i] <<= 8;
    readdata[i] |= replybuffer[i * 3 + 1];
  }
  delete[] replybuffer;
  return true;
}

uint8_t CAdafruit_SGP30::generateCRC(uint8_t *data, uint8_t datalen) {
  // calculates 8-Bit checksum with given polynomial
  uint8_t crc = SGP30_CRC8_INIT;

  for (uint8_t i = 0; i < datalen; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      if (crc & 0x80)
        crc = static_cast<uint8_t>((crc << 1) ^ SGP30_CRC8_POLYNOMIAL);
      else
        crc <<= 1;
    }
  }
  return crc;
}
