#ifndef __MLX90614_H__
#define __MLX90614_H__

#include    "I2C.h"

#include	"SensorInfo.h"
#include	"SenseBuffer.h"

// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x2E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F

class MLX90614 : public SensorInfo
{
  public:
	virtual	void 	initialize(I2C *i2c) {};
	virtual	void	get(SenseBuffer *buff){};
	virtual	size_t	build(char *p, SenseBuffer *buff);
	virtual	void	stop(int sec) {};
	virtual	void	start(void) {};
	virtual	const	char	*name(void) { return NULL; };
	virtual	const	char	*unit(void);
	virtual	const	char	*data_class_name(void) { return NULL; };
	virtual	size_t	size(void) { return sizeof(float); };
	static	const	uint8_t	precision = 1;
	virtual	float	value(void) { return 0.0; };
	esp_err_t	err_code;
	enum {
		I2C_ADDRESS = 0x5A,
    };
  protected:
	MLX90614() { fValid = true; };
   	~MLX90614();
	I2C		i2c = I2C(I2C_ADDRESS);
  private:
};
#endif