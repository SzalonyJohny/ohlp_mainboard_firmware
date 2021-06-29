#include <BQ25895.hpp>


void cBQ::set_inputcurrent(uint8_t icn_mA){
	REG[0x00] = static_cast<uint8_t>(icn_mA | (EN_HIZ<<7) | (EN_ILIM<<6));
	updateREG(0x00);
}

void cBQ::set_inputvoltagelimitofset(uint8_t ivlo_mV){
	REG[0x01] = static_cast<uint8_t>( ivlo_mV | (BCOLD<<5) | (BHOT[0]<<6) | (BHOT[1]<<7) );
	updateREG(0x01);
}

void cBQ::set_startADCconv(){
	REG[0x02] = 0b10011101;
	updateREG(0x02);
}

void cBQ::set_battload(bool on){
	battload = on;
	REG[0x03] = static_cast<uint8_t>((battload<<7) | (boostmode<<5) | (battcharge<<4) | battsysmin);
	updateREG(0x03);
}

void cBQ::set_boost_mode(bool on){
	boostmode = on;
	REG[0x03] = 0;
	REG[0x03] = static_cast<uint8_t>((battload<<7) | (boostmode<<5) | (battcharge<<4) | (battsysmin));
	updateREG(0x03);
}

void cBQ::set_battcharge(bool on){
	battcharge = on;
	REG[0x03] = static_cast<uint8_t>((battload<<7) | (boostmode<<5) | (battcharge<<4) | battsysmin);
	updateREG(0x03);
}

void cBQ::set_battsysmin(uint8_t sysmin){
	battsysmin = sysmin;
	REG[0x03] = static_cast<uint8_t>((battload<<7) | (boostmode<<5) | (battcharge<<4) | battsysmin);
	updateREG(0x03);
}

void cBQ::set_chargecurrent(uint8_t ichg){
	REG[0x04] = static_cast<uint8_t>(ichg | (EN_PUMPX<<7));
	updateREG(0x04);
}

void cBQ::set_precharge_current(uint8_t ipregchg){
	precharge_current = ipregchg;
	REG[0x05] = (ipregchg) | termination_current;
	updateREG(0x05);
}

void cBQ::set_termination_current(uint8_t iterm){
	termination_current = iterm;
	REG[0x05] = (precharge_current) | iterm;
	updateREG(0x05);
}


void cBQ::set_chargevoltage(uint8_t uchg){
	REG[0x06] = static_cast<uint8_t>(uchg | BATLOWV<<1 | 0<<0);
	updateREG(0x06);
}

void cBQ:: set_boost_voltage(uint8_t bmvr){
	REG[0x0A] = bmvr;
	updateREG(0x0A);
}

uint8_t cBQ::get_charge_adapter(){
	uint8_t charge_status = BQRead_VALUE(0x0B);
	return static_cast<uint8_t>(charge_status>>5);
}

void cBQ::shipmode(){
	REGset(0x09,0b01100100);
}

void cBQ::set_input_voltage_limit(uint8_t ivdpm){
	REGset(0x0D,(1<<7) | ivdpm);
}

uint16_t cBQ::get_battvoltage(){
	set_startADCconv();
	uint16_t reg_value = BQRead_VALUE(0x0E);
	return static_cast<uint16_t>(reg_value*20+2304);
}

uint16_t cBQ::get_sysvoltage(){
	set_startADCconv();
	uint16_t reg_value = BQRead_VALUE(0x0F);
	return static_cast<uint16_t>(reg_value*20+2304);
}

uint16_t cBQ::get_vbusvoltage(){
	set_startADCconv();
	uint16_t reg_value = BQRead_VALUE(0x11);
	return static_cast<uint16_t>((reg_value-(reg_value>>7)*128)*100 + 2600);

}

uint16_t cBQ::get_battcurrent(){
	set_startADCconv();
	uint16_t reg_value = BQRead_VALUE(0x12);
	return static_cast<uint16_t>(reg_value*50);
}

uint8_t cBQ::BQRead_VALUE(unsigned char Reg){
	uint8_t reciveBuffer = 0;
	HAL_I2C_Mem_Read(hi2c, BQaddress, Reg, 1, &reciveBuffer, 1, 10);
	return (reciveBuffer);
}

void cBQ::updateREG(uint8_t REGx){
	REGset(REGx,REG[REGx]);
}

cBQ::cBQ(){
	REG[0x00] = 0b00001000;// HIZmode & ilim & Imput Current LIMIT;
	REG[0x01] = 0b00001110;// boost temp mode monitor & imput voltage limit offset
	REG[0x02] = 0b00111101;// ADCconv settings & DCP & D+/D- & Boost f mode
	REG[0x03] = 0b00110000;//  boost_on & charge &
	REG[0x04] = 0b00001100;//charge current setting
	REG[0x05] = 0b00010011;
	REG[0x06] = 0b01011110;
	REG[0x07] = 0b10011101;

	EN_HIZ = 0;
	EN_ILIM = 0;
	BCOLD = 1;
	BHOT[0]=1;
	BHOT[1]=1;
	battload=0;
	battcharge=1;
	boostmode=1;
	battsysmin=0;
	EN_PUMPX = 1;
	statusVBUS = 7;
	BATLOWV = 1; //Precharge
	termination_current = 0;
	precharge_current = 0;
	hi2c = nullptr;
}

void cBQ::updateREGs(){
	//	uint8_t tempbuff[18]={};
	//	tempbuff[0] = 0x00;
	//	for(int i=0;i<7;i++)tempbuff[i+1]=REG[i];
	//	TWI_MasterWriteRead(&twiMaster,BQaddress,&tempbuff[0],2,0);
	//	while (twiMaster.status != TWIM_STATUS_READY) {}
}

void cBQ::REGset(uint8_t reg_number, uint8_t value){
	uint8_t temp[2] = {value, 0};
	if(reg_number<16)REG[reg_number] = value;
	HAL_I2C_Mem_Write(hi2c, BQaddress, reg_number, 1, &temp[0], 1, 10);
}



void cBQ::init_BQ(I2C_HandleTypeDef *phic){
	hi2c = phic;
	// Reset to default setting
	REGset(0x14,0b10111001);
	set_inputcurrent(inc_1600mA | inc_800mA | inc_400mA | inc_200mA | inc_100mA | inc_50mA); //3.25A max input current
	set_battsysmin(sysmin_3000mV);
	set_inputvoltagelimitofset(ivlo_1600mV | ivlo_800mV | ivlo_400mV | ivlo_200mV | ivlo_100mV);

	//charge current = 3072mA
	set_chargecurrent(ichg_2048mA | ichg_1024mA);
	// charge voltage = 4192mV
	set_chargevoltage(uchg_3840mV | uchg_256mV | uchg_64mV | uchg_32mV);
	// mosfet driver supply voltage measured at 4.84V
	set_boost_voltage(bmvr_4550mV | bmvr_256mV);

	set_precharge_current(iprechg_512mA | Iprechg_offset_64mA);
	set_termination_current(iterm_64mA | iterm_offset_64mA);

	set_input_voltage_limit(vinpm_6400mV | vinpm_3200mV | vinpm_1600mV | vinpm_800mV |vinpm_400mV | vinpm_200mV | vinpm_100mV);

	REGset(0x0D,0b01100000);

	REGset(0x07,0b11000001);
	REGset(0x08,0b00000011);
	REGset(0x09,0b01000100);
	set_startADCconv(); // to setup REG 0x02;
	update_VBUS(1,400); // update status
}


bool cBQ::update_VBUS(bool bq_int_flag, uint16_t max_count = 500){



	if(bq_int_flag)counter_vbus_update = 1;
	if(counter_vbus_update!=0){
		counter_vbus_update++;
		if(counter_vbus_update<max_count){
			statusVBUS = static_cast<uint8_t>(BQRead_VALUE(0x0B) >> 5);
		}
		else {
			counter_vbus_update=0;
		}
	}
	return counter_vbus_update;
}



uint8_t cBQ::get_statusVBUS(){
	return statusVBUS;
}
