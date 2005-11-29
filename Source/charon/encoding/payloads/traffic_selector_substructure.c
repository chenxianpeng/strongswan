/**
 * @file traffic_selector_substructure.c
 * 
 * @brief Interface of traffic_selector_substructure_t.
 * 
 */

/*
 * Copyright (C) 2005 Jan Hutter, Martin Willi
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "traffic_selector_substructure.h"

#include <encoding/payloads/encodings.h>
#include <utils/allocator.h>
#include <utils/linked_list.h>

/** 
 * String mappings for ts_type_t.
 */
mapping_t ts_type_m[] = {
{TS_IPV4_ADDR_RANGE, "TS_IPV4_ADDR_RANGE"},
{TS_IPV6_ADDR_RANGE, "TS_IPV6_ADDR_RANGE"},
{MAPPING_END, NULL}
};


typedef struct private_traffic_selector_substructure_t private_traffic_selector_substructure_t;

/**
 * Private data of an traffic_selector_substructure_t object.
 * 
 */
struct private_traffic_selector_substructure_t {
	/**
	 * Public traffic_selector_substructure_t interface.
	 */
	traffic_selector_substructure_t public;
	
	/**
	 * Type of traffic selector.
	 */
	u_int8_t ts_type;
	
	/**
	 * IP Protocol ID.
	 */
	u_int8_t  ip_protocol_id;

	/**
	 * Length of this payload.
	 */
	u_int16_t payload_length;
	
	/**
	 * Start port number.
	 */
	u_int16_t start_port;

	/**
	 * End port number.
	 */
	u_int16_t end_port;
	
	/**
	 * Starting address.
	 */
	chunk_t starting_address;

	/**
	 * Ending address.
	 */
	chunk_t ending_address;
};

/**
 * Encoding rules to parse or generate a TS payload
 * 
 * The defined offsets are the positions in a object of type 
 * private_traffic_selector_substructure_t.
 * 
 */
encoding_rule_t traffic_selector_substructure_encodings[] = {
 	/* 1 Byte next ts type*/
	{ TS_TYPE,			offsetof(private_traffic_selector_substructure_t, ts_type) 			},
 	/* 1 Byte IP protocol id*/
	{ U_INT_8,			offsetof(private_traffic_selector_substructure_t, ip_protocol_id) 	},
	/* Length of the whole payload*/	
	{ PAYLOAD_LENGTH,	offsetof(private_traffic_selector_substructure_t, payload_length)		},
 	/* 2 Byte start port*/
	{ U_INT_16,		offsetof(private_traffic_selector_substructure_t, start_port)			},
 	/* 2 Byte end port*/
	{ U_INT_16,		offsetof(private_traffic_selector_substructure_t, end_port)			},
	/* starting address is either 4 or 16 byte */
	{ ADDRESS,			offsetof(private_traffic_selector_substructure_t, starting_address)	},
	/* ending address is either 4 or 16 byte */
	{ ADDRESS,			offsetof(private_traffic_selector_substructure_t, ending_address)		}

};

/*
                           1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      !   TS Type     !IP Protocol ID*|       Selector Length         |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |           Start Port*         |           End Port*           |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      !                                                               !
      ~                         Starting Address*                     ~
      !                                                               !
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      !                                                               !
      ~                         Ending Address*                       ~
      !                                                               !
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

/**
 * Implementation of payload_t.verify.
 */
static status_t verify(private_traffic_selector_substructure_t *this)
{
	
	if (this->start_port > this->end_port)
	{
		return FAILED;
	}
	switch (this->ts_type)
	{
		case TS_IPV4_ADDR_RANGE:
		{
			if ((this->starting_address.len != 4) || 
				(this->ending_address.len != 4))
			{
				/* ipv4 address must be 4 bytes long */
				return FAILED;
			}
			break;
		}
		case TS_IPV6_ADDR_RANGE:
		default:
		{
			/* not supported ts type */
			return FAILED;
		}
	}

	
	return SUCCESS;
}

/**
 * Implementation of traffic_selector_substructure_t.get_encoding_rules.
 */
static void get_encoding_rules(private_traffic_selector_substructure_t *this, encoding_rule_t **rules, size_t *rule_count)
{
	*rules = traffic_selector_substructure_encodings;
	*rule_count = sizeof(traffic_selector_substructure_encodings) / sizeof(encoding_rule_t);
}

/**
 * Implementation of payload_t.get_type.
 */
static payload_type_t get_payload_type(private_traffic_selector_substructure_t *this)
{
	return TRAFFIC_SELECTOR_SUBSTRUCTURE;
}

/**
 * Implementation of payload_t.get_next_type.
 */
static payload_type_t get_next_type(private_traffic_selector_substructure_t *this)
{
	return 0;
}

/**
 * Implementation of payload_t.set_next_type.
 */
static void set_next_type(private_traffic_selector_substructure_t *this,payload_type_t type)
{
}

/**
 * Implementation of payload_t.get_length.
 */
static size_t get_length(private_traffic_selector_substructure_t *this)
{
	return this->payload_length;
}

/**
 * Implementation of traffic_selector_substructure_t.get_ts_type.
 */
static ts_type_t get_ts_type (private_traffic_selector_substructure_t *this)
{
	return this->ts_type;
}

/**
 * Implementation of traffic_selector_substructure_t.set_ts_type.
 */
static void set_ts_type (private_traffic_selector_substructure_t *this,ts_type_t ts_type)
{
	this->ts_type = ts_type;
}

/**
 * Implementation of traffic_selector_substructure_t.get_protocol_id.
 */
static u_int8_t get_protocol_id (private_traffic_selector_substructure_t *this)
{
	return this->ip_protocol_id;
}

/**
 * Implementation of traffic_selector_substructure_t.set_protocol_id.
 */	
static void set_protocol_id (private_traffic_selector_substructure_t *this,u_int8_t protocol_id)
{
	this->ip_protocol_id = protocol_id;
}

/**
 * Implementation of traffic_selector_substructure_t.get_start_host.
 */
static host_t * get_start_host (private_traffic_selector_substructure_t *this)
{
	return (host_create_from_chunk(AF_INET,this->starting_address, this->start_port));
}

/**
 * Implementation of traffic_selector_substructure_t.set_start_host.
 */
static void set_start_host (private_traffic_selector_substructure_t *this,host_t *start_host)
{
	this->start_port = start_host->get_port(start_host);
	if (this->starting_address.ptr != NULL)
	{
		allocator_free_chunk(&(this->starting_address));
	}
	this->starting_address = start_host->get_address_as_chunk(start_host);
	this->payload_length = TRAFFIC_SELECTOR_HEADER_LENGTH + this->starting_address.len + this->ending_address.len;
}

/**
 * Implementation of traffic_selector_substructure_t.get_end_host.
 */
static host_t *get_end_host (private_traffic_selector_substructure_t *this)
{
	return (host_create_from_chunk(AF_INET,this->ending_address, this->end_port));
}

/**
 * Implementation of traffic_selector_substructure_t.set_end_host.
 */
static void set_end_host (private_traffic_selector_substructure_t *this,host_t *end_host)
{
	this->end_port = end_host->get_port(end_host);
	if (this->ending_address.ptr != NULL)
	{
		allocator_free_chunk(&(this->ending_address));
	}
	this->ending_address = end_host->get_address_as_chunk(end_host);
	this->payload_length = TRAFFIC_SELECTOR_HEADER_LENGTH + this->starting_address.len + this->ending_address.len;	
}

/**
 * Implementation of payload_t.destroy and traffic_selector_substructure_t.destroy.
 */
static void destroy(private_traffic_selector_substructure_t *this)
{
	
	if (this->starting_address.ptr != NULL)
	{
		allocator_free_chunk(&(this->starting_address));
	}
	
	if (this->ending_address.ptr != NULL)
	{
		allocator_free_chunk(&(this->ending_address));
	}
	
	allocator_free(this);	
}

/*
 * Described in header
 */
traffic_selector_substructure_t *traffic_selector_substructure_create(bool is_initiator)
{
	private_traffic_selector_substructure_t *this = allocator_alloc_thing(private_traffic_selector_substructure_t);

	/* interface functions */
	this->public.payload_interface.verify = (status_t (*) (payload_t *))verify;
	this->public.payload_interface.get_encoding_rules = (void (*) (payload_t *, encoding_rule_t **, size_t *) ) get_encoding_rules;
	this->public.payload_interface.get_length = (size_t (*) (payload_t *)) get_length;
	this->public.payload_interface.get_next_type = (payload_type_t (*) (payload_t *)) get_next_type;
	this->public.payload_interface.set_next_type = (void (*) (payload_t *,payload_type_t)) set_next_type;
	this->public.payload_interface.get_type = (payload_type_t (*) (payload_t *)) get_payload_type;
	this->public.payload_interface.destroy = (void (*) (payload_t *))destroy;
	
	/* public functions */
	this->public.destroy = (void (*) (traffic_selector_substructure_t *)) destroy;
	this->public.get_ts_type = (ts_type_t (*) (traffic_selector_substructure_t *)) get_ts_type;
	this->public.set_ts_type = (void (*) (traffic_selector_substructure_t *,ts_type_t)) set_ts_type;
	this->public.get_protocol_id = (u_int8_t (*) (traffic_selector_substructure_t *)) get_protocol_id;
	this->public.set_protocol_id = (void (*) (traffic_selector_substructure_t *,u_int8_t)) set_protocol_id;
	this->public.get_start_host = (host_t * (*) (traffic_selector_substructure_t *))get_start_host;
	this->public.set_start_host = (void (*) (traffic_selector_substructure_t *, host_t *))set_start_host;
	this->public.get_end_host = (host_t * (*) (traffic_selector_substructure_t *))get_end_host;
	this->public.set_end_host = (void (*) (traffic_selector_substructure_t *, host_t *))set_end_host;
	
	
	/* private variables */
	this->payload_length =TRAFFIC_SELECTOR_HEADER_LENGTH;
	this->start_port = 0;
	this->end_port = 0;
	this->starting_address = CHUNK_INITIALIZER;
	this->ending_address = CHUNK_INITIALIZER;
	this->ip_protocol_id = 0;
	/* must be set to be valid */
	this->ts_type = TS_IPV4_ADDR_RANGE;

	return (&(this->public));
}
