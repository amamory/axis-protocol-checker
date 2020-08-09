/*
 * hermes_test.c
 *
 *  Created on: 7 de jun de 2020
 *      Author: lsa
 */

#include "xaxidma.h"
#include "xparameters.h"
#include "sleep.h"
#include "xil_cache.h"
//#include "platform.h"

u32 checkHalted(u32 baseAddress,u32 offset);

int main(){
	//init_platform();
	u32 hermes_pkg[] = {0x0001,0x0001,0x0003};
	u32 hermes_pkg_in[10] = {0};
    u32 status;

	XAxiDma_Config *myDmaConfig;
	XAxiDma myDma;

	myDmaConfig = XAxiDma_LookupConfigBaseAddr(XPAR_AXIDMA_0_BASEADDR);
	status = XAxiDma_CfgInitialize(&myDma, myDmaConfig);
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n");
		return -1;
	}
	print("DMA initialization success..\n");
	sleep(5);
	print("done ! \n");

	/*
	 * Primary high-speed DMA data movement between system memory and stream target
	 * is through the AXI4 Read Master to AXI4 memory-mapped to stream (MM2S) Master,
	 * and AXI stream to memory-mapped (S2MM) Slave to AXI4 Write Master.
     *  - 0x04 - MM2S DMA Status register --> write into hermes' slave local port
	 *  - 0x34 - S2MM DMA Status register --> hermes' master local port writes into the memory
	 */
	status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x4);
	xil_printf("Status before data transfer %0x\n",status);
	Xil_DCacheFlushRange((u32)hermes_pkg,3*sizeof(u32));

	status = XAxiDma_SimpleTransfer(&myDma, (u32)hermes_pkg_in, 3*sizeof(u32),XAXIDMA_DEVICE_TO_DMA);
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n");
		return -1;
	}

	status = XAxiDma_SimpleTransfer(&myDma, (u32)hermes_pkg, 3*sizeof(u32),XAXIDMA_DMA_TO_DEVICE);
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n");
		return -1;
	}
	print("DMA configured ...\n");

    status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x4);
    while(status != 1){
    	status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x4);
    }

    status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x34);
    while(status != 1){
    	status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x34);
    }


/*

    hermes_pkg[0] = 0x0021;

	xil_printf("Status2 before data transfer %0x\n",status);
	//Xil_DCacheFlushRange((u32)hermes_pkg,3*sizeof(u32));
	//status = XAxiDma_SimpleTransfer(&myDma, (u32)hermes_pkg2, 3*sizeof(u32),XAXIDMA_DEVICE_TO_DMA);

	status = XAxiDma_SimpleTransfer(&myDma, (u32)hermes_pkg, 3*sizeof(u32),XAXIDMA_DMA_TO_DEVICE);
	if(status != XST_SUCCESS){
		print("DMA initialization failed\n");
		return -1;
	}
	print("DMA configured ...\n");

    status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x4);
    while(status != 1){
    	status = checkHalted(XPAR_AXIDMA_0_BASEADDR,0x4);
    }
*/
/*
*/
	print("DMA transfer success..\n");
	for(int i=0;i<8;i++)
		xil_printf("%0x\n",hermes_pkg_in[i]);


    //cleanup_platform();
    return 0;
}


u32 checkHalted(u32 baseAddress,u32 offset){
	u32 status;
	status = (XAxiDma_ReadReg(baseAddress,offset))&XAXIDMA_HALTED_MASK;
	return status;
}
