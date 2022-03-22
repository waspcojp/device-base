#include 	"config.h"
#ifdef	USE_I2C

extern "C"{
#include	"driver/i2c.h"
#include 	"esp_system.h"
#include	"types.h"
#include	"misc.h"
#include	"config.h"
#include 	"debug.h"
}

#include	"I2C.h"

#define I2C_MASTER_TX_BUF_DISABLE	0	/*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE	0	/*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN   0x1				/*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  0x0				/*!< I2C master will not check ack from slave */

extern	esp_err_t
initialize_i2c(void)
{
	esp_err_t	err;
    i2c_config_t conf;

	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA_PORT;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = SCL_PORT;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_FREQUENCY;
	conf.clk_flags = 0;
    if	( ( err = i2c_param_config(I2C_PORT, &conf) ) == ESP_OK )	{
		err = i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	}
	return	(err);
}

esp_err_t I2C::lock ( void )
{
	if ( xSemaphoreTake ( sem, portMAX_DELAY ) != pdTRUE ) {
		return ESP_ERR_TIMEOUT;
	}
	return ESP_OK;
}

void I2C::unlock ( void )
{
	xSemaphoreGive ( sem );
}

I2C::I2C(
	int	slaveAddress)
{
	address = slaveAddress & 0x7F;
	err_code = 0;

	sem = xSemaphoreCreateBinary ();
	if ( sem == NULL ) {
		abort();
	}
	xSemaphoreGive ( sem );
}

I2C::~I2C()
{
	vSemaphoreDelete ( sem );
}

void
I2C::i2c_slave_write(
	byte address,
	byte *buf,
	int size
)
{
	err_code = i2c_master_write_to_device ( I2C_PORT, address, (uint8_t *)buf, size, 1000 / portTICK_PERIOD_MS );
}

void
I2C::i2c_slave_read(
	byte address,
	byte command,
	byte *data,
	int size
)
{
	err_code = i2c_master_write_read_device ( I2C_PORT, address, &command, 1, data, size, 1000 / portTICK_PERIOD_MS );
}

// write - read pair without register#
void
I2C::write_low(
	byte *buf,
	int size
     )
{
	//i2c_slave_write ( address, buf, size );
	err_code = i2c_master_write_to_device ( I2C_PORT, address, (uint8_t *)buf, size, portTICK_PERIOD_MS );
}

void
I2C::read_low(
	byte *data,
	int size
     )
{
	err_code = i2c_master_read_from_device ( I2C_PORT, address, (uint8_t *)data, size, 1000/portTICK_PERIOD_MS );
}

void
I2C::write(
	byte	command,
	byte	*data,
	int		size)
{
	byte	buf[size+1];

	buf[0] = command;
	memcpy(&buf[1], data, size);
	i2c_slave_write(address, buf, size+1);
}

void
I2C::read(
	byte	command,
	byte	*data,
	int		size)
{
	memset(data, 0, size);
	i2c_slave_read(address, command, data, size);
}

void
I2C::writeByte(
	byte	command,
	byte	data)
{
	write(command, &data, 1);
}

byte
I2C::readByte(
	byte	command)
{
	byte ret = 0;
	read(command, &ret, 1);

	return ret;
}

void
I2C::writeWord(
	byte	command,
	uint16_t	data)
{
	write(command, (byte*)(&data), 2);
}

uint16_t
I2C::readWord(
	byte	command)
{
	uint16_t	hi
		,		lo;

	lo = readByte(command);
	hi = readByte(command+1);

	return(( hi << 8 ) | lo);
}

int16_t
I2C::readInt16(
	byte	command)
{
	int	ret;
	uint16_t	hi
		,		lo;

	lo = readByte(command);
	hi = readByte(command+1);
	ret = ( hi << 8 ) | lo;
	if		( ret > 32767 ) ret -= 65536;

	return ret;
}
#endif
