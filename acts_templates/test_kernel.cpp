// #include "common.h"

// typedef ap_uint<512> HBM_channelAXI_t;

#define VDATA_SIZE 16

// TRIPCOUNT indentifier
const unsigned int c_dt_size = VDATA_SIZE;

typedef struct HBM_channelAXI_t { unsigned int data[VDATA_SIZE]; } v_dt;

//////////////////////////////////////////////////////////////////
extern "C" {
void top_function(HBM_channelAXI_t * HBM_channelA, HBM_channelAXI_t * HBM_channelB, HBM_channelAXI_t * HBM_centerA, HBM_channelAXI_t * HBM_centerB){	
#pragma HLS INTERFACE m_axi port = HBM_channelA offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = HBM_channelB offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = HBM_centerA offset = slave bundle = gmem2
#pragma HLS INTERFACE m_axi port = HBM_centerB offset = slave bundle = gmem3

#pragma HLS INTERFACE s_axilite port = HBM_channelA bundle = control
#pragma HLS INTERFACE s_axilite port = HBM_channelB bundle = control
	
#pragma HLS INTERFACE s_axilite port = HBM_centerA bundle = control
#pragma HLS INTERFACE s_axilite port = HBM_centerB bundle = control

#pragma HLS INTERFACE s_axilite port=return bundle=control

	unsigned int data[16];
	unsigned int offset = 2345;
	
	data[0] = HBM_centerA[offset].data[0];
	data[1] = HBM_centerA[offset].data[1];
	data[2] = HBM_centerA[offset].data[2];
	data[3] = HBM_centerA[offset].data[3];
	data[4] = HBM_centerA[offset].data[4];
	data[5] = HBM_centerA[offset].data[5];
	data[6] = HBM_centerA[offset].data[6];
	data[7] = HBM_centerA[offset].data[7];
	data[8] = HBM_centerA[offset].data[8];
	data[9] = HBM_centerA[offset].data[9];
	data[10] = HBM_centerA[offset].data[10];
	data[11] = HBM_centerA[offset].data[11];
	data[12] = HBM_centerA[offset].data[12];
	data[13] = HBM_centerA[offset].data[13];
	data[14] = HBM_centerA[offset].data[14];
	data[15] = HBM_centerA[offset].data[15];
	data[16] = HBM_centerB[offset].data[0];
	data[17] = HBM_centerB[offset].data[1];
	data[18] = HBM_centerB[offset].data[2];
	data[19] = HBM_centerB[offset].data[3];
	data[20] = HBM_centerB[offset].data[4];
	data[21] = HBM_centerB[offset].data[5];
	data[22] = HBM_centerB[offset].data[6];
	data[23] = HBM_centerB[offset].data[7];
	data[24] = HBM_centerB[offset].data[8];
	data[25] = HBM_centerB[offset].data[9];
	data[26] = HBM_centerB[offset].data[10];
	data[27] = HBM_centerB[offset].data[11];
	data[28] = HBM_centerB[offset].data[12];
	data[29] = HBM_centerB[offset].data[13];
	data[30] = HBM_centerB[offset].data[14];
	data[31] = HBM_centerB[offset].data[15];
	
	HBM_channelA[offset].data[0] = data[0];
	HBM_channelA[offset].data[1] = data[1];
	HBM_channelA[offset].data[2] = data[2];
	HBM_channelA[offset].data[3] = data[3];
	HBM_channelA[offset].data[4] = data[4];
	HBM_channelA[offset].data[5] = data[5];
	HBM_channelA[offset].data[6] = data[6];
	HBM_channelA[offset].data[7] = data[7];
	HBM_channelA[offset].data[8] = data[8];
	HBM_channelA[offset].data[9] = data[9];
	HBM_channelA[offset].data[10] = data[10];
	HBM_channelA[offset].data[11] = data[11];
	HBM_channelA[offset].data[12] = data[12];
	HBM_channelA[offset].data[13] = data[13];
	HBM_channelA[offset].data[14] = data[14];
	HBM_channelA[offset].data[15] = data[15];
	HBM_channelB[offset].data[0] = data[16];
	HBM_channelB[offset].data[1] = data[17];
	HBM_channelB[offset].data[2] = data[18];
	HBM_channelB[offset].data[3] = data[19];
	HBM_channelB[offset].data[4] = data[20];
	HBM_channelB[offset].data[5] = data[21];
	HBM_channelB[offset].data[6] = data[22];
	HBM_channelB[offset].data[7] = data[23];
	HBM_channelB[offset].data[8] = data[24];
	HBM_channelB[offset].data[9] = data[25];
	HBM_channelB[offset].data[10] = data[26];
	HBM_channelB[offset].data[11] = data[27];
	HBM_channelB[offset].data[12] = data[28];
	HBM_channelB[offset].data[13] = data[29];
	HBM_channelB[offset].data[14] = data[30];
	HBM_channelB[offset].data[15] = data[31];
	
	// data[0] = HBM_centerA[offset].range(31, 0);
	// data[1] = HBM_centerA[offset].range(63, 32);
	// data[2] = HBM_centerA[offset].range(95, 64);
	// data[3] = HBM_centerA[offset].range(127, 96);
	// data[4] = HBM_centerA[offset].range(159, 128);
	// data[5] = HBM_centerA[offset].range(191, 160);
	// data[6] = HBM_centerA[offset].range(223, 192);
	// data[7] = HBM_centerA[offset].range(255, 224);
	// data[8] = HBM_centerA[offset].range(287, 256);
	// data[9] = HBM_centerA[offset].range(319, 288);
	// data[10] = HBM_centerA[offset].range(351, 320);
	// data[11] = HBM_centerA[offset].range(383, 352);
	// data[12] = HBM_centerA[offset].range(415, 384);
	// data[13] = HBM_centerA[offset].range(447, 416);
	// data[14] = HBM_centerA[offset].range(479, 448);
	// data[15] = HBM_centerA[offset].range(511, 480);
	// data[16] = HBM_centerB[offset].range(31, 0);
	// data[17] = HBM_centerB[offset].range(63, 32);
	// data[18] = HBM_centerB[offset].range(95, 64);
	// data[19] = HBM_centerB[offset].range(127, 96);
	// data[20] = HBM_centerB[offset].range(159, 128);
	// data[21] = HBM_centerB[offset].range(191, 160);
	// data[22] = HBM_centerB[offset].range(223, 192);
	// data[23] = HBM_centerB[offset].range(255, 224);
	// data[24] = HBM_centerB[offset].range(287, 256);
	// data[25] = HBM_centerB[offset].range(319, 288);
	// data[26] = HBM_centerB[offset].range(351, 320);
	// data[27] = HBM_centerB[offset].range(383, 352);
	// data[28] = HBM_centerB[offset].range(415, 384);
	// data[29] = HBM_centerB[offset].range(447, 416);
	// data[30] = HBM_centerB[offset].range(479, 448);
	// data[31] = HBM_centerB[offset].range(511, 480);
	
	// HBM_centerA[offset].range(31, 0) = data[0];
	// HBM_centerA[offset].range(63, 32) = data[1];
	// HBM_centerA[offset].range(95, 64) = data[2];
	// HBM_centerA[offset].range(127, 96) = data[3];
	// HBM_centerA[offset].range(159, 128) = data[4];
	// HBM_centerA[offset].range(191, 160) = data[5];
	// HBM_centerA[offset].range(223, 192) = data[6];
	// HBM_centerA[offset].range(255, 224) = data[7];
	// HBM_centerA[offset].range(287, 256) = data[8];
	// HBM_centerA[offset].range(319, 288) = data[9];
	// HBM_centerA[offset].range(351, 320) = data[10];
	// HBM_centerA[offset].range(383, 352) = data[11];
	// HBM_centerA[offset].range(415, 384) = data[12];
	// HBM_centerA[offset].range(447, 416) = data[13];
	// HBM_centerA[offset].range(479, 448) = data[14];
	// HBM_centerA[offset].range(511, 480) = data[15];
	// HBM_centerB[offset].range(31, 0) = data[16];
	// HBM_centerB[offset].range(63, 32) = data[17];
	// HBM_centerB[offset].range(95, 64) = data[18];
	// HBM_centerB[offset].range(127, 96) = data[19];
	// HBM_centerB[offset].range(159, 128) = data[20];
	// HBM_centerB[offset].range(191, 160) = data[21];
	// HBM_centerB[offset].range(223, 192) = data[22];
	// HBM_centerB[offset].range(255, 224) = data[23];
	// HBM_centerB[offset].range(287, 256) = data[24];
	// HBM_centerB[offset].range(319, 288) = data[25];
	// HBM_centerB[offset].range(351, 320) = data[26];
	// HBM_centerB[offset].range(383, 352) = data[27];
	// HBM_centerB[offset].range(415, 384) = data[28];
	// HBM_centerB[offset].range(447, 416) = data[29];
	// HBM_centerB[offset].range(479, 448) = data[30];
	// HBM_centerB[offset].range(511, 480) = data[31];
		
}
}
