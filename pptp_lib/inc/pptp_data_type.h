#ifndef PPTP_STRUCT_DEFINITION_H_
#define PPTP_STRUCT_DEFINITION_H_

#include "stm32f10x.h"

#define null (void*)0

#define htonl(a) (((a & 0xff000000) >> 24) | ((a & 0x00ff0000) >> 8) | ((a & 0x0000ff00) << 8) | ((a & 0x000000ff) << 24))
#define htons(a) (((a & 0xff00) >> 8) | ((a & 0x00ff) << 8))

struct data_package{
	void* data;
	void* next;
};

struct header {
	uint16_t len;
	uint16_t pptp_type;
	uint32_t magic_cookie;
	uint16_t control_type;
	uint16_t reserved;
};

struct start_connection_req{
	struct header header;
	uint16_t version;
	uint16_t reserved;
	uint32_t framing;
	uint32_t bearer;
	uint16_t channel;
	uint16_t revision;
	uint8_t host[64];
	uint8_t vendor[64];
};

struct start_connection_rep{
	struct header header;
	uint16_t version;
	uint8_t result_code;
	uint8_t err_code;
	uint32_t framing;
	uint32_t bearer;
	uint16_t channel;
	uint16_t revision;
	uint8_t host[64];
	uint8_t vendor[64];
};

struct stop_connection_req{
	struct header header;
	uint8_t reason;
	uint8_t nonepkt;
	uint16_t reserved;
};

struct stop_connection_rep{
	struct header header;
	uint8_t result;
	uint8_t err_code;
	uint16_t reserved;
};

struct echo_req{
	struct header header;
	uint32_t identifier;
};

struct echo_rep{
	struct header header;
	uint32_t identifier;
	uint8_t result;
	uint8_t err_code;
	uint16_t reserved;
};

struct outgoing_req{
	struct header header;
	uint16_t call_id;
	uint16_t serial_id;
	uint32_t min_bps;
	uint32_t max_bps;
	uint32_t bearer_type;
	uint32_t framinig_type;
	uint16_t window_size;
	uint16_t delay;
	uint16_t pn_len;
	uint16_t reserved;
	uint8_t pn[64];
	uint8_t sub_addr[64];
};

struct outgoing_rep{
	struct header header;
	uint16_t call_id;
	uint16_t peer_id;
	uint8_t result_code;
	uint8_t err_code;
	uint16_t cause_code;
	uint32_t speed;
	uint16_t window_size;
	uint16_t delay;
	uint32_t phy_id;
};

struct incoming_req{
	struct header header;
	uint16_t call_id;
	uint16_t serial_id;
	uint32_t bearer_type;
	uint32_t phy_id;
	uint16_t dialed_len;
	uint16_t dialing_len;
	uint8_t dialed[64];
	uint8_t dialing[64];
	uint8_t sub_addr[64];
};

struct incoming_rep{
	struct header header;
	uint16_t call_id;
	uint16_t peer_id;
	uint8_t result_code;
	uint8_t err_code;
	uint16_t window_size;
	uint16_t delay;
	uint16_t reserved;
};

struct incoming_connected{
	struct header header;
	uint16_t peer_id;
	uint16_t reserved;
	uint16_t speed;
	uint16_t window_size;
	uint16_t delay;
	uint32_t framing;
};

struct clear_req{
	struct header header;
	uint16_t call_id;
	uint16_t reserved;
};

struct disconnect_noti{
	struct header header;
	uint16_t call_id;
	uint8_t result_code;
	uint8_t err_code;
	uint16_t cause_code;
	uint16_t reserved;
	uint8_t statistic[128];
};

struct wan_err_noti{
	struct header header;
	uint16_t call_id;
	uint16_t reserved;
	uint32_t err[6];
};

struct set_link_info{
	struct header header;
	uint16_t peer_id;
	uint16_t reserved;
	uint32_t send_accm;
	uint32_t receive_accm;
};

struct gre_lcp_cli{
	uint16_t flag;
	uint16_t prot_type;
	uint32_t key;
	uint32_t sequence;
	uint8_t addr;
	uint8_t ctrl;
	uint16_t protocol;
	uint8_t code;
	uint8_t identifier;
	uint16_t len;
	// options
	uint8_t max_recv_type;
	uint8_t max_recv_len;
	uint16_t max_recv_unit;
	uint8_t magic_type;
	uint8_t magic_len;
	uint32_t magic_number;
	// protocol field compression
	uint8_t pfc_type; 
	uint8_t pfc_len;
	// address and control field compression
	uint8_t acfc_type;
	uint8_t acfc_len;
	// callback
	uint8_t cb_type;
	uint8_t cb_len;
	uint8_t cb_operation;
};

struct gre_lcp_host{
	uint16_t flag;
	uint16_t prot_type;
	uint32_t key;
	uint32_t sequence;
	uint8_t addr;
	uint8_t ctrl;
	uint16_t protocol;
	uint8_t code;
	uint8_t identifier;
	uint16_t len;
	uint8_t async_type;
	uint8_t async_len;
	uint32_t async_map;
	uint8_t auth_type;
	uint8_t auth_len;
	uint16_t auth_protocol;
	uint8_t auth_algorithm;
	uint8_t magic_type;
	uint8_t magic_len;
	uint32_t magic_number;
	uint8_t pfc_type;
	uint8_t pfc_len;
	uint8_t acfc_type;
	uint8_t acfc_len;
};
#endif
