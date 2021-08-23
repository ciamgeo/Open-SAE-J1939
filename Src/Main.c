/*
 * Main.c
 *
 *  Created on: 16 juli 2021
 *      Author: Daniel Mårtensson
 */

#include <stdlib.h>
#include <stdio.h>

#include "ISO_11783/ISO_11783-7_Application_Layer/Application_Layer.h"
#include "Open_SAE_J1939/Open_SAE_J1939.h"
#include "SAE_J1939/SAE_J1939-71_Application_Layer/Application_Layer.h"
#include "SAE_J1939/SAE_J1939-73_Diagnostics_Layer/Diagnostics_Layer.h"
#include "SAE_J1939/SAE_J1939-81_Network_Management_Layer/Network_Management_Layer.h"

int main() {

	/* Create our J1939 structure with two ECU */
	J1939 j1939_1 = {0};
	J1939 j1939_2 = {0};

	/* Important to sent all non-address to 0xFF - Else we cannot use ECU address 0x0 */
	for(uint8_t i = 0; i < 255; i++){
		j1939_1.other_ECU_address[i] = 0xFF;
		j1939_2.other_ECU_address[i] = 0xFF;
	}

	/* Set the ECU address */
	j1939_1.this_ECU_address = 0x80;												/* From 0 to 253 because 254 = error address and 255 = broadcast address */
	j1939_2.this_ECU_address = 0xFD;

	/* Set the information about valve 1 for ECU 2 */
	uint8_t valve_number = 1;
	j1939_2.this_auxiliary_valve_measured_position[valve_number].measured_position_percent = 100;
	j1939_2.this_auxiliary_valve_measured_position[valve_number].measured_position_micrometer = 50000; 	/* 5 mm */
	j1939_2.this_auxiliary_valve_measured_position[valve_number].valve_state = VALVE_STATE_RETRACT;		/* Spool in reverse */

	/* Send a request from ECU 1 to ECU 2 */
	ISO_11783_Send_Request_Auxiliary_Valve_Measured_Position(&j1939_1, 0xFD, valve_number);

	/* Read the request for ECU 2*/
	Open_SAE_J1939_Listen_For_Messages(&j1939_2);

	/* Read the response request for ECU 1 */
	Open_SAE_J1939_Listen_For_Messages(&j1939_1);

	/* Display what we got */
	printf("Measured position percent = %i\nMeasured position micrometer = %i\nValve state = %i\nFrom ECU address = 0x%X"
			,j1939_1.from_other_ecu_auxiliary_valve_measured_position[valve_number].measured_position_percent
			,j1939_1.from_other_ecu_auxiliary_valve_measured_position[valve_number].measured_position_micrometer
			,j1939_1.from_other_ecu_auxiliary_valve_measured_position[valve_number].valve_state
			,j1939_1.from_other_ecu_auxiliary_valve_measured_position[valve_number].from_ecu_address);

	return 0;
}
