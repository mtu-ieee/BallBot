//#include <i2c.h>
#define WRITE 0
#define READ 1

#define X 0
#define Y 1

typedef struct {
	int x;
	int y;
	int z;
} accel_t;

typedef struct {
	int x;
	int y;
	int z;
} gyro_t;

typedef struct {
	accel_t accel;	
	gyro_t gyro;
} IMU_data_t;

//Initialize our IMU
void init() {

	//Turn on WMP and nunchuck
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x53, WRITE);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0xFE);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0x05);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_END);

	//Initialize WMP
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x53, WRITE);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0xF0);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0x55);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_END);

	//Set Read Address
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x52, WRITE);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0xFA);

	//Get the id for the accessory
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x52, READ);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	unsigned long tmp[6];
	tmp[0] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[1] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[2] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[3] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[4] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[5] = I2CMasterDataGet(I2C3_MASTER_BASE);
	unsigned long sum = tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5];
	if (sum == 0xCE) { //All good to go
		//nothing for now, change system status maybe?
	} else { //What is this thing?
		//Do debug print of sum here?
		exit(1);
	}
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_END);
}

//Read the IMU data
IMU_data_t readIMU() {

	//Prime sensor
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x52, WRITE);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	I2CMasterDataPut(I2C3_MASTER_BASE, 0x00);

	//Get data
	I2CMasterSlaveAddrSet(I2C3_MASTER_BASE, 0x52, READ);
	I2CMasterControl(I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	unsigned long tmp[6];
	tmp[0] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[1] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[2] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[3] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[4] = I2CMasterDataGet(I2C3_MASTER_BASE);
	tmp[5] = I2CMasterDataGet(I2C3_MASTER_BASE);

	unsigned long tmp[6];
	IMU_data_t data;
	data.accel.x = (tmp[2] << 2) + ((tmp[5] >> 3) & 2);
	data.accel.y = (tmp[3] << 2) + ((tmp[5] >> 4) & 2);
	data.accel.z = (tmp[4] << 2) + ((tmp[5] >> 5) & 6);
	data.gyro.z = (((tmp[5] & 0xFC)<<6) + tmp[0]);
	data.gyro.x = (((tmp[4] & 0xFC)<<6) + tmp[1]);
	data.gyro.y = (((tmp[3] & 0xFC)<<6) + tmp[2]);

	return data;
}

//Step the given motor!
void step(unsigned char motor, int steps) {
	unsigned char direction = 1;
	if (steps < 0) {
		direction = 0;
		steps = steps * -1;
	}

}

//Put PID controls in here
void doPID() {
	//Get IMU data
	readIMU();

	//After processing our IMU data, step the motors!
	//step(FORWARD,X, stepsx);//Just an example
	//step(BACKWARD,Y, stepsy);//Just an example
}

//Main loop...should be good enough
int main() {
	init();
	while(1)
		doPID();
	return 0;
}
