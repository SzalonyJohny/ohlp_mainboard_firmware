/* // @suppress("Lack of copyright information")
 * File:   BQ25895.h
 * Author: jan
 *
 * Created on April 15, 2020, 6:20 PM
 */

#ifndef BQ25895_H
#define	BQ25895_H


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32l4xx_hal.h"

#define BQaddress 0x6A<<1

enum {
	inc_50mA   =1,
	inc_100mA  =2,
	inc_200mA  =4,
	inc_400mA  =8,
	inc_800mA  =16,
	inc_1600mA =32,
};
enum {
	ivlo_100mV  =1,
	ivlo_200mV  =2,
	ivlo_400mV  =4,
	ivlo_800mV  =8,
	ivlo_1600mV =16,
};
enum {
	sysmin_3000mV =0,
	sysmin_100mV=2,
	sysmin_200mV=4,
	sysmin_400mV=8,
};
enum {
	ichg_64mA	=1,
	ichg_128mA	=2,
	ichg_256mA	=4,
	ichg_512mA	=8,
	ichg_1024mA	=16,
	ichg_2048mA	=32,
	ichg_4096mA =64,
};
enum {
	bmvr_4550mV = 0,
	bmvr_64mV   = 16,
	bmvr_128mV  = 32,
	bmvr_256mV  = 64,
	bmvr_512mV  = 128,
};
enum{
    uchg_3840mV = 0,
    uchg_512mV   = 128,
    uchg_256mV   = 64,
    uchg_128mV   = 32,
    uchg_64mV    = 16,
    uchg_32mV    = 8,
    uchg_16mV    = 4,
};
enum{
    vinpm_6400mV = 1<<6,
    vinpm_3200mV = 1<<5,
    vinpm_1600mV = 1<<4,
    vinpm_800mV = 1<<3,
    vinpm_400mV = 1<<2,
    vinpm_200mV = 1<<1,
    vinpm_100mV = 1<<0,
    vinpm_offset_2600mV = 0,
};

enum{
	iterm_512mA = 1<<4,
	iterm_256mA = 1<<3,
	iterm_128mA = 1<<2,
	iterm_64mA = 1<<1,
	iterm_offset_64mA = 0,
};

enum{
	iprechg_512mA = 1<<7,
	iprechg_256mA = 1<<6,
	iprechg_128mA = 1<<5,
	iprechg_64mA  = 1<<4,
	Iprechg_offset_64mA = 0,
};




class cBQ{

	private:
	uint8_t REG[16];		//od REG-Ox00 do REG-0x0D
	//REG 0x00
	bool EN_HIZ;
	bool EN_ILIM;		//ilim pin & hiz mode
	//REG 0x01
	bool BCOLD;
	bool BHOT[2];		// boost mode temperature monitor bits
	//REG 0x03
	bool battload;
	bool battcharge;
	bool boostmode;
	uint8_t battsysmin;
	//REG 0x04
	bool EN_PUMPX;
	//REG 0x05
	uint8_t precharge_current;
	uint8_t termination_current;

    bool BATLOWV;

    volatile uint8_t statusVBUS;

    I2C_HandleTypeDef *hi2c;


	public:

	cBQ();
	~cBQ() = default;
	cBQ(const cBQ&)= default;
	cBQ & operator= ( const cBQ&) = default;

	uint8_t get_statusVBUS();

	//REG 0x00
	void set_inputcurrent(uint8_t ic_mA);

	//REG 0x01
	void set_inputvoltagelimitofset(uint8_t ivlo_mV);

	//REG 0x02
	void set_startADCconv();
	//rest default

	//REG 0x03
	void set_battload(bool on);
	void set_battcharge(bool on);
	void set_boost_mode(bool on);
	void set_battsysmin(uint8_t sysmin_mV);

	//REG 0x04
	void set_chargecurrent(uint8_t ichg);

	//REG 0x05
	void set_precharge_current(uint8_t iprechg);
	void set_termination_current(uint8_t iterm);

    //REG 0x06
    void set_chargevoltage(uint8_t uichg);

	//REG 0x0A
	void set_boost_voltage(uint8_t bmvr);

    //REG 0x0B
    uint8_t read_charge_adapter();

	//REG 0x09
	void shipmode();

    //REG 0x0D
    void set_input_voltage_limit(uint8_t vindpm);

	//REG 0x0E
	uint16_t read_battvoltage();

	//REG 0x0F
	uint16_t read_sysvoltage();

	//REG 0x11
	uint16_t read_vbusvoltage();

	//REG 0x12
	uint16_t read_battcurrent();



    bool update_VBUS(bool bq_int_flag, uint16_t max_count);


	//All REGs
	void updateREG(uint8_t REGx);
	void updateREGs();
	void REGset(uint8_t, uint8_t);
	void init_BQ(I2C_HandleTypeDef *phic);
	uint8_t  BQRead_VALUE(unsigned char Reg);


};


#ifdef __cplusplus
}
#endif


#endif	/* BQ25895_H */

