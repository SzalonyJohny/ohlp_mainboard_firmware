#include <IMU_Gesture_Task.hpp>
#include "MPU6050.h"
#include "MPU6050.c"

#include "usb_device.h"
#include "usbd_cdc_if.h"

extern I2C_HandleTypeDef hi2c1;

// TODO remove -> use to debug
int16_t acc_x;
int16_t acc_y;
int16_t acc_z;

int16_t gyr_x;
int16_t gyr_y;
int16_t gyr_z;


void Start_IMU_Gesture_Task([[maybe_unused]] void const * argument){

	/* init */
	IMU_Init();


	/* USB Init */
	//MX_USB_DEVICE_Init();

	const uint32_t data_buff_size = 64;
	char data_buffer_usb[data_buff_size];


	// FIXME normal operation 240Hz
	const float send_rate = 0.1f;
	const float os_daley_time = floor(1000.0f/send_rate);

	for(;;){


		osDelay((uint32_t)os_daley_time);

		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		MPU6050_GetAccelerometerRAW(&acc_x, &acc_y, &acc_z);
		MPU6050_GetGyroscopeRAW(&gyr_x, &gyr_y,	 &gyr_z);

		snprintf( data_buffer_usb, data_buff_size, "%d,%d,%d,%d,%d,%d \n", acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z);
		/*
		auto status = CDC_Transmit_FS( (uint8_t*)data_buffer_usb, (uint16_t)strlen(data_buffer_usb) );

		if(USBD_OK != status){
			snprintf( data_buffer_usb, data_buff_size, "error");
			CDC_Transmit_FS( (uint8_t*)data_buffer_usb, (uint16_t)strlen(data_buffer_usb) );
		}

		*/

	}


}



void IMU_Init(){
	MPU6050_Init(&hi2c1);
	MPU6050_SetSleepEnabled(0);
	MPU6050_SetClockSource(MPU6050_CLOCK_INTERNAL);
	MPU6050_SetDlpf(MPU6050_DLPF_BW_20);
	MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
	MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_SetSleepEnabled(0);
	MPU6050_SetClockSource(MPU6050_CLOCK_INTERNAL);
	MPU6050_SetDlpf(MPU6050_DLPF_BW_20);
	MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
	MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	MPU6050_SetSleepEnabled(0);
	MPU6050_SetClockSource(MPU6050_CLOCK_INTERNAL);
	MPU6050_SetDlpf(MPU6050_DLPF_BW_20);
	MPU6050_SetFullScaleGyroRange(MPU6050_GYRO_FS_250);
	MPU6050_SetFullScaleAccelRange(MPU6050_ACCEL_FS_2);
}

