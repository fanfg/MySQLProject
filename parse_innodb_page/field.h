#ifndef FIELD_H
#define  FIELD_H
#include <iostream>



int16_t  generate_tinyint(const uint8_t * c, uint64_t start_position) {

	int16_t  result;
	// 1= non-negative, 0= negative
	if (c[start_position]) {
		result = (c[start_position] & 0x7f);
	}

	return  result;
}


int16_t  generate_smallint(const uint8_t * c, uint64_t start_position) {

	int16_t result;
	// 1= non-negative, 0= negative
	if (c[start_position]) {
		result = (c[start_position] & 0x7f) * 0xff + c[start_position + 1];
	}

	return  result;
}

int32_t  generate_mediumint(const uint8_t * c, uint64_t start_position) {

	int32_t result;


	result = (c[start_position] & 0x7f) * 0xffff + c[start_position + 1] * 0xff + c[start_position + 2];
	
	if (!c[start_position] >> 7)
		result = 0 - result;

	return  result;
}



int32_t  generate_int(const uint8_t * c, uint64_t start_position) {

	int32_t result;

	result = (c[start_position] & 0x7f) * 0xffffff + c[start_position + 1] * 0xffff + c[start_position + 2] * 0xff + c[start_position + 3];
	//first_bit 1= non-negative, 0= negative
	if (c[start_position] <=0x7f)
		result = 0 - result;
	return  result;
}

int64_t  generate_long(const uint8_t * c, uint64_t start_position) {

	int32_t result;
	// 1= non-negative, 0= negative
	if (c[start_position]) {
		result = (c[start_position] & 0x7f) * (1ULL << 56) + c[start_position + 1] * (1ULL << 48) + c[start_position + 2] * (1ULL << 40) + c[start_position + 3] * (1ULL << 32)
			+ c[start_position + 4] * (1ULL << 24) + c[start_position + 5] * (1ULL << 18) + c[start_position + 6] * (1ULL << 8) + c[start_position + 7] * 1;
	}

	return  result;
}


#endif // !FIELD_H


