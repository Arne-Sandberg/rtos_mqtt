#ifndef __I2C_SLAVE_PORT_H__
#define __I2C_SLAVE_PORT_H__


void i2c_slave_scl_int_en( void );
void i2c_slave_scl_int_dis( void );
void i2c_slave_sda_int_en( void );
void i2c_slave_sda_int_dis( void );

char i2c_slave_write_byte( unsigned char reg, unsigned char x );

char i2c_slave_read_byte( unsigned char reg, unsigned char *x_p );

#endif
