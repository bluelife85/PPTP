#include "pptp_data_type.h"

#include "debug_uart.h"
#include "system_tick.h"
#include "module_uart.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAGIC_COOKIE 0x1a2b3c4d

// PPTP message type
#define START_CONTROL_CONNECTION_REQUEST 1
#define START_CONTROL_CONNECTION_REPLY 2
#define STOP_CONTROL_CONNECTION_REQUEST 3
#define STOP_CONTROL_CONNECTION_REPLY 4
#define ECHO_REQUEST 5
#define ECHO_REPLY 6
#define OUTGOING_CALL_REQUEST 7
#define OUTGOING_CALL_REPLY 8
#define INCOMING_CALL_REQUEST 9
#define INCOMING_CALL_REPLY 10
#define INCOMING_CALL_CONNECTED 11
#define CALL_CLEAR_REQUEST 12
#define CALL_DISCONNECT_NOTIFY 13
#define WAN_ERROR_NOTIFY 14
#define SET_LINK_INFO 15

// Call-Disconnect-Notify Reuslt Codes
#define LOST_CARRIER 1
#define GENERAL_ERROR 2
#define ADMIN_SHUTDOWN 3
#define DISCONNECT_REQUEST 4

// Error Code list
#define GENERAL_ERROR_NO 0
#define GENERAL_ERROR_NOT_CONNECTED 1
#define GENERAL_ERROR_BAD_FORMAT 2
#define GENERAL_ERROR_BAD_VALUE 3
#define GENERAL_ERROR_NO_RESOURCE 4
#define GENERAL_ERROR_BAD_ID 5
#define GENERAL_ERROR_PAC_ERR 6

static uint8_t pptp_ring_buffer[512];
static uint32_t pptp_ring_index;
static struct data_package* ReceivePacket;
static uint8_t initiates;
static uint16_t call_id;
static uint16_t peer_id;
static uint16_t call_serial = 1;

void error_code_proc(uint8_t err){
	
	Output.puts("Error code : ");
	switch(err){
		case GENERAL_ERROR_NO:
			Output.puts("no error\r\n");
			break;
		case GENERAL_ERROR_NOT_CONNECTED:
			Output.puts("not connected\r\n");
			break;
		case GENERAL_ERROR_BAD_FORMAT:
			Output.puts("bad format\r\n");
			break;
		case GENERAL_ERROR_BAD_VALUE:
			Output.puts("bad value\r\n");
			break;
		case GENERAL_ERROR_NO_RESOURCE:
			Output.puts("no resource\r\n");
			break;
		case GENERAL_ERROR_BAD_ID:
			Output.puts("bad id\r\n");
			break;
		case GENERAL_ERROR_PAC_ERR:
			Output.puts("PAC error\r\n");
			break;
	}
}

void request_start_connection(void){
	
	struct start_connection_req pkt;
	
	memset(&pkt, 0x00, sizeof(struct start_connection_req));
	
	pkt.header.len = htons(156);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(START_CONTROL_CONNECTION_REQUEST);
	pkt.version = htons(0x0100);
	pkt.framing = htonl(1);
	pkt.bearer = htonl(1);
	memcpy(pkt.vendor,"Microsoft",9);
	
	Dev.puts((uint8_t*)&pkt, 156);
}

void request_outgoing_call(void){
	
	struct outgoing_req pkt;
	
	memset(&pkt, 0x00, sizeof(struct outgoing_req));
	
	pkt.header.len = htons(168);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(OUTGOING_CALL_REQUEST);
	pkt.call_id = htons(call_id);
	pkt.serial_id = htons(call_serial);
	pkt.min_bps = htonl(300);
	pkt.max_bps = htonl(100000000);
	pkt.bearer_type = htonl(3);
	pkt.framinig_type = htonl(3);
	pkt.window_size = htons(64);
	pkt.delay = htons(0);
	
	Dev.puts((uint8_t*)&pkt, 168);
}

uint16_t reply_outgoing_call(uint8_t* data){
	
	uint16_t peer;
	struct outgoing_rep* pkt = (struct outgoing_rep*)data;
	struct set_link_info req;
	struct gre_lcp_cli lcp;
	
	peer = htons(pkt->call_id);
	
	memset(&req, 0x00, 24);
	
	req.header.len = htons(24);
	req.header.pptp_type = htons(1);
	req.header.magic_cookie = htonl(MAGIC_COOKIE);
	req.header.control_type = htons(SET_LINK_INFO);
	req.peer_id = htons(peer);
	req.send_accm = 0xffffffff;
	req.receive_accm = 0xffffffff;
	
	Dev.puts((uint8_t*)&req, 24);
	
	memset(&lcp,0x00,sizeof(struct gre_lcp_cli));
	lcp.flag = htons(0x3001);
	lcp.prot_type = htons(0x880b);
	lcp.key = htonl((0x00190000|htons(peer)));
	lcp.sequence = 0;
	lcp.addr = 0xff;
	lcp.ctrl = 0x03;
	lcp.protocol = htons(0xc021);
	lcp.code = 1;
	lcp.identifier = 0;
	lcp.len = 21;
	lcp.max_recv_type = 1;
	lcp.max_recv_len = 4;
	lcp.max_recv_unit = htons(1400);
	lcp.magic_type = 5;
	lcp.magic_len = 6;
	lcp.magic_number = (uint32_t)(&call_id) + get_systick();
	lcp.pfc_type = 7;
	lcp.pfc_len = 2;
	lcp.acfc_type = 8;
	lcp.acfc_len = 2;
	lcp.cb_type = 13;
	lcp.cb_len = 3;
	lcp.cb_operation = 6;
	
	Dev.puts((uint8_t*)&lcp, sizeof(struct gre_lcp_cli));
	
	return peer;
}

void reply_echo(uint8_t* data){
	struct echo_req* req = (struct echo_req*)data;
	struct echo_rep pkt;
	
	memset(&pkt, 0x00, 20);
	pkt.header.len = htons(20);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(ECHO_REPLY);
	pkt.result = 1;
	pkt.err_code = 0;
	pkt.identifier = req->identifier;
	
	Dev.puts((uint8_t*)&pkt, 20);
}

void request_stop_connection(void){
	
	struct stop_connection_req pkt;
	
	memset(&pkt, 0x00, sizeof(struct stop_connection_rep));
	
	pkt.header.len = htons(32);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(STOP_CONTROL_CONNECTION_REQUEST);
	pkt.reason = 1;
	
	Dev.puts((uint8_t*)&pkt, 32);
}

void reply_stop_connection(uint8_t* data){
	
	struct stop_connection_req* p = (struct stop_connection_req*)data;
	struct stop_connection_rep pkt;
	
	Output.puts("--Stop request\r\n");
	Output.puts("Reason : \r\n");
	switch(p->reason){
		case 1:
			Output.puts("None\r\n");
			break;
		case 2:
			Output.puts("Stop-Protocol\r\n");
			break;
		case 3:
			Output.puts("Stop-Local-Shutdown\r\n");
			break;
	}
	
	memset(&pkt, 0x00, sizeof(struct stop_connection_rep));
	
	pkt.header.len = htons(16);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(STOP_CONTROL_CONNECTION_REPLY);
	pkt.result = 1;
	pkt.err_code = 0;
	
	Dev.puts((uint8_t*)&pkt, 16);
}

void call_disconnect_noti_pkt(uint8_t* data){
	
	struct disconnect_noti* pkt = (struct disconnect_noti*)data;
	
	pkt->cause_code = htons(pkt->cause_code);
	
	Output.puts("--Disconnect notified\r\n");
	Output.puts("Result Code : ");
	switch(pkt->result_code){
		case LOST_CARRIER:
			Output.puts("lost carrir\r\n");
			break;
		case GENERAL_ERROR:
			Output.puts("see error code\r\n");
			error_code_proc(pkt->err_code);
			break;
		case ADMIN_SHUTDOWN:
			Output.puts("admin shutdown\r\n");
			break;
		case DISCONNECT_REQUEST:
			Output.puts("disconnect request\r\n");
			break;
	}
}

void noti_call_disconn(void){
	
	struct disconnect_noti pkt;
	
	memset(&pkt, 0x00, sizeof(struct disconnect_noti));
	
	pkt.header.len = htons(148);
	pkt.header.pptp_type = htons(1);
	pkt.header.magic_cookie = htonl(MAGIC_COOKIE);
	pkt.header.control_type = htons(CALL_DISCONNECT_NOTIFY);
	pkt.call_id = htons(call_id);
	pkt.result_code = 4;
	pkt.err_code = 0;
	pkt.cause_code = 0;
	
	Dev.puts((uint8_t*)&pkt,148);
}

struct data_package* search_last(void){
	
	struct data_package* pkt = ReceivePacket;
	
	while(pkt->next != null)
		pkt = pkt->next;
	
	return pkt;
}

void pptp_get_pkt(uint8_t c){
	
	uint16_t len = 0;
	struct data_package* prior;
	struct data_package* npkt;
	
	pptp_ring_buffer[pptp_ring_index++] = c;
	
	if(pptp_ring_index >= 3){
		
		len = (pptp_ring_buffer[0] << 8) | pptp_ring_buffer[1];
	}
	
	if((len != 0) && (len == pptp_ring_index)){
		
		if(ReceivePacket == null){
			
			ReceivePacket = (struct data_package*)malloc(sizeof(struct data_package));
			memset(ReceivePacket,0x00,sizeof(struct data_package));
			ReceivePacket->data = (uint8_t*)malloc(pptp_ring_index);
			memcpy(ReceivePacket->data,pptp_ring_buffer,pptp_ring_index);
		}
		else{
			npkt = (struct data_package*)malloc(sizeof(struct data_package));
			memset(npkt, 0x00, sizeof(struct data_package));
			npkt->data = (uint8_t*)malloc(sizeof(struct data_package));
			npkt->next = null;
			memcpy(npkt->data,pptp_ring_buffer,pptp_ring_index);
			prior = search_last();
			prior->next = npkt;
		}
		
		pptp_ring_index = 0;
		memset(pptp_ring_buffer, 0x00, 512);
	}
}

void pptp_process(void){
	
	uint16_t control_message = 0;
	struct data_package* next;
	uint8_t* pkt;
	
	if(ReceivePacket != null){
		
		next = ReceivePacket->next;
		pkt = (uint8_t*)ReceivePacket->data;
		control_message = (pkt[8] << 8) | pkt[9];
		
		switch(control_message){
			case START_CONTROL_CONNECTION_REQUEST:
				Output.puts("--start connection request\r\n");
				break;
			case START_CONTROL_CONNECTION_REPLY:
				Output.puts("--start connection reply\r\n");
				request_outgoing_call();
				break;
			case STOP_CONTROL_CONNECTION_REQUEST:
				{
					reply_stop_connection(pkt);
				}
				break;
			case STOP_CONTROL_CONNECTION_REPLY:
				{
					Output.puts("-- reply stop connection\r\n");
				}
				break;
			case ECHO_REQUEST:
				Output.puts("--echo request\r\n");
				reply_echo(pkt);
				break;
			case ECHO_REPLY:
				Output.puts("--echo reply\r\n");
				break;
			case OUTGOING_CALL_REQUEST:
				Output.puts("--outgoing call request\r\n");
				break;
			case OUTGOING_CALL_REPLY:
				Output.puts("--outgoing call reply\r\n");
				peer_id = reply_outgoing_call(pkt);
				break;
			case INCOMING_CALL_REQUEST:
				Output.puts("--incoming call request\r\n");
				break;
			case INCOMING_CALL_REPLY:
				Output.puts("--incoming call reply\r\n");
				break;
			case INCOMING_CALL_CONNECTED:
				Output.puts("--incoming call connected\r\n");
				break;
			case CALL_CLEAR_REQUEST:
				Output.puts("--call clear request\r\n");
				noti_call_disconn();
				break;
			case CALL_DISCONNECT_NOTIFY:
				{
					call_disconnect_noti_pkt(pkt);
					request_stop_connection();
				}
				break;
			case WAN_ERROR_NOTIFY:
				noti_call_disconn();
				break;
			case SET_LINK_INFO:
				Output.puts("--set link info\r\n");
				break;
			default:
				Output.puts("unknown message code : ");
				Output.puthex(&control_message,2);
				break;
		}
		
		free(ReceivePacket->data);
		free(ReceivePacket);
		ReceivePacket = next;
	}
	else{
		
		if(!initiates && !(GPIOB->IDR & GPIO_Pin_7)){
			initiates = 1;
			call_serial++;
			call_id = rand() % 65535;
			Output.puts("request start connection\r\n");
			request_start_connection();
		}
		
		if(initiates && (GPIOB->IDR & GPIO_Pin_7)){
			initiates = 0;
		}
	}
}
