#include "IMU_Gesture_Task.h"

#include "MPU6050.h"
#include "MPU6050.c"

#include "usb_device.h"
#include "usbd_cdc_if.h"

extern I2C_HandleTypeDef hi2c1;

// TODO remove -> use to debug
float acc_x;
float acc_y;
float acc_z;

float gyr_x;
float gyr_y;
float gyr_z;


void Start_IMU_Gesture_Task([[maybe_unused]] void const * argument){

	/* init */
	IMU_Init();


	/* USB Init */
	MX_USB_DEVICE_Init();

	char data_buffer_usb[64];


	const float send_rate = 64;
	const float os_daley_time = floor(1000/send_rate);

	for(;;){


		osDelay((uint32_t)os_daley_time);

		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		MPU6050_GetAccelerometerScaled(&acc_x, &acc_y, &acc_z);
		MPU6050_GetGyroscopeScaled(&gyr_x, &gyr_y,	 &gyr_z);



		sprintf((data_buffer_usb), "a_x =  %d | a_y = %d | a_z = %d \r",
				acc_x*1000, acc_y*1000, acc_z*1000);

		CDC_Transmit_FS((uint8_t*)data_buffer_usb, strlen(data_buffer_usb));

		sprintf((data_buffer_usb), "g_x = %d | g_y = %d | g_z = %d \r \r",
				gyr_x*1000, gyr_y*1000, gyr_z*1000);

		CDC_Transmit_FS((uint8_t*)data_buffer_usb, strlen(data_buffer_usb));


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

