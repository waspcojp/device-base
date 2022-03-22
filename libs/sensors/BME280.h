#ifndef __BME280_H__
#define __BME280_H__

#include 	"I2C.h"

#include	"SensorInfo.h"
#include	"SenseBuffer.h"

class BME280:public SensorInfo
{
  public:
	virtual	void 	initialize();
	virtual	void	get(SenseBuffer *buff) {};
	virtual	size_t	build(char *p, SenseBuffer *buff) { return 0; };
	virtual	void	stop(int sec) {};
	virtual	void	start(void) {};
	virtual	const	char	*name(void) { return NULL; };
	virtual	const	char	*unit(void) { return NULL; };
	virtual	const	char	*data_class_name(void) { return NULL; };
	virtual	size_t	size(void) { return sizeof(float); };
	virtual	float	value(void) { return 0.0; };
	static	const	uint8_t	precision;

	esp_err_t	err_code;

  protected:
	BME280();
   	~BME280();

	static	const	int	SLA_BME280 = 0x76;
	I2C	mI2C = I2C(SLA_BME280);

	static	const	int	CONVERSION_TIME = 10;

	static	const	byte	REG_CONTROL = 0xF4;
	static	const	byte	REG_WORD_RESULT = 0b00100101;

	static	const	byte	BME280_START = 0xD0;
	static	const	byte	VAL_TEMPERATURE = 0xFA;
	static	const	byte	VAL_PRESSURE = 0xF7;
	static	const	byte	VAL_HUMIDITY = 0xFD;

	static	uint16_t	dig_T1;
	static	int16_t		dig_T2,
						dig_T3;
	static	uint16_t	dig_P1;
	static	int16_t		dig_P2,
						dig_P3,
						dig_P4,
						dig_P5,
						dig_P6,
						dig_P7,
						dig_P8,
						dig_P9;
	static	int8_t		dig_H1;
	static	int16_t		dig_H2;
	static	int8_t		dig_H3;
	static	int16_t		dig_H4,
						dig_H5;
	static	int8_t		dig_H6;
};

#endif // __BME280_H__
